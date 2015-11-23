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

#ifndef INTERNAL_H
#define INTERNAL_H

#include "nl.h"
#include "sock.h"
#include "loopback.h"
#include "serial.h"
#include "parallel.h"

#if (defined WIN32 || defined WIN64) && defined NL_INCLUDE_IPX
  #include "ipx.h"
#endif

#undef NL_SWAP_TRUE
#ifdef NL_LITTLE_ENDIAN
#define NL_SWAP_TRUE (nlState.nl_big_endian_data == NL_TRUE)
#else
#define NL_SWAP_TRUE (nlState.nl_big_endian_data != NL_TRUE)
#endif /* NL_LITTLE_ENDIAN */


/* internally for TCP packets and UDP connections, all data is big endien,
   so we force it so here using these  */
#undef writeShort
#define writeShort(x, y, z)     {*((NLushort *)((NLbyte *)&x[y])) = htons(z); y += 2;}
#undef readShort
#define readShort(x, y, z)      {z = ntohs(*(NLushort *)((NLbyte *)&x[y])); y += 2;}

#define NL_FIRST_GROUP          (NL_MAX_INT_SOCKETS + 1)

#ifdef NL_THREAD_SAFE
#include "mutex.h"
#else
#define nlMutexInit(x)
#define nlMutexLock(x)
#define nlMutexUnlock(x)
#define nlMutexDestroy(x)
#define NLmutex int
#endif /* NL_THREAD_SAFE */


/* number of buckets for average bytes/second */
#define NL_NUM_BUCKETS          8

/* number of packets stored for NL_LOOP_BACK */
#define NL_NUM_PACKETS          8
#define NL_MAX_ACCEPT           10

/* for nlLockSocket and nlUnlockSocket */
#define NL_READ                 0x0001
#define NL_WRITE                0x0002
#define NL_BOTH                 (NL_READ|NL_WRITE)

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* the driver object */
typedef struct
{
    const NLbyte /*@observer@*/*name;
    const NLbyte /*@observer@*/*connections;
    NLenum      type;
    NLboolean   initialized;
    NLboolean   (*Init)(void);
    void        (*Shutdown)(void);
    NLboolean   (*Listen)(NLsocket socket);
    NLsocket    (*AcceptConnection)(NLsocket socket);
    NLsocket    (*Open)(NLushort port, NLenum type);
    NLboolean   (*Connect)(NLsocket socket, NLaddress *address);
    void        (*Close)(NLsocket socket);
    NLint       (*Read)(NLsocket socket, /*@out@*/ NLvoid *buffer, NLint nbytes);
    NLint       (*Write)(NLsocket socket, NLvoid *buffer, NLint nbytes);
    NLbyte      *(*AddrToString)(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *string);
    void        (*StringToAddr)(NLbyte *string, /*@out@*/ NLaddress *address);
    void        (*GetLocalAddr)(NLsocket socket, /*@out@*/ NLaddress *address);
    void        (*SetLocalAddr)(NLaddress *address);
    NLbyte      *(*GetNameFromAddr)(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *name);
    void        (*GetNameFromAddrAsync)(NLaddress *address, /*@out@*/ NLbyte *name);
    void        (*GetAddrFromName)(NLbyte *name, /*@out@*/ NLaddress *address);
    void        (*GetAddrFromNameAsync)(NLbyte *name, /*@out@*/ NLaddress *address);
    NLboolean   (*AddrCompare)(NLaddress *address1, NLaddress *address2);
    NLushort    (*GetPortFromAddr)(NLaddress *address);
    void        (*SetAddrPort)(NLaddress *address, NLushort port);
    NLint       (*GetSystemError)(void);
    NLint       (*PollGroup)(NLint group, NLenum name, /*@out@*/ NLsocket *sockets,
                                NLint number, NLint timeout);
    void        (*Hint)(NLenum name, NLint arg);
} nl_netdriver_t;

