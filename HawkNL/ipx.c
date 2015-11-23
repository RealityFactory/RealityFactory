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


#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#define FD_SETSIZE              8192


#if defined WIN32 || defined WIN64

#include "wsock.h"
#define sleep(x)    Sleep((DWORD)(1000 * (x)))

#ifdef _MSC_VER
#pragma warning (disable:4100) /* disable "unreferenced formal parameter" */
#endif

#else
/* Unix-style systems */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#define readsocket read
#define writesocket write
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET int
/* define INADDR_NONE if not already */
#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#define sockerrno errno
#endif

#endif

#include "nlinternal.h"

#ifdef NL_INCLUDE_IPX

extern SOCKET nlGroupGetFdset(NLint group, /*@out@*/ fd_set *fd);

#define MAXHOSTNAMELEN      256
#define NL_CONNECT_STRING   "HawkNL request connection."
#define NL_REPLY_STRING     "HawkNL connection OK."

static NLaddress *ipx_ouraddress;
static NLint ipxport = 0;
static volatile int backlog = SOMAXCONN;
static volatile NLboolean reuseaddress = NL_FALSE;

static void ipx_SetReuseAddr(SOCKET socket)
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

static NLboolean ipx_SetNonBlocking(SOCKET socket)
{
    unsigned long i = 1;
    
    if(ioctl(socket, FIONBIO, &i) == SOCKET_ERROR)
    {
        return NL_FALSE;
    }
    return NL_TRUE;
}

static NLboolean ipx_SetBlocking(SOCKET socket)
{
    unsigned long i = 0;
    
    if(ioctl(socket, FIONBIO, &i) == SOCKET_ERROR)
    {
        return NL_FALSE;
    }
    return NL_TRUE;
}

static NLboolean ipx_SetBroadcast(SOCKET socket)
{
    int i = 1;

    if(setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, (int)sizeof(i)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        return NL_FALSE;
    }
    return NL_TRUE;

}

/* is there a better way to get our address? */
static NLaddress /*@null@*/ *ipx_GetHostAddress(void)
{
    static NLaddress    address;
    NLint               addrlen = (NLint)sizeof(NLaddress);
    SOCKET              sock;

    sock = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX);

    if(sock == INVALID_SOCKET)
    {
        nlSetError(NL_NO_NETWORK);
        return NULL;
    }

    ((struct sockaddr_ipx *)&address)->sa_family = AF_IPX;
    memset(((struct sockaddr_ipx *)&address)->sa_netnum, 0, 4);
    memset(((struct sockaddr_ipx *)&address)->sa_nodenum, 0, 6);
    ((struct sockaddr_ipx *)&address)->sa_socket = 0;

    if(bind(sock, (struct sockaddr *)&address, (int)sizeof(NLaddress)) == SOCKET_ERROR)
    {
        nlSetError(NL_NO_NETWORK);
        (void)closesocket(sock);
        return NULL;
    }

    if(getsockname(sock, (struct sockaddr *)&address, &addrlen) != 0)
    {
        nlSetError(NL_NO_NETWORK);
        (void)closesocket(sock);
        return NULL;
    }

    (void)closesocket(sock);
    return (NLaddress *)&address;
}

static NLushort ipx_GetPort(SOCKET socket)
{
    struct sockaddr_ipx   addr;
    int                  len;
    
    len = (int)sizeof(struct sockaddr_in);
    if(getsockname(socket, (struct sockaddr *)&addr, &len) != 0)
    {
        return 0;
    }
    
    return ntohs(addr.sa_socket);
}


NLboolean ipx_Init(void)
{
#if defined WIN32 || defined WIN64
    WSADATA libmibWSAdata;

    if(WSAStartup(0x101,&libmibWSAdata) != 0)
    {
        return NL_FALSE;
    }
#endif
    ipx_ouraddress = ipx_GetHostAddress();

    if(ipx_ouraddress == NULL)
    {
        ipx_Shutdown();
        return NL_FALSE;
    }

    return NL_TRUE;
}

