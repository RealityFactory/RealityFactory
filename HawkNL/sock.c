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
  The low level API, and some of the code, was inspired from the
  Quake source code release, courtesy of id Software. However,
  it has been heavily modified for use in HawkNL.
*/


#define FD_SETSIZE              8192

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#if defined WIN32 || defined WIN64

  #include "wsock.h"

#elif defined Macintosh

  #include <Types.h>
  #include <stdio.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <string.h>
  #include <errno.h>
  #include <sys/time.h>
  #include <LowMem.h>
  #define closesocket close
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define SOCKET int
  #define sockerrno errno

  /* define INADDR_NONE if not already */
  #ifndef INADDR_NONE
    #define INADDR_NONE ((unsigned long) -1)
  #endif

#else

  /* Unix-style systems */
  #ifdef SOLARIS
    #include <sys/filio.h> /* for FIONBIO */
  #endif

  #include <unistd.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/time.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <sys/ioctl.h>
  #define closesocket close
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define SOCKET int
  #define sockerrno errno

  /* define INADDR_NONE if not already */
  #ifndef INADDR_NONE
    #define INADDR_NONE ((unsigned long) -1)
  #endif

  /* SGI and MacOS X do not include socklen_t */
  #if defined __sgi || defined MACOSX
    typedef int socklen_t
  #endif

#endif /* WIN32*/


#include "nlinternal.h"
#include "thread.h"

#ifndef IN_MULTICAST
#define IN_MULTICAST(i) ((ntohl((unsigned long)(i)) & 0xF0000000) == (unsigned long)0xE0000000)
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN      NL_MAX_STRING_LENGTH
#endif

#define NL_CONNECT_STRING   "HawkNL request connection."
#define NL_REPLY_STRING     "HawkNL connection OK."
#define NL_HEADER_LEN       4

/*
 * Portable NLMAX() function macro
 */
#define NLMAX( a, b )             ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define NLMIN( a, b )             ( ( a ) < ( b ) ? ( a ) : ( b ) )

static volatile NLuint ouraddress, bindaddress;
static volatile int backlog = SOMAXCONN;
static volatile int multicastTTL = 1;
static volatile NLboolean keepalive = NL_FALSE;
static volatile NLboolean reuseaddress = NL_FALSE;
static volatile NLboolean nlTCPNoDelay = NL_FALSE;

typedef struct
{
    NLaddress   /*@temp@*/*address;
    NLbyte      /*@temp@*/*name;
    NLsocket    socket;
} NLaddress_ex_t;

extern SOCKET nlGroupGetFdset(NLint group, /*@out@*/ fd_set *fd);

/*
  This is a Winsock work around to be able to bind() to more than 3976 ports
*/

#if defined WIN32 || defined WIN64

static NLmutex  portlock; /* In memory of my step-father, Don Portlock,
                                       who passed away Jan 12, 2001 - Phil */

static volatile NLushort nextport = 1024;

static NLushort sock_getNextPort(void)
{
    nlMutexLock(&portlock);
    if(++nextport > 65535)
    {
        nextport = 1024;
    }
    nlMutexUnlock(&portlock);
    return nextport;
}

static NLint sock_bind(SOCKET socket, const struct sockaddr *a, int len)
{
    struct sockaddr_in  *addr = (struct sockaddr_in *)a;
    int                 ntries = 500; /* this is to prevent an infinite loop */
    NLboolean           found = NL_FALSE;

    /* check to see if the port is already specified */
    if(addr->sin_port != 0)
    {
        /* do the normal bind */
        return bind(socket, a, len);
    }

    /* let's find our own port number */
    while(ntries-- > 0)
    {
        addr->sin_port = htons(sock_getNextPort());
        if(bind(socket, (struct sockaddr *)addr, len) != SOCKET_ERROR)
        {
            found = NL_TRUE;
            break;
        }
    }
    if(found == NL_TRUE)
    {
        return 0;
    }
    /* could not find a port, restore the port number back to 0 */
    addr->sin_port = 0;
    /*  return error */
    return SOCKET_ERROR;
}

#else
#define sock_bind bind
#endif /* WIN32 */
/*
  handle some common connection errors so the app knows when a connection has been dropped
*/

static NLint sock_Error(void)
{
    switch (sockerrno) {

#if defined WIN32 || defined WIN64
    case WSABASEERR:
        return 0;
#endif

    case EWOULDBLOCK:
        return 0;

    case ENETRESET:
    case EHOSTUNREACH:
    case ECONNABORTED:
    case ECONNRESET:
    case ENETUNREACH:
    case ETIMEDOUT:
        nlSetError(NL_SOCK_DISCONNECT);
        break;

    default:
        nlSetError(NL_SOCKET_ERROR);
        break;
    }

    return SOCKET_ERROR;
}

static NLboolean sock_SetNonBlocking(SOCKET socket)
{
    int rc;
    unsigned long i = 1;

    rc = ioctl(socket, FIONBIO, &i);
    if(rc == SOCKET_ERROR)
    {
        return NL_FALSE;
    }
    return NL_TRUE;
}

static NLboolean sock_SetBroadcast(SOCKET socket)
{
    int i = 1;

    if(setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, (int)sizeof(i)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        return NL_FALSE;
    }
    return NL_TRUE;
}

static void sock_SetReuseAddr(SOCKET socket)
{
    int i = 1;

    if(reuseaddress == NL_FALSE)
    {
        return;
    }
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&i, (int)sizeof(i)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
    }
}

static void sock_SetTCPNoDelay(SOCKET socket)
{
    int i = 1;

    if(setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *)&i, (int)sizeof(i)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
    }
}

static void sock_SetKeepAlive(SOCKET socket)
{
    int i = 1;

    if(setsockopt(socket, IPPROTO_TCP, SO_KEEPALIVE, (char *)&i, (int)sizeof(i)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
    }
}

static NLboolean sock_SetMulticastTTL(SOCKET socket, NLint ttl)
{
    unsigned char   cttl;

    /* make sure we have a valid TTL */
    if(ttl > 255) ttl = 255;
    if(ttl < 1) ttl = 1;
    cttl = (unsigned char)ttl;

    /* first try setsockopt by passing a 'char', the Unix standard */
    if(setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
                    (char *)&cttl, (int)sizeof(cttl)) == SOCKET_ERROR)
    {
        /* if that failed, we might be on a Windows system
        that requires an 'int' */
        if(setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
                        (char *)&ttl, (int)sizeof(ttl)) == SOCKET_ERROR)
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_FALSE;
        }
    }
    return NL_TRUE;
}

