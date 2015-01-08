/*
 * Copyright (c) 2007 The FFmpeg Project
 *
 * This file is part of BVBase.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BV_PROTOCOL_NETWORK_H
#define BV_PROTOCOL_NETWORK_H

#include <errno.h>
#include <stdint.h>

#include "config.h"
#include "libbvutil/error.h"
#include "os_support.h"
#include "bvio.h"
#include "bvurl.h"

#if BV_HAVE_UNISTD_H
#include <unistd.h>
#endif

#if BV_HAVE_WINSOCK2_H
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT       WSAETIMEDOUT
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED    WSAECONNREFUSED
#endif
#ifndef EINPROGRESS
#define EINPROGRESS     WSAEINPROGRESS
#endif

#define getsockopt(a, b, c, d, e) getsockopt(a, b, c, (char*) d, e)
#define setsockopt(a, b, c, d, e) setsockopt(a, b, c, (const char*) d, e)

int bb_neterrno(void);
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define bb_neterrno() BVERROR(errno)
#endif /* BV_HAVE_WINSOCK2_H */

#if BV_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if BV_HAVE_POLL_H
#include <poll.h>
#endif

int bb_socket_nonblock(int socket, int enable);

extern int bb_network_inited_globally;
int bb_network_init(void);
void bb_network_close(void);

void bb_tls_init(void);
void bb_tls_deinit(void);

int bb_network_wait_fd(int fd, int write);

/**
 * This works similarly to bb_network_wait_fd, but waits up to 'timeout' microseconds
 * Uses bb_network_wait_fd in a loop
 *
 * @fd Socket descriptor
 * @write Set 1 to wait for socket able to be read, 0 to be written
 * @timeout Timeout interval, in microseconds. Actual precision is 100000 mcs, due to bb_network_wait_fd usage
 * @param int_cb Interrupt callback, is checked before each bb_network_wait_fd call
 * @return 0 if data can be read/written, BVERROR(ETIMEDOUT) if timeout expired, or negative error code
 */
int bb_network_wait_fd_timeout(int fd, int write, int64_t timeout, BVIOInterruptCB *int_cb);

int bb_inet_aton (const char * str, struct in_addr * add);

#if !BV_HAVE_STRUCT_SOCKADDR_STORAGE
struct sockaddr_storage {
#if BV_HAVE_STRUCT_SOCKADDR_SA_LEN
    uint8_t ss_len;
    uint8_t ss_family;
#else
    uint16_t ss_family;
#endif /* BV_HAVE_STRUCT_SOCKADDR_SA_LEN */
    char ss_pad1[6];
    int64_t ss_align;
    char ss_pad2[112];
};
#endif /* !BV_HAVE_STRUCT_SOCKADDR_STORAGE */

typedef union sockaddr_union {
    struct sockaddr_storage storage;
    struct sockaddr_in in;
#if BV_HAVE_STRUCT_SOCKADDR_IN6
    struct sockaddr_in6 in6;
#endif
} sockaddr_union;

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#if !BV_HAVE_STRUCT_ADDRINFO
struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    int ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;
    struct addrinfo *ai_next;
};
#endif /* !BV_HAVE_STRUCT_ADDRINFO */

/* getaddrinfo constants */
#ifndef EAI_AGAIN
#define EAI_AGAIN 2
#endif
#ifndef EAI_BADFLAGS
#define EAI_BADFLAGS 3
#endif
#ifndef EAI_FAIL
#define EAI_FAIL 4
#endif
#ifndef EAI_FAMILY
#define EAI_FAMILY 5
#endif
#ifndef EAI_MEMORY
#define EAI_MEMORY 6
#endif
#ifndef EAI_NODATA
#define EAI_NODATA 7
#endif
#ifndef EAI_NONAME
#define EAI_NONAME 8
#endif
#ifndef EAI_SERVICE
#define EAI_SERVICE 9
#endif
#ifndef EAI_SOCKTYPE
#define EAI_SOCKTYPE 10
#endif

#ifndef AI_PASSIVE
#define AI_PASSIVE 1
#endif

#ifndef AI_CANONNAME
#define AI_CANONNAME 2
#endif

#ifndef AI_NUMERICHOST
#define AI_NUMERICHOST 4
#endif

#ifndef NI_NOFQDN
#define NI_NOFQDN 1
#endif

#ifndef NI_NUMERICHOST
#define NI_NUMERICHOST 2
#endif

#ifndef NI_NAMERQD
#define NI_NAMERQD 4
#endif

#ifndef NI_NUMERICSERV
#define NI_NUMERICSERV 8
#endif

#ifndef NI_DGRAM
#define NI_DGRAM 16
#endif

#if !BV_HAVE_GETADDRINFO
int bb_getaddrinfo(const char *node, const char *service,
                   const struct addrinfo *hints, struct addrinfo **res);
void bb_freeaddrinfo(struct addrinfo *res);
int bb_getnameinfo(const struct sockaddr *sa, int salen,
                   char *host, int hostlen,
                   char *serv, int servlen, int flags);
#define getaddrinfo bb_getaddrinfo
#define freeaddrinfo bb_freeaddrinfo
#define getnameinfo bb_getnameinfo
#endif /* !BV_HAVE_GETADDRINFO */

#if !BV_HAVE_GETADDRINFO || BV_HAVE_WINSOCK2_H
const char *bb_gai_strerror(int ecode);
#undef gai_strerror
#define gai_strerror bb_gai_strerror
#endif /* !BV_HAVE_GETADDRINFO || BV_HAVE_WINSOCK2_H */

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN INET_ADDRSTRLEN
#endif

#ifndef IN_MULTICAST
#define IN_MULTICAST(a) ((((uint32_t)(a)) & 0xf0000000) == 0xe0000000)
#endif
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) (((uint8_t *) (a))[0] == 0xff)
#endif

int bb_is_multicast_address(struct sockaddr *addr);

#define POLLING_TIME 100 /// Time in milliseconds between interrupt check

/**
 * Bind to a file descriptor and poll for a connection.
 *
 * @param fd      First argument of bind().
 * @param addr    Second argument of bind().
 * @param addrlen Third argument of bind().
 * @param timeout Polling timeout in milliseconds.
 * @param h       BVURLContext providing interrupt check
 *                callback and logging context.
 * @return        A non-blocking file descriptor on success
 *                or an BVERROR on failure.
 */
int bb_listen_bind(int fd, const struct sockaddr *addr,
                   socklen_t addrlen, int timeout,
                   BVURLContext *h);

/**
 * Connect to a file descriptor and poll for result.
 *
 * @param fd       First argument of connect(),
 *                 will be set as non-blocking.
 * @param addr     Second argument of connect().
 * @param addrlen  Third argument of connect().
 * @param timeout  Polling timeout in milliseconds.
 * @param h        BVURLContext providing interrupt check
 *                 callback and logging context.
 * @param will_try_next Whether the caller will try to connect to another
 *                 address for the same host name, affecting the form of
 *                 logged errors.
 * @return         0 on success, BVERROR on failure.
 */
int bb_listen_connect(int fd, const struct sockaddr *addr,
                      socklen_t addrlen, int timeout,
                      BVURLContext *h, int will_try_next);

int bb_http_match_no_proxy(const char *no_proxy, const char *hostname);

int bb_socket(int domain, int type, int protocol);

#endif /* BV_PROTOCOL_NETWORK_H */
