/*
 *Copyright (C) 1994-2000 The XFree86 Project, Inc. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE XFREE86 PROJECT BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of the XFree86 Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors:	Dakshinamurthy Karra
 *		Suhaib M Siddiqi
 *		Peter Busch
 *		Harold L Hunt II
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winshaddd.c,v 1.22 2002/10/17 08:18:24 alanh Exp $ */

#include "win.h"

/*
 * FIXME: Headers are broken, DEFINE_GUID doesn't work correctly,
 * so we have to redefine it here.
 */
#ifdef DEFINE_GUID
#undef DEFINE_GUID
#define DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) GUID_EXT const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#endif /* DEFINE_GUID */


/*
 * FIXME: Headers are broken, IID_IDirectDraw4 has to be defined
 * here manually.  Should be handled by ddraw.h
 */
#ifndef IID_IDirectDraw2
DEFINE_GUID( IID_IDirectDraw2,0xB3A6F3E0,0x2B43,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 );
#endif /* IID_IDirectDraw2 */


/*
 * Create the primary surface and attach the clipper.
 * Used for both the initial surface creation and during
 * WM_DISPLAYCHANGE messages.
 */

Bool
winCreatePrimarySurfaceShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  HRESULT		ddrval = DD_OK;
  DDSURFACEDESC		ddsd;

  /* Describe the primary surface */
  ZeroMemory (&ddsd, sizeof (ddsd));
  ddsd.dwSize = sizeof (ddsd);
  ddsd.dwFlags = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  
  /* Create the primary surface */
  ddrval = IDirectDraw2_CreateSurface (pScreenPriv->pdd2,
				       &ddsd,
				       &pScreenPriv->pddsPrimary,
				       NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winCreatePrimarySurfaceShadowDD - Could not create primary "
	      "surface: %08x\n", ddrval);
      return FALSE;
    }
  
#if CYGDEBUG
  ErrorF ("winCreatePrimarySurfaceShadowDD - Created primary surface\n");
#endif

  /*
   * Attach a clipper to the primary surface that will clip our blits to our
   * display window.
   */
  ddrval = IDirectDrawSurface2_SetClipper (pScreenPriv->pddsPrimary,
					   pScreenPriv->pddcPrimary);
  if (FAILED (ddrval))
    {
      ErrorF ("winCreatePrimarySurfaceShadowDD - Primary attach clipper "
	      "failed: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winCreatePrimarySurfaceShadowDD - Attached clipper to "
	  "primary surface\n");
#endif

  /* Everything was correct */
  return TRUE;
}


/*
 * Detach the clipper and release the primary surface.
 * Called from WM_DISPLAYCHANGE.
 */

Bool
winReleasePrimarySurfaceShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);

  ErrorF ("winReleasePrimarySurfaceShadowDD - Hello\n");

  /* Release the primary surface and clipper, if they exist */
  if (pScreenPriv->pddsPrimary)
    {
      /*
       * Detach the clipper from the primary surface.
       * NOTE: We do this explicity for clarity.  The Clipper is not released.
       */
      IDirectDrawSurface2_SetClipper (pScreenPriv->pddsPrimary,
				      NULL);

      ErrorF ("winReleasePrimarySurfaceShadowDD - Detached clipper\n");

      /* Release the primary surface */
      IDirectDrawSurface2_Release (pScreenPriv->pddsPrimary);
      pScreenPriv->pddsPrimary = NULL;
    }

  ErrorF ("winReleasePrimarySurfaceShadowDD - Released primary surface\n");

  return TRUE;
}


/*
 * Create a DirectDraw surface for the shadow framebuffer; also create
 * a primary surface object so we can blit to the display.
 * 
 * Install a DirectDraw clipper on our primary surface object
 * that clips our blits to the unobscured client area of our display window.
 */

Bool
winAllocateFBShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;  
  HRESULT		ddrval = DD_OK;
  DDSURFACEDESC		ddsd;
  DDSURFACEDESC		*pddsdShadow = NULL;

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD\n");
#endif

  /* Create a clipper */
  ddrval = (*g_fpDirectDrawCreateClipper) (0,
					   &pScreenPriv->pddcPrimary,
					   NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDD - Could not create clipper: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD - Created a clipper\n");
#endif

  /* Get a device context for the screen  */
  pScreenPriv->hdcScreen = GetDC (pScreenPriv->hwndScreen);

  /* Attach the clipper to our display window */
  ddrval = IDirectDrawClipper_SetHWnd (pScreenPriv->pddcPrimary,
				       0,
				       pScreenPriv->hwndScreen);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDD - Clipper not attached to "
	      "window: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD - Attached clipper to window\n");
