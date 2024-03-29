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
/* $XFree86: xc/programs/Xserver/hw/xwin/winshadddnl.c,v 1.23 2002/10/17 08:18:25 alanh Exp $ */

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
#ifndef IID_IDirectDraw4
DEFINE_GUID( IID_IDirectDraw4, 0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5 );
#endif /* IID_IDirectDraw4 */


/*
 * Create the primary surface and attach the clipper.
 * Used for both the initial surface creation and during
 * WM_DISPLAYCHANGE messages.
 */

Bool
winCreatePrimarySurfaceShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  HRESULT		ddrval = DD_OK;
  DDSURFACEDESC2	ddsd;

  ErrorF ("winCreatePrimarySurfaceShadowDDNL - Creating primary surface\n");

  /* Describe the primary surface */
  ZeroMemory (&ddsd, sizeof (ddsd));
  ddsd.dwSize = sizeof (ddsd);
  ddsd.dwFlags = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  
  /* Create the primary surface */
  ddrval = IDirectDraw4_CreateSurface (pScreenPriv->pdd4,
				       &ddsd,
				       &pScreenPriv->pddsPrimary4,
				       NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winCreatePrimarySurfaceShadowDDNL - Could not create primary "
	      "surface: %08x\n",
	      ddrval);
      return FALSE;
    }
  
#if 1
  ErrorF ("winCreatePrimarySurfaceShadowDDNL - Created primary surface\n");
#endif

  /* Attach our clipper to our primary surface handle */
  ddrval = IDirectDrawSurface4_SetClipper (pScreenPriv->pddsPrimary4,
					   pScreenPriv->pddcPrimary);
  if (FAILED (ddrval))
    {
      ErrorF ("winCreatePrimarySurfaceShadowDDNL - Primary attach clipper "
	      "failed: %08x\n",
	      ddrval);
      return FALSE;
    }

#if 1
  ErrorF ("winCreatePrimarySurfaceShadowDDNL - Attached clipper to primary "
	  "surface\n");
#endif

  /* Everything was correct */
  return TRUE;
}


/*
 * Detach the clipper and release the primary surface.
 * Called from WM_DISPLAYCHANGE.
 */

Bool
winReleasePrimarySurfaceShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);

  ErrorF ("winReleasePrimarySurfaceShadowDDNL - Hello\n");

  /* Release the primary surface and clipper, if they exist */
  if (pScreenPriv->pddsPrimary4)
    {
      /*
       * Detach the clipper from the primary surface.
       * NOTE: We do this explicity for clarity.  The Clipper is not released.
       */
      IDirectDrawSurface4_SetClipper (pScreenPriv->pddsPrimary4,
				      NULL);
  
      ErrorF ("winReleasePrimarySurfaceShadowDDNL - Detached clipper\n");

      /* Release the primary surface */
      IDirectDrawSurface4_Release (pScreenPriv->pddsPrimary4);
      pScreenPriv->pddsPrimary4 = NULL;
    }

  ErrorF ("winReleasePrimarySurfaceShadowDDNL - Released primary surface\n");
  
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
winAllocateFBShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;  
  HRESULT		ddrval = DD_OK;
  DDSURFACEDESC2	ddsdShadow;
  char			*lpSurface = NULL;
  DDPIXELFORMAT		ddpfPrimary;

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDDNL - w %d h %d d %d\n",
	  pScreenInfo->dwWidth, pScreenInfo->dwHeight, pScreenInfo->dwDepth);
#endif

  /* Allocate memory for our shadow surface */
  lpSurface = malloc (pScreenInfo->dwPaddedWidth * pScreenInfo->dwHeight);
  if (lpSurface == NULL)
    {
      ErrorF ("winAllocateFBShadowDDNL - Could not allocate bits\n");
      return FALSE;
    }

  /*
   * Initialize the framebuffer memory so we don't get a 
   * strange display at startup
   */
  ZeroMemory (lpSurface, pScreenInfo->dwPaddedWidth * pScreenInfo->dwHeight);
  
  /* Create a clipper */
  ddrval = (*g_fpDirectDrawCreateClipper) (0,
					   &pScreenPriv->pddcPrimary,
					   NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDDNL - Could not attach clipper: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDDNL - Created a clipper\n");
#endif

  /* Get a device context for the screen  */
  pScreenPriv->hdcScreen = GetDC (pScreenPriv->hwndScreen);

  /* Attach the clipper to our display window */
  ddrval = IDirectDrawClipper_SetHWnd (pScreenPriv->pddcPrimary,
				       0,
				       pScreenPriv->hwndScreen);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDDNL - Clipper not attached "
	      "to window: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDDNL - Attached clipper to window\n");
