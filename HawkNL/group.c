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

#include <stdlib.h>
#include <string.h>

#define FD_SETSIZE              8192

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

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET int
#endif

#include "nlinternal.h"

static NLmutex  grouplock;

typedef struct
{
    NLsocket    sockets[NL_MAX_GROUP_SOCKETS];  /* the list of sockets in this group */
    fd_set      fdset;                          /* for nlPollGroup */
    SOCKET      highest;                        /* for nlPollGroup */
} nl_group_t;

static /*@only@*/ nl_group_t **groups;
static NLint nlnextgroup = 0;
static NLint nlnumgroups = 0;


/* Internal functions */

void nlGroupLock(void)
{
    nlMutexLock(&grouplock);
}

void nlGroupUnlock(void)
{
    nlMutexUnlock(&grouplock);
}

NLboolean nlGroupInit(void)
{
    if(groups == NULL)
    {
        groups = (nl_group_t **)malloc(NL_MAX_GROUPS * sizeof(nl_group_t *));
        memset(groups, 0, NL_MAX_GROUPS * sizeof(nl_group_t *));
    }
    if(groups == NULL)
    {
        nlSetError(NL_OUT_OF_MEMORY);
        return NL_FALSE;
    }

    nlMutexInit(&grouplock);
    return NL_TRUE;
}

void nlGroupShutdown(void)
{
    if(groups != NULL)
    {
        NLint i;

        for(i=0;i<NL_MAX_GROUPS;i++)
        {
            if(groups[i] != NULL)
            {
                free(groups[i]);
            }
        }
        free(groups);
        groups = NULL;
    }
    nlMutexDestroy(&grouplock);
}

SOCKET nlGroupGetFdset(NLint group, fd_set *fd)
{
    NLint       realgroup = group - NL_FIRST_GROUP;
    nl_group_t  *pgroup = NULL;

    if(groups == NULL)
    {
        nlSetError(NL_NO_NETWORK);
        return INVALID_SOCKET;
    }
    if(realgroup < 0)
    {
        nlSetError(NL_INVALID_GROUP);
        return INVALID_SOCKET;
    }
    pgroup = groups[realgroup];
    if(pgroup == NULL)
    {
        nlSetError(NL_INVALID_GROUP);
        return INVALID_SOCKET;
    }
    memcpy(fd, &pgroup->fdset, sizeof(fd_set));

    return pgroup->highest;
}

/* Group management API */

NL_EXP NLint NL_APIENTRY nlGroupCreate(void)
{
    NLint       newgroup = NL_INVALID;
    nl_group_t  *pgroup = NULL;

    if(groups == NULL)
    {
        nlSetError(NL_NO_NETWORK);
        return NL_INVALID;
    }
    nlMutexLock(&grouplock);
    if(nlnumgroups == NL_MAX_GROUPS)
    {
        nlSetError(NL_OUT_OF_GROUPS);
        nlMutexUnlock(&grouplock);
        return NL_INVALID;
    }
    /* get a group number */
    if(nlnumgroups == nlnextgroup)
    {
        newgroup = nlnextgroup++;
    }
    else
    /* there is an open group slot somewhere below nlnextgroup */
    {
        NLint   i;

        for(i=0;i<nlnextgroup;i++)
        {
            if(groups[i] == NULL)
            {
                /* found an open group slot */
                newgroup = i;
            }
        }
        /* let's check just to make sure we did find a group */
        if(newgroup == NL_INVALID)
        {
            nlSetError(NL_OUT_OF_MEMORY);
            nlMutexUnlock(&grouplock);
            return NL_INVALID;
        }
    }
    /* allocate the memory */
    pgroup = (nl_group_t *)malloc((size_t)(sizeof(nl_group_t)));
    if(pgroup == NULL)
    {
        nlSetError(NL_OUT_OF_MEMORY);
        nlMutexUnlock(&grouplock);
        return NL_INVALID;
    }
    else
    {
        NLint   i;

        /* fill with -1, since 0 is a valid socket number */
        for(i=0;i<NL_MAX_GROUP_SOCKETS;i++)
        {
            pgroup->sockets[i] =  -1;
        }
        FD_ZERO(&pgroup->fdset);
        pgroup->highest = 0;
        groups[newgroup] = pgroup;
    }
    
    nlnumgroups++;
    nlMutexUnlock(&grouplock);
    /* adjust the group number */
    return (newgroup + NL_FIRST_GROUP);
}

NL_EXP void NL_APIENTRY nlGroupDestroy(NLint group)
{
    NLint   realgroup = group - NL_FIRST_GROUP;

    if(groups == NULL)
    {
        nlSetError(NL_NO_NETWORK);
        return;
    }
    if(realgroup < 0)
    {
        nlSetError(NL_INVALID_GROUP);
        return;
    }
    nlMutexLock(&grouplock);
    if(groups[realgroup] != NULL)
    {
        free(groups[realgroup]);
        groups[realgroup] = NULL;
    }
    nlMutexUnlock(&grouplock);
}

