/****************************************************************************
 * net/local/local_recvmsg.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <debug.h>
#include <fcntl.h>

#include <nuttx/kmalloc.h>
#include <nuttx/fs/fs.h>
#include <nuttx/net/net.h>

#include "socket/socket.h"
#include "local/local.h"
#include "utils/utils.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: psock_fifo_read
 *
 * Description:
 *   A thin layer around local_fifo_read that handles socket-related loss-of-
 *   connection events.
 *
 ****************************************************************************/

static int psock_fifo_read(FAR struct socket *psock, FAR void *buf,
                           size_t offset, FAR size_t *readlen,
                           int flags, bool once)
{
  FAR struct local_conn_s *conn = psock->s_conn;
  int ret;

  if (flags & MSG_PEEK)
    {
      struct pipe_peek_s peek =
        {
          buf,
          offset,
          *readlen
        };

      ret = file_ioctl(&conn->lc_infile, PIPEIOC_PEEK,
                       (unsigned long)((uintptr_t)&peek));
    }
  else
    {
      ret = local_fifo_read(&conn->lc_infile, buf, readlen, once);
    }

  if (ret < 0)
    {
      /* -ECONNRESET is a special case.  We may or not have received
       * data, then the peer closed the connection.
       */

      if (ret == -ECONNRESET)
        {
          nerr("ERROR: Lost connection: %d\n", ret);

          /* Report an ungraceful loss of connection.  This should
           * eventually be reported as ENOTCONN.
           */

          conn->lc_conn.s_flags &= ~(_SF_CONNECTED | _SF_CLOSED);
          conn->lc_state = LOCAL_STATE_DISCONNECTED;

          /* Did we receive any data? */

          if (*readlen <= 0)
            {
              /* No.. return the ECONNRESET error now.  Otherwise,
               * process the received data and return ENOTCONN the
               * next time that psock_recvfrom() is called.
               */

              return ret;
            }
        }
      else
        {
          if (ret != -EAGAIN)
            {
              nerr("ERROR: Failed to read packet: %d\n", ret);
            }

          return ret;
        }
    }

  return OK;
}

/****************************************************************************
 * Name: local_recvctl
 *
 * Description:
 *   Handle the socket message conntrol field
 *
 * Input Parameters:
 *   conn     Local connection instance
 *   msg      Message to send
 *
 ****************************************************************************/

#ifdef CONFIG_NET_LOCAL_SCM
static void local_recvctl(FAR struct local_conn_s *conn,
                          FAR struct msghdr *msg, int flags)
{
  FAR struct local_conn_s *peer;
  int count;
  int *fds;
  int i;

  net_lock();

  if (conn->lc_peer == NULL)
    {
      peer = local_peerconn(conn);
      if (peer == NULL)
        {
          goto out;
        }
    }
  else
    {
      peer = conn;
    }

  if (peer->lc_cfpcount == 0)
    {
      goto out;
    }

  fds = cmsg_append(msg, SOL_SOCKET, SCM_RIGHTS, NULL,
                    sizeof(int) * peer->lc_cfpcount);
  if (fds == NULL)
    {
      goto out;
    }

  count = peer->lc_cfpcount;
  for (i = 0; i < count; i++)
    {
      fds[i] = file_dup(peer->lc_cfps[i], 0,
                        flags & MSG_CMSG_CLOEXEC ? O_CLOEXEC : 0);
      file_close(peer->lc_cfps[i]);
      kmm_free(peer->lc_cfps[i]);
      peer->lc_cfps[i] = NULL;
      peer->lc_cfpcount--;
      if (fds[i] < 0)
        {
          i++;
          break;
        }
    }

  if (i > 0)
    {
      if (peer->lc_cfpcount)
        {
          memmove(peer->lc_cfps[0], peer->lc_cfps[i],
                  sizeof(FAR void *) * peer->lc_cfpcount);
        }
    }

out:
  net_unlock();
}
#endif /* CONFIG_NET_LOCAL_SCM */

/****************************************************************************
 * Name: psock_stream_recvfrom
 *
 * Description:
 *   psock_stream_recvfrom() receives messages from a local stream socket.
 *
 * Input Parameters:
 *   psock    A pointer to a NuttX-specific, internal socket structure
 *   buf      Buffer to receive data
 *   len      Length of buffer
 *   flags    Receive flags
 *   from     Address of source (may be NULL)
 *   fromlen  The length of the address structure
 *
 * Returned Value:
 *   On success, returns the number of characters received.  If no data is
 *   available to be received and the peer has performed an orderly shutdown,
 *   recv() will return 0.  Otherwise, on errors, -1 is returned, and errno
 *   is set appropriately (see receive from for the complete list).
 *
 ****************************************************************************/

