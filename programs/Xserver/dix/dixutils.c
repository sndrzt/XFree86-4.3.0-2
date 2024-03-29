/* $XFree86: xc/programs/Xserver/dix/dixutils.c,v 3.12 2002/11/30 06:21:49 keithp Exp $ */
/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*

(c)Copyright 1988,1991 Adobe Systems Incorporated. All rights reserved.

Permission to use, copy, modify, distribute, and sublicense this software and its
documentation for any purpose and without fee is hereby granted, provided that
the above copyright notices appear in all copies and that both those copyright
notices and this permission notice appear in supporting documentation and that
the name of Adobe Systems Incorporated not be used in advertising or publicity
pertaining to distribution of the software without specific, written prior
permission.  No trademark license to use the Adobe trademarks is hereby
granted.  If the Adobe trademark "Display PostScript"(tm) is used to describe
this software, its functionality or for any other purpose, such use shall be
limited to a statement that this software works in conjunction with the Display
PostScript system.  Proper trademark attribution to reflect Adobe's ownership
of the trademark shall be given whenever any such reference to the Display
PostScript system is made.

ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR ANY
PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.  ADOBE
DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-
INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE TO YOU
OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,NEGLIGENCE, STRICT
LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT PROVIDE ANY TRAINING OR OTHER
SUPPORT FOR THE SOFTWARE.

Adobe, PostScript, and Display PostScript are trademarks of Adobe Systems
Incorporated which may be registered in certain jurisdictions.

Author:  Adobe Systems Incorporated

*/

/* $Xorg: dixutils.c,v 1.4 2001/02/09 02:04:40 xorgcvs Exp $ */

#include "X.h"
#include "Xmd.h"
#include "misc.h"
#include "windowstr.h"
#include "dixstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#define  XK_LATIN1
#include "keysymdef.h"
#ifdef XCSECURITY
#define _SECURITY_SERVER
#include "security.h"
#endif

/*
 * CompareTimeStamps returns -1, 0, or +1 depending on if the first
 * argument is less than, equal to or greater than the second argument.
 */

int
CompareTimeStamps(a, b)
    TimeStamp a, b;
{
    if (a.months < b.months)
	return EARLIER;
    if (a.months > b.months)
	return LATER;
    if (a.milliseconds < b.milliseconds)
	return EARLIER;
    if (a.milliseconds > b.milliseconds)
	return LATER;
    return SAMETIME;
}

/*
 * convert client times to server TimeStamps
 */

#define HALFMONTH ((unsigned long) 1<<31)
TimeStamp
ClientTimeToServerTime(c)
     CARD32 c;
{
    TimeStamp ts;
    if (c == CurrentTime)
	return currentTime;
    ts.months = currentTime.months;
    ts.milliseconds = c;
    if (c > currentTime.milliseconds)
    {
	if (((unsigned long) c - currentTime.milliseconds) > HALFMONTH)
	    ts.months -= 1;
    }
    else if (c < currentTime.milliseconds)
    {
	if (((unsigned long)currentTime.milliseconds - c) > HALFMONTH)
	    ts.months += 1;
    }
    return ts;
}

/*
 * ISO Latin-1 case conversion routine
 *
 * this routine always null-terminates the result, so
 * beware of too-small buffers
 */

void
CopyISOLatin1Lowered(dest, source, length)
    register unsigned char *dest, *source;
    int length;
{
    register int i;

    for (i = 0; i < length; i++, source++, dest++)
    {
	if ((*source >= XK_A) && (*source <= XK_Z))
	    *dest = *source + (XK_a - XK_A);
	else if ((*source >= XK_Agrave) && (*source <= XK_Odiaeresis))
	    *dest = *source + (XK_agrave - XK_Agrave);
	else if ((*source >= XK_Ooblique) && (*source <= XK_Thorn))
	    *dest = *source + (XK_oslash - XK_Ooblique);
	else
	    *dest = *source;
    }
    *dest = '\0';
}

#ifdef XCSECURITY

/* SecurityLookupWindow and SecurityLookupDrawable:
 * Look up the window/drawable taking into account the client doing
 * the lookup and the type of access desired.  Return the window/drawable
 * if it exists and the client is allowed access, else return NULL.
 * Most Proc* functions should be calling these instead of
 * LookupWindow and LookupDrawable, which do no access checks.
 */