#endif

  /* Create a DirectDraw object, store the address at lpdd */
  ddrval = (*g_fpDirectDrawCreate) (NULL,
				    (LPDIRECTDRAW*) &pScreenPriv->pdd,
				    NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDDNL - Could not start "
	      "DirectDraw: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDDNL - Created and initialized DD\n");
#endif

  /* Get a DirectDraw4 interface pointer */
  ddrval = IDirectDraw_QueryInterface (pScreenPriv->pdd,
				       &IID_IDirectDraw4,
				       (LPVOID*) &pScreenPriv->pdd4);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDDNL - Failed DD4 query: %08x\n",
	      ddrval);
      return FALSE;
    }

  /* Are we full screen? */
  if (pScreenInfo->fFullScreen)
    {
      DDSURFACEDESC2	ddsdCurrent;
      DWORD		dwRefreshRateCurrent = 0;
      HDC		hdc = NULL;

      /* Set the cooperative level to full screen */
      ddrval = IDirectDraw4_SetCooperativeLevel (pScreenPriv->pdd4,
						 pScreenPriv->hwndScreen,
						 DDSCL_EXCLUSIVE
						 | DDSCL_FULLSCREEN);
      if (FAILED (ddrval))
	{
	  ErrorF ("winAllocateFBShadowDDNL - Could not set "
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
	  ddrval = IDirectDraw4_GetDisplayMode (pScreenPriv->pdd4,
						&ddsdCurrent);
	  if (FAILED (ddrval))
	    {
	      ErrorF ("winAllocateFBShadowDDNL - Could not get current "
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
	  ErrorF ("winAllocateFBShadowDDNL - GetDC () failed\n");
	  return FALSE;
	}

      /* Only change the video mode when different than current mode */
      if (!pScreenInfo->fMultipleMonitors
	  && (pScreenInfo->dwWidth != GetSystemMetrics (SM_CXSCREEN)
	      || pScreenInfo->dwHeight != GetSystemMetrics (SM_CYSCREEN)
	      || pScreenInfo->dwBPP != GetDeviceCaps (hdc, BITSPIXEL)
	      || pScreenInfo->dwRefreshRate != 0))
	{
	  ErrorF ("winAllocateFBShadowDDNL - Changing video mode\n");

	  /* Change the video mode to the mode requested */
	  ddrval = IDirectDraw4_SetDisplayMode (pScreenPriv->pdd4,
						pScreenInfo->dwWidth,
						pScreenInfo->dwHeight,
						pScreenInfo->dwBPP,
						pScreenInfo->dwRefreshRate,
						0);	       
	  if (FAILED (ddrval))
	    {
	      ErrorF ("winAllocateFBShadowDDNL - Could not set "
		      "full screen display mode: %08x\n",
		      ddrval);
	      return FALSE;
	    }
	}
      else
	{
	  ErrorF ("winAllocateFBShadowDDNL - Not changing video mode\n");
	}

      /* Release our DC */
      ReleaseDC (NULL, hdc);
      hdc = NULL;
    }
  else
    {
      /* Set the cooperative level for windowed mode */
      ddrval = IDirectDraw4_SetCooperativeLevel (pScreenPriv->pdd4,
						 pScreenPriv->hwndScreen,
						 DDSCL_NORMAL);
      if (FAILED (ddrval))
	{
	  ErrorF ("winAllocateFBShadowDDNL - Could not set "
		  "cooperative level: %08x\n",
		  ddrval);
	  return FALSE;
	}
    }

  /* Create the primary surface */
  if (!winCreatePrimarySurfaceShadowDDNL (pScreen))
    {
      ErrorF ("winAllocateFBShadowDDNL - winCreatePrimarySurfaceShadowDDNL "
	      "failed\n");
      return FALSE;
    }

  /* Get primary surface's pixel format */
  ZeroMemory (&ddpfPrimary, sizeof (ddpfPrimary));
  ddpfPrimary.dwSize = sizeof (ddpfPrimary);
  ddrval = IDirectDrawSurface4_GetPixelFormat (pScreenPriv->pddsPrimary4,
					       &ddpfPrimary);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDDNL - Could not get primary "
	      "pixformat: %08x\n",
	      ddrval);
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDDNL - Primary masks: %08x %08x %08x "
	  "dwRGBBitCount: %d\n",
	  ddpfPrimary.u2.dwRBitMask,
	  ddpfPrimary.u3.dwGBitMask,
	  ddpfPrimary.u4.dwBBitMask,
	  ddpfPrimary.u1.dwRGBBitCount);
#endif

  /* Describe the shadow surface to be created */
  /*
   * NOTE: Do not use a DDSCAPS_VIDEOMEMORY surface,
   * as drawing, locking, and unlocking take forever
   * with video memory surfaces.  In addition,
   * video memory is a somewhat scarce resource,
   * so you shouldn't be allocating video memory when
   * you have the option of using system memory instead.
   */
  ZeroMemory (&ddsdShadow, sizeof (ddsdShadow));
  ddsdShadow.dwSize = sizeof (ddsdShadow);
  ddsdShadow.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH
    | DDSD_LPSURFACE | DDSD_PITCH | DDSD_PIXELFORMAT;
  ddsdShadow.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  ddsdShadow.dwHeight = pScreenInfo->dwHeight;
  ddsdShadow.dwWidth = pScreenInfo->dwWidth;
  ddsdShadow.u1.lPitch = pScreenInfo->dwPaddedWidth;
  ddsdShadow.lpSurface = lpSurface;
  ddsdShadow.u4.ddpfPixelFormat = ddpfPrimary;
  
  ErrorF ("winAllocateFBShadowDDNL - lPitch: %d\n",
	  pScreenInfo->dwPaddedWidth);

  /* Create the shadow surface */
  ddrval = IDirectDraw4_CreateSurface (pScreenPriv->pdd4,
				       &ddsdShadow,
				       &pScreenPriv->pddsShadow4,
				       NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winAllocateFBShadowDDNL - Could not create shadow "
	      "surface: %08x\n", ddrval);
      return FALSE;
    }
  
