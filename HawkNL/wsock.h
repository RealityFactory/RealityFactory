/*
  HawkNL cross platform network library
  Copyright (C) 2000-2002 Phil Frisbie, Jr. (phil@hawksoft.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.

  Or go to http://www.gnu.org/copyleft/lgpl.html
*/

#ifndef WSOCK_H
#define WSOCK_H

/* Windows systems */
#ifdef _MSC_VER
#pragma warning (disable:4201)
#pragma warning (disable:4214)
#pragma warning (disable:4115)
#pragma warning (disable:4514)
#pragma warning (disable:4127)
#endif /* _MSC_VER */

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>

#ifdef _MSC_VER
#pragma warning (default:4201)
#pragma warning (default:4214)
#pragma warning (default:4115)
#endif /* _MSC_VER */

#define ioctl ioctlsocket 

#undef  EBADF
#define EBADF                   WSAEBADF

#undef  EFAULT
#define EFAULT                  WSAEFAULT

#undef  EMFILE
#define EMFILE                  WSAEMFILE

#undef  EINTR
#define EINTR                   WSAEINTR

#undef  EINVAL
#define EINVAL                  WSAEINVAL

#undef  EACCES
#define EACCES                  WSAEACCES

#ifndef ENAMETOOLONG
#define ENAMETOOLONG            WSAENAMETOOLONG
#endif

#ifndef ENOTEMPTY
#define ENOTEMPTY               WSAENOTEMPTY
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT               WSAETIMEDOUT
#endif

#define TRY_AGAIN               WSATRY_AGAIN
#define NO_RECOVERY             WSANO_RECOVERY
#define NO_DATA                 WSANO_DATA
#define HOST_NOT_FOUND          WSAHOST_NOT_FOUND
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
#define sockerrno   WSAGetLastError()
/* get rid of some nasty LCLint messages */
#undef  FIONBIO
#define FIONBIO 0x8004667e

typedef int socklen_t;

typedef struct sockaddr_ipx
{
    short sa_family;
    char  sa_netnum[4];
    char  sa_nodenum[6];
    unsigned short sa_socket;
} SOCKADDR_IPX;

#define NSPROTO_IPX      1000
#define NSPROTO_SPX      1256
#define NSPROTO_SPXII    1257

#endif /* WSOCK_H */