#ifdef CONFIG_NET_LOCAL_STREAM
static inline ssize_t
psock_stream_recvfrom(FAR struct socket *psock, FAR void *buf, size_t len,
                      int flags, FAR struct sockaddr *from,
                      FAR socklen_t *fromlen)
{
  FAR struct local_conn_s *conn = psock->s_conn;
  size_t readlen = len;
  int ret;

  /* Verify that this is a connected peer socket */

  if (conn->lc_state != LOCAL_STATE_CONNECTED)
    {
      nerr("ERROR: not connected\n");
      return -ENOTCONN;
    }

  /* Check shutdown state */

  if (conn->lc_infile.f_inode == NULL)
    {
      return 0;
    }

  /* If it is non-blocking mode, the data in fifo is 0 and
   * returns directly
   */

  if (flags & MSG_DONTWAIT)
    {
      int data_len = 0;
      ret = file_ioctl(&conn->lc_infile, FIONREAD, &data_len);
      if (ret < 0)
        {
          return ret;
        }

      if (data_len == 0)
        {
          return -EAGAIN;
        }
    }

  /* Read the packet */

  ret = psock_fifo_read(psock, buf, 0, &readlen, flags, true);
  if (ret < 0)
    {
      return ret;
    }

  /* Return the address family */

  if (from)
    {
      ret = local_getaddr(conn, from, fromlen);
      if (ret < 0)
        {
          return ret;
        }
    }

  return readlen;
}
#endif /* CONFIG_NET_LOCAL_STREAM */

/****************************************************************************
 * Name: psock_fifo_discard
 *
 * Description:
 *   psock_fifo_discard() discard buffer from a local socket.
 *
 * Input Parameters:
 *   psock      A pointer to a NuttX-specific, internal socket structure
 *   len        Remaining length of buffer
 *   flags      Receive flags
 *
 * Returned Value:
 *   On success, returns OK.  Otherwise, on errors, errno is returned
 *
 ****************************************************************************/

#ifdef CONFIG_NET_LOCAL_DGRAM
static int psock_fifo_discard(FAR struct socket *psock, size_t len,
                              int flags)
{
  uint8_t bitbucket[256];
  size_t tmplen;
  int ret;

  if (flags & MSG_PEEK)
    {
      return OK;
    }

  while (len > 0)
    {
      /* Read 256 bytes into the bit bucket */

      tmplen = MIN(len, sizeof(bitbucket));
      ret = psock_fifo_read(psock, bitbucket, 0, &tmplen, flags, false);
      if (ret < 0)
        {
          nerr("ERROR: Failed to get bitbucket : ret %d\n", ret);
          return ret;
        }

      /* Adjust the number of bytes len to be read from the packet */

      DEBUGASSERT(tmplen <= len);
      len -= tmplen;
    }

  return OK;
}

/****************************************************************************
 * Name: psock_dgram_recvfrom
 *
 * Description:
 *   psock_dgram_recvfrom() receives messages from a local datagram socket.
 *
 * Input Parameters:
 *   psock    A pointer to a NuttX-specific, internal socket structure
 *   buf      Buffer to receive data
 *   len      Length of buffer
 *   flags    Receive flags
 *   from     Address of source (may be NULL)
 *   fromlen  The length of the address structure
 *
 * Returned Value:
 *   On success, returns the number of characters received.  Otherwise, on
 *   errors, -1 is returned, and errno is set appropriately (see receive
 *   from for the complete list).
 *
 ****************************************************************************/

