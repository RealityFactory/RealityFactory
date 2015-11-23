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


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nlinternal.h"

#if defined WIN32 || defined WIN64
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

#else
/* Unix-style systems or macs with posix support */
#include <netinet/in.h> /* for ntohs and htons */
#endif

#ifdef _MSC_VER
#pragma warning (disable:4100) /* disable "unreferenced formal parameter" */
#endif


static NLint loopgroup;
static NLbyte localname[] = "localhost";
static volatile NLboolean reuseaddress = NL_FALSE;

static NLmutex  portlock; /* In memory of my step-father, Don Portlock,
                                       who passed away Jan 12, 2001 - Phil */

static volatile NLushort newport = 1024;

static NLushort loopback_getNextPort(void)
{
    nlMutexLock(&portlock);
    if(++newport > 65535)
    {
        newport = 1024;
    }
    nlMutexUnlock(&portlock);
    return newport;
}

static NLboolean loopback_ScanPort(NLushort port, NLenum type)
{
    NLint           numsockets = NL_MAX_GROUP_SOCKETS;
    NLsocket        temp[NL_MAX_GROUP_SOCKETS];

    if(reuseaddress == NL_TRUE)
    {
        return NL_TRUE;
    }
    nlGroupGetSockets(loopgroup, (NLint *)&temp, &numsockets);
    if(numsockets < 0)
    {
        return NL_FALSE;
    }
    if(numsockets == 0)
    {
        return NL_TRUE;
    }
    while(numsockets-- > 0)
    {
        nl_socket_t *sock = nlSockets[temp[numsockets]];

        if(sock->type == type && sock->localport == port)
        {
            return NL_FALSE;
        }
    }
    return NL_TRUE;
}

static NLushort loopback_TryPort(NLushort port, NLenum type)
{
    NLint   ntries = 500; /* this is to prevent an infinite loop */

    if(port > 0)
    {
        if(loopback_ScanPort(port, type) == NL_TRUE)
        {
            return port;
        }
        else
        {
            return 0;
        }
    }
    /* let's find our own port number */
    while(ntries-- > 0)
    {
        port = loopback_getNextPort();
        if(loopback_ScanPort(port, type) == NL_TRUE)
        {
            return port;
        }
    }
    return 0;
}

NLboolean loopback_Init(void)
{
    loopgroup = nlGroupCreate();

    if(loopgroup == NL_INVALID)
    {
        return NL_FALSE;
    }
    return NL_TRUE;
}

void loopback_Shutdown(void)
{
    nlGroupDestroy(loopgroup);
}

NLboolean loopback_Listen(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       i = NL_MAX_ACCEPT;

    if(sock->type == NL_BROADCAST)
    {
        nlSetError(NL_WRONG_TYPE);
        return NL_FALSE;
    }
    sock->listen = NL_TRUE;
    while(i-- > 0)
    {
        sock->accept[i] = NL_INVALID;
    }
    return NL_TRUE;
}

NLsocket loopback_AcceptConnection(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       i;

    if(sock->listen == NL_FALSE)
    {
        nlSetError(NL_NOT_LISTEN);
        return NL_INVALID;
    }
    /* look for a valid socket */
    for(i=0;i<NL_MAX_ACCEPT;i++)
    {
        if(sock->accept[i] != NL_INVALID)
        {
            NLsocket    newsocket = loopback_Open(0, sock->type);
            NLsocket    osock = sock->accept[i];
            nl_socket_t *othersock = nlSockets[osock];

            sock->accept[i] = NL_INVALID;
            if(newsocket != NL_INVALID)
            {
                nl_socket_t *newsock = nlSockets[newsocket];

                nlLockSocket(osock, NL_BOTH);
                /* do the connecting */
                newsock->consock = osock;
                newsock->remoteport = othersock->localport;
                othersock->consock = newsocket;
                othersock->remoteport = newsock->localport;
                newsock->connected = NL_TRUE;
                othersock->connected = NL_TRUE;
                othersock->connecting = NL_FALSE;
                loopback_SetAddrPort(&othersock->address, othersock->remoteport);
                loopback_SetAddrPort(&newsock->address, newsock->remoteport);
                nlUnlockSocket(osock, NL_BOTH);
                return newsocket;
            }
        }
    }
    nlSetError(NL_NO_PENDING);
    return NL_INVALID;
}

