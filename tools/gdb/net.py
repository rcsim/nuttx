############################################################################
# tools/gdb/net.py
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

import socket

import gdb
import utils
from lists import dq_for_every, sq_for_every

NET_IPv4 = utils.get_symbol_value("CONFIG_NET_IPv4")
NET_IPv6 = utils.get_symbol_value("CONFIG_NET_IPv6")

# NuttX's AF_INET and AF_INET6 have same value as Linux's
AF_INET = socket.AF_INET
AF_INET6 = socket.AF_INET6


def ntohs(val):
    """Convert a 16-bit value from network byte order to host byte order"""

    if utils.get_target_endianness() == utils.BIG_ENDIAN:
        return val
    return utils.swap16(val)


def get_ip_port(conn):
    """Get the IP address and port of a network connection"""

    domain = utils.get_field(conn, "domain", AF_INET if NET_IPv4 else AF_INET6)
    ip_binding = conn["u"]["ipv4" if domain == AF_INET else "ipv6"]
    lport = ntohs(conn["lport"])
    rport = ntohs(conn["rport"])
    laddr = inet_ntop(domain, ip_binding["laddr"])
    raddr = inet_ntop(domain, ip_binding["raddr"])
    return laddr, lport, raddr, rport


def inet_ntop(domain, addr):
    """Convert a network address to a string"""

    addr_len = 16 if domain == AF_INET6 else 4
    return socket.inet_ntop(domain, utils.get_bytes(addr, addr_len))


def socket_for_each_entry(proto):
    """Iterate over a dq of socket structs, usage:
    for conn in socket_for_each_entry("icmp"):
        readahead = conn["readahead"]
    """

    sock_gdbtype = gdb.lookup_type("struct socket_conn_s").pointer()
    conn_gdbtype = gdb.lookup_type("struct %s_conn_s" % proto).pointer()

    for node in dq_for_every(
        gdb.parse_and_eval("g_active_%s_connections" % proto), None
    ):
        yield utils.container_of(
            utils.container_of(
                node, sock_gdbtype, "node"
            ),  # struct socket_conn_s::dq_entry_t node
            conn_gdbtype,
            "sconn",
        )  # udp_conn_s::socket_conn_s sconn


def wrbuffer_inqueue_size(queue=None, protocol="tcp"):
    """Calculate the total size of all iob in the write queue of a udp connection"""

    total = 0
    if queue:
        wrb_gdbtype = gdb.lookup_type("struct %s_wrbuffer_s" % protocol).pointer()
        for entry in sq_for_every(queue, None):
            entry = utils.container_of(entry, wrb_gdbtype, "wb_node")
            total += entry["wb_iob"]["io_pktlen"]
    return total


def tcp_ofoseg_bufsize(conn):
    """Calculate the pending size of out-of-order buffer of a tcp connection"""

    total = 0
    if utils.get_symbol_value("CONFIG_NET_TCP_OUT_OF_ORDER"):
        for i in range(conn["nofosegs"]):
            total += conn["ofosegs"][i]["data"]["io_pktlen"]
    return total


