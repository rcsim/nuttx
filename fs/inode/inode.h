/****************************************************************************
 * fs/inode/inode.h
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

#ifndef __FS_INODE_INODE_H
#define __FS_INODE_INODE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <sched.h>

#include <nuttx/kmalloc.h>
#include <nuttx/sched.h>
#include <nuttx/fs/fs.h>
#include <nuttx/lib/lib.h>

#include "fs_heap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SETUP_SEARCH(d,p,n) \
  do \
    { \
      (d)->path     = (p); \
      (d)->node     = NULL; \
      (d)->peer     = NULL; \
      (d)->parent   = NULL; \
      (d)->relpath  = NULL; \
      (d)->buffer   = NULL; \
      (d)->nofollow = (n); \
    } \
  while (0)

#define RELEASE_SEARCH(d) \
  do \
    { \
      if ((d)->buffer != NULL) \
        { \
          fs_heap_free((d)->buffer); \
          (d)->buffer  = NULL; \
        } \
    } \
  while (0)

#if CONFIG_FS_BACKTRACE > 0
#  define FS_ADD_BACKTRACE(fd) \
     do \
       { \
          int n = sched_backtrace(_SCHED_GETTID(), \
                                  (fd)->f_backtrace, \
                                  CONFIG_FS_BACKTRACE, \
                                  CONFIG_FS_BACKTRACE_SKIP); \
          if (n < CONFIG_FS_BACKTRACE) \
            { \
              (fd)->f_backtrace[n] = NULL; \
            } \
       } \
     while (0)
#else
#  define FS_ADD_BACKTRACE(fd)
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* This is the type of the argument to inode_search().
 *
 *  path     - INPUT:  Path of inode to find
 *             OUTPUT: Residual part of path not traversed
 *  node     - INPUT:  (not used)
 *             OUTPUT: On success, holds the pointer to the inode found.
 *  peer     - INPUT:  (not used)
 *             OUTPUT: The inode to the "left" of the inode found.
 *  parent   - INPUT:  (not used)
 *             OUTPUT: The inode to the "above" of the inode found.
 *  relpath  - INPUT:  (not used)
 *             OUTPUT: If the returned inode is a mountpoint, this is the
 *                     relative path from the mountpoint.
 *             OUTPUT: If a symbolic link into a mounted file system is
 *                     detected while traversing the path, then the link
 *                     will be converted to a mountpoint inode if the
 *                     mountpoint link is in an intermediate node of the
 *                     path or at the final node of the path with
 *                     nofollow=true.
 *  nofollow - INPUT:  true: terminal node is returned; false: if the
 *                     terminal is a soft link, then return the inode of
 *                     the link target.
 *           - OUTPUT: (not used)
 *  buffer   - INPUT:  Not used
 *           - OUTPUT: May hold an allocated intermediate path which is
 *                     probably of no interest to the caller unless it holds
 *                     the relpath.
 */

struct inode_search_s
{
  FAR const char *path;      /* Path of inode to find */
  FAR struct inode *node;    /* Pointer to the inode found */
  FAR struct inode *peer;    /* Node to the "left" for the found inode */
  FAR struct inode *parent;  /* Node "above" the found inode */
  FAR const char *relpath;   /* Relative path into the mountpoint */
  FAR char *buffer;          /* Path expansion buffer */
  bool nofollow;             /* true: Don't follow terminal soft link */
};

/* Callback used by foreach_inode to traverse all inodes in the pseudo-
 * file system.
 */

typedef int (*foreach_inode_t)(FAR struct inode *inode,
                               FAR char dirpath[PATH_MAX],
                               FAR void *arg);

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

EXTERN FAR struct inode *g_root_inode;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: inode_initialize
 *
 * Description:
 *   This is called from the OS initialization logic to configure the file
 *   system.
 *
 ****************************************************************************/

void inode_initialize(void);

/****************************************************************************
 * Name: inode_lock
 *
 * Description:
 *   Get writeable exclusive access to the in-memory inode tree.
 *
 ****************************************************************************/