#endif

  /* Create a DirectDraw object, store the address at lpdd */
  ddrval = (*g_fpDirectDrawCreate) (NULL, &pScreenPriv->pdd, NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDD - Could not start DirectDraw: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD () - Created and initialized DD\n");
#endif

  /* Get a DirectDraw2 interface pointer */
  ddrval = IDirectDraw_QueryInterface (pScreenPriv->pdd,
				       &IID_IDirectDraw2,
				       (LPVOID*) &pScreenPriv->pdd2);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDD - Failed DD2 query: %08x\n",
	      ddrval);
      return FALSE;
    }

  /* Are we full screen? */
  if (pScreenInfo->fFullScreen)
    {
      DDSURFACEDESC	ddsdCurrent;
      DWORD		dwRefreshRateCurrent = 0;
      HDC		hdc = NULL;

      /* Set the cooperative level to full screen */
      ddrval = IDirectDraw2_SetCooperativeLevel (pScreenPriv->pdd2,
						 pScreenPriv->hwndScreen,
						 DDSCL_EXCLUSIVE
						 | DDSCL_FULLSCREEN);
      if (FAILED (ddrval))
	{
	  ErrorF ("winAllocateFBShadowDD - Could not set "
		  "cooperative level: %08x\n",
		  ddrval);
	  return FALSE;
	}

      /*
       * We only need to get the current refresh rate for comparison
       * if a refresh rate has been passed on the command line.
       */
      if (pScreenInfo->dwRefreshRate != 0)
	{
	  ZeroMemory (&ddsdCurrent, sizeof (ddsdCurrent));
	  ddsdCurrent.dwSize = sizeof (ddsdCurrent);
	  
	  /* Get information about current display settings */
	  ddrval = IDirectDraw2_GetDisplayMode (pScreenPriv->pdd2,
						&ddsdCurrent);
	  if (FAILED (ddrval))
	    {
	      ErrorF ("winAllocateFBShadowDD - Could not get current "
		      "refresh rate: %08x.  Continuing.\n",
		      ddrval);
	      dwRefreshRateCurrent = 0;
	    }
	  else
	    {
	      /* Grab the current refresh rate */
	      dwRefreshRateCurrent = ddsdCurrent.u2.dwRefreshRate;
	    }
	}

      /* Clean up the refresh rate */
      if (dwRefreshRateCurrent == pScreenInfo->dwRefreshRate)
	{
	  /*
	   * Refresh rate is non-specified or equal to current.
	   */
	  pScreenInfo->dwRefreshRate = 0;
	}

      /* Grab a device context for the screen */
      hdc = GetDC (NULL);
      if (hdc == NULL)
	{
	  ErrorF ("winAllocateFBShadowDD - GetDC () failed\n");
	  return FALSE;
	}

      /* Only change the video mode when different than current mode */
      if (!pScreenInfo->fMultipleMonitors
	  && (pScreenInfo->dwWidth != GetSystemMetrics (SM_CXSCREEN)
	      || pScreenInfo->dwHeight != GetSystemMetrics (SM_CYSCREEN)
	      || pScreenInfo->dwBPP != GetDeviceCaps (hdc, BITSPIXEL)
	      || pScreenInfo->dwRefreshRate != 0))
	{
	  ErrorF ("winAllocateFBShadowDD - Changing video mode\n");

	  /* Change the video mode to the mode requested */
	  ddrval = IDirectDraw2_SetDisplayMode (pScreenPriv->pdd2,
						pScreenInfo->dwWidth,
						pScreenInfo->dwHeight,
						pScreenInfo->dwBPP,
						pScreenInfo->dwRefreshRate,
						0);
	  if (FAILED (ddrval))
	    {
	      ErrorF ("winAllocateFBShadowDD - Could not set "\
		      "full screen display mode: %08x\n",
		      ddrval);
	      return FALSE;
	    }
	}
      else
	{
	  ErrorF ("winAllocateFBShadowDD - Not changing video mode\n");
	}

      /* Release our DC */
      ReleaseDC (NULL, hdc);
      hdc = NULL;
    }
  else
    {
      /* Set the cooperative level for windowed mode */
      ddrval = IDirectDraw2_SetCooperativeLevel (pScreenPriv->pdd2,
						 pScreenPriv->hwndScreen,
						 DDSCL_NORMAL);
      if (FAILED (ddrval))
	{
	  ErrorF ("winAllocateFBShadowDD - Could not set "\
		  "cooperative level: %08x\n",
		  ddrval);
	  return FALSE;
	}
    }

  /* Create the primary surface */
  if (!winCreatePrimarySurfaceShadowDD (pScreen))
    {
      ErrorF ("winAllocateFBShadowDD - winCreatePrimarySurfaceShadowDD "
	      "failed\n");
      return FALSE;
    }

  /* Describe the shadow surface to be created */
  /* NOTE: Do not use a DDSCAPS_VIDEOMEMORY surface,
   * as drawing, locking, and unlocking take forever
   * with video memory surfaces.  In addition,
   * video memory is a somewhat scarce resource,
   * so you shouldn't be allocating video memory when
   * you have the option of using system memory instead.
   */
  ZeroMemory (&ddsd, sizeof (ddsd));
  ddsd.dwSize = sizeof (ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  ddsd.dwHeight = pScreenInfo->dwHeight;
  ddsd.dwWidth = pScreenInfo->dwWidth;

  /* Create the shadow surface */
  ddrval = IDirectDraw2_CreateSurface (pScreenPriv->pdd2,
				       &ddsd,
				       &pScreenPriv->pddsShadow,
				       NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDD - Could not create shadow "\
	      "surface: %08x\n", ddrval);
      return FALSE;
    }
  
#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD - Created shadow\n");
#endif

  /* Allocate a DD surface description for our screen privates */
  pddsdShadow = pScreenPriv->pddsdShadow = malloc (sizeof (DDSURFACEDESC));
  if (pddsdShadow == NULL)
    {
      ErrorF ("winAllocateFBShadowDD - Could not allocate surface "\
	      "description memory\n");
      return FALSE;
    }
  ZeroMemory (pddsdShadow, sizeof (*pddsdShadow));
  pddsdShadow->dwSize = sizeof (*pddsdShadow);

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD - Locking shadow\n");
#endif

  /* Lock the shadow surface */
  ddrval = IDirectDrawSurface2_Lock (pScreenPriv->pddsShadow,
				     NULL,
				     pddsdShadow,
				     DDLOCK_WAIT,
				     NULL);
  if (FAILED (ddrval) || pddsdShadow->lpSurface == NULL)
    {
      ErrorF ("winAllocateFBShadowDD - Could not lock shadow "\
	      "surface: %08x\n", ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD - Locked shadow\n");
#endif

  /* We don't know how to deal with anything other than RGB */
  if (!(pddsdShadow->ddpfPixelFormat.dwFlags & DDPF_RGB))
    {
      ErrorF ("winAllocateFBShadowDD - Color format other than RGB\n");
      return FALSE;
    }

  /* Grab the pitch from the surface desc */
  pScreenInfo->dwStride = (pddsdShadow->u1.lPitch * 8)
    / pScreenInfo->dwBPP;

  /* Save the pointer to our surface memory */
  pScreenInfo->pfb = pddsdShadow->lpSurface;
  
  /* Grab the color depth and masks from the surface description */
  pScreenPriv->dwRedMask = pddsdShadow->ddpfPixelFormat.u2.dwRBitMask;
  pScreenPriv->dwGreenMask = pddsdShadow->ddpfPixelFormat.u3.dwGBitMask;
  pScreenPriv->dwBlueMask = pddsdShadow->ddpfPixelFormat.u4.dwBBitMask;

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDD - Returning\n");
#endif

  return TRUE;
}


/*
 * Transfer the damaged regions of the shadow framebuffer to the display.
 */

void
winShadowUpdateDD (ScreenPtr pScreen, 
		   shadowBufPtr pBuf)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  RegionPtr		damage = &pBuf->damage;
  HRESULT		ddrval = DD_OK;
  RECT			rcDest, rcSrc;
  POINT			ptOrigin;
  DWORD			dwBox = REGION_NUM_RECTS (damage);
  BoxPtr		pBox = REGION_RECTS (damage);
  HRGN			hrgnTemp = NULL, hrgnCombined = NULL;

  /*
   * Return immediately if the app is not active
   * and we are fullscreen, or if we have a bad display depth
   */
  if ((!pScreenPriv->fActive && pScreenInfo->fFullScreen)
      || pScreenPriv->fBadDepth) return;

  /* Get the origin of the window in the screen coords */
  ptOrigin.x = pScreenInfo->dwXOffset;
  ptOrigin.y = pScreenInfo->dwYOffset;
  MapWindowPoints (pScreenPriv->hwndScreen,
		   HWND_DESKTOP,
		   (LPPOINT)&ptOrigin, 1);

  /* Unlock the shadow surface, so we can blit */
  ddrval = IDirectDrawSurface2_Unlock (pScreenPriv->pddsShadow, NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winShadowUpdateProcDD - Unlock failed\n");
      return;
    }

  /*
   * Handle small regions with multiple blits,
   * handle large regions by creating a clipping region and 
   * doing a single blit constrained to that clipping region.
   */
  if (pScreenInfo->dwClipUpdatesNBoxes == 0
      || dwBox < pScreenInfo->dwClipUpdatesNBoxes)
    {
      /* Loop through all boxes in the damaged region */
      while (dwBox--)
	{
	  /* Assign damage box to source rectangle */
	  rcSrc.left = pBox->x1;
	  rcSrc.top = pBox->y1;
	  rcSrc.right = pBox->x2;
	  rcSrc.bottom = pBox->y2;
	  
	  /* Calculate destination rectange */
	  rcDest.left = ptOrigin.x + rcSrc.left;
	  rcDest.top = ptOrigin.y + rcSrc.top;
	  rcDest.right = ptOrigin.x + rcSrc.right;
	  rcDest.bottom = ptOrigin.y + rcSrc.bottom;
	  
	  /* Blit the damaged areas */
	  ddrval = IDirectDrawSurface2_Blt (pScreenPriv->pddsPrimary,
					    &rcDest,
					    pScreenPriv->pddsShadow,
					    &rcSrc,
					    DDBLT_WAIT,
					    NULL);
	  
	  /* Get a pointer to the next box */
	  ++pBox;
	}
    }
  else
    {
      BoxPtr		pBoxExtents = REGION_EXTENTS (pScreen, damage);

      /* Compute a GDI region from the damaged region */
      hrgnCombined = CreateRectRgn (pBox->x1, pBox->y1, pBox->x2, pBox->y2);
      dwBox--;
      pBox++;
      while (dwBox--)
	{
	  hrgnTemp = CreateRectRgn (pBox->x1, pBox->y1, pBox->x2, pBox->y2);
	  CombineRgn (hrgnCombined, hrgnCombined, hrgnTemp, RGN_OR);
	  DeleteObject (hrgnTemp);
	  pBox++;
	}  

      /* Install the GDI region as a clipping region */
      SelectClipRgn (pScreenPriv->hdcScreen, hrgnCombined);
      DeleteObject (hrgnCombined);
      hrgnCombined = NULL;

      /* Calculating a bounding box for the source is easy */
      rcSrc.left = pBoxExtents->x1;
      rcSrc.top = pBoxExtents->y1;
      rcSrc.right = pBoxExtents->x2;
      rcSrc.bottom = pBoxExtents->y2;

      /* Calculating a bounding box for the destination is trickier */
      rcDest.left = ptOrigin.x + rcSrc.left;
      rcDest.top = ptOrigin.y + rcSrc.top;
      rcDest.right = ptOrigin.x + rcSrc.right;
      rcDest.bottom = ptOrigin.y + rcSrc.bottom;
      
      /* Our Blt should be clipped to the invalidated region */
      ddrval = IDirectDrawSurface2_Blt (pScreenPriv->pddsPrimary,
					&rcDest,
					pScreenPriv->pddsShadow,
					&rcSrc,
					DDBLT_WAIT,
					NULL);

      /* Reset the clip region */
      SelectClipRgn (pScreenPriv->hdcScreen, NULL);
    }

  /* Relock the shadow surface */
  ddrval = IDirectDrawSurface2_Lock (pScreenPriv->pddsShadow,
				     NULL,
				     pScreenPriv->pddsdShadow,
				     DDLOCK_WAIT,
				     NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winShadowUpdateProcDD - Lock failed\n");
      return;
    }

  /* Has our memory pointer changed? */
  if (pScreenInfo->pfb != pScreenPriv->pddsdShadow->lpSurface)
    {
      ErrorF ("winShadowUpdateProcDD - Memory location of the shadow "
	      "surface has changed, trying to update the root window "
	      "pixmap header to point to the new address.  If you get "
	      "this message and Cygwin/XFree86 freezes or crashes "
	      "after this message then send a problem report and your "
	      "/tmp/XWin.log file to cygwin-xfree@cygwin.com\n");

      /* Location of shadow framebuffer has changed */
      pScreenInfo->pfb = pScreenPriv->pddsdShadow->lpSurface;
      
      /* Update the screen pixmap */
      if (!(*pScreen->ModifyPixmapHeader)(pScreen->devPrivate,
					  pScreen->width,
					  pScreen->height,
					  pScreen->rootDepth,
					  BitsPerPixel (pScreen->rootDepth),
					  PixmapBytePad (pScreenInfo->dwStride,
							 pScreenInfo->dwBPP),
					  pScreenInfo->pfb))
	{
	  ErrorF ("winShadowUpdateProcDD - Bits changed, could not "
		  "notify fb.\n");
	  return;
	}
    }
}


/*
 * Call the wrapped CloseScreen function.
 * 
 * Free our resources and private structures.
 */

Bool
winCloseScreenShadowDD (int nIndex, ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  Bool			fReturn;
  
#if CYGDEBUG
  ErrorF ("winCloseScreenShadowDD - Freeing screen resources\n");
#endif

  /* Flag that the screen is closed */
  pScreenPriv->fClosed = TRUE;
  pScreenPriv->fActive = FALSE;

  /* Call the wrapped CloseScreen procedure */
  pScreen->CloseScreen = pScreenPriv->CloseScreen;
  fReturn = (*pScreen->CloseScreen) (nIndex, pScreen);

  /* Free the screen DC */
  ReleaseDC (pScreenPriv->hwndScreen, pScreenPriv->hdcScreen);

  /* Delete the window property */
  RemoveProp (pScreenPriv->hwndScreen, WIN_SCR_PROP);

  /* Free the shadow surface, if there is one */
  if (pScreenPriv->pddsShadow)
    {
      IDirectDrawSurface2_Unlock (pScreenPriv->pddsShadow, NULL);
      IDirectDrawSurface2_Release (pScreenPriv->pddsShadow);
      pScreenPriv->pddsShadow = NULL;
    }

  /* Detach the clipper from the primary surface and release the clipper. */
  if (pScreenPriv->pddcPrimary)
    {
      /* Detach the clipper */
      IDirectDrawSurface2_SetClipper (pScreenPriv->pddsPrimary,
				      NULL);

      /* Release the clipper object */
      IDirectDrawClipper_Release (pScreenPriv->pddcPrimary);
      pScreenPriv->pddcPrimary = NULL;
    }

  /* Release the primary surface, if there is one */
  if (pScreenPriv->pddsPrimary)
    {
      IDirectDrawSurface2_Release (pScreenPriv->pddsPrimary);
      pScreenPriv->pddsPrimary = NULL;
    }

  /* Free the DirectDraw2 object, if there is one */
  if (pScreenPriv->pdd2)
    {
      IDirectDraw2_RestoreDisplayMode (pScreenPriv->pdd2);
      IDirectDraw2_Release (pScreenPriv->pdd2);
      pScreenPriv->pdd2 = NULL;
    }

  /* Free the DirectDraw object, if there is one */
  if (pScreenPriv->pdd)
    {
      IDirectDraw_Release (pScreenPriv->pdd);
      pScreenPriv->pdd = NULL;
    }

  /* Kill our window */
  if (pScreenPriv->hwndScreen)
    {
      DestroyWindow (pScreenPriv->hwndScreen);
      pScreenPriv->hwndScreen = NULL;
    }

  /* Destroy the thread startup mutex */
  pthread_mutex_destroy (&pScreenPriv->pmServerStarted);

  /* Kill our screeninfo's pointer to the screen */
  pScreenInfo->pScreen = NULL;

  /* Invalidate the ScreenInfo's fb pointer */
  pScreenInfo->pfb = NULL;

  /* Free the screen privates for this screen */
  free ((pointer) pScreenPriv);

  return fReturn;
}


/*
 * Tell mi what sort of visuals we need.
 * 
 * Generally we only need one visual, as our screen can only
 * handle one format at a time, I believe.  You may want
 * to verify that last sentence.
 */

Bool
winInitVisualsShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  DWORD			dwRedBits, dwGreenBits, dwBlueBits;

  /* Count the number of ones in each color mask */
  dwRedBits = winCountBits (pScreenPriv->dwRedMask);
  dwGreenBits = winCountBits (pScreenPriv->dwGreenMask);
  dwBlueBits = winCountBits (pScreenPriv->dwBlueMask);
  
  /* Store the maximum number of ones in a color mask as the bitsPerRGB */
  if (dwRedBits == 0 || dwGreenBits == 0 || dwBlueBits == 0)
    pScreenPriv->dwBitsPerRGB = 8;
  else if (dwRedBits > dwGreenBits && dwRedBits > dwBlueBits)
    pScreenPriv->dwBitsPerRGB = dwRedBits;
  else if (dwGreenBits > dwRedBits && dwGreenBits > dwBlueBits)
    pScreenPriv->dwBitsPerRGB = dwGreenBits;
  else
    pScreenPriv->dwBitsPerRGB = dwBlueBits;
  
  ErrorF ("winInitVisualsShadowDD - Masks %08x %08x %08x BPRGB %d d %d "
	  "bpp %d\n",
	  pScreenPriv->dwRedMask,
	  pScreenPriv->dwGreenMask,
	  pScreenPriv->dwBlueMask,
	  pScreenPriv->dwBitsPerRGB,
	  pScreenInfo->dwDepth,
	  pScreenInfo->dwBPP);

  /* Create a single visual according to the Windows screen depth */
  switch (pScreenInfo->dwDepth)
    {
    case 24:
    case 16:
    case 15:
#if defined(XFree86Server)
      /* Create the real visual */
      if (!miSetVisualTypesAndMasks (pScreenInfo->dwDepth,
				     TrueColorMask,
				     pScreenPriv->dwBitsPerRGB,
				     TrueColor,
				     pScreenPriv->dwRedMask,
				     pScreenPriv->dwGreenMask,
				     pScreenPriv->dwBlueMask))
	{
	  ErrorF ("winInitVisualsShadowDD - miSetVisualTypesAndMasks "
		  "failed for TrueColor\n");
	  return FALSE;
	}

#if WIN_EMULATE_PSEUDO_SUPPORT
      if (!pScreenInfo->fEmulatePseudo)
	break;

      /* Setup a pseudocolor visual */
      if (!miSetVisualTypesAndMasks (8,
				     PseudoColorMask,
				     8,
				     -1,
				     0,
				     0,
				     0))
	{
	  ErrorF ("winInitVisualsShadowDD - miSetVisualTypesAndMasks "
		  "failed for PseudoColor\n");
	  return FALSE;
	}
#endif
#else /* XFree86Server */
      /* Create the real visual */
      if (!fbSetVisualTypesAndMasks (pScreenInfo->dwDepth,
				     TrueColorMask,
				     pScreenPriv->dwBitsPerRGB,
				     pScreenPriv->dwRedMask,
				     pScreenPriv->dwGreenMask,
				     pScreenPriv->dwBlueMask))
	{
	  ErrorF ("winInitVisualsShadowDD - fbSetVisualTypesAndMasks "
		  "failed for TrueColor\n");
	  return FALSE;
	}

#if WIN_EMULATE_PSEUDO_SUPPORT
      if (!pScreenInfo->fEmulatePseudo)
	break;

      /* Setup a pseudocolor visual */
      if (!fbSetVisualTypesAndMasks (8,
				     PseudoColorMask,
				     8,
				     0,
				     0,
				     0))
	{
	  ErrorF ("winInitVisualsShadowDD - fbSetVisualTypesAndMasks "
		  "failed for PseudoColor\n");
	  return FALSE;
	}
#endif
#endif /* XFree86Server */
      break;

    case 8:
#if defined(XFree86Server)
      if (!miSetVisualTypesAndMasks (pScreenInfo->dwDepth,
				     pScreenInfo->fFullScreen 
				     ? PseudoColorMask : StaticColorMask,
				     pScreenPriv->dwBitsPerRGB,
				     pScreenInfo->fFullScreen 
				     ? PseudoColor : StaticColor,
				     pScreenPriv->dwRedMask,
				     pScreenPriv->dwGreenMask,
				     pScreenPriv->dwBlueMask))
	{
	  ErrorF ("winInitVisualsShadowDD - miSetVisualTypesAndMasks "
		  "failed\n");
	  return FALSE;
	}
#else /* XFree86Server */
      if (!fbSetVisualTypesAndMasks (pScreenInfo->dwDepth,
				     pScreenInfo->fFullScreen 
				     ? PseudoColorMask : StaticColorMask,
				     pScreenPriv->dwBitsPerRGB,
				     pScreenPriv->dwRedMask,
				     pScreenPriv->dwGreenMask,
				     pScreenPriv->dwBlueMask))
	{
	  ErrorF ("winInitVisualsShadowDD - fbSetVisualTypesAndMasks "
		  "failed\n");
	  return FALSE;
	}
#endif /* XFree86Server */
      break;

    default:
      ErrorF ("winInitVisualsShadowDD - Unknown screen depth\n");
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winInitVisualsShadowDD - Returning\n");
#endif

  return TRUE;
}


/*
 * Adjust the user proposed video mode
 */

Bool
winAdjustVideoModeShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  HDC			hdc = NULL;
  DWORD			dwBPP;

  /* We're in serious trouble if we can't get a DC */
  hdc = GetDC (NULL);
  if (hdc == NULL)
    {
      ErrorF ("winAdjustVideoModeShadowDD - GetDC () failed\n");
      return FALSE;
    }

  /* Query GDI for current display depth */
  dwBPP = GetDeviceCaps (hdc, BITSPIXEL);

  /* DirectDraw can only change the depth in fullscreen mode */
  if (pScreenInfo->dwBPP == WIN_DEFAULT_BPP)
    {
      /* No -depth parameter passed, let the user know the depth being used */
      ErrorF ("winAdjustVideoModeShadowDD - Using Windows display "
	      "depth of %d bits per pixel\n", dwBPP);

      /* Use GDI's depth */
      pScreenInfo->dwBPP = dwBPP;
    }
  else if (pScreenInfo->fFullScreen
	   && pScreenInfo->dwBPP != dwBPP)
    {
      /* FullScreen, and GDI depth differs from -depth parameter */
      ErrorF ("winAdjustVideoModeShadowDD - FullScreen, using command line "
	      "bpp: %d\n", pScreenInfo->dwBPP);
    }
  else if (dwBPP != pScreenInfo->dwBPP)
    {
      /* Windowed, and GDI depth differs from -depth parameter */
      ErrorF ("winAdjustVideoModeShadowDD - Windowed, command line bpp: "
	      "%d, using bpp: %d\n", pScreenInfo->dwBPP, dwBPP);

      /* We'll use GDI's depth */
      pScreenInfo->dwBPP = dwBPP;
    }
  
  /* See if the shadow bitmap will be larger than the DIB size limit */
  if (pScreenInfo->dwWidth * pScreenInfo->dwHeight * pScreenInfo->dwBPP
      >= WIN_DIB_MAXIMUM_SIZE)
    {
      ErrorF ("winAdjustVideoModeShadowDD - Requested DirectDraw surface "
	      "will be larger than %d MB.  The surface may fail to be "
	      "allocated on Windows 95, 98, or Me, due to a %d MB limit in "
	      "DIB size.  This limit does not apply to Windows NT/2000, and "
	      "this message may be ignored on those platforms.\n",
	      WIN_DIB_MAXIMUM_SIZE_MB, WIN_DIB_MAXIMUM_SIZE_MB);
    }

  /* Release our DC */
  ReleaseDC (NULL, hdc);
  return TRUE;
}


