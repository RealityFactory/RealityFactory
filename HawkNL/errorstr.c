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

/*
  Based on code supplied by Ryan Haksi <cryogen@infoserve.net>
*/

#if defined WIN32 || defined WIN64
#include <errno.h>
#include "wsock.h"

#elif macintosh
/* POSIX compat Mac systems ie pre OSX with GUSI2 installed */
#include <unistd.h>
#include <types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#else
/* POSIX systems */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netdb.h>
#include <string.h>
#endif

#include "nlinternal.h"


NL_EXP const /*@observer@*/  NLbyte* NL_APIENTRY nlGetSystemErrorStr(NLint err)
{
    NLbyte *lpszRetStr;
    
    switch(err) {
#if defined WIN32 || defined WIN64
    case WSABASEERR:
        lpszRetStr="No error, you should never see this.";
        break;
    case WSAVERNOTSUPPORTED:
        lpszRetStr="Version of WinSock not supported.";
        break;
    case WSASYSNOTREADY:
        lpszRetStr="WinSock not present or not responding.";
        break;
    case WSANOTINITIALISED:
        lpszRetStr="WSA Startup not initialized.";
        break;
#endif
        /* ELOOK is used on some Macs */
#ifdef ELOOK
    case ELOOK:
        lpszRetStr="Internal mapping for kOTLookErr, don't return to client.";
        break;
#endif
        /* EPROCLIM not defined in some UNIXs */
#ifdef EPROCLIM
    case EPROCLIM:
        lpszRetStr="Too many processes.";
        break;
#endif
        /* On some UNIXs, EINTR and NO_DATA have the same value */
#if (EINTR != NO_DATA)
    case EINTR:
        lpszRetStr="Interrupted function call.";
        break;
#if defined(NO_DATA)       
    case NO_DATA:
        lpszRetStr="Valid name, no data record for type.";
        break;
#endif        
#else
    case NO_DATA:
        lpszRetStr="Interrupted function call or no data record for type.";
        break;
#endif
    case EBADF:
        lpszRetStr="Bad file descriptor.";
        break;
    case EFAULT:
        lpszRetStr="The namelen argument is incorrect.";
        break;
    case EMFILE:
        lpszRetStr="Too many open files.";
        break;
    case EINVAL:
        lpszRetStr="App version not supported by DLL.";
        break;
#if defined(TRY_AGAIN)        
    case TRY_AGAIN:
        lpszRetStr="Non-authoritive: host not found or server failure.";
        break;
#endif        
#if defined(NO_RECOVERY)
    case NO_RECOVERY:
        lpszRetStr="Non-recoverable: refused or not implemented.";
        break;
#endif
#if defined(HOST_NOT_FOUND)        
    case HOST_NOT_FOUND:
        lpszRetStr="Authoritive: Host not found.";
        break;
#endif        
    case EACCES:
        lpszRetStr="Permission to access socket denied.";
        break;
    case ENETDOWN:
        lpszRetStr="Network subsystem failed.";
        break;
    case EAFNOSUPPORT:
        lpszRetStr="Address family not supported.";
        break;
    case ENOBUFS:
        lpszRetStr="No buffer space available.";
        break;
    case EPROTONOSUPPORT:
        lpszRetStr="Specified protocol not supported.";
        break;
    case EPROTOTYPE:
        lpszRetStr="Protocol wrong type for this socket.";
        break;
    case ESOCKTNOSUPPORT:
        lpszRetStr="Socket type not supported for address family.";
        break;
    case ENOTSOCK:
        lpszRetStr="Descriptor is not a socket.";
        break;
    case EWOULDBLOCK:
        lpszRetStr="Socket marked as non-blocking and SO_LINGER set not 0.";
        break;
    case EADDRINUSE:
        lpszRetStr="Address already in use.";
        break;
    case ECONNABORTED:
        lpszRetStr="Connection aborted.";
        break;
    case ECONNRESET:
        lpszRetStr="Connection reset.";
        break;
    case ENOTCONN:
        lpszRetStr="Not connected.";
        break;
    case ETIMEDOUT:
        lpszRetStr="Connection timed out.";
        break;
    case ECONNREFUSED:
        lpszRetStr="Connection was refused.";
        break;
    case EHOSTDOWN:
        lpszRetStr="Host is down.";
        break;
    case ENETUNREACH:
        lpszRetStr="Network unreachable.";
        break;
    case EHOSTUNREACH:
        lpszRetStr="Host unreachable.";
        break;
    case EADDRNOTAVAIL:
        lpszRetStr="Address not available.";
        break;
    case EINPROGRESS:
#if defined WIN32 || defined WIN64
        lpszRetStr="A blocking sockets call is in progress.";
#else
        lpszRetStr="The socket is non-blocking and the connection could not be established immediately.";
#endif
        break;
    case EDESTADDRREQ:
        lpszRetStr="Destination address is required.";
        break;
    case EISCONN:
        lpszRetStr="Socket is already connected.";
        break;
    case ENETRESET:
        lpszRetStr="Connection has been broken due to the remote host resetting.";
        break;
    case EOPNOTSUPP:
        lpszRetStr="Operation not supported on socket";
        break;
    case ESHUTDOWN:
        lpszRetStr="Socket has been shut down.";
        break;
    case EMSGSIZE:
        lpszRetStr="The message was too large to fit into the specified buffer and was truncated.";
        break;
    case EALREADY:
        lpszRetStr="A non-blocking connect call is in progress on the specified socket.";
        break;
    case ENOPROTOOPT:
        lpszRetStr="Bad protocol option.";
        break;
    case EPFNOSUPPORT:
        lpszRetStr="Protocol family not supported.";
        break;
    case ETOOMANYREFS:
        lpszRetStr="Too many references; can't splice.";
        break;
    case ELOOP:
        lpszRetStr="Too many levels of symbolic links.";
        break;
    case ENAMETOOLONG:
        lpszRetStr="File name too long.";
        break;
    case ENOTEMPTY:
        lpszRetStr="Directory not empty.";
        break;
        
#if !defined(macintosh)
    case EUSERS:
        lpszRetStr="Too many users.";
        break;
    case EDQUOT:
        lpszRetStr="Disc quota exceeded.";
        break;
    case ESTALE:
        lpszRetStr="Stale NFS file handle.";
        break;
    case EREMOTE:
        lpszRetStr="Too many levels of remote in path.";
        break;
#endif        

    default:
        lpszRetStr=strerror(err);
        break;
  }
  return (const NLbyte*)lpszRetStr;
}