static NLsocket sock_SetSocketOptions(NLsocket s)
{
    nl_socket_t     *sock = nlSockets[s];
    NLenum          type = sock->type;
    SOCKET          realsocket = (SOCKET)sock->realsocket;

    if(type == NL_RELIABLE || type == NL_RELIABLE_PACKETS)
    {
        sock->reliable = NL_TRUE;
        if(nlTCPNoDelay == NL_TRUE)
        {
            sock_SetTCPNoDelay(realsocket);
        }
        if(keepalive == NL_TRUE)
        {
            sock_SetKeepAlive(realsocket);
        }
        if(type == NL_RELIABLE_PACKETS)
        {
            sock->packetsync = NL_TRUE;
        }
    }
    else
    {
        sock->reliable = NL_FALSE;
    }

    sock_SetReuseAddr(realsocket);

    if(sock->blocking == NL_FALSE)
    {
        if(sock_SetNonBlocking(realsocket) == NL_FALSE)
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_INVALID;
        }
    }

    return s;
}

static NLuint sock_GetHostAddress(void)
{
    struct hostent  *local;
    char            buff[MAXHOSTNAMELEN];

    if(gethostname(buff, MAXHOSTNAMELEN) == SOCKET_ERROR)
    {
        return INADDR_NONE;
    }
    local = gethostbyname(buff);
    if(!local)
        return (NLuint)htonl(0xf7000001);
    return *(NLuint *)local->h_addr_list[0];
}

static NLushort sock_GetPort(SOCKET socket)
{
    struct sockaddr_in   addr;
    socklen_t            len;

    len = (socklen_t)sizeof(struct sockaddr_in);
    if(getsockname(socket, (struct sockaddr *) &addr, &len) == SOCKET_ERROR)
    {
        return 0;
    }

    return ntohs(addr.sin_port);
}

NLboolean sock_Init(void)
{
#if defined WIN32 || defined WIN64
    WSADATA libmibWSAdata;

    /* first try Winsock 2.0 */
    if(WSAStartup(MAKEWORD(2, 0),&libmibWSAdata) != 0)
    {
        /* Winsock 2.0 failed, so now try 1.1 */
        if(WSAStartup(MAKEWORD(1, 1),&libmibWSAdata) != 0)
        {
            return NL_FALSE;
        }
    }
    nlMutexInit(&portlock);
#endif

    ouraddress = sock_GetHostAddress();
    if(ouraddress == (NLuint)INADDR_NONE)
    {
        return NL_FALSE;
    }

    bindaddress = INADDR_ANY;
    return NL_TRUE;
}

void sock_Shutdown(void)
{
#if defined WIN32 || defined WIN64
    (void)WSACleanup();
    nlMutexDestroy(&portlock);
#endif
}

NLboolean sock_Listen(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];

    if(sock->reliable == NL_TRUE) /* TCP */
    {
        if(listen((SOCKET)sock->realsocket, backlog) == SOCKET_ERROR)
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_FALSE;
        }
    }

    sock->listen = NL_TRUE;
    return NL_TRUE;
}

