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

#ifndef LOOPBACK_H
#define LOOPBACK_H

#ifdef __cplusplus
extern "C" {
#endif

NLboolean loopback_Init(void);
void loopback_Shutdown(void);
NLboolean loopback_Listen(NLsocket socket);
NLsocket loopback_AcceptConnection(NLsocket socket);
NLsocket loopback_Open(NLushort port, NLenum type);
NLboolean loopback_Connect(NLsocket socket, NLaddress *address);
void loopback_Close(NLsocket socket);
NLint loopback_Read(NLsocket socket, /*@out@*/ NLvoid *buffer, NLint nbytes);
NLint loopback_Write(NLsocket socket, NLvoid *buffer, NLint nbytes);
NLbyte *loopback_AddrToString(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *string);
void loopback_StringToAddr(NLbyte *string, /*@out@*/ NLaddress *address);
void loopback_GetLocalAddr(NLsocket socket, /*@out@*/ NLaddress *address);
void loopback_SetLocalAddr(NLaddress *address);
NLbyte *loopback_GetNameFromAddr(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *name);
void loopback_GetNameFromAddrAsync(NLaddress *address, /*@out@*/ NLbyte *name);
void loopback_GetAddrFromName(NLbyte *name, /*@out@*/ NLaddress *address);
void loopback_GetAddrFromNameAsync(NLbyte *name, /*@out@*/ NLaddress *address);
NLboolean loopback_AddrCompare(NLaddress *address1, NLaddress *address2);
NLushort loopback_GetPortFromAddr(NLaddress *address);
void loopback_SetAddrPort(NLaddress *address, NLushort port);
NLint loopback_GetSystemError(void);
NLint loopback_PollGroup(NLint group, NLenum name, /*@out@*/ NLsocket *sockets, NLint number, NLint timeout);
void loopback_Hint(NLenum name, NLint arg);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* LOOPBACK_H */