WindowPtr
SecurityLookupWindow(rid, client, access_mode)
    XID rid;
    ClientPtr client;
    Mask access_mode;
{
    WindowPtr	pWin;

    client->errorValue = rid;
    if(rid == INVALID)
	return NULL;
    if (client->trustLevel != XSecurityClientTrusted)
	return (WindowPtr)SecurityLookupIDByType(client, rid, RT_WINDOW, access_mode);
    if (client->lastDrawableID == rid)
    {
        if (client->lastDrawable->type == DRAWABLE_WINDOW)
            return ((WindowPtr) client->lastDrawable);
        return (WindowPtr) NULL;
    }
    pWin = (WindowPtr)SecurityLookupIDByType(client, rid, RT_WINDOW, access_mode);
    if (pWin && pWin->drawable.type == DRAWABLE_WINDOW) {
	client->lastDrawable = (DrawablePtr) pWin;
	client->lastDrawableID = rid;
	client->lastGCID = INVALID;
	client->lastGC = (GCPtr)NULL;
    }
    return pWin;
}


pointer
SecurityLookupDrawable(rid, client, access_mode)
    XID rid;
    ClientPtr client;
    Mask access_mode;
{
    register DrawablePtr pDraw;

    if(rid == INVALID)
	return (pointer) NULL;
    if (client->trustLevel != XSecurityClientTrusted)
	return (DrawablePtr)SecurityLookupIDByClass(client, rid, RC_DRAWABLE,
						    access_mode);
    if (client->lastDrawableID == rid)
	return ((pointer) client->lastDrawable);
    pDraw = (DrawablePtr)SecurityLookupIDByClass(client, rid, RC_DRAWABLE,
						 access_mode);
    if (pDraw && (pDraw->type != UNDRAWABLE_WINDOW))
        return (pointer)pDraw;		
    return (pointer)NULL;
}

/* We can't replace the LookupWindow and LookupDrawable functions with
 * macros because of compatibility with loadable servers.
 */

WindowPtr
LookupWindow(rid, client)
    XID rid;
    ClientPtr client;
{
    return SecurityLookupWindow(rid, client, SecurityUnknownAccess);
}

pointer
LookupDrawable(rid, client)
    XID rid;
    ClientPtr client;
{
    return SecurityLookupDrawable(rid, client, SecurityUnknownAccess);
}

#else /* not XCSECURITY */

WindowPtr
LookupWindow(rid, client)
    XID rid;
    ClientPtr client;
{
    WindowPtr	pWin;

    client->errorValue = rid;
    if(rid == INVALID)
	return NULL;
    if (client->lastDrawableID == rid)
    {
        if (client->lastDrawable->type == DRAWABLE_WINDOW)
            return ((WindowPtr) client->lastDrawable);
        return (WindowPtr) NULL;
    }
    pWin = (WindowPtr)LookupIDByType(rid, RT_WINDOW);
    if (pWin && pWin->drawable.type == DRAWABLE_WINDOW) {
	client->lastDrawable = (DrawablePtr) pWin;
	client->lastDrawableID = rid;
	client->lastGCID = INVALID;
	client->lastGC = (GCPtr)NULL;
    }
    return pWin;
}


pointer
LookupDrawable(rid, client)
    XID rid;
    ClientPtr client;
{
    register DrawablePtr pDraw;

    if(rid == INVALID)
	return (pointer) NULL;
    if (client->lastDrawableID == rid)
	return ((pointer) client->lastDrawable);
    pDraw = (DrawablePtr)LookupIDByClass(rid, RC_DRAWABLE);
    if (pDraw && (pDraw->type != UNDRAWABLE_WINDOW))
        return (pointer)pDraw;		
    return (pointer)NULL;
}

#endif /* XCSECURITY */

ClientPtr
LookupClient(rid, client)
    XID rid;
    ClientPtr client;
{
    pointer pRes = (pointer)SecurityLookupIDByClass(client, rid, RC_ANY,
						    SecurityReadAccess);
    int clientIndex = CLIENT_ID(rid);

    if (clientIndex && pRes && clients[clientIndex] && !(rid & SERVER_BIT))
    {
	return clients[clientIndex];
    }
    return (ClientPtr)NULL;
}