typedef struct
{
    NLlong      bytes;          /* bytes sent/received */
    NLlong      packets;        /* packets sent/received */
    NLlong      highest;        /* highest bytes/sec sent/received */
    NLlong      average;        /* average bytes/sec sent/received */
    time_t      stime;          /* the last time stats were updated */
    NLint       lastbucket;     /* the last bucket that was used */
    NLlong      curbytes;       /* current bytes sent/received */
    NLlong      bucket[NL_NUM_BUCKETS];/* buckets for sent/received counts */
} nl_stats_t;

/* the internal socket object */
typedef struct
{
    /* the current status of the socket */
    NLenum      type;           /* type of socket */
    NLboolean   inuse;          /* is in use */
#ifdef NL_THREAD_SAFE
    NLmutex     readlock;       /* socket is locked to update data */
    NLmutex     writelock;      /* socket is locked to update data */
#endif
    NLboolean   connecting;     /* a non-blocking TCP or UDP connection is in process */
    NLboolean   conerror;       /* an error occured on a UDP connect */
    NLboolean   connected;      /* is connected */
    NLboolean   reliable;       /* do we use reliable */
    NLboolean   blocking;       /* is set to blocking */
    NLboolean   listen;         /* can receive an incoming connection */
    NLboolean   packetsync;     /* is the reliable packet stream in sync */
    NLint       realsocket;     /* the real socket number */
    NLushort    localport;      /* local port number */
    NLushort    remoteport;     /* remote port number */
    NLaddress   address;        /* address of remote system, same as the socket sockaddr_in structure */
    NLaddress   multicastaddr;  /* the mulitcast address set by nlConnect */

    /* the current read/write statistics for the socket */
    nl_stats_t  instats;        /* stats for received */
    nl_stats_t  outstats;       /* stats for sent */

    /* info for NL_LOOP_BACK, NL_SERIAL, and NL_PARALLEL */
    NLbyte      *outpacket[NL_NUM_PACKETS];/* temp storage for packet data */
    NLbyte      *inpacket[NL_NUM_PACKETS];/* temp storage for packet data */
    NLint       outlen[NL_NUM_PACKETS];/* the length of each packet */
    NLint       inlen[NL_NUM_PACKETS];/* the length of each packet */
    NLint       nextoutused;    /* the next used packet */
    NLint       nextinused;     /* the next used packet */
    NLint       nextoutfree;    /* the next free packet */
    NLint       nextinfree;     /* the next free packet */
    NLint       accept[NL_MAX_ACCEPT];/* pending connects */
    NLint       consock;        /* the socket this socket is connected to */

    /* NL_RELIABLE_PACKETS info and storage */
    NLbyte      *outbuf;        /* temp storage for partially sent reliable packet data */
    NLint       outbuflen;      /* the length of outbuf */
    NLint       sendlen;        /* how much still needs to be sent */
    NLbyte      *inbuf;         /* temp storage for partially received reliable packet data */
    NLint       inbuflen;       /* the length of inbuf */
    NLint       reclen;         /* how much of the reliable packet we have received */
    NLboolean   readable;       /* a complete packet is in inbuf */
    NLboolean   message_end;    /* a message end error ocured but was not yet reported */
} nl_socket_t;

typedef struct
{
    NLboolean   socketStats;    /* enable collection of socket read/write statistics, default disabled */
    NLboolean   nl_big_endian_data; /* is the packet data big endian? */
} nl_state_t;

/* used by the drivers to allocate and free socket objects */
NLsocket nlGetNewSocket(void);
void nlFreeSocket(NLsocket socket);

/* other functions */
NLboolean nlGroupInit(void);
void nlGroupShutdown(void);
void nlGroupLock(void);
void nlGroupUnlock(void);
void nlGroupGetSocketsINT(NLint group, NLsocket *socket, NLint *number);
NLboolean nlIsValidSocket(NLsocket socket);
void nlLockSocket(NLsocket socket, NLint which);
void nlUnlockSocket(NLsocket socket, NLint which);
void nlSetError(NLenum err);

/* globals (as few as possible) */
extern volatile nl_state_t nlState;
extern nl_socket_t **nlSockets;

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* INTERNAL_H */