/*
 * Blt exposed regions to the screen
 */

Bool
winBltExposedRegionsShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  RECT			rcSrc, rcDest;
  POINT			ptOrigin;
  HDC			hdcUpdate = NULL;
  PAINTSTRUCT		ps;
  HRESULT		ddrval = DD_OK;
  Bool			fReturn = TRUE;
  Bool			fLocked = TRUE;
  int			i;

  /* BeginPaint gives us an hdc that clips to the invalidated region */
  hdcUpdate = BeginPaint (pScreenPriv->hwndScreen, &ps);
  if (hdcUpdate == NULL)
    {
      ErrorF ("winBltExposedRegionsShadowDD - BeginPaint () returned "
	      "a NULL device context handle.  Aborting blit attempt.\n");
      return FALSE;
    }
  
  /* Unlock the shadow surface, so we can blit */
  ddrval = IDirectDrawSurface2_Unlock (pScreenPriv->pddsShadow, NULL);
  if (FAILED (ddrval))
    {
      fReturn = FALSE;
      goto winBltExposedRegionsShadowDD_Exit;
    }
  else
    {
      /* Flag that we have unlocked the shadow surface */
      fLocked = FALSE;
    }

  /* Get the origin of the window in the screen coords */
  ptOrigin.x = pScreenInfo->dwXOffset;
  ptOrigin.y = pScreenInfo->dwYOffset;

  MapWindowPoints (pScreenPriv->hwndScreen,
		   HWND_DESKTOP,
		   (LPPOINT)&ptOrigin, 1);
  rcDest.left = ptOrigin.x;
  rcDest.right = ptOrigin.x + pScreenInfo->dwWidth;
  rcDest.top = ptOrigin.y;
  rcDest.bottom = ptOrigin.y + pScreenInfo->dwHeight;

  /* Source can be enter shadow surface, as Blt should clip */
  rcSrc.left = 0;
  rcSrc.top = 0;
  rcSrc.right = pScreenInfo->dwWidth;
  rcSrc.bottom = pScreenInfo->dwHeight;

  /* Try to regain the primary surface and blit again if we've lost it */
  for (i = 0; i <= WIN_REGAIN_SURFACE_RETRIES; ++i)
    {
      /* Our Blt should be clipped to the invalidated region */
      ddrval = IDirectDrawSurface2_Blt (pScreenPriv->pddsPrimary,
					&rcDest,
					pScreenPriv->pddsShadow,
					&rcSrc,
					DDBLT_WAIT,
					NULL);
      if (ddrval == DDERR_SURFACELOST)
	{
	  /* Surface was lost */
	  ErrorF ("winBltExposedRegionsShadowDD - IDirectDrawSurface2_Blt "
		  "reported that the primary surface was lost, "
		  "trying to restore, retry: %d\n", i + 1);

	  /* Try to restore the surface, once */
	  ddrval = IDirectDrawSurface2_Restore (pScreenPriv->pddsPrimary);
	  ErrorF ("winBltExposedRegionsShadowDDNL - "
		  "IDirectDrawSurface2_Restore returned: ");
	  if (ddrval == DD_OK)
	    continue;
	  else if (ddrval == DDERR_WRONGMODE)
	    ErrorF ("DDERR_WRONGMODE\n");
	  else if (ddrval == DDERR_INCOMPATIBLEPRIMARY)
	    ErrorF ("DDERR_INCOMPATIBLEPRIMARY\n");
	  else if (ddrval == DDERR_UNSUPPORTED)
	    ErrorF ("DDERR_UNSUPPORTED\n");
	  else if (ddrval == DDERR_INVALIDPARAMS)
	    ErrorF ("DDERR_INVALIDPARAMS\n");
	  else if (ddrval == DDERR_INVALIDOBJECT)
	    ErrorF ("DDERR_INVALIDOBJECT\n");
	  else
	    ErrorF ("unknown error: %08x\n", ddrval);

	  /* Loop around to try the blit one more time */
	  continue;
	}
      else if (FAILED (ddrval))
	{
	  fReturn = FALSE;
	  ErrorF ("winBltExposedRegionsShadowDD - IDirectDrawSurface2_Blt "
		  "failed, but surface not lost: %08x %d\n", ddrval, ddrval);
	  goto winBltExposedRegionsShadowDD_Exit;
	}
      else
	{
	  /* Success, stop looping */
	  break;
	}
    }

  /* Relock the shadow surface */
  ddrval = IDirectDrawSurface2_Lock (pScreenPriv->pddsShadow,
				     NULL,
				     pScreenPriv->pddsdShadow,
				     DDLOCK_WAIT,
				     NULL);
  if (FAILED (ddrval))
    {
      fReturn = FALSE;
      ErrorF ("winBltExposedRegionsShadowDD - IDirectDrawSurface2_Lock "
	      "failed\n");
      goto winBltExposedRegionsShadowDD_Exit;
    }
  else
    {
      /* Indicate that we have relocked the shadow surface */
      fLocked = TRUE;
    }

  /* Has our memory pointer changed? */
  if (pScreenInfo->pfb != pScreenPriv->pddsdShadow->lpSurface)
    winUpdateFBPointer (pScreen,
			pScreenPriv->pddsdShadow->lpSurface);

 winBltExposedRegionsShadowDD_Exit:
  /* EndPaint frees the DC */
  if (hdcUpdate != NULL)
    EndPaint (pScreenPriv->hwndScreen, &ps);

  /*
   * Relock the surface if it is not locked.  We don't care if locking fails,
   * as it will cause the server to shutdown within a few more operations.
   */
  if (!fLocked)
    {
      IDirectDrawSurface2_Lock (pScreenPriv->pddsShadow,
				NULL,
				pScreenPriv->pddsdShadow,
				DDLOCK_WAIT,
				NULL);

      /* Has our memory pointer changed? */
      if (pScreenInfo->pfb != pScreenPriv->pddsdShadow->lpSurface)
	winUpdateFBPointer (pScreen,
			    pScreenPriv->pddsdShadow->lpSurface);
      
      fLocked = TRUE;
    }
  return fReturn;
}