class NetStats(gdb.Command):
    """Network statistics
    Usage: netstats [iob|pkt|tcp|udp|all]

    Examples: netstats - Show all stats
              netstats all - Show all stats
              netstats iob - Show IOB stats
              netstats tcp udp - Show both TCP and UDP stats
    """

    def __init__(self):
        super(NetStats, self).__init__("netstats", gdb.COMMAND_USER)

    def iob_stats(self):
        try:
            size = utils.get_symbol_value("CONFIG_IOB_BUFSIZE")
            ntotal = utils.get_symbol_value("CONFIG_IOB_NBUFFERS")

            nfree = gdb.parse_and_eval("g_iob_sem")["semcount"]
            nwait, nfree = (0, nfree) if nfree >= 0 else (-nfree, 0)

            nthrottle = (
                gdb.parse_and_eval("g_throttle_sem")["semcount"]
                if utils.get_symbol_value("CONFIG_IOB_THROTTLE") > 0
                else 0
            )

            gdb.write(
                "IOB: %10s%10s%10s%10s%10s\n"
                % ("size", "ntotal", "nfree", "nwait", "nthrottle")
            )
            gdb.write(
                "     %10d%10d%10d%10d%10d\n" % (size, ntotal, nfree, nwait, nthrottle)
            )
        except gdb.error as e:
            gdb.write("Failed to get IOB stats: %s\n" % e)

    def pkt_stats(self):
        try:
            netstats = gdb.parse_and_eval("g_netstats")
            gdb.write(
                "Packets:%7s%7s%7s%7s%7s%7s\n"
                % ("IPv4", "IPv6", "TCP", "UDP", "ICMP", "ICMPv6")
            )

            def stats_line(title, member):
                gdb.write("%-8s" % title)
                for proto in ("ipv4", "ipv6", "tcp", "udp", "icmp", "icmpv6"):
                    gdb.write(
                        "%7s"
                        % utils.get_field(utils.get_field(netstats, proto), member, "-")
                    )
                gdb.write("\n")

            stats_line("Received", "recv")
            stats_line("Dropped", "drop")
            stats_line("  VHL", "vhlerr")
            stats_line("  Frag", "fragerr")
            stats_line("  Chksum", "chkerr")
            stats_line("  Type", "typeerr")
            stats_line("  Proto", "protoerr")
            # TODO: Maybe print TCP's ackerr, rst, syndrop, synrst here
            stats_line("Sent", "sent")
            stats_line("  Rexmit", "rexmit")

        except gdb.error as e:
            gdb.write("Failed to get Net Stats: %s\n" % e)

    def tcp_stats(self):
        try:
            gdb.write(
                "TCP Conn: %3s %3s %3s %3s %4s %3s"
                % ("st", "flg", "ref", "tmr", "uack", "nrt")
            )
            gdb.write(
                " %11s %11s+%-5s %21s %21s\n"
                % ("txbuf", "rxbuf", "ofo", "local_address", "remote_address")
            )
            for idx, conn in enumerate(socket_for_each_entry("tcp")):
                state = conn["tcpstateflags"]
                flags = conn["sconn"]["s_flags"]
                ref = conn["crefs"]
                timer = conn["timer"]
                unacked = conn["tx_unacked"]
                nrtx = conn["nrtx"]

                txbuf = utils.get_field(conn, "snd_bufs", -1)
                rxbuf = utils.get_field(conn, "rcv_bufs", -1)
                txsz = wrbuffer_inqueue_size(
                    utils.get_field(conn, "unacked_q"), "tcp"
                ) + wrbuffer_inqueue_size(utils.get_field(conn, "write_q"), "tcp")
                rxsz = conn["readahead"]["io_pktlen"] if conn["readahead"] else 0
                ofosz = tcp_ofoseg_bufsize(conn)
                laddr, lport, raddr, rport = get_ip_port(conn)

                gdb.write(
                    "%-4d      %3x %3x %3d %3d %4d %3d"
                    % (idx, state, flags, ref, timer, unacked, nrtx)
                )
                gdb.write(
                    " %5d/%-5d %5d/%-5d+%-5d %15s:%-5d %15s:%-5d\n"
                    % (txsz, txbuf, rxsz, rxbuf, ofosz, laddr, lport, raddr, rport)
                )
        except gdb.error as e:
            gdb.write("Failed to get TCP stats: %s\n" % e)

    def udp_stats(self):
        try:
            gdb.write(
                "UDP Conn: %4s %11s %11s %21s %21s\n"
                % ("flg", "txbuf", "rxbuf", "local_address", "remote_address")
            )
            for idx, conn in enumerate(socket_for_each_entry("udp")):
                flags = conn["sconn"]["s_flags"]
                txbuf = utils.get_field(conn, "sndbufs", -1)
                rxbuf = utils.get_field(conn, "rcvbufs", -1)
                txsz = wrbuffer_inqueue_size(utils.get_field(conn, "write_q"), "udp")
                rxsz = conn["readahead"]["io_pktlen"] if conn["readahead"] else 0
                laddr, lport, raddr, rport = get_ip_port(conn)

                gdb.write(
                    "%-4d      %4x %5d/%-5d %5d/%-5d %15s:%-5d %15s:%-5d\n"
                    % (idx, flags, txsz, txbuf, rxsz, rxbuf, laddr, lport, raddr, rport)
                )
        except gdb.error as e:
            gdb.write("Failed to get UDP stats: %s\n" % e)

    def invoke(self, args, from_tty):
        # Parse the arguments in a simple way
        if not args or "all" in args:
            args = "iob pkt tcp udp"

        # Call the corresponding function
        if utils.get_symbol_value("CONFIG_MM_IOB") and "iob" in args:
            self.iob_stats()
            gdb.write("\n")
        if utils.get_symbol_value("CONFIG_NET_STATISTICS") and "pkt" in args:
            self.pkt_stats()
            gdb.write("\n")
        if utils.get_symbol_value("CONFIG_NET_TCP") and "tcp" in args:
            self.tcp_stats()
            gdb.write("\n")
        if utils.get_symbol_value("CONFIG_NET_UDP") and "udp" in args:
            self.udp_stats()
            gdb.write("\n")


if utils.get_symbol_value("CONFIG_NET"):
    NetStats()