int
AlterSaveSetForClient(client, pWin, mode)
    ClientPtr client;
    WindowPtr pWin;
    unsigned mode;
{
    int numnow;
    pointer *pTmp = NULL;
    int j;

    numnow = client->numSaved;
    j = 0;
    if (numnow)
    {
	pTmp = client->saveSet;
	while ((j < numnow) && (pTmp[j] != (pointer)pWin))
	    j++;
    }
    if (mode == SetModeInsert)
    {
	if (j < numnow)         /* duplicate */
	   return(Success);
	numnow++;
	pTmp = (pointer *)xrealloc(client->saveSet, sizeof(pointer) * numnow);
	if (!pTmp)
	    return(BadAlloc);
	client->saveSet = pTmp;
       	client->numSaved = numnow;
	client->saveSet[numnow - 1] = (pointer)pWin;
	return(Success);
    }
    else if ((mode == SetModeDelete) && (j < numnow))
    {
	while (j < numnow-1)
	{
           pTmp[j] = pTmp[j+1];
	   j++;
	}
	numnow--;
        if (numnow)
	{
    	    pTmp = (pointer *)xrealloc(client->saveSet,
				       sizeof(pointer) * numnow);
	    if (pTmp)
		client->saveSet = pTmp;
	}
        else
        {
            xfree(client->saveSet);
	    client->saveSet = (pointer *)NULL;
	}
	client->numSaved = numnow;
	return(Success);
    }
    return(Success);
}

void
DeleteWindowFromAnySaveSet(pWin)
    WindowPtr pWin;
{
    register int i;
    register ClientPtr client;
    
    for (i = 0; i< currentMaxClients; i++)
    {    
	client = clients[i];
	if (client && client->numSaved)
	    (void)AlterSaveSetForClient(client, pWin, SetModeDelete);
    }
}

/* No-op Don't Do Anything : sometimes we need to be able to call a procedure
 * that doesn't do anything.  For example, on screen with only static
 * colormaps, if someone calls install colormap, it's easier to have a dummy
 * procedure to call than to check if there's a procedure 
 */
void
NoopDDA(void)
{
}

typedef struct _BlockHandler {
    BlockHandlerProcPtr BlockHandler;
    WakeupHandlerProcPtr WakeupHandler;
    pointer blockData;
    Bool    deleted;
} BlockHandlerRec, *BlockHandlerPtr;

static BlockHandlerPtr	handlers;
static int		numHandlers;
static int		sizeHandlers;
static Bool		inHandler;
static Bool		handlerDeleted;

/* called from the OS layer */
void
BlockHandler(pTimeout, pReadmask)
pointer	pTimeout;	/* DIX doesn't want to know how OS represents time */
pointer pReadmask;	/* nor how it represents the set of descriptors */
{
    register int i, j;
    
    ++inHandler;
    for (i = 0; i < screenInfo.numScreens; i++)
	(* screenInfo.screens[i]->BlockHandler)(i, 
				screenInfo.screens[i]->blockData,
				pTimeout, pReadmask);
    for (i = 0; i < numHandlers; i++)
	(*handlers[i].BlockHandler) (handlers[i].blockData,
				     pTimeout, pReadmask);
    if (handlerDeleted)
    {
	for (i = 0; i < numHandlers;)
	    if (handlers[i].deleted)
	    {
	    	for (j = i; j < numHandlers - 1; j++)
		    handlers[j] = handlers[j+1];
	    	numHandlers--;
	    }
	    else
		i++;
	handlerDeleted = FALSE;
    }
    --inHandler;
}

void
WakeupHandler(result, pReadmask)
int	result;	/* 32 bits of undefined result from the wait */
pointer pReadmask;	/* the resulting descriptor mask */
{
    register int i, j;

    ++inHandler;
    for (i = numHandlers - 1; i >= 0; i--)
	(*handlers[i].WakeupHandler) (handlers[i].blockData,
				      result, pReadmask);
    for (i = 0; i < screenInfo.numScreens; i++)
	(* screenInfo.screens[i]->WakeupHandler)(i, 
				screenInfo.screens[i]->wakeupData,
				result, pReadmask);
    if (handlerDeleted)
    {
	for (i = 0; i < numHandlers;)
	    if (handlers[i].deleted)
	    {
	    	for (j = i; j < numHandlers - 1; j++)
		    handlers[j] = handlers[j+1];
	    	numHandlers--;
	    }
	    else
		i++;
	handlerDeleted = FALSE;
    }
    --inHandler;
}

/* Reentrant with BlockHandler and WakeupHandler, except wakeup won't
 * get called until next time
 */

