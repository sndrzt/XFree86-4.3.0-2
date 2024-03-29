/*
 * Copyright 1998 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors:  Alan Hourihane, <alanh@fairlite.demon.co.uk>
 *
 * Generic RAMDAC access routines.
 */
/* $XFree86: xc/programs/Xserver/hw/xfree86/ramdac/xf86RamDac.c,v 1.5 1999/01/14 13:05:21 dawes Exp $ */

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86_ansic.h"

#include "xf86RamDacPriv.h"

int RamDacHWPrivateIndex = -1;
int RamDacScreenPrivateIndex = -1;

RamDacRecPtr
RamDacCreateInfoRec()
{
    RamDacRecPtr infoRec;

    infoRec = xcalloc(1, sizeof(RamDacRec));

    return infoRec;
}

RamDacHelperRecPtr
RamDacHelperCreateInfoRec()
{
    RamDacHelperRecPtr infoRec;

    infoRec = xcalloc(1, sizeof(RamDacHelperRec));

    return infoRec;
}

void 
RamDacDestroyInfoRec(RamDacRecPtr infoRec)
{
    xfree(infoRec);
}

void 
RamDacHelperDestroyInfoRec(RamDacHelperRecPtr infoRec)
{
    xfree(infoRec);
}

Bool
RamDacInit(ScrnInfoPtr pScrn, RamDacRecPtr ramdacPriv)
{
    RamDacHWRecPtr ramdacHWPtr;
    RamDacRegRecPtr ramdacReg;
    RamDacScreenRecPtr ramdacScrPtr;

    /*
     * make sure the RamDacRec is allocated
     */
    if (!RamDacGetRec(pScrn))
	return FALSE;
    ramdacHWPtr = RAMDACHWPTR(pScrn);
    ramdacReg = &ramdacHWPtr->ModeReg;
    ramdacScrPtr = ((RamDacScreenRecPtr)
			(pScrn)->privates[RamDacGetScreenIndex()].ptr);
    ramdacScrPtr->RamDacRec = ramdacPriv;

    return(TRUE);
}

void
RamDacGetRecPrivate()
{
    if (RamDacHWPrivateIndex < 0)
	RamDacHWPrivateIndex = xf86AllocateScrnInfoPrivateIndex();
    if (RamDacScreenPrivateIndex < 0)
	RamDacScreenPrivateIndex = xf86AllocateScrnInfoPrivateIndex();
    return;
}

Bool
RamDacGetRec(ScrnInfoPtr scrp)
{
    RamDacRegRecPtr regp;
    
    RamDacGetRecPrivate();
    /*
     * New privates are always set to NULL, so we can check if the allocation
     * has already been done.
     */
    if (scrp->privates[RamDacHWPrivateIndex].ptr != NULL)
	return TRUE;
    if (scrp->privates[RamDacScreenPrivateIndex].ptr != NULL)
	return TRUE;

    scrp->privates[RamDacHWPrivateIndex].ptr = 
					xnfcalloc(sizeof(RamDacHWRec), 1);
    scrp->privates[RamDacScreenPrivateIndex].ptr = 
					xnfcalloc(sizeof(RamDacScreenRec), 1);
    /* Does this really belong here? */
    regp = &((RamDacHWRecPtr)scrp->privates[RamDacHWPrivateIndex].ptr)->ModeReg;
    
    return TRUE;
}

void
RamDacFreeRec(ScrnInfoPtr pScrn)
{
    RamDacHWRecPtr ramdacHWPtr;
    RamDacScreenRecPtr ramdacScrPtr;

    if (RamDacHWPrivateIndex < 0)
	return;

    if (RamDacScreenPrivateIndex < 0)
	return;

    ramdacHWPtr = RAMDACHWPTR(pScrn);
    ramdacScrPtr = ((RamDacScreenRecPtr)
				(pScrn)->privates[RamDacGetScreenIndex()].ptr);
    
    if (ramdacHWPtr)
	xfree(ramdacHWPtr);
    ramdacHWPtr = NULL;

    if (ramdacScrPtr)
	xfree(ramdacScrPtr);
    ramdacScrPtr = NULL;
}

int
RamDacGetHWIndex()
{
    return RamDacHWPrivateIndex;
}

int
RamDacGetScreenIndex()
{
    return RamDacScreenPrivateIndex;
}