void ipx_Shutdown(void)
{
#if defined WIN32 || defined WIN64
    (void)WSACleanup();
#endif
}

NLboolean ipx_Listen(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];

    if(sock->reliable == NL_TRUE)
    {
        if(listen((SOCKET)sock->realsocket, backlog) != 0)
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_FALSE;
        }
    }

    sock->listen = NL_TRUE;
    return NL_TRUE;
}

static SOCKET ipx_AcceptIPX(NLsocket nlsocket, struct sockaddr_ipx /*@out@*/ *newaddr)
{
    nl_socket_t         *sock = nlSockets[nlsocket];
    struct sockaddr_ipx ouraddr;
    SOCKET              realsocket;
    NLbyte              buffer[512];
    NLint               len = (NLint)sizeof(struct sockaddr_ipx);
    NLint               slen = (NLint)sizeof(NL_CONNECT_STRING);

    /* Get the packet and remote host address */
    if(recvfrom((SOCKET)sock->realsocket, buffer, (int)sizeof(buffer), 0, 
            (struct sockaddr *)newaddr, &len) < (int)sizeof(NL_CONNECT_STRING))
    {
        return INVALID_SOCKET;
    }
    /* Lets check for the connection string */
    buffer[slen - 1] = (NLbyte)0; /* null terminate for peace of mind */
    if(strcmp(buffer, NL_CONNECT_STRING) != 0)
    {
        return INVALID_SOCKET;
    }
    /* open up a new socket on this end, system assigned port number */
    realsocket = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if(realsocket == INVALID_SOCKET)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(realsocket);
        return INVALID_SOCKET;
    }

    ouraddr.sa_family = AF_IPX;
    memset(&ouraddr.sa_netnum, 0, 4);
    memset(&ouraddr.sa_nodenum, 0, 6);
    ouraddr.sa_socket = 0;

    if(bind(realsocket, (struct sockaddr *)&ouraddr, len) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(realsocket);
        return INVALID_SOCKET;
    }

    /* send back a packet so the client knows our new port */
    if(sendto(realsocket, NL_REPLY_STRING, (int)sizeof(NL_REPLY_STRING), 0, 
                        (struct sockaddr *)newaddr,
                        (int)sizeof(struct sockaddr_in)) < (NLint)sizeof(NL_REPLY_STRING))
    {
        nlSetError(NL_SOCKET_ERROR);
        (void)closesocket(realsocket);
        return INVALID_SOCKET;
    }

    return realsocket;
}

NLsocket ipx_AcceptConnection(NLsocket socket)
{
    nl_socket_t         *sock = nlSockets[socket];
    nl_socket_t         *newsock;
    NLsocket            newsocket;
    SOCKET              realsocket;
    struct sockaddr_ipx newaddr;

    if(sock->listen == NL_FALSE)
    {
        nlSetError(NL_NOT_LISTEN);
        return NL_INVALID;
    }

    if(sock->reliable == NL_TRUE)
    {
        NLint   len = (NLint)sizeof(newaddr);

        realsocket = accept((SOCKET)sock->realsocket,
                                    (struct sockaddr *)&newaddr, &len);
    }
    else
    {
        realsocket = ipx_AcceptIPX(socket, &newaddr);
    }

    if(realsocket == INVALID_SOCKET)
    {
        if(sockerrno != EWOULDBLOCK)
        {
            nlSetError(NL_SOCKET_ERROR);
        }
        return NL_INVALID;
    }

    newsocket = nlGetNewSocket();
    if(newsocket == NL_INVALID)
    {
        return NL_INVALID;
    }
    newsock = nlSockets[newsocket];

    /* update the remote address */
    memcpy((char *)&newsock->address, (char *)&newaddr, sizeof(struct sockaddr_ipx));
    newsock->realsocket = (NLint)realsocket;
    newsock->localport = ipx_GetPort(realsocket);
    newsock->remoteport = ipx_GetPortFromAddr((NLaddress *)&newsock->address);

    if(newsock->blocking == NL_FALSE)
    {
        if(ipx_SetNonBlocking((SOCKET)newsock->realsocket) == NL_FALSE)
        {
            nlSetError(NL_SOCKET_ERROR);
            ipx_Close(newsocket);
            return NL_INVALID;
        }
    }

    newsock->reliable = sock->reliable;

    return newsocket;
}