NL_EXP NLboolean NL_APIENTRY nlGroupAddSocket(NLint group, NLsocket socket)
{
    NLint       realgroup = group - NL_FIRST_GROUP;
    SOCKET      realsock;
    NLint       i;
    nl_group_t  *pgroup = NULL;

    if(groups == NULL)
    {
        nlSetError(NL_NO_NETWORK);
        return NL_FALSE;
    }
    if(realgroup < 0)
    {
        nlSetError(NL_INVALID_GROUP);
        return NL_FALSE;
    }
    /* make sure the socket is valid */
    if(nlIsValidSocket(socket) == NL_FALSE)
    {
        nlSetError(NL_INVALID_SOCKET);
        return NL_FALSE;
    }
    realsock = (SOCKET)nlSockets[socket]->realsocket;

    /* add the socket to the group */
    nlMutexLock(&grouplock);
    pgroup = groups[realgroup];
    for(i=0;i<NL_MAX_GROUP_SOCKETS;i++)
    {
        if(pgroup->sockets[i] == -1)
        {
            pgroup->sockets[i] = socket;
            FD_SET(realsock, &pgroup->fdset);
            if(pgroup->highest < realsock)
            {
                pgroup->highest = realsock;
            }
            break;
        }
    }
    if(i == NL_MAX_GROUP_SOCKETS)
    {
        nlSetError(NL_OUT_OF_GROUP_SOCKETS);
        nlMutexUnlock(&grouplock);
        return NL_FALSE;
    }
    nlMutexUnlock(&grouplock);
    return NL_TRUE;
}

void nlGroupGetSocketsINT(NLint group, NLsocket *socket, NLint *number)
{
    NLint       realgroup = group - NL_FIRST_GROUP;
    NLint       len;
    NLint       count = 0;
    NLint       next;
    nl_group_t  *pgroup = NULL;

    if(groups == NULL)
    {
        nlSetError(NL_NO_NETWORK);
        return;
    }
    if(socket == NULL || number == NULL)
    {
        nlSetError(NL_NULL_POINTER);
        return;
    }
    if(realgroup < 0)
    {
        nlSetError(NL_INVALID_GROUP);
        *number = 0;
        return;
    }
	len = *number;
    if(len > NL_MAX_GROUP_SOCKETS)
    {
        len = NL_MAX_GROUP_SOCKETS;
    }
    pgroup = groups[realgroup];
    next = pgroup->sockets[count];
    while(next != -1 && count <= len)
    {
        socket[count++] = next;
        next = pgroup->sockets[count];
    }
    *number = count;
}

NL_EXP void NL_APIENTRY nlGroupGetSockets(NLint group, NLsocket *socket, NLint *number)
{
    nlMutexLock(&grouplock);
    nlGroupGetSocketsINT(group, socket, number);
    nlMutexUnlock(&grouplock);
}
NL_EXP void NL_APIENTRY nlGroupDeleteSocket(NLint group, NLsocket socket)
{
    NLint       realgroup = group - NL_FIRST_GROUP;
    SOCKET      realsock;
    NLint       i;
    nl_group_t  *pgroup = NULL;

    if(groups == NULL)
    {
        nlSetError(NL_NO_NETWORK);
        return;
    }
    if(realgroup < 0)
    {
        nlSetError(NL_INVALID_GROUP);
        return;
    }
    /* make sure the socket is valid */
    if(nlIsValidSocket(socket) == NL_FALSE)
    {
        nlSetError(NL_INVALID_SOCKET);
        return;
    }
    realsock = (SOCKET)nlSockets[socket]->realsocket;

    /* delete the socket from the group */
    nlMutexLock(&grouplock);
    pgroup = groups[realgroup];
    for(i=0;i<NL_MAX_GROUP_SOCKETS;i++)
    {
        /* check for match */
        if(pgroup->sockets[i] == socket)
            break;
        /* check for end of list */
        if(pgroup->sockets[i] == -1)
            break;
    }
    if(i == NL_MAX_GROUP_SOCKETS)
    {
        /* did not find the socket */
        nlMutexUnlock(&grouplock);
        return;
    }
    /* now pgroup[i] points to the socket to delete */
    /* shift all other sockets down to close the gap */
    i++;
    for(;i<NL_MAX_GROUP_SOCKETS;i++)
    {
        pgroup->sockets[i - 1] = pgroup->sockets[i];
        /* check for end of list */
        if(pgroup->sockets[i] == -1)
            break;
    }
    FD_CLR(realsock, &pgroup->fdset);
    nlMutexUnlock(&grouplock);
}