/*
 * Do any engine-specific appliation-activation processing
 */

Bool
winActivateAppShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);

  /*
   * Do we have a surface?
   * Are we active?
   * Are we fullscreen?
   */
  if (pScreenPriv != NULL
      && pScreenPriv->pddsPrimary != NULL
      && pScreenPriv->fActive)
    {
      /* Primary surface was lost, restore it */
      IDirectDrawSurface2_Restore (pScreenPriv->pddsPrimary);
    }

  return TRUE;
}


/*
 * Reblit the shadow framebuffer to the screen.
 */

Bool
winRedrawScreenShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  HRESULT		ddrval = DD_OK;
  RECT			rcSrc, rcDest;
  POINT			ptOrigin;

  /* Get the origin of the window in the screen coords */
  ptOrigin.x = pScreenInfo->dwXOffset;
  ptOrigin.y = pScreenInfo->dwYOffset;
  MapWindowPoints (pScreenPriv->hwndScreen,
		   HWND_DESKTOP,
		   (LPPOINT)&ptOrigin, 1);
  rcDest.left = ptOrigin.x;
  rcDest.right = ptOrigin.x + pScreenInfo->dwWidth;
  rcDest.top = ptOrigin.y;
  rcDest.bottom = ptOrigin.y + pScreenInfo->dwHeight;

  /* Source can be entire shadow surface, as Blt should clip for us */
  rcSrc.left = 0;
  rcSrc.top = 0;
  rcSrc.right = pScreenInfo->dwWidth;
  rcSrc.bottom = pScreenInfo->dwHeight;

  /* Redraw the whole window, to take account for the new colors */
  ddrval = IDirectDrawSurface2_Blt (pScreenPriv->pddsPrimary,
				    &rcDest,
				    pScreenPriv->pddsShadow,
				    &rcSrc,
				    DDBLT_WAIT,
				    NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winRedrawScreenShadowDD - IDirectDrawSurface_Blt () "
	      "failed: %08x\n",
	      ddrval);
    }

  return TRUE;
}