NLsocket ipx_Open(NLushort port, NLenum type)
{
    nl_socket_t *newsock;
    NLsocket    newsocket;
    SOCKET      realsocket;

    switch (type) {

    case NL_RELIABLE:
    case NL_RELIABLE_PACKETS:
        realsocket = socket(PF_IPX, SOCK_SEQPACKET, NSPROTO_SPXII);
        break;

    case NL_UNRELIABLE:
    case NL_BROADCAST:
        realsocket = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX);
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

    if(type == NL_RELIABLE || type == NL_RELIABLE_PACKETS)
    {
        newsock->reliable = NL_TRUE;
    }
    else
    {
        newsock->reliable = NL_FALSE;
    }

    ipx_SetReuseAddr(realsocket);

    if(newsock->blocking == NL_FALSE)
    {
        if(ipx_SetNonBlocking(realsocket) == NL_FALSE)
        {
            nlSetError(NL_SOCKET_ERROR);
            ipx_Close(newsocket);
            return NL_INVALID;
        }
    }

    ((struct sockaddr_ipx *)&newsock->address)->sa_family = AF_IPX;
    memset(((struct sockaddr_ipx *)&newsock->address)->sa_netnum, 0, 4);
    memset(((struct sockaddr_ipx *)&newsock->address)->sa_nodenum, 0, 6);
    ((struct sockaddr_ipx *)&newsock->address)->sa_socket = htons((unsigned short)port);

    if(bind((SOCKET)realsocket, (struct sockaddr *)&newsock->address, (int)sizeof(newsock->address)) == SOCKET_ERROR)
    {
        nlSetError(NL_SOCKET_ERROR);
        ipx_Close(newsocket);
        return NL_INVALID;
    }
    if(type == NL_BROADCAST)
    {
        if(ipx_SetBroadcast(realsocket) == NL_FALSE)
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_INVALID;
        }
    }

    newsock->localport = ipx_GetPort(realsocket);
    newsock->type = type;
    return newsocket;
}

static NLboolean ipx_ConnectIPX(NLsocket socket)
{
    NLint               retries = 3;
    nl_socket_t         *sock = nlSockets[socket];


    if(ipx_Write(socket, (NLvoid *)NL_CONNECT_STRING, (NLint)sizeof(NL_CONNECT_STRING))
        < (NLint)sizeof(NL_CONNECT_STRING))
    {
        return NL_FALSE;
    }

    sleep(1);

    while(retries-- > 0)
    {
        NLbyte              buffer[512];
        NLint               len = (NLint)sizeof(struct sockaddr_in);
        NLint               slen = (NLint)sizeof(NL_REPLY_STRING);
        NLint               received;

        /* Get the packet and remote host address */
        received = recvfrom((SOCKET)sock->realsocket, buffer, (int)sizeof(buffer), 0, 
                (struct sockaddr *)&sock->address, &len);
        
        if(received >= slen)
        {
            /* Lets check for the reply string */
            buffer[slen - 1] = (NLbyte)0; /* null terminate for peace of mind */
            if(strcmp(buffer, NL_REPLY_STRING) != 0)
            {
                if(connect((SOCKET)sock->realsocket, (struct sockaddr *)&sock->address,
                            (int)sizeof(struct sockaddr_in)) != 0)
                {
                    nlSetError(NL_SOCKET_ERROR);
                    return NL_FALSE;
                }
                return NL_TRUE;
            }
        }
        sleep(1);
    }

    return NL_FALSE;
}