Bool
RegisterBlockAndWakeupHandlers (blockHandler, wakeupHandler, blockData)
    BlockHandlerProcPtr blockHandler;
    WakeupHandlerProcPtr wakeupHandler;
    pointer blockData;
{
    BlockHandlerPtr new;

    if (numHandlers >= sizeHandlers)
    {
    	new = (BlockHandlerPtr) xrealloc (handlers, (numHandlers + 1) *
				      	  sizeof (BlockHandlerRec));
    	if (!new)
	    return FALSE;
    	handlers = new;
	sizeHandlers = numHandlers + 1;
    }
    handlers[numHandlers].BlockHandler = blockHandler;
    handlers[numHandlers].WakeupHandler = wakeupHandler;
    handlers[numHandlers].blockData = blockData;
    handlers[numHandlers].deleted = FALSE;
    numHandlers = numHandlers + 1;
    return TRUE;
}

void
RemoveBlockAndWakeupHandlers (blockHandler, wakeupHandler, blockData)
    BlockHandlerProcPtr blockHandler;
    WakeupHandlerProcPtr wakeupHandler;
    pointer blockData;
{
    int	    i;

    for (i = 0; i < numHandlers; i++)
	if (handlers[i].BlockHandler == blockHandler &&
	    handlers[i].WakeupHandler == wakeupHandler &&
	    handlers[i].blockData == blockData)
	{
	    if (inHandler)
	    {
		handlerDeleted = TRUE;
		handlers[i].deleted = TRUE;
	    }
	    else
	    {
	    	for (; i < numHandlers - 1; i++)
		    handlers[i] = handlers[i+1];
	    	numHandlers--;
	    }
	    break;
	}
}

void
InitBlockAndWakeupHandlers ()
{
    xfree (handlers);
    handlers = (BlockHandlerPtr) 0;
    numHandlers = 0;
    sizeHandlers = 0;
}

/*
 * A general work queue.  Perform some task before the server
 * sleeps for input.
 */

WorkQueuePtr		workQueue;
static WorkQueuePtr	*workQueueLast = &workQueue;

void
ProcessWorkQueue(void)
{
    WorkQueuePtr    q, *p;

    p = &workQueue;
    /*
     * Scan the work queue once, calling each function.  Those
     * which return TRUE are removed from the queue, otherwise
     * they will be called again.  This must be reentrant with
     * QueueWorkProc.
     */
    while ((q = *p))
    {
	if ((*q->function) (q->client, q->closure))
	{
	    /* remove q from the list */
	    *p = q->next;    /* don't fetch until after func called */
	    xfree (q);
	}
	else
	{
	    p = &q->next;    /* don't fetch until after func called */
	}
    }
    workQueueLast = p;
}

void
ProcessWorkQueueZombies(void)
{
    WorkQueuePtr    q, *p;

    p = &workQueue;
    while ((q = *p))
    {
	if (q->client && q->client->clientGone)
	{
	    (void) (*q->function) (q->client, q->closure);
	    /* remove q from the list */
	    *p = q->next;    /* don't fetch until after func called */
	    xfree (q);
	}
	else
	{
	    p = &q->next;    /* don't fetch until after func called */
	}
    }
    workQueueLast = p;
}

Bool
#if NeedFunctionPrototypes
QueueWorkProc (
    Bool	(*function)(
#if NeedNestedPrototypes
		ClientPtr	/* pClient */,
		pointer		/* closure */
#endif
		),
    ClientPtr	client,
    pointer	closure)
#else
QueueWorkProc (function, client, closure)
    Bool	(*function)();
    ClientPtr	client;
    pointer	closure;
#endif
{
    WorkQueuePtr    q;

    q = (WorkQueuePtr) xalloc (sizeof *q);
    if (!q)
	return FALSE;
    q->function = function;
    q->client = client;
    q->closure = closure;
    q->next = NULL;
    *workQueueLast = q;
    workQueueLast = &q->next;
    return TRUE;
}

/*
 * Manage a queue of sleeping clients, awakening them
 * when requested, by using the OS functions IgnoreClient
 * and AttendClient.  Note that this *ignores* the troubles
 * with request data interleaving itself with events, but
 * we'll leave that until a later time.
 */

typedef struct _SleepQueue {
    struct _SleepQueue	*next;
    ClientPtr		client;
    ClientSleepProcPtr  function;
    pointer		closure;
} SleepQueueRec, *SleepQueuePtr;

static SleepQueuePtr	sleepQueue = NULL;