#if CYGDEBUG || YES
  ErrorF ("winAllocateFBShadowDDNL - Created shadow pitch: %d\n",
	  ddsdShadow.u1.lPitch);
#endif

  /* Grab the pitch from the surface desc */
  pScreenInfo->dwStride = (ddsdShadow.u1.lPitch * 8)
    / pScreenInfo->dwBPP;

#if CYGDEBUG || YES
  ErrorF ("winAllocateFBShadowDDNL - Created shadow stride: %d\n",
	  pScreenInfo->dwStride);
#endif

  /* Save the pointer to our surface memory */
  pScreenInfo->pfb = lpSurface;
  
  /* Grab the masks from the surface description */
  pScreenPriv->dwRedMask = ddsdShadow.u4.ddpfPixelFormat.u2.dwRBitMask;
  pScreenPriv->dwGreenMask = ddsdShadow.u4.ddpfPixelFormat.u3.dwGBitMask;
  pScreenPriv->dwBlueMask = ddsdShadow.u4.ddpfPixelFormat.u4.dwBBitMask;

#if CYGDEBUG
  ErrorF ("winAllocateFBShadowDDNL - Returning\n");
#endif

  return TRUE;
}


/*
 * Transfer the damaged regions of the shadow framebuffer to the display.
 */

void
winShadowUpdateDDNL (ScreenPtr pScreen, 
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
	  
	  /* Calculate destination rectangle */
	  rcDest.left = ptOrigin.x + rcSrc.left;
	  rcDest.top = ptOrigin.y + rcSrc.top;
	  rcDest.right = ptOrigin.x + rcSrc.right;
	  rcDest.bottom = ptOrigin.y + rcSrc.bottom;
	  
	  /* Blit the damaged areas */
	  ddrval = IDirectDrawSurface4_Blt (pScreenPriv->pddsPrimary4,
					    &rcDest,
					    pScreenPriv->pddsShadow4,
					    &rcSrc,
					    DDBLT_WAIT,
					    NULL);
	  if (FAILED (ddrval))
	    {
	      ErrorF ("winShadowUpdateDDNL - IDirectDrawSurface4_Blt () "
		      "failed: %08x\n",
		      ddrval);
	    }
	  
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

#if CYGDEBUG
      ErrorF ("winShadowUpdateDDNL - be x1 %d y1 %d x2 %d y2 %d\n",
	      pBoxExtents->x1, pBoxExtents->y1,
	      pBoxExtents->x2, pBoxExtents->y2);
#endif

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
      ddrval = IDirectDrawSurface4_Blt (pScreenPriv->pddsPrimary4,
					&rcDest,
					pScreenPriv->pddsShadow4,
					&rcSrc,
					DDBLT_WAIT,
					NULL);

      /* Reset the clip region */
      SelectClipRgn (pScreenPriv->hdcScreen, NULL);
    }
}