void inode_lock(void);

/****************************************************************************
 * Name: inode_rlock
 *
 * Description:
 *   Get readable exclusive access to the in-memory inode tree.
 *
 ****************************************************************************/

void inode_rlock(void);

/****************************************************************************
 * Name: inode_unlock
 *
 * Description:
 *   Relinquish writeable exclusive access to the in-memory inode tree.
 *
 ****************************************************************************/

void inode_unlock(void);

/****************************************************************************
 * Name: inode_runlock
 *
 * Description:
 *   Relinquish read exclusive access to the in-memory inode tree.
 *
 ****************************************************************************/

void inode_runlock(void);

/****************************************************************************
 * Name: inode_search
 *
 * Description:
 *   Find the inode associated with 'path' returning the inode references
 *   and references to its companion nodes.
 *
 *   If a mountpoint is encountered in the search prior to encountering the
 *   terminal node, the search will terminate at the mountpoint inode.  That
 *   inode and the relative path from the mountpoint, 'relpath' will be
 *   returned.
 *
 *   inode_search will follow soft links in path leading up to the terminal
 *   node.  Whether or no inode_search() will deference that terminal node
 *   depends on the 'nofollow' input.
 *
 *   If a soft link is encountered that is not the terminal node in the path,
 *   that link WILL be deferenced unconditionally.
 *
 * Assumptions:
 *   The caller holds the g_inode_sem semaphore
 *
 ****************************************************************************/

int inode_search(FAR struct inode_search_s *desc);

/****************************************************************************
 * Name: inode_find
 *
 * Description:
 *   This is called from the open() logic to get a reference to the inode
 *   associated with a path.  This is accomplished by calling inode_search().
 *   inode_find() is a simple wrapper around inode_search().  The primary
 *   difference between inode_find() and inode_search is that inode_find()
 *   will lock the inode tree and increment the reference count on the inode.
 *
 ****************************************************************************/

int inode_find(FAR struct inode_search_s *desc);

/****************************************************************************
 * Name: inode_stat
 *
 * Description:
 *   The inode_stat() function will obtain information about an 'inode' in
 *   the pseudo file system and will write it to the area pointed to by
 *   'buf'.
 *
 *   The 'buf' argument is a pointer to a stat structure, as defined in
 *   <sys/stat.h>, into which information is placed concerning the file.
 *
 * Input Parameters:
 *   inode   - The inode of interest
 *   buf     - The caller-provided location in which to return information
 *             about the inode.
 *   resolve - Whether to resolve the symbolic link:
 *               0: Don't resolve the symbolic line
 *               1: Resolve the symbolic link
 *             >=2: The recursive count in the resolving process
 *
 * Returned Value:
 *   Zero (OK) returned on success.  Otherwise, a negated errno value is
 *   returned to indicate the nature of the failure.
 *
 ****************************************************************************/

int inode_stat(FAR struct inode *inode, FAR struct stat *buf, int resolve);

/****************************************************************************
 * Name: inode_chstat
 *
 * Description:
 *   The inode_chstat() function will change information about an 'inode'
 *   in the pseudo file system according the area pointed to by 'buf'.
 *
 *   The 'buf' argument is a pointer to a stat structure, as defined in
 *   <sys/stat.h>, which information is placed concerning the file.
 *
 * Input Parameters:
 *   inode   - The inode of interest
 *   buf     - The caller provide location in which to apply information
 *             about the inode.
 *   flags   - The valid field in buf
 *   resolve - Whether to resolve the symbolic link
 *
 * Returned Value:
 *   Zero (OK) returned on success.  Otherwise, a negated errno value is
 *   returned to indicate the nature of the failure.
 *
 ****************************************************************************/

int inode_chstat(FAR struct inode *inode,
                 FAR const struct stat *buf, int flags, int resolve);

/****************************************************************************
 * Name: inode_getpath
 *
 * Description:
 *   Given the full path from inode.
 *
 ****************************************************************************/