NLboolean ipx_Connect(NLsocket socket, NLaddress *address)
{
    nl_socket_t *sock = nlSockets[socket];

    memcpy((char *)&sock->address, (char *)address, sizeof(struct sockaddr_in));

    if(sock->reliable == NL_TRUE)
    {
        if(sock->blocking == NL_FALSE)
        {
            (void)ipx_SetBlocking((SOCKET)sock->realsocket);
        }

        if(connect((SOCKET)sock->realsocket, (struct sockaddr *)&sock->address,
                    (int)sizeof(struct sockaddr_in)) != 0)
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_FALSE;
        }
        if(sock->blocking == NL_FALSE)
        {
            (void)ipx_SetNonBlocking((SOCKET)sock->realsocket);
        }
    }
    else
    {
        if(ipx_ConnectIPX(socket) == NL_FALSE)
        {
            return NL_FALSE;
        }
    }

    sock->localport = ipx_GetPort((SOCKET)sock->realsocket);
    sock->remoteport = ipx_GetPortFromAddr((NLaddress *)&sock->address);

    return NL_TRUE;
}

void ipx_Close(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];

    (void)closesocket((SOCKET)sock->realsocket);
    nlFreeSocket(socket);
}

NLint ipx_Read(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       count;
    NLint       len = (NLint)sizeof(struct sockaddr_ipx);


    if(sock->reliable == NL_TRUE)
    {
        count = recv((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
    }
    else
    {
        count = recvfrom((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0, 
                        (struct sockaddr *)&sock->address, &len);
    }

    if(count < 0)
    {
        if(sockerrno == EWOULDBLOCK)
        {
            return 0;
        }
    }
    return count;
}

NLint ipx_Write(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       count;

    if(sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS)
    {
        count = send((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0);
    }
    else /* IPX */
    {
        if(nbytes > 1466)
        {
            nlSetError(NL_PACKET_SIZE);
            return NL_INVALID;
        }
        if(sock->type == NL_BROADCAST)
        {
            memset(((struct sockaddr_ipx *)&sock->address)->sa_nodenum, 0xff, 6);
        }
        count = sendto((SOCKET)sock->realsocket, (char *)buffer, nbytes, 0, 
                        (struct sockaddr *)&sock->address,
                        (int)sizeof(struct sockaddr_ipx));
    }

    if(count < 0)
    {
        if(sockerrno == EWOULDBLOCK)
        {
            return 0;
        }
        else
        {
            nlSetError(NL_SOCKET_ERROR);
            return NL_INVALID;
        }
    }
    return count;
}

NLbyte *ipx_AddrToString(NLaddress *address, NLbyte *string)
{
    sprintf(string, "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%u",
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[0] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[1] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[2] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[3] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[0] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[1] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[2] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[3] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[4] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[5] & 0xff,
        ntohs(((struct sockaddr_ipx *)address)->sa_socket));
    return string;
}

void ipx_StringToAddr(NLbyte *string, NLaddress *address)
{
    int  val;
    char buffer[3];

    buffer[2] = (char)0;
    memset(address, 0, sizeof(NLaddress));
    address->sa_family = AF_IPX;
    address->valid = NL_TRUE;

#define DO(src,dest)    \
    buffer[0] = string[src];    \
    buffer[1] = string[src + 1];    \
    if(sscanf (buffer, "%x", &val) != 1)   \
        return; \
    ((struct sockaddr_ipx *)address)->dest = (char)val

    DO(0, sa_netnum[0]);
    DO(2, sa_netnum[1]);
    DO(4, sa_netnum[2]);
    DO(6, sa_netnum[3]);
    DO(9, sa_nodenum[0]);
    DO(11, sa_nodenum[1]);
    DO(13, sa_nodenum[2]);
    DO(15, sa_nodenum[3]);
    DO(17, sa_nodenum[4]);
    DO(19, sa_nodenum[5]);
#undef DO

    (void)sscanf(&string[22], "%d", &val);
    ((struct sockaddr_ipx *)address)->sa_socket = htons((unsigned short)val);
}

void ipx_GetLocalAddr(NLsocket socket, NLaddress *address)
{
    nl_socket_t *sock = nlSockets[socket];

    memcpy(address, ipx_ouraddress, sizeof(NLaddress));
	ipx_SetAddrPort(address, sock->localport);
    address->valid = NL_TRUE;
}

void ipx_SetLocalAddr(NLaddress *address)
{
    memcpy(ipx_ouraddress, address, sizeof(NLaddress));
}

NLbyte *ipx_GetNameFromAddr(NLaddress *address, NLbyte *name)
{
    return ipx_AddrToString(address, name);
}

void ipx_GetNameFromAddrAsync(NLaddress *address, NLbyte *name)
{
    (void)ipx_AddrToString(address, name);
}

void ipx_GetAddrFromName(NLbyte *name, NLaddress *address)
{
    NLint n;
    char buffer[32];

    address->valid = NL_TRUE;
    n = (NLint)strlen(name);

    if(n == 12)
    {
        sprintf(buffer, "00000000:%s:%d", name, ipxport);
        ipx_StringToAddr (buffer, address);
        return;
    }
    if(n == 21)
    {
        sprintf(buffer, "%s:%d", name, ipxport);
        ipx_StringToAddr (buffer, address);
        return;
    }
    if((n > 21) && (n <= 27))
    {
        ipx_StringToAddr (name, address);
        return;
    }
    memset(address, 0, sizeof(NLaddress));
    address->valid = NL_FALSE;
    return;
}

void ipx_GetAddrFromNameAsync(NLbyte *name, NLaddress *address)
{
    ipx_GetAddrFromName(name, address);
	address->valid = NL_TRUE;
}

NLboolean ipx_AddrCompare(NLaddress *address1, NLaddress *address2)
{
    if(address1->sa_family != address2->sa_family)
        return NL_FALSE;

    if(memcmp(((struct sockaddr_ipx *)address1)->sa_netnum, ((struct sockaddr_ipx *)address2)->sa_netnum, 4) != 0)
        return NL_FALSE;
    if(memcmp(((struct sockaddr_ipx *)address1)->sa_nodenum, ((struct sockaddr_ipx *)address2)->sa_nodenum, 6) != 0)
        return NL_FALSE;

    if(((struct sockaddr_ipx *)address1)->sa_socket != ((struct sockaddr_ipx *)address2)->sa_socket)
        return NL_FALSE;

    return NL_TRUE;
}

NLushort ipx_GetPortFromAddr(NLaddress *address)
{
    return ntohs(((struct sockaddr_ipx *)address)->sa_socket);
}

void ipx_SetAddrPort(NLaddress *address, NLushort port)
{
    ((struct sockaddr_ipx *)address)->sa_socket = htons((NLushort)port);
}

NLint ipx_GetSystemError(void)
{
    return sockerrno;
}

NLint ipx_PollGroup(NLint group, NLenum name, NLsocket *sockets, NLint number, NLint timeout)
{
    NLint           numselect, count = 0;
    NLint           numsockets = NL_MAX_GROUP_SOCKETS;
    NLsocket        temp[NL_MAX_GROUP_SOCKETS];
    NLsocket        *ptemp = temp;
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

    nlGroupGetSockets(group, ptemp, &numsockets);
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
    while(numsockets-- > 0 && numselect > count)
    {
        if(FD_ISSET(nlSockets[*ptemp]->realsocket, &fdset) != 0)
        {
            *sockets = *ptemp;
            sockets++;
            count ++;
        }
        ptemp++;
    }
    return count;
}

void ipx_Hint(NLenum name, NLint arg)
{
    switch(name) {

    case NL_LISTEN_BACKLOG:
        backlog = arg;
        break;

    case NL_REUSE_ADDRESS:
        reuseaddress = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
    }
}

#endif /* NL_INCLUDE_IPX */