/*
 * Call the wrapped CloseScreen function.
 *
 * Free our resources and private structures.
 */

Bool
winCloseScreenShadowDDNL (int nIndex, ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  Bool			fReturn;

#if CYGDEBUG
  ErrorF ("winCloseScreenShadowDDNL - Freeing screen resources\n");
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
  if (pScreenPriv->pddsShadow4)
    {
      IDirectDrawSurface4_Release (pScreenPriv->pddsShadow4);
      free (pScreenInfo->pfb);
      pScreenInfo->pfb = NULL;
      pScreenPriv->pddsShadow4 = NULL;
    }

  /* Detach the clipper from the primary surface and release the clipper. */
  if (pScreenPriv->pddcPrimary)
    {
      /* Detach the clipper */
      IDirectDrawSurface4_SetClipper (pScreenPriv->pddsPrimary4,
				      NULL);

      /* Release the clipper object */
      IDirectDrawClipper_Release (pScreenPriv->pddcPrimary);
      pScreenPriv->pddcPrimary = NULL;
    }

  /* Release the primary surface, if there is one */
  if (pScreenPriv->pddsPrimary4)
    {
      IDirectDrawSurface4_Release (pScreenPriv->pddsPrimary4);
      pScreenPriv->pddsPrimary4 = NULL;
    }

  /* Free the DirectDraw4 object, if there is one */
  if (pScreenPriv->pdd4)
    {
      IDirectDraw4_RestoreDisplayMode (pScreenPriv->pdd4);
      IDirectDraw4_Release (pScreenPriv->pdd4);
      pScreenPriv->pdd4 = NULL;
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
winInitVisualsShadowDDNL (ScreenPtr pScreen)
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

  ErrorF ("winInitVisualsShadowDDNL - Masks %08x %08x %08x BPRGB %d d %d "
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
      /* Setup the real visual */
      if (!miSetVisualTypesAndMasks (pScreenInfo->dwDepth,
				     TrueColorMask,
				     pScreenPriv->dwBitsPerRGB,
				     -1,
				     pScreenPriv->dwRedMask,
				     pScreenPriv->dwGreenMask,
				     pScreenPriv->dwBlueMask))
	{
	  ErrorF ("winInitVisualsShadowDDNL - miSetVisualTypesAndMasks "
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
	  ErrorF ("winInitVisualsShadowDDNL - miSetVisualTypesAndMasks "
		  "failed for PseudoColor\n");
	  return FALSE;
	}
#endif
#else /* XFree86Server */
      /* Setup the real visual */
      if (!fbSetVisualTypesAndMasks (pScreenInfo->dwDepth,
				     TrueColorMask,
				     pScreenPriv->dwBitsPerRGB,
				     pScreenPriv->dwRedMask,
				     pScreenPriv->dwGreenMask,
				     pScreenPriv->dwBlueMask))
	{
	  ErrorF ("winInitVisualsShadowDDNL - fbSetVisualTypesAndMasks "
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
	  ErrorF ("winInitVisualsShadowDDNL - fbSetVisualTypesAndMasks "
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
	  ErrorF ("winInitVisualsShadowDDNL - miSetVisualTypesAndMasks "
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
	  ErrorF ("winInitVisualsShadowDDNL - fbSetVisualTypesAndMasks "
		  "failed\n");
	  return FALSE;
	}    
#endif /* XFree86Server */
      break;

    default:
      ErrorF ("winInitVisualsShadowDDNL - Unknown screen depth\n");
      return FALSE;
    }

#if CYGDEBUG
  ErrorF ("winInitVisualsShadowDDNL - Returning\n");
#endif

  return TRUE;
}


/*
 * Adjust the user proposed video mode
 */

Bool
winAdjustVideoModeShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  HDC			hdc = NULL;
  DWORD			dwBPP;

  /* We're in serious trouble if we can't get a DC */
  hdc = GetDC (NULL);
  if (hdc == NULL)
    {
      ErrorF ("winAdjustVideoModeShadowDDNL - GetDC () failed\n");
      return FALSE;
    }

  /* Query GDI for current display depth */
  dwBPP = GetDeviceCaps (hdc, BITSPIXEL);

  /* DirectDraw can only change the depth in fullscreen mode */
  if (pScreenInfo->dwBPP == WIN_DEFAULT_BPP)
    {
      /* No -depth parameter passed, let the user know the depth being used */
      ErrorF ("winAdjustVideoModeShadowDDNL - Using Windows display "
	      "depth of %d bits per pixel\n", dwBPP);

      /* Use GDI's depth */
      pScreenInfo->dwBPP = dwBPP;
    }
  else if (pScreenInfo->fFullScreen
	   && pScreenInfo->dwBPP != dwBPP)
    {
      /* FullScreen, and GDI depth differs from -depth parameter */
      ErrorF ("winAdjustVideoModeShadowDDNL - FullScreen, using command "
	      "line bpp: %d\n", pScreenInfo->dwBPP);
    }
  else if (dwBPP != pScreenInfo->dwBPP)
    {
      /* Windowed, and GDI depth differs from -depth parameter */
      ErrorF ("winAdjustVideoModeShadowDDNL - Windowed, command line "
	      "bpp: %d, using bpp: %d\n",
	      pScreenInfo->dwBPP, dwBPP);

      /* We'll use GDI's depth */
      pScreenInfo->dwBPP = dwBPP;
    }

  /* See if the shadow bitmap will be larger than the DIB size limit */
  if (pScreenInfo->dwWidth * pScreenInfo->dwHeight * pScreenInfo->dwBPP
      >= WIN_DIB_MAXIMUM_SIZE)
    {
      ErrorF ("winAdjustVideoModeShadowDDNL - Requested DirectDraw surface "
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
winBltExposedRegionsShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  RECT			rcSrc, rcDest;
  POINT			ptOrigin;
  HDC			hdcUpdate;
  PAINTSTRUCT		ps;
  HRESULT		ddrval = DD_OK;
  Bool			fReturn = TRUE;
  int			i;

  /* BeginPaint gives us an hdc that clips to the invalidated region */
  hdcUpdate = BeginPaint (pScreenPriv->hwndScreen, &ps);
  if (hdcUpdate == NULL)
    {
      fReturn = FALSE;
      ErrorF ("winBltExposedRegionsShadowDDNL - BeginPaint () returned "
	      "a NULL device context handle.  Aborting blit attempt.\n");
      goto winBltExposedRegionsShadowDDNL_Exit;
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

  /* Source can be entire shadow surface, as Blt should clip for us */
  rcSrc.left = 0;
  rcSrc.top = 0;
  rcSrc.right = pScreenInfo->dwWidth;
  rcSrc.bottom = pScreenInfo->dwHeight;

  /* Try to regain the primary surface and blit again if we've lost it */
  for (i = 0; i <= WIN_REGAIN_SURFACE_RETRIES; ++i)
    {
      /* Our Blt should be clipped to the invalidated region */
      ddrval = IDirectDrawSurface4_Blt (pScreenPriv->pddsPrimary4,
					&rcDest,
					pScreenPriv->pddsShadow4,
					&rcSrc,
					DDBLT_WAIT,
					NULL);
      if (ddrval == DDERR_SURFACELOST)
	{
	  /* Surface was lost */
	  ErrorF ("winBltExposedRegionsShadowDDNL - IDirectDrawSurface4_Blt "
		  "reported that the primary surface was lost, "
		  "trying to restore, retry: %d\n", i + 1);

	  /* Try to restore the surface, once */
	  
	  ddrval = IDirectDrawSurface4_Restore (pScreenPriv->pddsPrimary4);
	  ErrorF ("winBltExposedRegionsShadowDDNL - "
		  "IDirectDrawSurface4_Restore returned: ");
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
	  ErrorF ("winBltExposedRegionsShadowDDNL - IDirectDrawSurface4_Blt "
		  "failed, but surface not lost: %08x %d\n", ddrval, ddrval);
	  goto winBltExposedRegionsShadowDDNL_Exit;
	}
      else
	{
	  /* Success, stop looping */
	  break;
	}
    }

 winBltExposedRegionsShadowDDNL_Exit:
  /* EndPaint frees the DC */
  if (hdcUpdate != NULL)
    EndPaint (pScreenPriv->hwndScreen, &ps);
  return fReturn;
}


/*
 * Do any engine-specific appliation-activation processing
 */

Bool
winActivateAppShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);

  /*
   * Do we have a surface?
   * Are we active?
   * Are we full screen?
   */
  if (pScreenPriv != NULL
      && pScreenPriv->pddsPrimary4 != NULL
      && pScreenPriv->fActive)
    {
      /* Primary surface was lost, restore it */
      IDirectDrawSurface4_Restore (pScreenPriv->pddsPrimary4);
    }

  return TRUE;
}


/*
 * Reblit the shadow framebuffer to the screen.
 */

Bool
winRedrawScreenShadowDDNL (ScreenPtr pScreen)
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
  ddrval = IDirectDrawSurface4_Blt (pScreenPriv->pddsPrimary4,
				    &rcDest,
				    pScreenPriv->pddsShadow4,
				    &rcSrc,
				    DDBLT_WAIT,
				    NULL);
  if (FAILED (ddrval))
    {
      ErrorF ("winRedrawScreenShadowDDNL - IDirectDrawSurface4_Blt () "
	      "failed: %08x\n",
	      ddrval);
    }

  return TRUE;
}


/*
 * Realize the currently installed colormap
 */

Bool
winRealizeInstalledPaletteShadowDDNL (ScreenPtr pScreen)
{
  return TRUE;
}


/*
 * Install the specified colormap
 */

Bool
winInstallColormapShadowDDNL (ColormapPtr pColormap)
{
  ScreenPtr		pScreen = pColormap->pScreen;
  winScreenPriv(pScreen);
  winCmapPriv(pColormap);
  HRESULT		ddrval = DD_OK;

  /* Install the DirectDraw palette on the primary surface */
  ddrval = IDirectDrawSurface4_SetPalette (pScreenPriv->pddsPrimary4,
					   pCmapPriv->lpDDPalette);
  if (FAILED (ddrval))
    {
      ErrorF ("winInstallColormapShadowDDNL - Failed installing the "
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
winStoreColorsShadowDDNL (ColormapPtr pColormap, 
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

  if (!winInstallColormapShadowDDNL (pColormap))
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
winCreateColormapShadowDDNL (ColormapPtr pColormap)
{
  HRESULT		ddrval = DD_OK;
  ScreenPtr		pScreen = pColormap->pScreen;
  winScreenPriv(pScreen);
  winCmapPriv(pColormap);
  
  /* Create a DirectDraw palette */
  ddrval = IDirectDraw4_CreatePalette (pScreenPriv->pdd4,
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
winDestroyColormapShadowDDNL (ColormapPtr pColormap)
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
      ddrval = IDirectDrawSurface4_SetPalette (pScreenPriv->pddsPrimary4,
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
 * Set pointers to our engine specific functions
 */

Bool
winSetEngineFunctionsShadowDDNL (ScreenPtr pScreen)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  
  /* Set our pointers */
  pScreenPriv->pwinAllocateFB = winAllocateFBShadowDDNL;
  pScreenPriv->pwinShadowUpdate = winShadowUpdateDDNL;
  pScreenPriv->pwinCloseScreen = winCloseScreenShadowDDNL;
  pScreenPriv->pwinInitVisuals = winInitVisualsShadowDDNL;
  pScreenPriv->pwinAdjustVideoMode = winAdjustVideoModeShadowDDNL;
  if (pScreenInfo->fFullScreen)
    pScreenPriv->pwinCreateBoundingWindow = winCreateBoundingWindowFullScreen;
  else
    pScreenPriv->pwinCreateBoundingWindow = winCreateBoundingWindowWindowed;
  pScreenPriv->pwinFinishScreenInit = winFinishScreenInitFB;
  pScreenPriv->pwinBltExposedRegions = winBltExposedRegionsShadowDDNL;
  pScreenPriv->pwinActivateApp = winActivateAppShadowDDNL;
  pScreenPriv->pwinRedrawScreen = winRedrawScreenShadowDDNL;
  pScreenPriv->pwinRealizeInstalledPalette
    = winRealizeInstalledPaletteShadowDDNL;
  pScreenPriv->pwinInstallColormap = winInstallColormapShadowDDNL;
  pScreenPriv->pwinStoreColors = winStoreColorsShadowDDNL;
  pScreenPriv->pwinCreateColormap = winCreateColormapShadowDDNL;
  pScreenPriv->pwinDestroyColormap = winDestroyColormapShadowDDNL;
  pScreenPriv->pwinHotKeyAltTab = (winHotKeyAltTabProcPtr) (void (*)())NoopDDA;
  pScreenPriv->pwinCreatePrimarySurface = winCreatePrimarySurfaceShadowDDNL;
  pScreenPriv->pwinReleasePrimarySurface = winReleasePrimarySurfaceShadowDDNL;

  return TRUE;
}


