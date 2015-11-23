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

#ifndef IPX_H
#define IPX_H

#ifdef __cplusplus
extern "C" {
#endif

NLboolean ipx_Init(void);
void ipx_Shutdown(void);
NLboolean ipx_Listen(NLsocket socket);
NLulong ipx_Peek(NLsocket socket);
NLsocket ipx_AcceptConnection(NLsocket socket);
NLsocket ipx_Open(NLushort port, NLenum type);
NLboolean ipx_Connect(NLsocket socket, NLaddress *address);
void ipx_Close(NLsocket socket);
NLint ipx_Read(NLsocket socket, /*@out@*/ NLvoid *buffer, NLint nbytes);
NLint ipx_Write(NLsocket socket, NLvoid *buffer, NLint nbytes);
NLbyte *ipx_AddrToString(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *string);
void ipx_StringToAddr(NLbyte *string, /*@out@*/ NLaddress *address);
void ipx_GetLocalAddr(NLsocket socket, /*@out@*/ NLaddress *address);
void ipx_SetLocalAddr(NLaddress *address);
NLbyte *ipx_GetNameFromAddr(NLaddress *address, /*@returned@*/ /*@out@*/ NLbyte *name);
void ipx_GetNameFromAddrAsync(NLaddress *address, /*@out@*/ NLbyte *name);
void ipx_GetAddrFromName(NLbyte *name, /*@out@*/ NLaddress *address);
void ipx_GetAddrFromNameAsync(NLbyte *name, /*@out@*/ NLaddress *address);
NLboolean ipx_AddrCompare(NLaddress *address1, NLaddress *address2);
NLushort ipx_GetPortFromAddr(NLaddress *address);
void ipx_SetAddrPort(NLaddress *address, NLushort port);
NLint ipx_GetSystemError(void);
NLint ipx_PollGroup(NLint group, NLenum name, /*@out@*/ NLsocket *sockets, NLint number, NLint timeout);
void ipx_Hint(NLenum name, NLint arg);


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* IPX_H */

