/* $XFree86: xc/lib/GL/mesa/src/drv/r128/r128_lock.c,v 1.4 2002/02/22 21:44:58 dawes Exp $ */
/**************************************************************************

Copyright 1999, 2000 ATI Technologies Inc. and Precision Insight, Inc.,
                                               Cedar Park, Texas.
All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
on the rights to use, copy, modify, merge, publish, distribute, sub
license, and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
ATI, PRECISION INSIGHT AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

/*
 * Authors:
 *   Gareth Hughes <gareth@valinux.com>
 *
 */

#include "r128_context.h"
#include "r128_lock.h"
#include "r128_tex.h"

#if DEBUG_LOCKING
char *prevLockFile = NULL;
int prevLockLine = 0;
#endif


/* Update the hardware state.  This is called if another context has
 * grabbed the hardware lock, which includes the X server.  This
 * function also updates the driver's window state after the X server
 * moves, resizes or restacks a window -- the change will be reflected
 * in the drawable position and clip rects.  Since the X server grabs
 * the hardware lock when it changes the window state, this routine will
 * automatically be called after such a change.
 */
void r128GetLock( r128ContextPtr rmesa, GLuint flags )
{
   __DRIdrawablePrivate *dPriv = rmesa->driDrawable;
   __DRIscreenPrivate *sPriv = rmesa->driScreen;
   R128SAREAPrivPtr sarea = rmesa->sarea;
   int i;

   drmGetLock( rmesa->driFd, rmesa->hHWContext, flags );

   /* The window might have moved, so we might need to get new clip
    * rects.
    *
    * NOTE: This releases and regrabs the hw lock to allow the X server
    * to respond to the DRI protocol request for new drawable info.
    * Since the hardware state depends on having the latest drawable
    * clip rects, all state checking must be done _after_ this call.
    */
   DRI_VALIDATE_DRAWABLE_INFO( rmesa->display, sPriv, dPriv );

   if ( rmesa->lastStamp != dPriv->lastStamp ) {
      rmesa->lastStamp = dPriv->lastStamp;
      rmesa->new_state |= R128_NEW_WINDOW | R128_NEW_CLIP;
      rmesa->SetupNewInputs = ~0;
   }

   rmesa->dirty |= R128_UPLOAD_CONTEXT | R128_UPLOAD_CLIPRECTS;

   rmesa->numClipRects = dPriv->numClipRects;
   rmesa->pClipRects = dPriv->pClipRects;

   if ( sarea->ctxOwner != rmesa->hHWContext ) {
      sarea->ctxOwner = rmesa->hHWContext;
      rmesa->dirty = R128_UPLOAD_ALL;
   }

   for ( i = 0 ; i < rmesa->lastTexHeap ; i++ ) {
      if ( rmesa->texHeap[i] && sarea->texAge[i] != rmesa->lastTexAge[i] ) {
	 r128AgeTextures( rmesa, i );
      }
   }
}