/*
 * Realize the currently installed colormap
 */

Bool
winRealizeInstalledPaletteShadowDD (ScreenPtr pScreen)
{
  return TRUE;
}


/*
 * Install the specified colormap
 */

Bool
winInstallColormapShadowDD (ColormapPtr pColormap)
{
  ScreenPtr		pScreen = pColormap->pScreen;
  winScreenPriv(pScreen);
  winCmapPriv(pColormap);
  HRESULT		ddrval = DD_OK;

  /* Install the DirectDraw palette on the primary surface */
  ddrval = IDirectDrawSurface2_SetPalette (pScreenPriv->pddsPrimary,
					   pCmapPriv->lpDDPalette);
  if (FAILED (ddrval))
    {
      ErrorF ("winInstallColormapShadowDD - Failed installing the "
	      "DirectDraw palette.\n");
      return FALSE;
    }

  /* Save a pointer to the newly installed colormap */
  pScreenPriv->pcmapInstalled = pColormap;

  return TRUE;
}


/*
 * Store the specified colors in the specified colormap
 */

Bool
winStoreColorsShadowDD (ColormapPtr pColormap, 
			int ndef,
			xColorItem *pdefs)
{
  ScreenPtr		pScreen = pColormap->pScreen;
  winScreenPriv(pScreen);
  winCmapPriv(pColormap);
  ColormapPtr		curpmap = pScreenPriv->pcmapInstalled;
  HRESULT		ddrval = DD_OK;
  
  /* Put the X colormap entries into the Windows logical palette */
  ddrval = IDirectDrawPalette_SetEntries (pCmapPriv->lpDDPalette,
					  0,
					  pdefs[0].pixel,
					  ndef,
					  pCmapPriv->peColors 
					  + pdefs[0].pixel);
  if (FAILED (ddrval))
    {
      ErrorF ("winStoreColorsShadowDDNL - SetEntries () failed\n");
      return FALSE;
    }

  /* Don't install the DirectDraw palette if the colormap is not installed */
  if (pColormap != curpmap)
    {
      return TRUE;
    }

  if (!winInstallColormapShadowDD (pColormap))
    {
      ErrorF ("winStoreColorsShadowDDNL - Failed installing colormap\n");
      return FALSE;
    }

  return TRUE;
}