int inode_getpath(FAR struct inode *inode, FAR char *path, size_t len);

/****************************************************************************
 * Name: inode_free
 *
 * Description:
 *   Free resources used by an inode
 *
 ****************************************************************************/

void inode_free(FAR struct inode *inode);

/****************************************************************************
 * Name: inode_nextname
 *
 * Description:
 *   Given a path with node names separated by '/', return the next node
 *   name.
 *
 ****************************************************************************/

const char *inode_nextname(FAR const char *name);

/****************************************************************************
 * Name: inode_root_reserve
 *
 * Description:
 *   Reserve the root node for the pseudo file system.
 *
 ****************************************************************************/

void inode_root_reserve(void);

/****************************************************************************
 * Name: inode_reserve
 *
 * Description:
 *   Reserve an (initialized) inode the pseudo file system.
 *
 *   NOTE: Caller must hold the inode semaphore
 *
 * Input Parameters:
 *   path - The path to the inode to create
 *   mode - inmode privileges
 *   inode - The location to return the inode pointer
 *
 * Returned Value:
 *   Zero on success (with the inode point in 'inode'); A negated errno
 *   value is returned on failure:
 *
 *   EINVAL - 'path' is invalid for this operation
 *   EEXIST - An inode already exists at 'path'
 *   ENOMEM - Failed to allocate in-memory resources for the operation
 *
 ****************************************************************************/

int inode_reserve(FAR const char *path,
                  mode_t mode, FAR struct inode **inode);

/****************************************************************************
 * Name: inode_remove
 *
 * Description:
 *   Given a path, remove a the node from the in-memory, inode tree that the
 *   path refers to and free all resources related to the inode.  If the
 *   inode is in-use, then it will be unlinked, but will not be freed until
 *   the last reference to the inode is released.
 *
 * Assumptions/Limitations:
 *   The caller must hold the inode semaphore
 *
 ****************************************************************************/

int inode_remove(FAR const char *path);

/****************************************************************************
 * Name: inode_addref
 *
 * Description:
 *   Increment the reference count on an inode (as when a file descriptor
 *   is dup'ed).
 *
 ****************************************************************************/

void inode_addref(FAR struct inode *inode);

/****************************************************************************
 * Name: inode_release
 *
 * Description:
 *   This is called from close() logic when it no longer refers to the inode.
 *
 ****************************************************************************/

void inode_release(FAR struct inode *inode);

/****************************************************************************
 * Name: foreach_inode
 *
 * Description:
 *   Visit each inode in the pseudo-file system.  The traversal is terminated
 *   when the callback 'handler' returns a non-zero value, or when all of
 *   the inodes have been visited.
 *
 *   NOTE 1: Use with caution... The pseudo-file system is locked throughout
 *   the traversal.
 *   NOTE 2: The search algorithm is recursive and could, in principle, use
 *   an indeterminate amount of stack space.  This will not usually be a
 *   real work issue.
 *
 ****************************************************************************/

int foreach_inode(foreach_inode_t handler, FAR void *arg);

/****************************************************************************
 * Name: dir_allocate
 *
 * Description:
 *   Allocate a directory instance and bind it to f_priv of filep.
 *
 ****************************************************************************/

int dir_allocate(FAR struct file *filep, FAR const char *relpath);

/****************************************************************************
 * Name: pseudofile_create
 *
 * Description:
 *   Create the pseudo-file with specified path and mode, and alloc inode
 *   of this pseudo-file.
 *
 ****************************************************************************/

#ifdef CONFIG_PSEUDOFS_FILE
int pseudofile_create(FAR struct inode **node, FAR const char *path,
                      mode_t mode);
#endif

/****************************************************************************
 * Name: inode_is_pseudofile
 *
 * Description:
 *    Check inode whether is a pseudo file.
 *
 ****************************************************************************/

#ifdef CONFIG_PSEUDOFS_FILE
bool inode_is_pseudofile(FAR struct inode *inode);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __FS_INODE_INODE_H */