NLsocket loopback_Open(NLushort port, NLenum type)
{
    nl_socket_t *newsock;
    NLsocket    newsocket;
    NLint       i;
    NLushort    lport;

    switch (type) {

    case NL_RELIABLE:
    case NL_UNRELIABLE:
    case NL_RELIABLE_PACKETS:
    case NL_BROADCAST:
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_INVALID;
    }

    lport = loopback_TryPort(port, type);
    if(lport == 0)
    {
        nlSetError(NL_INVALID_PORT);
        return NL_INVALID;
    }
    newsocket = nlGetNewSocket();
    if(newsocket == NL_INVALID)
    {
        return NL_INVALID;
    }
    newsock = nlSockets[newsocket];
    newsock->type = type;
    newsock->localport = lport;
    if(type == NL_BROADCAST)
    {
        newsock->remoteport = lport;
    }

    for(i=0;i<NL_NUM_PACKETS;i++)
    {
        NLboolean err = NL_FALSE;

        /* malloc the max packet length plus two bytes for the port number */
        if((newsock->inpacket[i] = (NLbyte *)malloc((size_t)(NL_MAX_PACKET_LENGTH + 2))) == NULL)
        {
            nlSetError(NL_OUT_OF_MEMORY);
            err = NL_TRUE;
        }
        if(err == NL_TRUE)
        {
            while(i-- > 0)
            {
                free(newsock->inpacket[i]);
            }
            sock_Close(newsocket);
            return NL_INVALID;
        }
    }

    (void)nlGroupAddSocket(loopgroup, newsocket);

    return newsocket;
}

NLboolean loopback_Connect(NLsocket socket, NLaddress *address)
{
    nl_socket_t *sock = nlSockets[socket];
    NLushort    port;
    NLint       numsockets = NL_MAX_GROUP_SOCKETS;
    NLsocket    temp[NL_MAX_GROUP_SOCKETS];

    /* no need to connect a broadcast socket */
    if(sock->type == NL_BROADCAST)
    {
        nlSetError(NL_WRONG_TYPE);
    }
    port = loopback_GetPortFromAddr(address);
    /* make sure socket is not already connected */
    if(sock->connected == NL_TRUE || sock->connecting == NL_TRUE)
    {
        nlSetError(NL_CON_REFUSED);
        return NL_FALSE;
    }
    nlGroupGetSockets(loopgroup, (NLint *)&temp, &numsockets);
    if(numsockets <= 0)
    {
        return NL_FALSE;
    }
    while(numsockets-- > 0)
    {
        nl_socket_t *othersock = nlSockets[temp[numsockets]];

        if(sock->type == othersock->type && port == othersock->localport
            && othersock->listen == NL_TRUE && othersock->connected == NL_FALSE
            && othersock->connecting == NL_FALSE)
        {
            /* we found the right socket, so connect */
            NLint i;

            for(i=0;i<NL_MAX_ACCEPT;i++)
            {
                if(othersock->accept[i] == NL_INVALID)
                {
                    othersock->accept[i] = socket;
                    sock->connecting = NL_TRUE;
                    sock->consock = temp[numsockets];
                    return NL_TRUE;
                }
            }
        }
    }
    nlSetError(NL_CON_REFUSED);
    return NL_FALSE;
}

void loopback_Close(NLsocket socket)
{
    nl_socket_t *sock = nlSockets[socket];
    int         i;

    if(sock->connected == NL_TRUE || sock->connecting == NL_TRUE)
    {
        /* break the connection */
        nl_socket_t *othersock = nlSockets[sock->consock];

        othersock->consock = NL_INVALID;
    }
    for(i=0;i<NL_NUM_PACKETS;i++)
    {
        void /*@owned@*/*t = sock->inpacket[i];

        free(t);
    }
    nlFreeSocket(socket);
}

NLint loopback_Read(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    NLint       len = sock->inlen[sock->nextinused];
    NLint       c = 0;
    NLushort    port;

    if(len > 0)
    {
        if(len > nbytes)
        {
            nlSetError(NL_BUFFER_SIZE);
            return NL_INVALID;
        }
        if(sock->connecting == NL_TRUE)
        {
            nlSetError(NL_CON_PENDING);
            return NL_INVALID;
        }
        /* get the port number */
        readShort(sock->inpacket[sock->nextinused], c, port);
        loopback_SetAddrPort(&sock->address, port);
        /* copy the packet */
        memcpy(buffer, sock->inpacket[sock->nextinused] + 2, (size_t)len);
        /* zero out length and set up for next packet */
        sock->inlen[sock->nextinused] = 0;
        sock->nextinused++;
        if(sock->nextinused >= NL_NUM_PACKETS)
        {
            sock->nextinused = 0;
        }
    }
    /* check for broken connection */
    if(sock->connected == NL_TRUE && sock->consock == NL_INVALID)
    {
        nlSetError(NL_CON_TERM);
        return NL_INVALID;
    }
    return len;
}