Bool
ClientSleep (client, function, closure)
    ClientPtr	client;
    ClientSleepProcPtr function;
    pointer	closure;
{
    SleepQueuePtr   q;

    q = (SleepQueuePtr) xalloc (sizeof *q);
    if (!q)
	return FALSE;

    IgnoreClient (client);
    q->next = sleepQueue;
    q->client = client;
    q->function = function;
    q->closure = closure;
    sleepQueue = q;
    return TRUE;
}

Bool
ClientSignal (client)
    ClientPtr	client;
{
    SleepQueuePtr   q;

    for (q = sleepQueue; q; q = q->next)
	if (q->client == client)
	{
	    return QueueWorkProc (q->function, q->client, q->closure);
	}
    return FALSE;
}

void
ClientWakeup (client)
    ClientPtr	client;
{
    SleepQueuePtr   q, *prev;

    prev = &sleepQueue;
    while ( (q = *prev) )
    {
	if (q->client == client)
	{
	    *prev = q->next;
	    xfree (q);
	    if (client->clientGone)
		/* Oops -- new zombie cleanup code ensures this only
		 * happens from inside CloseDownClient; don't want to
		 * recurse here...
		 */
		/* CloseDownClient(client) */;
	    else
		AttendClient (client);
	    break;
	}
	prev = &q->next;
    }
}

Bool
ClientIsAsleep (client)
    ClientPtr	client;
{
    SleepQueuePtr   q;

    for (q = sleepQueue; q; q = q->next)
	if (q->client == client)
	    return TRUE;
    return FALSE;
}

/*
 *  Generic Callback Manager
 */

/* ===== Private Procedures ===== */

static int numCallbackListsToCleanup = 0;
static CallbackListPtr **listsToCleanup = NULL;

static Bool 
#if NeedFunctionPrototypes
_AddCallback(
    CallbackListPtr *pcbl,
    CallbackProcPtr callback,
    pointer         data)
#else
_AddCallback(pcbl, callback, data)
    CallbackListPtr *pcbl;
    CallbackProcPtr callback;
    pointer         data;
#endif
{
    CallbackPtr     cbr;

    cbr = (CallbackPtr) xalloc(sizeof(CallbackRec));
    if (!cbr)
	return FALSE;
    cbr->proc = callback;
    cbr->data = data;
    cbr->next = (*pcbl)->list;
    cbr->deleted = FALSE;
    (*pcbl)->list = cbr;
    return TRUE;
}

static Bool 
#if NeedFunctionPrototypes
_DeleteCallback(
    CallbackListPtr *pcbl,
    CallbackProcPtr callback,
    pointer         data)
#else
_DeleteCallback(pcbl, callback, data)
    CallbackListPtr *pcbl;
    CallbackProcPtr callback;
    pointer         data;
#endif
{
    CallbackListPtr cbl = *pcbl;
    CallbackPtr     cbr, pcbr;

    for (pcbr = NULL, cbr = cbl->list;
	 cbr != NULL;
	 pcbr = cbr, cbr = cbr->next)
    {
	if ((cbr->proc == callback) && (cbr->data == data))
	    break;
    }
    if (cbr != NULL)
    {
	if (cbl->inCallback)
	{
	    ++(cbl->numDeleted);
	    cbr->deleted = TRUE;
	}
	else
	{
	    if (pcbr == NULL)
		cbl->list = cbr->next;
	    else
		pcbr->next = cbr->next;
	    xfree(cbr);
	}
	return TRUE;
    }
    return FALSE;
}

static void 
#if NeedFunctionPrototypes
_CallCallbacks(
    CallbackListPtr    *pcbl,
    pointer	    call_data)
#else
_CallCallbacks(pcbl, call_data)
    CallbackListPtr    *pcbl;
    pointer	    call_data;
#endif
{
    CallbackListPtr cbl = *pcbl;
    CallbackPtr     cbr, pcbr;

    ++(cbl->inCallback);
    for (cbr = cbl->list; cbr != NULL; cbr = cbr->next)
    {
	(*(cbr->proc)) (pcbl, cbr->data, call_data);
    }
    --(cbl->inCallback);

    if (cbl->inCallback) return;

    /* Was the entire list marked for deletion? */

    if (cbl->deleted)
    {
	DeleteCallbackList(pcbl);
	return;
    }

    /* Were some individual callbacks on the list marked for deletion?
     * If so, do the deletions.
     */

    if (cbl->numDeleted)
    {
	for (pcbr = NULL, cbr = cbl->list; (cbr != NULL) && cbl->numDeleted; )
	{
	    if (cbr->deleted)
	    {
		if (pcbr)
		{
		    cbr = cbr->next;
		    xfree(pcbr->next);
		    pcbr->next = cbr;
		} else
		{
		    cbr = cbr->next;
		    xfree(cbl->list);
		    cbl->list = cbr;
		}
		cbl->numDeleted--;
	    }
	    else /* this one wasn't deleted */
	    {
		pcbr = cbr;
		cbr = cbr->next;
	    }
	}
    }
}

