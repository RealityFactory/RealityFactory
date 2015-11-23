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

#ifndef SOCKETS_H
#define SOCKETS_H

#ifdef __cplusplus
extern "C" {
#endif

NLboolean sock_Init(void);
void sock_Shutdown(void);
NLboolean sock_Listen(NLsocket socket);
NLsocket sock_AcceptConnection(NLsocket socket);
NLsocket sock_Open(NLushort port, NLenum type);
NLboolean sock_Connect(NLsocket socket, NLaddress *address);
void sock_Close(NLsocket socket);
NLint sock_Read(NLsocket socket, /*@out@*/ NLvoid *buffer, NLint nbytes);
NLint sock_Write(NLsocket socket, NLvoid *buffer, NLint nbytes);
NLbyte *sock_AddrToString(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *string);
void sock_StringToAddr(NLbyte *string, /*@out@*/ NLaddress *address);
void sock_GetLocalAddr(NLsocket socket, /*@out@*/ NLaddress *address);
void sock_SetLocalAddr(NLaddress *address);
NLbyte *sock_GetNameFromAddr(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *name);
void sock_GetNameFromAddrAsync(NLaddress *address, /*@out@*/ NLbyte *name);
void sock_GetAddrFromName(NLbyte *name, /*@out@*/ NLaddress *address);
void sock_GetAddrFromNameAsync(NLbyte *name, /*@out@*/ NLaddress *address);
NLboolean sock_AddrCompare(NLaddress *address1, NLaddress *address2);
NLushort sock_GetPortFromAddr(NLaddress *address);
void sock_SetAddrPort(NLaddress *address, NLushort port);
NLint sock_GetSystemError(void);
NLint sock_PollGroup(NLint group, NLenum name, /*@out@*/ NLsocket *sockets, NLint number, NLint timeout);
void sock_Hint(NLenum name, NLint arg);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* SOCKETS_H */