static NLint loopback_WritePacket(NLsocket to, NLvoid *buffer, NLint nbytes, NLushort fromport)
{
    nl_socket_t *sock = nlSockets[to];
    NLint       i, j;
    NLint       c = 0;

    /* check the packet size */
    if(nbytes > NL_MAX_PACKET_LENGTH)
    {
        nlSetError(NL_PACKET_SIZE);
        return NL_INVALID;
    }
    nlLockSocket(to, NL_READ);
    /* make sure we have an empty packet buffer */
    if(sock->nextinfree == NL_INVALID)
    {
        /* all buffers were filled by last write */
        /* check to see if any were emptied by a read */
        i = NL_NUM_PACKETS;
        j = sock->nextinused;

        while(i-- > 0)
        {
            if(sock->inlen[j] == 0)
            {
                /* found the first free */
                sock->nextinfree = j;
                break;
            }
            j++;
            if(j >= NL_NUM_PACKETS)
            {
                j = 0;
            }
        }
        if(sock->nextinfree == NL_INVALID)
        {
            nlUnlockSocket(to, NL_READ);
            /* none are free */
            if(sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS)
            {
                return 0;
            }
            else
            {
                /* silently fail */
                return nbytes;
            }
        }
    }
    /* write the port number */
    writeShort(sock->inpacket[sock->nextinfree], c, fromport);
    /* copy the packet buffer */
    memcpy(sock->inpacket[sock->nextinfree] + 2, buffer, (size_t)nbytes);
    sock->inlen[sock->nextinfree] = nbytes;
    sock->nextinfree++;
    if(sock->nextinfree >= NL_NUM_PACKETS)
    {
        sock->nextinfree = 0;
    }
    /* check for full packet buffers */
    if(sock->inlen[sock->nextinfree] != 0)
    {
        sock->nextinfree = NL_INVALID;
    }
    nlUnlockSocket(to, NL_READ);
    return nbytes;
}

NLint loopback_Write(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    nl_socket_t *sock = nlSockets[socket];
    nl_socket_t *othersock;
    NLint       s[NL_MAX_GROUP_SOCKETS];
    NLint       number = NL_MAX_GROUP_SOCKETS;
    NLint       i;
    NLint       count;

    switch (sock->type) {

    case NL_RELIABLE:
    case NL_UNRELIABLE:
    case NL_RELIABLE_PACKETS:
    default:
        {
            if(sock->connected == NL_TRUE)
            {
                /* check for broken connection */
                if(sock->consock == NL_INVALID)
                {
                    nlSetError(NL_CON_TERM);
                    return NL_INVALID;
                }
                count = loopback_WritePacket(sock->consock, buffer, nbytes, sock->localport);
            }
            else if(sock->connecting == NL_TRUE)
            {
                nlSetError(NL_CON_PENDING);
                return NL_INVALID;
            }
            count = nbytes;
            nlGroupGetSockets(loopgroup, s, &number);
            for(i=0;i<number;i++)
            {
                if(nlIsValidSocket(s[i]) == NL_TRUE)
                {
                    othersock = nlSockets[s[i]];

                    if(sock->remoteport == othersock->localport &&
                        othersock->connected == NL_FALSE &&
                        sock->type == othersock->type)
                    {
                        (void)loopback_WritePacket(s[i], buffer, nbytes, sock->localport);
                    }
                }
            }
        }
        break;
    case NL_BROADCAST:
        {
            count = nbytes;
            nlGroupGetSockets(loopgroup, s, &number);
            for(i=0;i<number;i++)
            {
                if(nlIsValidSocket(s[i]) == NL_TRUE)
                {
                    othersock = nlSockets[s[i]];

                    if(sock->localport == othersock->localport &&
                        sock->type == othersock->type)
                    {
                        (void)loopback_WritePacket(s[i], buffer, nbytes, sock->localport);
                    }
                }
            }
        }
    }

    return count;
}

NLbyte *loopback_AddrToString(NLaddress *address, NLbyte *string)
{
    sprintf(string, "127.0.0.1:%u", loopback_GetPortFromAddr(address));

    return string;
}