static void
#if NeedFunctionPrototypes
_DeleteCallbackList(
    CallbackListPtr    *pcbl)
#else
_DeleteCallbackList(pcbl)
    CallbackListPtr    *pcbl;
#endif
{
    CallbackListPtr cbl = *pcbl;
    CallbackPtr     cbr, nextcbr;
    int i;

    if (cbl->inCallback)
    {
	cbl->deleted = TRUE;
	return;
    }

    for (i = 0; i < numCallbackListsToCleanup; i++)
    {
	if ((listsToCleanup[i] = pcbl) != 0)
	{
	    listsToCleanup[i] = NULL;
	    break;
	}
    }

    for (cbr = cbl->list; cbr != NULL; cbr = nextcbr)
    {
	nextcbr = cbr->next;
	xfree(cbr);
    }
    xfree(cbl);
    *pcbl = NULL;
}

static CallbackFuncsRec default_cbfuncs =
{
    _AddCallback,
    _DeleteCallback,
    _CallCallbacks,
    _DeleteCallbackList
};

/* ===== Public Procedures ===== */

Bool
CreateCallbackList(pcbl, cbfuncs)
    CallbackListPtr  *pcbl;
    CallbackFuncsPtr cbfuncs;
{
    CallbackListPtr  cbl;
    int i;

    if (!pcbl) return FALSE;
    cbl = (CallbackListPtr) xalloc(sizeof(CallbackListRec));
    if (!cbl) return FALSE;
    cbl->funcs = cbfuncs ? *cbfuncs : default_cbfuncs;
    cbl->inCallback = 0;
    cbl->deleted = FALSE;
    cbl->numDeleted = 0;
    cbl->list = NULL;
    *pcbl = cbl;

    for (i = 0; i < numCallbackListsToCleanup; i++)
    {
	if (!listsToCleanup[i])
	{
	    listsToCleanup[i] = pcbl;
	    return TRUE;
	}    
    }

    listsToCleanup = (CallbackListPtr **)xnfrealloc(listsToCleanup,
		sizeof(CallbackListPtr *) * (numCallbackListsToCleanup+1));
    listsToCleanup[numCallbackListsToCleanup] = pcbl;
    numCallbackListsToCleanup++;
    return TRUE;
}

Bool 
AddCallback(pcbl, callback, data)
    CallbackListPtr *pcbl;
    CallbackProcPtr callback;
    pointer         data;
{
    if (!pcbl) return FALSE;
    if (!*pcbl)
    {	/* list hasn't been created yet; go create it */
	if (!CreateCallbackList(pcbl, (CallbackFuncsPtr)NULL))
	    return FALSE;
    }
    return ((*(*pcbl)->funcs.AddCallback) (pcbl, callback, data));
}

Bool 
DeleteCallback(pcbl, callback, data)
    CallbackListPtr *pcbl;
    CallbackProcPtr callback;
    pointer         data;
{
    if (!pcbl || !*pcbl) return FALSE;
    return ((*(*pcbl)->funcs.DeleteCallback) (pcbl, callback, data));
}

void 
CallCallbacks(pcbl, call_data)
    CallbackListPtr    *pcbl;
    pointer	    call_data;
{
    if (!pcbl || !*pcbl) return;
    (*(*pcbl)->funcs.CallCallbacks) (pcbl, call_data);
}

void
DeleteCallbackList(pcbl)
    CallbackListPtr    *pcbl;
{
    if (!pcbl || !*pcbl) return;
    (*(*pcbl)->funcs.DeleteCallbackList) (pcbl);
}

void 
InitCallbackManager()
{
    int i;

    for (i = 0; i < numCallbackListsToCleanup; i++)
    {
	DeleteCallbackList(listsToCleanup[i]);
    }
    if (listsToCleanup) xfree(listsToCleanup);

    numCallbackListsToCleanup = 0;
    listsToCleanup = NULL;
}