/*
 * Colormap initialization procedure
 */

Bool
winCreateColormapShadowDD (ColormapPtr pColormap)
{
  HRESULT		ddrval = DD_OK;
  ScreenPtr		pScreen = pColormap->pScreen;
  winScreenPriv(pScreen);
  winCmapPriv(pColormap);
  
  /* Create a DirectDraw palette */
  ddrval = IDirectDraw2_CreatePalette (pScreenPriv->pdd,
				       DDPCAPS_8BIT | DDPCAPS_ALLOW256,
				       pCmapPriv->peColors,
				       &pCmapPriv->lpDDPalette,
				       NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winCreateColormapShadowDDNL - CreatePalette failed\n");
      return FALSE;
    }

  return TRUE;
}


/*
 * Colormap destruction procedure
 */

Bool
winDestroyColormapShadowDD (ColormapPtr pColormap)
{
  winScreenPriv(pColormap->pScreen);
  winCmapPriv(pColormap);
  HRESULT		ddrval = DD_OK;

  /*
   * Is colormap to be destroyed the default?
   *
   * Non-default colormaps should have had winUninstallColormap
   * called on them before we get here.  The default colormap
   * will not have had winUninstallColormap called on it.  Thus,
   * we need to handle the default colormap in a special way.
   */
  if (pColormap->flags & IsDefault)
    {
#if CYGDEBUG
      ErrorF ("winDestroyColormapShadowDDNL - Destroying default "
	      "colormap\n");
#endif
      
      /*
       * FIXME: Walk the list of all screens, popping the default
       * palette out of each screen device context.
       */
      
      /* Pop the palette out of the primary surface */
      ddrval = IDirectDrawSurface2_SetPalette (pScreenPriv->pddsPrimary,
					       NULL);
      if (FAILED (ddrval))
	{
	  ErrorF ("winDestroyColormapShadowDDNL - Failed freeing the "
		  "default colormap DirectDraw palette.\n");
	  return FALSE;
	}

      /* Clear our private installed colormap pointer */
      pScreenPriv->pcmapInstalled = NULL;
    }
  
  /* Release the palette */
  IDirectDrawPalette_Release (pCmapPriv->lpDDPalette);
 
  /* Invalidate the colormap privates */
  pCmapPriv->lpDDPalette = NULL;

  return TRUE;
}


