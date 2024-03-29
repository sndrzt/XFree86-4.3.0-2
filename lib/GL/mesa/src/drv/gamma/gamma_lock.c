/* $XFree86: xc/lib/GL/mesa/src/drv/gamma/gamma_lock.c,v 1.3tsi Exp $ */

#include "gamma_context.h"

#ifdef DEBUG_LOCKING
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
void gammaGetLock( gammaContextPtr gmesa, GLuint flags )
{
   __DRIdrawablePrivate *dPriv = gmesa->driDrawable;
   __DRIscreenPrivate *sPriv = gmesa->driScreen;

   drmGetLock( gmesa->driFd, gmesa->hHWContext, flags );

   /* The window might have moved, so we might need to get new clip
    * rects.
    *
    * NOTE: This releases and regrabs the hw lock to allow the X server
    * to respond to the DRI protocol request for new drawable info.
    * Since the hardware state depends on having the latest drawable
    * clip rects, all state checking must be done _after_ this call.
    */
   DRI_VALIDATE_DRAWABLE_INFO( gmesa->display, sPriv, dPriv );

   if ( gmesa->lastStamp != dPriv->lastStamp ) {
      gmesa->lastStamp = dPriv->lastStamp;
      gmesa->new_state |= GAMMA_NEW_WINDOW | GAMMA_NEW_CLIP;
   }

   gmesa->numClipRects = dPriv->numClipRects;
   gmesa->pClipRects = dPriv->pClipRects;

#if 0
   gmesa->dirty = ~0;

   if ( sarea->ctxOwner != gmesa->hHWContext ) {
      sarea->ctxOwner = gmesa->hHWContext;
      gmesa->dirty = GAMMA_UPLOAD_ALL;
   }

   for ( i = 0 ; i < gmesa->lastTexHeap ; i++ ) {
      if ( sarea->texAge[i] != gmesa->lastTexAge[i] ) {
	 gammaAgeTextures( gmesa, i );
      }
   }
#endif
}