static SOCKET sock_AcceptUDP(NLsocket nlsocket, /*@out@*/struct sockaddr_in *newaddr)
{
    nl_socket_t         *sock = nlSockets[nlsocket];
    struct sockaddr_in  ouraddr;
    SOCKET              newsocket;
    NLushort            localport;
    NLbyte              buffer[NL_MAX_STRING_LENGTH];
    socklen_t           len = (socklen_t)sizeof(struct sockaddr_in);
    NLint               slen = (NLint)sizeof(NL_CONNECT_STRING);
    NLbyte              reply = (NLbyte)0x00;
    NLint               count = 0;

    /* Get the packet and remote host address */
    if(recvfrom((SOCKET)sock->realsocket, buffer, (int)sizeof(buffer), 0,
            (struct sockaddr *)newaddr, &len) < (int)sizeof(NL_CONNECT_STRING))
    {
        nlSetError(NL_NO_PENDING);
        return INVALID_SOCKET;
    }
    /* Let's check for the connection string */
    buffer[slen - 1] = (NLbyte)0; /* null terminate for peace of mind */
    if(strcmp(buffer, NL_CONNECT_STRING) != 0)
    {
        nlSetError(NL_NO_PENDING);
        return INVALID_SOCKET;
    }
    /* open up a new socket on this end */
    newsocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(newsocket == INVALID_SOCKET)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }

    ouraddr.sin_family = AF_INET;
    ouraddr.sin_addr.s_addr = bindaddress;
    /* system assigned port number */
    ouraddr.sin_port = 0;

    if(sock_bind(newsocket, (struct sockaddr *)&ouraddr, len) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    /* get the new port */
    localport = sock_GetPort(newsocket);

    /* create the return message */
    writeShort(buffer, count, localport);
    writeString(buffer, count, NL_REPLY_STRING);

    /* send back the reply with our new port */
    if(sendto((SOCKET)sock->realsocket, buffer, count, 0, (struct sockaddr *)newaddr,
                        (int)sizeof(struct sockaddr_in)) < count)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    /* send back a 0 length packet from our new port, needed for firewalls */
    if(sendto(newsocket, &reply, 0, 0,
                        (struct sockaddr *)newaddr,
                        (int)sizeof(struct sockaddr_in)) < 0)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    /* connect the socket */
    if(connect(newsocket, (struct sockaddr *)newaddr,
               (int)sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }

    return newsocket;
}

NLsocket sock_AcceptConnection(NLsocket socket)
{
    nl_socket_t         *sock = nlSockets[socket];
    nl_socket_t         *newsock = NULL;
    NLsocket            newsocket = NL_INVALID;
    SOCKET              realsocket = INVALID_SOCKET;
    struct sockaddr_in  newaddr;
    socklen_t           len = (socklen_t)sizeof(newaddr);

    memset(&newaddr, 0, sizeof(newaddr));
    if(sock->listen != NL_TRUE)
    {
        nlSetError(NL_NOT_LISTEN);
        return NL_INVALID;
    }

    if(sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS) /* TCP */
    {
        SOCKET s = 101;
        
        /* !@#$% metrowerks compiler, try to get it to actually produce some code */
        s = accept((SOCKET)(sock->realsocket), (struct sockaddr *)&newaddr, &len);

        realsocket = s;
        if(realsocket == INVALID_SOCKET)
        {
            if(sockerrno == EWOULDBLOCK || errno == EAGAIN)/* yes, we need to use errno here */
            {
                nlSetError(NL_NO_PENDING);
            }
            else
            {
                nlSetError(NL_SOCKET_ERROR);
            }
            return NL_INVALID;
        }

    }
    else if(sock->type == NL_UNRELIABLE)/* UDP*/
    {
        realsocket = sock_AcceptUDP(socket, &newaddr);

        if(realsocket == INVALID_SOCKET)
        {
            /* error is already set in sock_AcceptUDP */
            return NL_INVALID;
        }
    }
    else /* broadcast or multicast */
    {
        nlSetError(NL_WRONG_TYPE);
        return NL_INVALID;
    }

    newsocket = nlGetNewSocket();
    if(newsocket == NL_INVALID)
    {
        return NL_INVALID;
    }
    newsock = nlSockets[newsocket];

    /* update the remote address */
    memcpy((char *)&newsock->address, (char *)&newaddr, sizeof(struct sockaddr_in));
    newsock->realsocket = (NLint)realsocket;
    newsock->localport = sock_GetPort(realsocket);
    newsock->remoteport = sock_GetPortFromAddr((NLaddress *)&newsock->address);

    newsock->type = sock->type;
    newsock->connected = NL_TRUE;

    return sock_SetSocketOptions(newsocket);
}

NLsocket sock_Open(NLushort port, NLenum type)
{
    nl_socket_t *newsock;
    NLsocket    newsocket;
    SOCKET      realsocket;

    switch (type) {

    case NL_RELIABLE: /* TCP */
    case NL_RELIABLE_PACKETS: /* TCP packets */
        realsocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

    case NL_UNRELIABLE: /* UDP */
    case NL_BROADCAST:  /* UDP broadcast */
    case NL_MULTICAST:  /* UDP multicast */
        realsocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_INVALID;
    }

    if(realsocket == INVALID_SOCKET)
    {
        nlSetError(NL_SOCKET_ERROR);
        return NL_INVALID;
    }

    newsocket = nlGetNewSocket();
    if(newsocket == NL_INVALID)
    {
        return NL_INVALID;
    }
    newsock = nlSockets[newsocket];
    newsock->realsocket = (NLint)realsocket;
    newsock->type = type;

    if(sock_SetSocketOptions(newsocket) == NL_INVALID)
    {
        (void)sock_Close(newsocket);
        return NL_INVALID;
    }
    /* do not bind a TCP socket here if the port is 0; let connect assign the port */
    if((type == NL_RELIABLE || type == NL_RELIABLE_PACKETS) && port == 0)
    {
        newsock->localport = 0;
    }
    else
    {
        ((struct sockaddr_in *)&newsock->address)->sin_family = AF_INET;
        ((struct sockaddr_in *)&newsock->address)->sin_addr.s_addr = bindaddress;
        ((struct sockaddr_in *)&newsock->address)->sin_port = htons((unsigned short)port);

        if(sock_bind(realsocket, (struct sockaddr *)&newsock->address, (int)sizeof(struct sockaddr)) == SOCKET_ERROR)
        {
            nlSetError(NL_SOCKET_ERROR);
            (void)sock_Close(newsocket);
            return NL_INVALID;
        }
        if(type == NL_BROADCAST)
        {
            if(sock_SetBroadcast(realsocket) == NL_FALSE)
            {
                nlSetError(NL_SOCKET_ERROR);
                (void)sock_Close(newsocket);
                return NL_INVALID;
            }
        }
        newsock->localport = sock_GetPort(realsocket);
    }
    return newsocket;
}

static NLboolean sock_ConnectUDP(NLsocket socket, NLaddress *address)
{
    nl_socket_t     *sock = nlSockets[socket];
    time_t          begin, t;

    if(sendto((SOCKET)sock->realsocket, (char *)NL_CONNECT_STRING, (NLint)sizeof(NL_CONNECT_STRING),
                    0, (struct sockaddr *)address, (int)sizeof(struct sockaddr_in))
                    == SOCKET_ERROR)
    {
        if(sock->blocking == NL_TRUE)
        {
            nlSetError(NL_SOCKET_ERROR);
        }
        else
        {
            sock->conerror = NL_TRUE;
        }
        return NL_FALSE;
    }

    (void)time(&begin);

    /* try for six seconds */
    while((time(&t) - begin) < 6)
    {
        NLbyte              buffer[NL_MAX_STRING_LENGTH];
        NLbyte              *pbuffer = buffer;
        NLushort            newport;
        NLint               slen = (NLint)(sizeof(NL_REPLY_STRING) + sizeof(newport));
        NLint               received;
        NLbyte              reply = (NLbyte)0;
        socklen_t           len = (socklen_t)sizeof(struct sockaddr_in);

        received = recvfrom((SOCKET)sock->realsocket, (char *)buffer, (int)sizeof(buffer), 0,
                            (struct sockaddr *)&sock->address, &len);

        if(received == SOCKET_ERROR)
        {
            if(sockerrno != EWOULDBLOCK)
            {
                if(sock->blocking == NL_TRUE)
                {
                    nlSetError(NL_CON_REFUSED);
                }
                else
                {
                    sock->conerror = NL_TRUE;
                }
                return NL_FALSE;
            }
        }
        if(received >= slen)
        {
            NLint count = 0;

            /* retrieve the port number */
            readShort(buffer, count, newport);
            ((struct sockaddr_in *)&sock->address)->sin_port = htons(newport);
            /* Lets check for the reply string */
            pbuffer[slen - 1] = (NLbyte)0; /* null terminate for peace of mind */
            pbuffer += sizeof(newport);
            if(strcmp(pbuffer, NL_REPLY_STRING) == 0)
            {
                if(connect((SOCKET)sock->realsocket, (struct sockaddr *)&sock->address,
                            (int)sizeof(struct sockaddr_in)) == SOCKET_ERROR)
                {
                    if(sock->blocking == NL_TRUE)
                    {
                        nlSetError(NL_SOCKET_ERROR);
                    }
                    else
                    {
                        sock->conerror = NL_TRUE;
                    }
                    return NL_FALSE;
                }
                /* send back a 0 length packet to the new port, needed for firewalls */
                if(send((SOCKET)sock->realsocket, &reply, 0, 0) == SOCKET_ERROR)
                {
                    if(sock->blocking == NL_TRUE)
                    {
                        nlSetError(NL_SOCKET_ERROR);
                    }
                    else
                    {
                        sock->conerror = NL_TRUE;
                    }
                    return NL_FALSE;
                }
                /* success! */
                sock->localport = sock_GetPort((SOCKET)sock->realsocket);
                sock->remoteport = sock_GetPortFromAddr((NLaddress *)&sock->address);
                sock->connected = NL_TRUE;
                sock->connecting = NL_FALSE;
                return NL_TRUE;
            }
        }
        nlThreadYield();
    }

    if(sock->blocking == NL_TRUE)
    {
        nlSetError(NL_CON_REFUSED);
    }
    else
    {
        sock->conerror = NL_TRUE;
    }
    return NL_FALSE;
}

static void *sock_ConnectUDPAsynchInt(void /*@owned@*/*addr)
{
    NLaddress_ex_t *address = (NLaddress_ex_t *)addr;

    (void)sock_ConnectUDP(address->socket, address->address);
    free(addr);
    return NULL;
}

static NLboolean sock_ConnectUDPAsynch(NLsocket socket, NLaddress *address)
{
    NLaddress_ex_t  /*@dependent@*/*addr;
    nl_socket_t     *sock = nlSockets[socket];

    addr = (NLaddress_ex_t *)malloc(sizeof(NLaddress_ex_t));
    if(addr == NULL)
    {
        nlSetError(NL_OUT_OF_MEMORY);
        return NL_FALSE;
    }
    addr->address = address;
    addr->socket = socket;
    sock->connecting = NL_TRUE;
    sock->conerror = NL_FALSE;
    nlThreadCreate(sock_ConnectUDPAsynchInt, (void *)addr);
    return NL_TRUE;
}

static NLboolean sock_ConnectMulticast(NLsocket socket, NLaddress *address)
{
    struct ip_mreq  mreq;
    nl_socket_t     *sock = nlSockets[socket];

    if(sock->reliable == NL_TRUE)
    {
        nlSetError(NL_NO_MULTICAST);
        return NL_FALSE;
    }
    if(!IN_MULTICAST(((struct sockaddr_in *)address)->sin_addr.s_addr))
    {
        nlSetError(NL_BAD_ADDR);
        return NL_FALSE;
    }

    memcpy((char *)&sock->address, (char *)address, sizeof(struct sockaddr_in));
    memcpy((char *)&sock->multicastaddr, (char *)address, sizeof(struct sockaddr_in));

    /* join the multicast group */
    mreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)address)->sin_addr.s_addr;
    mreq.imr_interface.s_addr = bindaddress;

    if(setsockopt((SOCKET)sock->realsocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                    (char *)&mreq, (int)sizeof(mreq)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        return NL_FALSE;
    }
    sock->localport = sock_GetPort((SOCKET)sock->realsocket);
    sock->remoteport = sock_GetPortFromAddr((NLaddress *)&sock->address);

    return sock_SetMulticastTTL((SOCKET)sock->realsocket, multicastTTL);
}