/*
 * Set engine specific functions
 */

Bool
winSetEngineFunctionsShadowDD (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  
  /* Set our pointers */
  pScreenPriv->pwinAllocateFB = winAllocateFBShadowDD;
  pScreenPriv->pwinShadowUpdate = winShadowUpdateDD;
  pScreenPriv->pwinCloseScreen = winCloseScreenShadowDD;
  pScreenPriv->pwinInitVisuals = winInitVisualsShadowDD;
  pScreenPriv->pwinAdjustVideoMode = winAdjustVideoModeShadowDD;
  if (pScreenInfo->fFullScreen)
    pScreenPriv->pwinCreateBoundingWindow = winCreateBoundingWindowFullScreen;
  else
    pScreenPriv->pwinCreateBoundingWindow = winCreateBoundingWindowWindowed;
  pScreenPriv->pwinFinishScreenInit = winFinishScreenInitFB;
  pScreenPriv->pwinBltExposedRegions = winBltExposedRegionsShadowDD;
  pScreenPriv->pwinActivateApp = winActivateAppShadowDD;
  pScreenPriv->pwinRedrawScreen = winRedrawScreenShadowDD;
  pScreenPriv->pwinRealizeInstalledPalette
    = winRealizeInstalledPaletteShadowDD;
  pScreenPriv->pwinInstallColormap = winInstallColormapShadowDD;
  pScreenPriv->pwinStoreColors = winStoreColorsShadowDD;
  pScreenPriv->pwinCreateColormap = winCreateColormapShadowDD;
  pScreenPriv->pwinDestroyColormap = winDestroyColormapShadowDD;
  pScreenPriv->pwinHotKeyAltTab = (winHotKeyAltTabProcPtr) (void (*)())NoopDDA;
  pScreenPriv->pwinCreatePrimarySurface = winCreatePrimarySurfaceShadowDD;
  pScreenPriv->pwinReleasePrimarySurface = winReleasePrimarySurfaceShadowDD;

  return TRUE;
}

