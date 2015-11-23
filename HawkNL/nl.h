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

#ifndef NL_H
#define NL_H

#include <string.h> /* for strcpy, strlen, and memcpy in macros */

#ifdef __cplusplus
extern "C" {
#endif


#define NL_MAJOR_VERSION 1
#define NL_MINOR_VERSION 5
#define NL_VERSION_STRING "HawkNL 1.5 beta 1"


/* define NL_SAFE_COPY for Sparc and other processors that do not allow non-aligned
   memory access. Needed for nlRead and nlWrite macros */
/*#define NL_SAFE_COPY */

/* undefine this to remove IPX code, Windows only  */
#define NL_INCLUDE_IPX

/* undefine this to remove serial code */
#define NL_INCLUDE_SERIAL

/* undefine this to remove modem code */
#define NL_INCLUDE_MODEM

/* undefine this to remove parallel code */
#define NL_INCLUDE_PARALLEL

/* undefine this to remove mutex locks and per-thread error codes */
#define NL_THREAD_SAFE

#if defined WIN32 || defined WIN64
  /* define NL_WIN_THREADS to use native Windows threads instead of pthreads */
#define NL_WIN_THREADS 
  #ifdef _MSC_VER
    #pragma warning (disable:4514) /* disable "unreferenced inline function has
                                    been removed" warning */
  #endif /* _MSC_VER */
  /* The default build for Windows is as a DLL. */
  /* If you want a static library, define WIN_STATIC_LIB. */
  #ifdef WIN_STATIC_LIB
    #define NL_EXP
  #else
    #if defined __LCC__
     #define NL_EXP extern
    #else
     #define NL_EXP __declspec(dllexport)
    #endif /* __LCC__ */
  #endif /* WIN_STATIC_LIB */
  #define NL_APIENTRY __stdcall
  #define NL_CALLBACK __cdecl
  #ifdef __GNUC__
    #define NL_INLINE extern __inline__
  #else
    #define NL_INLINE __inline
  #endif /* __GNUC__ */
#else
  #define NL_EXP extern
  #define NL_APIENTRY
  #define NL_CALLBACK
  #ifdef __GNUC__
    #define NL_INLINE extern __inline__
  #else
    #define NL_INLINE inline /* assuming C99 compliant compiler */
  #endif /* __GNUC__ */
#endif /* WIN32 || WIN64 */

/* Any more needed here? */
#if defined WIN32 || defined WIN64 || defined __i386__ || defined __alpha__ || defined __mips__
  #define NL_LITTLE_ENDIAN
#else
  #define NL_BIG_ENDIAN
#endif

/* How do we detect Solaris 64 and Linux 64 bit? */
#if defined WIN64
#define IS_64_BIT
#endif

/* 8 bit */
typedef char NLbyte;
typedef unsigned char NLubyte;
typedef unsigned char NLboolean;
/* 16 bit */
typedef short NLshort;
typedef unsigned short NLushort;
/* 32 bit */
#ifdef IS_64_BIT
typedef int NLlong;             /* Longs are 64 bit on a 64 bit CPU, but integers are still 32 bit. */
typedef unsigned int NLulong;   /* This is, of course, not true on Windows (yet another exception), */
                                /* but it does not hurt. */
#else
typedef long NLlong;
typedef unsigned long NLulong;
#endif

typedef float NLfloat;
/* 64 bit */
typedef double NLdouble;
/* misc. */
typedef int NLint;
typedef unsigned int NLuint;
typedef unsigned int NLenum;
typedef void NLvoid;
typedef int NLsocket;

#if defined(__sgi)
  #undef sa_family
  #undef sa_data
#endif

typedef struct _NLaddress NLaddress;

struct _NLaddress
{
     NLshort     sa_family;      /* same as struct sockaddr */
     NLubyte     sa_data[14];    /* same as struct sockaddr */
     NLboolean   valid;          /* set to true by nlGetAddrFromNameAsync */
};
/* for backwards compatability */
#if !defined(address_t)
   typedef struct _NLaddress address_t;
#endif

/* max string size limited to 256 (255 plus NULL termination) for MacOS */
#define NL_MAX_STRING_LENGTH   256

/* max packet size for NL_UNRELIABLE and NL_RELIABLE_PACKETS */
#define NL_MAX_PACKET_LENGTH   16384

#if defined (macintosh)
  /* WARNING: Macs only allow up to 32K of local data, don't exceed 4096 */
  #define NL_MAX_INT_SOCKETS      4096

#else
  /* max number of sockets NL will handle */
  #define NL_MAX_INT_SOCKETS      8192    /* Should be high enough for most games */

#endif


/* max number groups and sockets per group */
#define NL_MAX_GROUPS           128
#define NL_MAX_GROUP_SOCKETS    NL_MAX_INT_SOCKETS

/* Boolean values */
#define NL_INVALID              (-1)
#define NL_FALSE                (NLboolean)(0)
#define NL_TRUE                 (NLboolean)(1)

/* Network types */
/* Only one can be selected at a time */
#define NL_IP                   0x0003  /* all platforms */
#define NL_LOOP_BACK            0x0004  /* all platforms, for single player client/server emulation with no network */
#define NL_IPX                  0x0005  /* Windows */
#define NL_SERIAL               0x0006  /* Windows and Linux only? */
#define NL_MODEM                0x0007  /* Windows and Linux only? */
#define NL_PARALLEL             0x0008  /* Windows and Linux only? */

/* Connection types */
#define NL_RELIABLE             0x0010  /* NL_IP (TCP), NL_IPX (SPX), NL_LOOP_BACK */
#define NL_UNRELIABLE           0x0011  /* NL_IP (UDP), NL_IPX, NL_LOOP_BACK */
#define NL_RELIABLE_PACKETS     0x0012  /* NL_IP (TCP), NL_IPX (SPX), NL_LOOP_BACK */
#define NL_BROADCAST            0x0013  /* NL_IP (UDP), NL_IPX, or NL_LOOP_BACK broadcast packets */
#define NL_MULTICAST            0x0014  /* NL_IP (UDP) multicast */
#define NL_RAW                  0x0015  /* NL_SERIAL or NL_PARALLEL */

/* nlGetString */
#define NL_VERSION              0x0020  /* the version string */
#define NL_NETWORK_TYPES        0x0021  /* space delimited list of available network types */
#define NL_CONNECTION_TYPES     0x0022  /* space delimited list of available connection types */
                                        /* only valid AFTER nlSelectNetwork */

/* nlGetInteger, nlGetSocketStat, nlClear */
#define NL_PACKETS_SENT         0x0030  /* total packets sent since last nlClear */
#define NL_BYTES_SENT           0x0031  /* total bytes sent since last nlClear */
#define NL_AVE_BYTES_SENT       0x0032  /* average bytes sent per second for the last 8 seconds */
#define NL_HIGH_BYTES_SENT      0x0033  /* highest bytes per second ever sent */
#define NL_PACKETS_RECEIVED     0x0034  /* total packets received since last nlClear */
#define NL_BYTES_RECEIVED       0x0035  /* total bytes received since last nlClear */
#define NL_AVE_BYTES_RECEIVED   0x0036  /* average bytes received per second for the last 8 seconds */
#define NL_HIGH_BYTES_RECEIVED  0x0037  /* highest bytes per second ever received */
#define NL_ALL_STATS            0x0038  /* nlClear only, clears out all counters */
#define NL_MAX_SOCKETS          0x0039  /* max sockets that can be opened by HawkNL, actual allowed by the system may be less */
#define NL_OPEN_SOCKETS         0x003a  /* number of open sockets */

/* nlEnable, nlDisable */
#define NL_BLOCKING_IO          0x0040  /* set IO to blocking, default is NL_FALSE for non-blocking IO */
#define NL_SOCKET_STATS         0x0041  /* enable collection of socket read/write statistics, default disabled */
#define NL_BIG_ENDIAN_DATA      0x0042  /* enable big endian data for nlSwap* and read/write macros, default enabled */
#define NL_LITTLE_ENDIAN_DATA   0x0043  /* enable little endian data for nlSwap* and read/write macros, default disabled */

/* nlPollGroup */
#define NL_READ_STATUS          0x0050  /* poll the read status for all sockets in the group */
#define NL_WRITE_STATUS         0x0051  /* poll the write status for all sockets in the group */

/* nlHint, advanced network settings for experienced developers */
#define NL_LISTEN_BACKLOG       0x0060  /* TCP, SPX: the backlog of connections for listen */
#define NL_KEEPALIVE            0x0061  /* TCP : enable the use of "keep-alive" packets. Default : NL_FALSE */
#define NL_MULTICAST_TTL        0x0062  /* UDP : The multicast TTL value. Default : 1 */
#define NL_REUSE_ADDRESS        0x0063  /* TCP, UDP : Allow IP address to be reused. Default : NL_FALSE */
#define NL_TCP_NO_DELAY         0x0064  /* TCP : disable Nagle algorithm */


/* errors */
#define NL_NO_ERROR             0x0000  /* no error is stored */
#define NL_NO_NETWORK           0x0100  /* no network was found on init */
#define NL_OUT_OF_MEMORY        0x0101  /* out of memory */
#define NL_INVALID_ENUM         0x0102  /* function called with an invalid NLenum */
#define NL_INVALID_SOCKET       0x0103  /* socket is not valid, or has been terminated */
#define NL_INVALID_PORT         0x0104  /* the port could not be opened */
#define NL_INVALID_TYPE         0x0105  /* the network type is not available */
#define NL_SOCKET_ERROR         0x0106  /* a system error occurred, call nlGetSystemError */
#define NL_SOCK_DISCONNECT      0x0107  /* the socket should be closed because of a connection loss or error */
#define NL_NOT_LISTEN           0x0108  /* the socket has not been set to listen */
#define NL_NO_BROADCAST         0x0109  /* could not broadcast, or tried to broadcast on a TCP stream */
#define NL_CON_REFUSED          0x010a  /* connection refused, or socket already connected */
#define NL_NO_MULTICAST         0x010b  /* could not set multicast, or tried to set multicast on a TCP stream */
#define NL_NO_PENDING           0x010c  /* there are no pending connections to accept */
#define NL_BAD_ADDR             0x010d  /* the address or port are not valid */
#define NL_OUT_OF_SOCKETS       0x010e  /* out of internal socket objects */
#define NL_MESSAGE_END          0x010f  /* the end of a reliable stream (TCP) message has been reached */
#define NL_NULL_POINTER         0x0110  /* a NULL pointer was passed to a function */
#define NL_INVALID_GROUP        0x0111  /* the group is not valid, or has been destroyed */
#define NL_OUT_OF_GROUPS        0x0112  /* out of internal group objects */
#define NL_OUT_OF_GROUP_SOCKETS 0x0113  /* the group has no more room for sockets */
#define NL_BUFFER_SIZE          0x0114  /* the buffer was too small to store the data, retry with a larger buffer */
#define NL_PACKET_SIZE          0x0115  /* the size of the packet exceeds NL_MAX_PACKET_LENGTH or the protocol max */
#define NL_WRONG_TYPE           0x0116  /* the function does not support the socket type */
#define NL_CON_PENDING          0x0117  /* a non-blocking connection is still pending */
#define NL_SELECT_NET_ERROR     0x0118  /* a network is already selected, call nlShutDown and nlInit first */
#define NL_CON_TERM             0x0119  /* the connection has been terminated */
#define NL_PACKET_SYNC          0x011a  /* the NL_RELIABLE_PACKET stream is out of sync */
#define NL_TLS_ERROR            0x011b  /* thread local storage could not be created */

/* standard multicast TTL settings as recommended by the */
/* white paper at http://www.ipmulticast.com/community/whitepapers/howipmcworks.html */
#define NL_TTL_LOCAL                1   /* local LAN only */
#define NL_TTL_SITE                 15  /* this site */
#define NL_TTL_REGION               63  /* this region */
#define NL_TTL_WORLD                127 /* the world */

/*

  Low level API, a thin layer over Sockets or other network provider.

*/

NL_EXP NLboolean NL_APIENTRY nlListen(NLsocket socket);

NL_EXP NLsocket  NL_APIENTRY nlAcceptConnection(NLsocket socket);

NL_EXP NLsocket  NL_APIENTRY nlOpen(NLushort port, NLenum type);

NL_EXP NLboolean NL_APIENTRY nlConnect(NLsocket socket, NLaddress *address);

NL_EXP void      NL_APIENTRY nlClose(NLsocket socket);

NL_EXP NLint     NL_APIENTRY nlRead(NLsocket socket, /*@out@*/ NLvoid *buffer, NLint nbytes);

NL_EXP NLint     NL_APIENTRY nlWrite(NLsocket socket, NLvoid *buffer, NLint nbytes);

NL_EXP NLlong    NL_APIENTRY nlGetSocketStat(NLsocket socket, NLenum name);

NL_EXP void      NL_APIENTRY nlClearSocketStat(NLsocket socket, NLenum name);

NL_EXP NLint     NL_APIENTRY nlPollGroup(NLint group, NLenum name, /*@out@*/ NLsocket *sockets, NLint number, NLint timeout);

NL_EXP void      NL_APIENTRY nlHint(NLenum name, NLint arg);

/* 

  Address management API

*/

NL_EXP /*@null@*/ NLbyte*   NL_APIENTRY nlAddrToString(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *string);

NL_EXP void      NL_APIENTRY nlStringToAddr(NLbyte *string, /*@out@*/ NLaddress *address);

NL_EXP void      NL_APIENTRY nlGetRemoteAddr(NLsocket socket, /*@out@*/ NLaddress *address);

NL_EXP void      NL_APIENTRY nlSetRemoteAddr(NLsocket socket, NLaddress *address);

NL_EXP void      NL_APIENTRY nlGetLocalAddr(NLsocket socket, /*@out@*/ NLaddress *address);

NL_EXP void      NL_APIENTRY nlSetLocalAddr(NLaddress *address);

NL_EXP /*@null@*/ NLbyte* NL_APIENTRY nlGetNameFromAddr(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *name);

NL_EXP void      NL_APIENTRY nlGetNameFromAddrAsync(NLaddress *address, /*@out@*/ NLbyte *name);

NL_EXP void      NL_APIENTRY nlGetAddrFromName(NLbyte *name, /*@out@*/ NLaddress *address);

NL_EXP void      NL_APIENTRY nlGetAddrFromNameAsync(NLbyte *name, /*@out@*/ NLaddress *address);

NL_EXP NLboolean NL_APIENTRY nlAddrCompare(NLaddress *address1, NLaddress *address2);

NL_EXP NLushort  NL_APIENTRY nlGetPortFromAddr(NLaddress *address);

NL_EXP void      NL_APIENTRY nlSetAddrPort(NLaddress *address, NLushort port);


/* 

  Group management API

 */

NL_EXP NLint     NL_APIENTRY nlGroupCreate(void);

NL_EXP void      NL_APIENTRY nlGroupDestroy(NLint group);

NL_EXP NLboolean NL_APIENTRY nlGroupAddSocket(NLint group, NLsocket socket);

NL_EXP void      NL_APIENTRY nlGroupGetSockets(NLint group, /*@out@*/ NLsocket *sockets, /*@out@*/ NLint *number);

NL_EXP void      NL_APIENTRY nlGroupDeleteSocket(NLint group, NLsocket socket);

/*

  Misc. API

*/

NL_EXP NLboolean NL_APIENTRY nlInit(void);

NL_EXP void      NL_APIENTRY nlShutdown(void);

NL_EXP NLboolean NL_APIENTRY nlSelectNetwork(NLenum network);

NL_EXP const /*@observer@*//*@null@*/ NLbyte* NL_APIENTRY nlGetString(NLenum name);

NL_EXP NLlong    NL_APIENTRY nlGetInteger(NLenum name);

NL_EXP NLboolean NL_APIENTRY nlGetBoolean(NLenum name);

NL_EXP void      NL_APIENTRY nlClear(NLenum name);

NL_EXP NLenum    NL_APIENTRY nlGetError(void);

NL_EXP const /*@observer@*/ NLbyte* NL_APIENTRY nlGetErrorStr(NLenum err);

NL_EXP NLint     NL_APIENTRY nlGetSystemError(void);

NL_EXP const /*@observer@*/ NLbyte* NL_APIENTRY nlGetSystemErrorStr(NLint err);

NL_EXP void      NL_APIENTRY nlEnable(NLenum name);

NL_EXP void      NL_APIENTRY nlDisable(NLenum name);

NL_EXP NLushort  NL_APIENTRY nlGetCRC16(NLubyte *data, NLint len);

NL_EXP NLulong   NL_APIENTRY nlGetCRC32(NLubyte *data, NLint len);

NL_EXP NLushort  NL_APIENTRY nlSwaps(NLushort x);

NL_EXP NLulong   NL_APIENTRY nlSwapl(NLulong x);

NL_EXP NLfloat   NL_APIENTRY nlSwapf(NLfloat f);

NL_EXP NLdouble  NL_APIENTRY nlSwapd(NLdouble d);


/* macros for writing/reading packet buffers */
/* NOTE: these also endian swap the data as needed */
/* write* or read* (buffer *, count, data [, length]) */

#ifdef NL_SAFE_COPY
#define writeShort(x, y, z)     {NLushort nl_temps = nlSwaps(z); memcpy((char *)&x[y], (char *)&nl_temps, 2); y += 2;}
#define writeLong(x, y, z)      {NLulong  nl_templ = nlSwapl(z); memcpy((char *)&x[y], (char *)&nl_templ, 4); y += 4;}
#define writeFloat(x, y, z)     {NLfloat  nl_tempf = nlSwapf(z); memcpy((char *)&x[y], (char *)&nl_tempf, 4); y += 4;}
#define writeDouble(x, y, z)    {NLdouble nl_tempd = nlSwapd(z); memcpy((char *)&x[y], (char *)&nl_tempd, 8); y += 8;}
#define readShort(x, y, z)      {memcpy((char *)&z, (char *)&x[y], 2); z = nlSwaps(z); y += 2;}
#define readLong(x, y, z)       {memcpy((char *)&z, (char *)&x[y], 4); z = nlSwapl(z); y += 4;}
#define readFloat(x, y, z)      {memcpy((char *)&z, (char *)&x[y], 4); z = nlSwapf(z); y += 4;}
#define readDouble(x, y, z)     {memcpy((char *)&z, (char *)&x[y], 8); z = nlSwapd(z); y += 8;}

#else
#define writeShort(x, y, z)     {*((NLushort *)((NLbyte *)&x[y])) = nlSwaps(z); y += 2;}
#define writeLong(x, y, z)      {*((NLulong  *)((NLbyte *)&x[y])) = nlSwapl(z); y += 4;}
#define writeFloat(x, y, z)     {*((NLfloat  *)((NLbyte *)&x[y])) = nlSwapf(z); y += 4;}
#define writeDouble(x, y, z)    {*((NLdouble *)((NLbyte *)&x[y])) = nlSwapd(z); y += 8;}
#define readShort(x, y, z)      {z = nlSwaps(*(NLushort *)((NLbyte *)&x[y])); y += 2;}
#define readLong(x, y, z)       {z = nlSwapl(*(NLulong  *)((NLbyte *)&x[y])); y += 4;}
#define readFloat(x, y, z)      {z = nlSwapf(*(NLfloat  *)((NLbyte *)&x[y])); y += 4;}
#define readDouble(x, y, z)     {z = nlSwapd(*(NLdouble *)((NLbyte *)&x[y])); y += 8;}
#endif

#define writeByte(x, y, z)      (*(NLbyte *)&x[y++] = (NLbyte)z)
#define writeString(x, y, z)    {strcpy((char *)&x[y], (char *)z); y += (strlen((char *)z) + 1);}
#define writeBlock(x, y, z, a)  {memcpy((char *)&x[y], (char *)z, a);y += a;}
#define readByte(x, y, z)       (z = *(NLbyte *)&x[y++])
#define readString(x, y, z)     {strcpy((char *)z, (char *)&x[y]); y += (strlen((char *)z) + 1);}
#define readBlock(x, y, z, a)   {memcpy((char *)z, (char *)&x[y], a);y += a;}

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* NL_H */