NLboolean sock_Connect(NLsocket socket, NLaddress *address)
{
    nl_socket_t *sock = nlSockets[socket];

    memcpy((char *)&sock->address, (char *)address, sizeof(NLaddress));

    if(sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS)
    {

        if(connect((SOCKET)sock->realsocket, (struct sockaddr *)&sock->address,
                    (int)sizeof(struct sockaddr_in)) == SOCKET_ERROR)
        {
            if(sock->blocking == NL_FALSE &&
                (sockerrno == EWOULDBLOCK || sockerrno == EINPROGRESS))
            {
                sock->connecting = NL_TRUE;
            }
            else
            {
                nlSetError(NL_SOCKET_ERROR);
                return NL_FALSE;
            }
        }
        sock->localport = sock_GetPort((SOCKET)sock->realsocket);
        sock->remoteport = sock_GetPortFromAddr((NLaddress *)&sock->address);
        sock->connected = NL_TRUE;
        return NL_TRUE;
    }
    else if(sock->type == NL_MULTICAST)
    {
        return sock_ConnectMulticast(socket, address);
    }
    else if(sock->type == NL_UNRELIABLE)
    {
        if(sock->blocking == NL_TRUE)
        {
            return sock_ConnectUDP(socket, address);
        }
        else
        {
            return sock_ConnectUDPAsynch(socket, address);
        }
    }
    else
    {
        nlSetError(NL_WRONG_TYPE);
    }
    return NL_FALSE;
}

void sock_Close(NLsocket socket)
{
    nl_socket_t     *sock = nlSockets[socket];
    struct ip_mreq  mreq;

    if(sock->type == NL_MULTICAST)
    {
        /* leave the multicast group */
        mreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)&sock->address)->sin_addr.s_addr;
        mreq.imr_interface.s_addr = bindaddress;

        (void)setsockopt((SOCKET)sock->realsocket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                            (char *)&mreq, (int)sizeof(mreq));
    }
    (void)closesocket((SOCKET)sock->realsocket);
    /* free for reuse */
    nlFreeSocket(socket);
}