static inline ssize_t
psock_dgram_recvfrom(FAR struct socket *psock, FAR void *buf, size_t len,
                     int flags, FAR struct sockaddr *from,
                     FAR socklen_t *fromlen)
{
  FAR struct local_conn_s *conn = psock->s_conn;
  size_t readlen;
  size_t pathlen;
  bool bclose = false;
  lc_size_t addrlen;
  lc_size_t pktlen;
  int offset = 0;
  int ret;

  /* Verify that this is a bound, un-connected peer socket */

  if (conn->lc_state != LOCAL_STATE_BOUND &&
      conn->lc_state != LOCAL_STATE_CONNECTED)
    {
      /* Either not bound to address or it is connected */

      nerr("ERROR: Connected or not bound\n");
      return -EISCONN;
    }

  if (conn->lc_infile.f_inode == NULL)
    {
      bclose = true;

      /* Make sure that half duplex FIFO has been created */

      ret = local_create_halfduplex(conn, conn->lc_path, conn->lc_rcvsize);
      if (ret < 0)
        {
          nerr("ERROR: Failed to create FIFO for %s: %d\n",
               conn->lc_path, ret);
          return ret;
        }

      /* Open the receiving side of the transfer */

      ret = local_open_receiver(conn, (flags & MSG_DONTWAIT) != 0 ||
                                _SS_ISNONBLOCK(conn->lc_conn.s_flags));
      if (ret < 0)
        {
          nerr("ERROR: Failed to open FIFO for %s: %d\n",
               conn->lc_path, ret);
          goto errout_with_halfduplex;
        }
    }

  readlen = sizeof(addrlen);
  ret = psock_fifo_read(psock, &addrlen, offset, &readlen, flags, false);
  if (ret < 0)
    {
      nerr("ERROR: Failed to get path length: ret %d\n", ret);
      return ret;
    }

  /* Sync to the start of the next packet in the stream and get the size of
   * the next packet.
   */

  readlen = sizeof(pktlen);
  offset += sizeof(addrlen);
  ret = psock_fifo_read(psock, &pktlen, offset, &readlen, flags, false);
  if (ret < 0)
    {
      nerr("ERROR: Failed to get packet length: ret %d\n", ret);
      goto errout_with_infd;
    }

  readlen = addrlen;
  offset += sizeof(pktlen);

  if (from && fromlen && *fromlen)
    {
      pathlen = MIN(*fromlen - 1, readlen);
      ret = psock_fifo_read(psock, from->sa_data, offset,
                            &pathlen, flags, false);
      if (ret < 0)
        {
          nerr("ERROR: Failed to get path : ret %d\n", ret);
          goto errout_with_infd;
        }

      from->sa_family = AF_LOCAL;
      from->sa_data[pathlen] = '\0';
      *fromlen = pathlen;
      readlen -= pathlen;
    }

  if (readlen)
    {
      ret = psock_fifo_discard(psock, readlen, flags);
      if (ret < 0)
        {
          nerr("ERROR: Failed to discard redunance address: ret %d\n", ret);
          goto errout_with_infd;
        }
    }

  /* Read the packet */

  readlen = MIN(pktlen, len);
  offset += addrlen;
  ret     = psock_fifo_read(psock, buf, offset, &readlen, flags, false);
  if (ret < 0)
    {
      nerr("ERROR: Failed to get packet : ret %d\n", ret);
      goto errout_with_infd;
    }

  /* If there are unread bytes remaining in the packet, flush the remainder
   * of the packet to the bit bucket.
   */

  DEBUGASSERT(readlen <= pktlen);
  if (readlen < pktlen)
    {
      ret = psock_fifo_discard(psock, pktlen - readlen, flags);
    }

errout_with_infd:

  /* Close the read-only file descriptor */

  if (bclose)
    {
      /* Now we can close the read-only file descriptor */

      file_close(&conn->lc_infile);
      conn->lc_infile.f_inode = NULL;

errout_with_halfduplex:

      /* Release our reference to the half duplex FIFO */

      local_release_halfduplex(conn);
    }

  return ret < 0 ? ret : readlen;
}
#endif /* CONFIG_NET_LOCAL_DGRAM */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: local_recvmsg
 *
 * Description:
 *   recvmsg() receives messages from a local socket and may be used to
 *   receive data on a socket whether or not it is connection-oriented.
 *
 *   If from is not NULL, and the underlying protocol provides the source
 *   address, this source address is filled in. The argument fromlen
 *   initialized to the size of the buffer associated with from, and modified
 *   on return to indicate the actual size of the address stored there.
 *
 * Input Parameters:
 *   psock    A pointer to a NuttX-specific, internal socket structure
 *   msg      Buffer to receive the message
 *   flags    Receive flags (ignored for now)
 *
 * Returned Value:
 *   On success, returns the number of characters received. If no data is
 *   available to be received and the peer has performed an orderly shutdown,
 *   recvmsg() will return 0.  Otherwise, on errors, a negated errno value is
 *   returned (see recvmsg() for the list of appropriate error values).
 *
 ****************************************************************************/

ssize_t local_recvmsg(FAR struct socket *psock, FAR struct msghdr *msg,
                      int flags)
{
  FAR socklen_t *fromlen = &msg->msg_namelen;
  FAR struct sockaddr *from = msg->msg_name;
  FAR void *buf = msg->msg_iov->iov_base;
  size_t len = msg->msg_iov->iov_len;

  if (msg->msg_iovlen != 1)
    {
      return -ENOTSUP;
    }

  DEBUGASSERT(buf);

  /* Check for a stream socket */

#ifdef CONFIG_NET_LOCAL_STREAM
  if (psock->s_type == SOCK_STREAM)
    {
      len = psock_stream_recvfrom(psock, buf, len, flags, from, fromlen);
    }
  else
#endif

#ifdef CONFIG_NET_LOCAL_DGRAM
  if (psock->s_type == SOCK_DGRAM)
    {
      len = psock_dgram_recvfrom(psock, buf, len, flags, from, fromlen);
    }
  else
#endif
    {
      DEBUGPANIC();
      nerr("ERROR: Unrecognized socket type: %" PRIu8 "\n", psock->s_type);
      len = -EINVAL;
    }

#ifdef CONFIG_NET_LOCAL_SCM
  /* Receive the control message */

  if (len >= 0 && msg->msg_control &&
      msg->msg_controllen > sizeof(struct cmsghdr))
    {
      local_recvctl(psock->s_conn, msg, flags);
    }
#endif /* CONFIG_NET_LOCAL_SCM */

  return len;
}