NL_EXP const /*@observer@*/ NLbyte* NL_APIENTRY nlGetErrorStr(NLenum err)
{
    NLbyte *retStr;
    
    switch(err) {
    case NL_NO_ERROR:
        retStr = "No HawkNL error.";
        break;
    case NL_NO_NETWORK:
        retStr = "No network was found on init.";
        break;
    case NL_OUT_OF_MEMORY:
        retStr = "Out of memory.";
        break;
    case NL_INVALID_ENUM:
        retStr = "Invalid NLenum.";
        break;
    case NL_INVALID_SOCKET:
        retStr = "Socket is not valid, or has been terminated.";
        break;
    case NL_INVALID_PORT:
        retStr = "Port could not be opened.";
        break;
    case NL_INVALID_TYPE:
        retStr = "Network type is not available.";
        break;
    case NL_SOCKET_ERROR:
        retStr = "A system error occurred, call nlGetSystemError.";
        break;
    case NL_SOCK_DISCONNECT:
        retStr = "Connection error: Close socket.";
        break;
    case NL_NOT_LISTEN:
        retStr = "Socket has not been set to listen.";
        break;
    case NL_NO_BROADCAST:
        retStr = "Could not broadcast.";
        break;
    case NL_CON_REFUSED:
        retStr = "Connection refused.";
        break;
    case NL_NO_MULTICAST:
        retStr = "Could not set multicast.";
        break;
    case NL_NO_PENDING:
        retStr = "No pending connections to accept.";
        break;
    case NL_BAD_ADDR:
        retStr = "The address or port are not valid.";
        break;
    case NL_OUT_OF_SOCKETS:
        retStr = "Out of socket objects.";
        break;
    case NL_MESSAGE_END:
        retStr = "TCP message end.";
        break;
    case NL_NULL_POINTER:
        retStr = "A NULL pointer was passed to a function.";
        break;
    case NL_INVALID_GROUP:
        retStr = "The group is not valid, or has been destroyed.";
        break;
    case NL_OUT_OF_GROUPS:
        retStr = "Out of internal group objects.";
        break;
    case NL_OUT_OF_GROUP_SOCKETS:
        retStr = "The group is full.";
        break;
    case NL_BUFFER_SIZE:
        retStr = "The buffer was too small for the packet.";
        break;
    case NL_PACKET_SIZE:
        retStr = "The packet is too large to send.";
        break;
    case NL_WRONG_TYPE:
        retStr = "The function does not support the socket type.";
        break;
    case NL_CON_PENDING:
        retStr = "A non-blocking connection is still pending.";
        break;
    case NL_SELECT_NET_ERROR:
        retStr = "A network type is already selected.";
        break;
    case NL_CON_TERM:
        retStr = "The connection has been terminated.";
        break;
    case NL_PACKET_SYNC:
        retStr = "The NL_RELIABLE_PACKET stream is out of sync.";
        break;
    case NL_TLS_ERROR:
        retStr = "Thread local storage could not be created.";
        break;

    default:
        retStr = "Undefined HawkNL error.";
        break;
    }
    return (const NLbyte*)retStr;
}