/* internal function to read reliable packets from TCP stream */
static NLint sock_ReadPacket(NLsocket socket, NLvoid /*@out@*/ *buffer, NLint nbytes,
                             NLboolean checkonly)
{
    nl_socket_t *sock = nlSockets[socket];
    
    /* skip error reporting if checkonly is TRUE */
    /* check for sync */
    if(sock->packetsync == NL_FALSE)
    {
        if(checkonly == NL_FALSE)
        {
            nlSetError(NL_PACKET_SYNC);
        }
        else
        {
            sock->readable = NL_TRUE;
        }
        return NL_INVALID;
    }
    
    if(sock->message_end == NL_TRUE && checkonly == NL_FALSE && sock->reclen == 0)
    {
        sock->message_end = NL_FALSE;
        nlSetError(NL_MESSAGE_END);
        return NL_INVALID;
    }
    /* allocate some temp storage the first time through */
    if(sock->inbuf == NULL)
    {
        sock->inbuflen = (nbytes + nbytes / 4 + NL_HEADER_LEN);

        if(sock->inbuflen < 1024)
        {
            sock->inbuflen = 1024;
        }
        else if(sock->inbuflen > NL_MAX_PACKET_LENGTH + NL_MAX_PACKET_LENGTH/4)
        {
            sock->inbuflen = NL_MAX_PACKET_LENGTH + NL_MAX_PACKET_LENGTH/4;
        }
        sock->inbuf = (NLbyte *)malloc((size_t)sock->inbuflen);
        if(sock->inbuf == NULL)
        {
            if(checkonly == NL_FALSE)
            {
                nlSetError(NL_OUT_OF_MEMORY);
            }
            return NL_INVALID;
        }
    }
    /* if inbuf is empty, get some data */
    if(sock->reclen == 0)
    {
        NLint count;

        count = recv((SOCKET)sock->realsocket, (char *)sock->inbuf, sock->inbuflen, 0);
        if(count == SOCKET_ERROR)
        {
            if(checkonly == NL_FALSE)
            {
                return sock_Error();
            }
            else
            {
                sock->readable = NL_TRUE;
                return NL_INVALID;
            }
        }
        if(count == 0)
        {
            if(checkonly == NL_FALSE)
            {
                nlSetError(NL_MESSAGE_END);
            }
            else
            {
                sock->message_end = NL_TRUE;
                sock->readable = NL_TRUE;
            }
            return NL_INVALID;
        }
        sock->reclen = count;
    }
    /* start parsing the packet */
    if(sock->reclen >= NL_HEADER_LEN)
    {
        NLboolean   done = NL_FALSE;
        NLushort    len;
        NLint       c = 2;

        /* check for valid packet */
        if(sock->inbuf[0] != 'N' || sock->inbuf[1] != 'L')
        {
            /* packet is not valid, we are somehow out of sync,
               or we are talking to a regular TCP stream */
            if(checkonly == NL_FALSE)
            {
                nlSetError(NL_PACKET_SYNC);
            }
            else
            {
                sock->readable = NL_TRUE;
            }
            sock->packetsync = NL_FALSE;
            return NL_INVALID;
        }

        /* read the length of the packet */
        readShort(sock->inbuf, c, len);
        if(len > NL_MAX_PACKET_LENGTH)
        {
            /* packet is not valid, or we are talking to a regular TCP stream */
            if(checkonly == NL_FALSE)
            {
                nlSetError(NL_PACKET_SYNC);
            }
            else
            {
                sock->readable = NL_TRUE;
            }
            sock->packetsync = NL_FALSE;
            return NL_INVALID;
        }
        /* check to see if we need to make the inbuf storage larger */
        if(len > sock->inbuflen)
        {
            sock->inbuflen = (len + len / 4 + NL_HEADER_LEN);
            sock->inbuf = (NLbyte *)realloc(sock->inbuf, (size_t)sock->inbuflen);
            if(sock->inbuf == NULL)
            {
                if(checkonly == NL_FALSE)
                {
                    nlSetError(NL_OUT_OF_MEMORY);
                }
                return NL_INVALID;
            }
        }
        if(checkonly == NL_FALSE)
        {
            if(len > (NLushort)nbytes)
            {
                nlSetError(NL_BUFFER_SIZE);
                return NL_INVALID;
            }
        }
        /* see if we need to get more of the packet */
        if(len > (NLushort)(sock->reclen - c))
        {
            done = NL_FALSE;
            while(done == NL_FALSE)
            {
                NLint count;

                if(checkonly == NL_FALSE)
                {
                    count = recv((SOCKET)sock->realsocket,
                        (char *)(sock->inbuf + sock->reclen),
                        (sock->inbuflen - sock->reclen), 0);
                }
                else
                {
                    /* we are calling this from PollGroup, so it cannot block */
                    fd_set          fdset;
                    struct timeval  t = {0,0};

                    FD_ZERO(&fdset);
                    FD_SET((SOCKET)sock->realsocket, &fdset);
                    if(select(sock->realsocket + 1, &fdset, NULL, NULL, &t) == 1)
                    {
                        count = recv((SOCKET)sock->realsocket,
                            (char *)(sock->inbuf + sock->reclen),
                            (sock->inbuflen - sock->reclen), 0);
                    }
                    else
                    {
                        /* socket would block, so break */
                        break;
                    }
                }
                if(count == SOCKET_ERROR)
                {
                    if(checkonly == NL_FALSE)
                    {
                        /* check to see if we already have all the packet */
                        if(len <= (NLushort)(sock->reclen - c))
                        {
                            done = NL_TRUE;
                        }
                        else
                        {
                            /* report the error */
                            return sock_Error();
                        }
                    }
                    else
                    {
                        done = NL_TRUE;
                    }
                }
                else if(count == 0)
                {
                    sock->message_end = NL_TRUE;
                    sock->readable = NL_TRUE;
                    done = NL_TRUE;
                }
                else
                {
                    sock->reclen += count;
                    if(len <= (NLushort)(sock->reclen - c))
                    {
                        done = NL_TRUE;
                    }
                }
            }
        }
        /* see if we now have all of the packet */
        if(len <= (NLushort)(sock->reclen - c))
        {
            sock->readable = NL_TRUE;
            
            if(checkonly == NL_FALSE)
            {
                /* copy the packet */
                memcpy(buffer, (sock->inbuf + c), (size_t)len);
                
                /* check for another packet */
                sock->reclen -= (len + c);
                if(sock->reclen > 0)
                {
                    /* move it down to the beginning of inbuf */
                    memmove(sock->inbuf, (sock->inbuf + c + len), (size_t)sock->reclen);
                }
                /* quick check to see if we have another complete packet buffered */
                if(sock->reclen >= NL_HEADER_LEN)
                {
                    /* read the length of the packet */
                    c = 2;
                    readShort(sock->inbuf, c, len);
                    
                    /* check the length */
                    if(len <= (NLushort)(sock->reclen - c))
                    {
                        /* we have another complete packet, so mark as readable for PollGroup */
                        sock->readable = NL_TRUE;
                    }
                    else
                    {
                        sock->readable = NL_FALSE;
                    }
                }
                else
                {
                    sock->readable = NL_FALSE;
                }
                return (NLint)len;
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

NLint sock_Read(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       count;

    if(nbytes < 0)
    {
        return 0;
    }
    if(sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS) /* TCP */
    {
        /* check for a non-blocking connection pending */
        if(sock->connecting == NL_TRUE)
        {
            fd_set          fdset;
            struct timeval  t = {0,0};

            FD_ZERO(&fdset);
            FD_SET((SOCKET)sock->realsocket, &fdset);
            if(select(sock->realsocket + 1, NULL, &fdset, NULL, &t) == 1)
            {
                /* the connect has completed */
                sock->connected = NL_TRUE;
                sock->connecting = NL_FALSE;
            }
            else
            {
                /* check for a failed connect */
                FD_ZERO(&fdset);
                FD_SET((SOCKET)sock->realsocket, &fdset);
                if(select(sock->realsocket + 1, NULL, NULL, &fdset, &t) == 1)
                {
                    nlSetError(NL_CON_REFUSED);
                }
                else
                {
                    nlSetError(NL_CON_PENDING);
                }
                return NL_INVALID;
            }
        }
        /* check for reliable packets */
        if(sock->type == NL_RELIABLE_PACKETS)
        {
            return sock_ReadPacket(socket, buffer, nbytes, NL_FALSE);
        }
        count = recv((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
        if(count == 0)
        {
            /* end of message */
            nlSetError(NL_MESSAGE_END);
            return NL_INVALID;
        }
    }
    else /* UDP */
    {
        /* check for a non-blocking connection pending */
        if(sock->connecting == NL_TRUE)
        {
            nlSetError(NL_CON_PENDING);
            return NL_INVALID;
        }
        /* check for a connection error */
        if(sock->conerror == NL_TRUE)
        {
            nlSetError(NL_CON_REFUSED);
            return NL_INVALID;
        }
        if(sock->connected == NL_TRUE)
        {
            count = recv((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
        }
        else
        {
            socklen_t   len = (socklen_t)sizeof(struct sockaddr_in);

            count = recvfrom((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0,
                            (struct sockaddr *)&sock->address, &len);
       }
    }
    if(count == SOCKET_ERROR)
    {
        return sock_Error();
    }
    return count;
}

static void sock_WritePacketCheckPending(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];

    /* first check for data waiting to be sent */
    if(sock->sendlen > 0)
    {
        NLint       count;

        count = send((SOCKET)sock->realsocket, (char *)sock->outbuf, sock->sendlen, 0);
        if(count > 0)
        {
            sock->sendlen -= count;
            if(sock->sendlen > 0)
            {
                /* move remaining data to beginning of outbuf */
                memmove(sock->outbuf, (sock->outbuf + count), (size_t)sock->sendlen);
            }
        }
    }
}

static NLint sock_WritePacket(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       count;
    NLbyte      temp[NL_HEADER_LEN];
    NLint       c = 0;

    /* send any unsent data from last packet */
    sock_WritePacketCheckPending(socket);

    /* allocate memory for outbuf */
    if(sock->outbuf == NULL)
    {
        NLint size = NLMAX((nbytes + NL_HEADER_LEN) * 2, 1024);

        sock->outbuf = (NLbyte *)malloc((size_t)size);
        if(sock->outbuf == NULL)
        {
            sock->outbuflen = 0;
            nlSetError(NL_OUT_OF_MEMORY);
            return NL_INVALID;
        }
        sock->outbuflen = size;
    }

    /* ID for packets is 'NL'*/
    writeByte(temp, c, 'N');
    writeByte(temp, c, 'L');
    /* add the packet length */
    writeShort(temp, c, (NLushort)nbytes);
    count = send((SOCKET)sock->realsocket, (char *)temp, c, 0);
    if(count == SOCKET_ERROR)
    {
        return sock_Error();
    }
    if(count < c)
    {
        int dif = c - count;

        /* reallocate if outbuf is too small */
        if(dif > (sock->outbuflen - sock->sendlen))
        {
            NLint size = NLMAX((dif + nbytes) * 2, 1024);

            size += sock->outbuflen;
            /* should we have a check for excesive memory useage here? */

            sock->outbuf = (NLbyte *)realloc(sock->outbuf, (size_t)size);
            if(sock->outbuf == NULL)
            {
                sock->outbuflen = 0;
                nlSetError(NL_OUT_OF_MEMORY);
                return NL_INVALID;
            }
            sock->outbuflen = size;
        }
        /* store it */
        memcpy((sock->outbuf + sock->sendlen), (temp + count), (size_t)(c - count));
        sock->sendlen += (c - count);
        memcpy((sock->outbuf + sock->sendlen), ((NLbyte *)buffer), (size_t)(nbytes));
        sock->sendlen += (nbytes);
    }
    else
    {
        count = send((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
        if(count == SOCKET_ERROR)
        {
            return sock_Error();
        }
        /* make sure all was sent */
        if(count < nbytes)
        {
            int dif = nbytes - count;
            
            /* reallocate if outbuf is too small */
            if(dif > (sock->outbuflen - sock->sendlen))
            {
                NLint size = NLMAX(dif * 2, 1024);
                
                size += sock->outbuflen;
                /* should we have a check for excesive memory useage here? */

                sock->outbuf = (NLbyte *)realloc(sock->outbuf, (size_t)size);
                if(sock->outbuf == NULL)
                {
                    sock->outbuflen = 0;
                    nlSetError(NL_OUT_OF_MEMORY);
                    return NL_INVALID;
                }
                sock->outbuflen = size;
            }
            /* store it */
            memcpy((sock->outbuf + sock->sendlen), ((NLbyte *)buffer + count), (size_t)(nbytes - count));
            sock->sendlen += (nbytes - count);
        }
    }
    count = nbytes;
    return count;
}
NLint sock_Write(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       count;

    if(nbytes < 0)
    {
        return 0;
    }
    if((sock->type == NL_RELIABLE) || (sock->type == NL_RELIABLE_PACKETS)) /* TCP */
    {
        if(sock->connecting == NL_TRUE)
        {
            fd_set          fdset;
            struct timeval  t = {0,0};

            FD_ZERO(&fdset);
            FD_SET((SOCKET)sock->realsocket, &fdset);
            if(select(sock->realsocket + 1, NULL, &fdset, NULL, &t) == 1)
            {
                /* the connect has completed */
                sock->connected = NL_TRUE;
                sock->connecting = NL_FALSE;
            }
            else
            {
                /* check for a failed connect */
                FD_ZERO(&fdset);
                FD_SET((SOCKET)sock->realsocket, &fdset);
                if(select(sock->realsocket + 1, NULL, NULL, &fdset, &t) == 1)
                {
                    nlSetError(NL_CON_REFUSED);
                }
                else
                {
                    nlSetError(NL_CON_PENDING);
                }
                return NL_INVALID;
            }
        }
        /* check for reliable packets */
        if(sock->type == NL_RELIABLE_PACKETS)
        {
            if(nbytes > NL_MAX_PACKET_LENGTH)
            {
                nlSetError(NL_PACKET_SIZE);
                return NL_INVALID;
            }
            return sock_WritePacket(socket, buffer, nbytes);
        }
        count = send((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
    }
    else /* unconnected UDP */
    {
        /* check for a non-blocking connection pending */
        if(sock->connecting == NL_TRUE)
        {
            nlSetError(NL_CON_PENDING);
            return NL_INVALID;
        }
        /* check for a connection error */
        if(sock->conerror == NL_TRUE)
        {
            nlSetError(NL_CON_REFUSED);
            return NL_INVALID;
        }
        if(nbytes > NL_MAX_PACKET_LENGTH)
        {
            nlSetError(NL_PACKET_SIZE);
            return NL_INVALID;
        }
        if(sock->type == NL_BROADCAST)
        {
            ((struct sockaddr_in *)&sock->address)->sin_addr.s_addr = INADDR_BROADCAST;
        }
        if(sock->type == NL_MULTICAST)
        {
            count = sendto((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0,
                            (struct sockaddr *)&sock->multicastaddr,
                            (int)sizeof(struct sockaddr_in));
        }
        else if(sock->connected == NL_TRUE)
        {
            count = send((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
        }
        else
        {
            count = sendto((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0,
                            (struct sockaddr *)&sock->address,
                            (int)sizeof(struct sockaddr_in));
        }
    }
    if(count == SOCKET_ERROR)
    {
        return sock_Error();
    }
    return count;
}

NLbyte *sock_AddrToString(NLaddress *address, NLbyte *string)
{
    NLulong     addr;
    NLushort    port;

    addr = ntohl(((struct sockaddr_in *)address)->sin_addr.s_addr);
    port = ntohs(((struct sockaddr_in *)address)->sin_port);
    if(port == 0)
    {
        sprintf(string, "%lu.%lu.%lu.%lu", (addr >> 24) & 0xff, (addr >> 16)
                & 0xff, (addr >> 8) & 0xff, addr & 0xff);
    }
    else
    {
        sprintf(string, "%lu.%lu.%lu.%lu:%u", (addr >> 24) & 0xff, (addr >> 16)
                & 0xff, (addr >> 8) & 0xff, addr & 0xff, port);
    }
    return string;
}

void sock_StringToAddr(NLbyte *string, NLaddress *address)
{
    NLulong     a1, a2, a3, a4;
    NLushort    port = 0;
    NLulong     ipaddress;
    int         ret;

    ret = sscanf((const char *)string, "%lu.%lu.%lu.%lu:%hu", &a1, &a2, &a3, &a4, &port);

    if(a1 > 255 || a2 > 255 || a3 > 255 || a4 > 255 || port > 65535 || ret < 4)
    {
        /* bad address */
        address->sa_family = AF_INET;
        ((struct sockaddr_in *)address)->sin_addr.s_addr = INADDR_NONE;
        ((struct sockaddr_in *)address)->sin_port = 0;
        nlSetError(NL_BAD_ADDR);
	    address->valid = NL_FALSE;
    }
    else
    {
        ipaddress = (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
        address->sa_family = AF_INET;
        ((struct sockaddr_in *)address)->sin_addr.s_addr = htonl(ipaddress);
        ((struct sockaddr_in *)address)->sin_port = htons(port);
	    address->valid = NL_TRUE;
    }
}

void sock_GetLocalAddr(NLint socket, NLaddress *address)
{
    nl_socket_t *sock = nlSockets[socket];
    socklen_t   len;

    memset(address, 0, sizeof(NLaddress));
    address->sa_family = AF_INET;
    address->valid = NL_TRUE;
    len = (socklen_t)sizeof(struct sockaddr_in);
    /* if the socket is connected, this will get us
    the correct address on a multihomed system*/
    if(getsockname((SOCKET)sock->realsocket, (struct sockaddr *)address, &len) == SOCKET_ERROR)
    {
        /* ignore error if socket has not been bound or connected yet */
        if(sockerrno != EINVAL)
        {
            nlSetError(NL_SOCKET_ERROR);
			address->valid = NL_FALSE;
			return;
        }
    }

    /* if not connected, substitute the NIC address */
    if(((struct sockaddr_in *)address)->sin_addr.s_addr == INADDR_ANY)
    {
        ((struct sockaddr_in *)address)->sin_addr.s_addr = ouraddress;
    }
	sock_SetAddrPort(address, sock->localport);
}

void sock_SetLocalAddr(NLaddress *address)
{
    bindaddress = ouraddress = (NLuint)((struct sockaddr_in *)address)->sin_addr.s_addr;
}

NLbyte *sock_GetNameFromAddr(NLaddress *address, NLbyte *name)
{
    struct hostent *hostentry;

    hostentry = gethostbyaddr((char *)&((struct sockaddr_in *)address)->sin_addr,
                                (int)sizeof(struct in_addr), AF_INET);
    if(hostentry != NULL)
    {
		NLushort port = sock_GetPortFromAddr(address);

		if(port != 0)
		{
			sprintf(name, "%s:%hu", (const char *)hostentry->h_name, port);
		}
		else
		{
			strncpy((char *)name, (const char *)hostentry->h_name, (size_t)(MAXHOSTNAMELEN - 1));
		}
    }
    else
    {
        if(((struct sockaddr_in *)address)->sin_addr.s_addr == (unsigned long)INADDR_NONE)
        {
            strcpy(name, "Bad address");
        }
        else
        {
            (void)sock_AddrToString(address, name);
        }
    }
    return name;
}

static void *sock_GetNameFromAddrAsyncInt(void /*@owned@*/ * addr)
{
    NLaddress_ex_t *address = (NLaddress_ex_t *)addr;

    (void)sock_GetNameFromAddr(address->address, address->name);
    free(addr);
    return NULL;
}

void sock_GetNameFromAddrAsync(NLaddress *address, NLbyte *name)
{
    NLaddress_ex_t  *addr;

    memset(name, 0, MAXHOSTNAMELEN);
    addr = (NLaddress_ex_t *)malloc(sizeof(NLaddress_ex_t));
    if(addr == NULL)
    {
        nlSetError(NL_OUT_OF_MEMORY);
        return;
    }
    addr->address = address;
    addr->name = name;
    nlThreadCreate(sock_GetNameFromAddrAsyncInt, addr);
}

void sock_GetAddrFromName(NLbyte *name, NLaddress *address)
{
    struct hostent *hostentry;
	NLushort    port = 0;
	NLbyte      *string;
	int			pos;

    address->valid = NL_FALSE;
    /* first check to see if we have a numeric address already */
    sock_StringToAddr(name, address);
    /* clear out an NL_BAD_ADDR error */
    if(nlGetError() == NL_BAD_ADDR)
    {
        nlSetError(NL_NO_ERROR);
    }
    if(((struct sockaddr_in *)address)->sin_addr.s_addr != (unsigned long)INADDR_NONE)
    {
        /* we are already done! */
        address->valid = NL_TRUE;
        return;
    }

	pos = (int)strcspn( name, ":" );
	string = &name[pos+1];
	name[pos] = (char)0;
	(void)sscanf((const char *)string, "%hu", &port);
    hostentry = gethostbyname((const char *)name);
    if(hostentry != NULL)
    {
        address->sa_family = AF_INET;
        ((struct sockaddr_in *)address)->sin_port = htons(port);
        ((struct sockaddr_in *)address)->sin_addr.s_addr = *(NLulong *)hostentry->h_addr_list[0];
		address->valid = NL_TRUE;
    }
    else
    {
        address->sa_family = AF_INET;
        ((struct sockaddr_in *)address)->sin_addr.s_addr = INADDR_NONE;
        ((struct sockaddr_in *)address)->sin_port = 0;
        nlSetError(NL_SOCKET_ERROR);
    }
    return;
}

static void *sock_GetAddrFromNameAsyncInt(void /*@owned@*/ *addr)
{
    NLaddress_ex_t *address = (NLaddress_ex_t *)addr;

    sock_GetAddrFromName(address->name, address->address);
	address->address->valid = NL_TRUE;
    free(addr);
    return NULL;
}

void sock_GetAddrFromNameAsync(NLbyte *name, NLaddress *address)
{
    NLaddress_ex_t  *addr;

    address->valid = NL_FALSE;
    addr = (NLaddress_ex_t *)malloc(sizeof(NLaddress_ex_t));
    if(addr == NULL)
    {
        nlSetError(NL_OUT_OF_MEMORY);
        return;
    }
    addr->address = address;
    addr->name = name;
    nlThreadCreate(sock_GetAddrFromNameAsyncInt, (void *)addr);
}

NLboolean sock_AddrCompare(NLaddress *address1, NLaddress *address2)
{
    if(address1->sa_family != address2->sa_family)
        return NL_FALSE;

    if(((struct sockaddr_in *)address1)->sin_addr.s_addr
        != ((struct sockaddr_in *)address2)->sin_addr.s_addr)
        return NL_FALSE;

    if(((struct sockaddr_in *)address1)->sin_port
        != ((struct sockaddr_in *)address2)->sin_port)
        return NL_FALSE;

    return NL_TRUE;
}

NLushort sock_GetPortFromAddr(NLaddress *address)
{
    return ntohs(((struct sockaddr_in *)address)->sin_port);
}

void sock_SetAddrPort(NLaddress *address, NLushort port)
{
    ((struct sockaddr_in *)address)->sin_port = htons((NLushort)port);
}

NLint sock_GetSystemError(void)
{
    int err = sockerrno;

#if defined WIN32 || defined WIN64
    if(err < WSABASEERR)
    {
        err = errno;
    }
#endif
    return err;
}

NLint sock_PollGroup(NLint group, NLenum name, NLsocket *sockets, NLint number, NLint timeout)
{
    NLint           numselect, count = 0;
    NLint           numsockets = NL_MAX_GROUP_SOCKETS;
    NLsocket        temp[NL_MAX_GROUP_SOCKETS];
    NLboolean       reliable[NL_MAX_GROUP_SOCKETS];
    NLsocket        *ptemp = temp;
    int             i, found = 0;
    fd_set          fdset;
    SOCKET          highest;
    struct timeval  t = {0,0}; /* {seconds, useconds}*/
    struct timeval  *tp = &t;

    nlGroupLock();
    highest = nlGroupGetFdset(group, &fdset);

    if(highest == INVALID_SOCKET)
    {
        /* error is set by nlGroupGetFdset */
        nlGroupUnlock();
        return NL_INVALID;
    }

    nlGroupGetSocketsINT(group, ptemp, &numsockets);
    nlGroupUnlock();

    if(numsockets < 0)
    {
        /* any error is set by nlGroupGetSockets */
        return NL_INVALID;
    }
    if(numsockets == 0)
    {
        return 0;
    }

    if(name == NL_READ_STATUS)
    {
        /* check for buffered reliable packets */
        for(i=0;i<numsockets;i++)
        {
            nl_socket_t *s = nlSockets[ptemp[i]];
            
            if(s->type == NL_RELIABLE_PACKETS && s->readable == NL_TRUE)
            {
                /* mark as readable */
                reliable[i] = NL_TRUE;
                found++;
                /* change the timeout to 0, or non-blocking since we */
                /* have at least one reliable packet to read */
                timeout = 0;
            }
            else
            {
                reliable[i] = NL_FALSE;
            }
        }
    }
    
    /* check for full blocking call */
    if(timeout < 0)
    {
        tp = NULL;
    }
    else /* set t values */
    {
        t.tv_sec = timeout/1000;
        t.tv_usec = (timeout%1000) * 1000;
    }

    /* call select to check the status */
    switch(name) {

    case NL_READ_STATUS:
        numselect = select(highest + 1, &fdset, NULL, NULL, tp);
        break;

    case NL_WRITE_STATUS:
        numselect = select(highest + 1, NULL, &fdset, NULL, tp);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_INVALID;
    }
    if(numselect == SOCKET_ERROR)
    {
        if(sockerrno == ENOTSOCK)
        {
            nlSetError(NL_INVALID_SOCKET);
        }
        else
        {
            nlSetError(NL_SOCKET_ERROR);
        }
        return NL_INVALID;
    }

    if(numselect > number)
    {
        nlSetError(NL_BUFFER_SIZE);
        return NL_INVALID;
    }
    /* fill *sockets with a list of the sockets ready to be read */
    numselect += found;
    i = 0;
    while(numsockets-- > 0 && numselect > count)
    {
        nl_socket_t *s = nlSockets[*ptemp];

        if((FD_ISSET(s->realsocket, &fdset) != 0) || (reliable[i] == NL_TRUE ))
        {
            /* must check for a complete packet */
            if(s->type == NL_RELIABLE_PACKETS && s->listen == NL_FALSE)
            {
                if(s->readable != NL_TRUE)
                {
                    (void)sock_ReadPacket(*ptemp, NULL, 0, NL_TRUE);
                }
                if(s->readable == NL_TRUE)
                {
                    /* we do have a complete packet */
                    *sockets = *ptemp;
                    sockets++;
                    count ++;
                }
            }
            else
            {
                *sockets = *ptemp;
                sockets++;
                count ++;
            }
        }
        i++;
        ptemp++;
    }
    return count;
}

void sock_Hint(NLenum name, NLint arg)
{
    switch(name) {

    case NL_LISTEN_BACKLOG:
        backlog = arg;
        break;

    case NL_KEEPALIVE:
        keepalive = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        break;

    case NL_MULTICAST_TTL:
        if(arg < 1)
        {
            arg = 1;
        }
        else if(arg > 255)
        {
            arg = 255;
        }
        multicastTTL = arg;
        break;

    case NL_REUSE_ADDRESS:
        reuseaddress = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        break;

    case NL_TCP_NO_DELAY:
        nlTCPNoDelay = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
    }
}