void loopback_StringToAddr(NLbyte *string, NLaddress *address)
{
    NLbyte      *st;
    NLint       port;

    memset(address, 0, sizeof(NLaddress));
    address->valid = NL_TRUE;
    /* check for a port number */
    st = strchr(string, ':');
    if(st != NULL)
    {
        st++;
        port = atoi(st);
        if(port < 0)
        {
            port = 0;
        }
        else if(port > 65535)
        {
            port = 0;
        }
        loopback_SetAddrPort(address, (NLushort)port);
    }
}

void loopback_GetLocalAddr(NLsocket socket, NLaddress *address)
{
    memset(address, 0, sizeof(NLaddress));
    address->valid = NL_TRUE;
}

void loopback_SetLocalAddr(NLaddress *address)
{
}

NLbyte *loopback_GetNameFromAddr(NLaddress *address, NLbyte *name)
{
    sprintf(name, "%s:%u", localname, loopback_GetPortFromAddr(address));
    return name;
}

void loopback_GetNameFromAddrAsync(NLaddress *address, NLbyte *name)
{
    (void)loopback_GetNameFromAddr(address, name);
}

void loopback_GetAddrFromName(NLbyte *name, NLaddress *address)
{
    loopback_StringToAddr(name, address);
    address->valid = NL_TRUE;
}

void loopback_GetAddrFromNameAsync(NLbyte *name, NLaddress *address)
{
    loopback_GetAddrFromName(name, address);
}

NLboolean loopback_AddrCompare(NLaddress *address1, NLaddress *address2)
{
    if(*(NLushort *)(&address1->sa_data[0]) == *(NLushort *)(&address2->sa_data[0]))
    {
        return NL_TRUE;
    }
    return NL_FALSE;
}

NLushort loopback_GetPortFromAddr(NLaddress *address)
{
    return *(NLushort *)(&address->sa_data[0]);
}

void loopback_SetAddrPort(NLaddress *address, NLushort port)
{
    *(NLushort *)(&address->sa_data[0]) = port;
}

NLint loopback_GetSystemError(void)
{
    return errno;
}

NLint loopback_PollGroup(NLint group, NLenum name, NLsocket *sockets, NLint number, NLint timeout)
{
    NLint           count = 0;
    NLint           numsockets = NL_MAX_GROUP_SOCKETS;
    NLsocket        temp[NL_MAX_GROUP_SOCKETS];

    nlGroupGetSockets(group, (NLint *)&temp, &numsockets);
    if(numsockets < 0)
    {
        /* any error is set by nlGroupGetSockets */
        return NL_INVALID;
    }
    if(numsockets == 0)
    {
        return 0;
    }

    switch(name) {

    case NL_READ_STATUS:
        {
            NLint   i = 0;

            while(numsockets-- > 0)
            {
                /* check for a packet */
                nl_socket_t *sock;

                if(nlIsValidSocket(temp[i]) != NL_TRUE)
                {
                    nlSetError(NL_INVALID_SOCKET);
                    return NL_INVALID;
                }
                sock = nlSockets[temp[i]];

                if(sock->inlen[sock->nextinused] > 0)
                {
                    *sockets = temp[i];
                    sockets++;
                    count++;
                    if(count > number)
                    {
                        nlSetError(NL_BUFFER_SIZE);
                        return NL_INVALID;
                    }
                }
                i++;
            }
        }
        break;

    case NL_WRITE_STATUS:
        {
            NLint   i = 0;

            while(numsockets-- > 0)
            {
                nl_socket_t *sock;

                if(nlIsValidSocket(temp[i]) != NL_TRUE)
                {
                    nlSetError(NL_INVALID_SOCKET);
                    return NL_INVALID;
                }
                sock = nlSockets[temp[i]];

                /* check for a free packet if reliable and connected */
                if((sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS)
                    && (sock->connecting == NL_TRUE || sock->connected == NL_TRUE))
                {
                    nl_socket_t *othersock = nlSockets[sock->consock];

                    if(othersock->nextinfree == NL_INVALID)
                    {
                        continue;
                    }
                }
                /* add the socket to the list */
                *sockets = temp[i];
                sockets++;
                count++;
                if(count > number)
                {
                    nlSetError(NL_BUFFER_SIZE);
                    return NL_INVALID;
                }
                i++;
            }
        }
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_INVALID;
    }
    return count;
}

void loopback_Hint(NLenum name, NLint arg)
{
    switch (name) {

    case NL_REUSE_ADDRESS:
        reuseaddress = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        break;
    }
}



