/* 

   XFree86 Xv DDX written by Mark Vojkovich (markv@valinux.com) 
   Adapted for KDrive by Pontus Lidman <pontus.lidman@nokia.com>

   Copyright (C) 2000, 2001 - Nokia Home Communications
   Copyright (C) 1998, 1999 - The XFree86 Project Inc.

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice(s) and this permission notice appear in all copies of
the Software and that both the above copyright notice(s) and this
permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY
SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder
shall not be used in advertising or otherwise to promote the sale, use
or other dealings in this Software without prior written authorization
of the copyright holder.

*/

/* $XFree86: xc/programs/Xserver/hw/kdrive/kxv.c,v 1.3 2001/08/09 09:08:54 keithp Exp $ */

#include "kdrive.h"

#include "scrnintstr.h"
#include "regionstr.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "mivalidate.h"
#include "validate.h"
#include "resource.h"
#include "gcstruct.h"
#include "dixstruct.h"

#include "Xv.h"
#include "Xvproto.h"

#include "kxv.h"


/* XvScreenRec fields */

static Bool KdXVCloseScreen(int, ScreenPtr);
static int KdXVQueryAdaptors(ScreenPtr, XvAdaptorPtr *, int *);

/* XvAdaptorRec fields */

static int KdXVAllocatePort(unsigned long, XvPortPtr, XvPortPtr*);
static int KdXVFreePort(XvPortPtr);
static int KdXVPutVideo(ClientPtr, DrawablePtr,XvPortPtr, GCPtr,
   				INT16, INT16, CARD16, CARD16, 
				INT16, INT16, CARD16, CARD16);
static int KdXVPutStill(ClientPtr, DrawablePtr,XvPortPtr, GCPtr,
   				INT16, INT16, CARD16, CARD16, 
				INT16, INT16, CARD16, CARD16);
static int KdXVGetVideo(ClientPtr, DrawablePtr,XvPortPtr, GCPtr,
   				INT16, INT16, CARD16, CARD16, 
				INT16, INT16, CARD16, CARD16);
static int KdXVGetStill(ClientPtr, DrawablePtr,XvPortPtr, GCPtr,
   				INT16, INT16, CARD16, CARD16, 
				INT16, INT16, CARD16, CARD16);
static int KdXVStopVideo(ClientPtr, XvPortPtr, DrawablePtr);
static int KdXVSetPortAttribute(ClientPtr, XvPortPtr, Atom, INT32);
static int KdXVGetPortAttribute(ClientPtr, XvPortPtr, Atom, INT32 *);
static int KdXVQueryBestSize(ClientPtr, XvPortPtr, CARD8,
   				CARD16, CARD16,CARD16, CARD16, 
				unsigned int*, unsigned int*);
static int KdXVPutImage(ClientPtr, DrawablePtr, XvPortPtr, GCPtr,
   				INT16, INT16, CARD16, CARD16, 
				INT16, INT16, CARD16, CARD16,
				XvImagePtr, unsigned char*, Bool,
				CARD16, CARD16);
static int KdXVQueryImageAttributes(ClientPtr, XvPortPtr, XvImagePtr, 
				CARD16*, CARD16*, int*, int*);


/* ScreenRec fields */

static Bool KdXVCreateWindow(WindowPtr pWin);
static Bool KdXVDestroyWindow(WindowPtr pWin);
static void KdXVWindowExposures(WindowPtr pWin, RegionPtr r1, RegionPtr r2);
static void KdXVClipNotify(WindowPtr pWin, int dx, int dy);

/* misc */
static Bool KdXVInitAdaptors(ScreenPtr, KdVideoAdaptorPtr*, int);


int KdXVWindowIndex = -1;
int KdXvScreenIndex = -1;
static unsigned long KdXVGeneration = 0;
static unsigned long PortResource = 0;

int (*XvGetScreenIndexProc)(void) = XvGetScreenIndex;
unsigned long (*XvGetRTPortProc)(void) = XvGetRTPort;
int (*XvScreenInitProc)(ScreenPtr) = XvScreenInit;

#define GET_XV_SCREEN(pScreen) \
	((XvScreenPtr)((pScreen)->devPrivates[KdXvScreenIndex].ptr))

#define GET_KDXV_SCREEN(pScreen) \
  	((KdXVScreenPtr)(GET_XV_SCREEN(pScreen)->devPriv.ptr))

#define GET_KDXV_WINDOW(pWin) \
	((KdXVWindowPtr)((pWin)->devPrivates[KdXVWindowIndex].ptr))

static KdXVInitGenericAdaptorPtr *GenDrivers = NULL;
static int NumGenDrivers = 0;

int
KdXVRegisterGenericAdaptorDriver(
    KdXVInitGenericAdaptorPtr InitFunc
){
  KdXVInitGenericAdaptorPtr *newdrivers;

/*   fprintf(stderr,"KdXVRegisterGenericAdaptorDriver\n"); */

  newdrivers = xrealloc(GenDrivers, sizeof(KdXVInitGenericAdaptorPtr) * 
			(1 + NumGenDrivers));
  if (!newdrivers)
    return 0;
  GenDrivers = newdrivers;
  
  GenDrivers[NumGenDrivers++] = InitFunc;

  return 1;
}

int
KdXVListGenericAdaptors(
    KdScreenInfo *          screen,
    KdVideoAdaptorPtr **adaptors
){
    int i,j,n,num;
    KdVideoAdaptorPtr *DrivAdap,*new;

    num = 0;
    *adaptors = NULL;
    for (i = 0; i < NumGenDrivers; i++) {
	n = GenDrivers[i](screen,&DrivAdap);
	if (0 == n)
	    continue;
	new = xrealloc(*adaptors, sizeof(KdVideoAdaptorPtr) * (num+n));
	if (NULL == new)
	    continue;
	*adaptors = new;
	for (j = 0; j < n; j++, num++)
	    (*adaptors)[num] = DrivAdap[j];
    }
    return num;
}

KdVideoAdaptorPtr
KdXVAllocateVideoAdaptorRec(KdScreenInfo * screen)
{
    return xcalloc(1, sizeof(KdVideoAdaptorRec));
}

void
KdXVFreeVideoAdaptorRec(KdVideoAdaptorPtr ptr)
{
    xfree(ptr);
}


Bool
KdXVScreenInit(
   ScreenPtr pScreen, 
   KdVideoAdaptorPtr *adaptors,
   int num
){
    KdScreenPriv(pScreen);
  KdXVScreenPtr ScreenPriv;
  XvScreenPtr pxvs;

/*   fprintf(stderr,"KdXVScreenInit initializing %d adaptors\n",num); */

  if(KdXVGeneration != serverGeneration) {
	if((KdXVWindowIndex = AllocateWindowPrivateIndex()) < 0)
	    return FALSE;
	KdXVGeneration = serverGeneration;
  }

  if(!AllocateWindowPrivate(pScreen,KdXVWindowIndex,0))
        return FALSE;

  if(!XvGetScreenIndexProc || !XvGetRTPortProc || !XvScreenInitProc)
	return FALSE;  

  if(Success != (*XvScreenInitProc)(pScreen)) return FALSE;

  KdXvScreenIndex = (*XvGetScreenIndexProc)();
  PortResource = (*XvGetRTPortProc)();

  pxvs = GET_XV_SCREEN(pScreen);


  /* Anyone initializing the Xv layer must provide these two.
     The Xv di layer calls them without even checking if they exist! */

  pxvs->ddCloseScreen = KdXVCloseScreen;
  pxvs->ddQueryAdaptors = KdXVQueryAdaptors;

  /* The Xv di layer provides us with a private hook so that we don't
     have to allocate our own screen private.  They also provide
     a CloseScreen hook so that we don't have to wrap it.  I'm not
     sure that I appreciate that.  */

  ScreenPriv = xalloc(sizeof(KdXVScreenRec));
  pxvs->devPriv.ptr = (pointer)ScreenPriv;

  if(!ScreenPriv) return FALSE;


  ScreenPriv->CreateWindow = pScreen->CreateWindow;
  ScreenPriv->DestroyWindow = pScreen->DestroyWindow;
  ScreenPriv->WindowExposures = pScreen->WindowExposures;
  ScreenPriv->ClipNotify = pScreen->ClipNotify;

/*   fprintf(stderr,"XV: Wrapping screen funcs\n"); */

  pScreen->CreateWindow = KdXVCreateWindow;
  pScreen->DestroyWindow = KdXVDestroyWindow;
  pScreen->WindowExposures = KdXVWindowExposures;
  pScreen->ClipNotify = KdXVClipNotify;

  if(!KdXVInitAdaptors(pScreen, adaptors, num))
	return FALSE;

  return TRUE;
}

static void
KdXVFreeAdaptor(XvAdaptorPtr pAdaptor)
{
   int i;

   if(pAdaptor->name)
      xfree(pAdaptor->name);

   if(pAdaptor->pEncodings) {
      XvEncodingPtr pEncode = pAdaptor->pEncodings;

      for(i = 0; i < pAdaptor->nEncodings; i++, pEncode++) {
          if(pEncode->name) xfree(pEncode->name);
      }
      xfree(pAdaptor->pEncodings);
   }

   if(pAdaptor->pFormats) 
      xfree(pAdaptor->pFormats);

   if(pAdaptor->pPorts) {
      XvPortPtr pPort = pAdaptor->pPorts;
      XvPortRecPrivatePtr pPriv;

      for(i = 0; i < pAdaptor->nPorts; i++, pPort++) {
          pPriv = (XvPortRecPrivatePtr)pPort->devPriv.ptr;
	  if(pPriv) {
	     if(pPriv->clientClip) 
		REGION_DESTROY(pAdaptor->pScreen, pPriv->clientClip);
             if(pPriv->pCompositeClip && pPriv->FreeCompositeClip) 
		REGION_DESTROY(pAdaptor->pScreen, pPriv->pCompositeClip);
	     xfree(pPriv);
	  }
      }
      xfree(pAdaptor->pPorts);
   }

   if(pAdaptor->nAttributes) {
      XvAttributePtr pAttribute = pAdaptor->pAttributes;

      for(i = 0; i < pAdaptor->nAttributes; i++, pAttribute++) {
          if(pAttribute->name) xfree(pAttribute->name);
      }

      xfree(pAdaptor->pAttributes);
   }

   if(pAdaptor->nImages)
      xfree(pAdaptor->pImages);
	
   if(pAdaptor->devPriv.ptr)
      xfree(pAdaptor->devPriv.ptr);
}

static Bool
KdXVInitAdaptors(
   ScreenPtr pScreen, 
   KdVideoAdaptorPtr *infoPtr,
   int number
) {
    KdScreenPriv(pScreen);
    KdScreenInfo * screen = pScreenPriv->screen;

  XvScreenPtr pxvs = GET_XV_SCREEN(pScreen);
  KdVideoAdaptorPtr adaptorPtr;
  XvAdaptorPtr pAdaptor, pa;
  XvAdaptorRecPrivatePtr adaptorPriv;
  int na, numAdaptor;
  XvPortRecPrivatePtr portPriv;
  XvPortPtr pPort, pp;
  int numPort;
  KdAttributePtr attributePtr;
  XvAttributePtr pAttribute, pat;
  KdVideoFormatPtr formatPtr;
  XvFormatPtr pFormat, pf;
  int numFormat, totFormat;
  KdVideoEncodingPtr encodingPtr;
  XvEncodingPtr pEncode, pe;
  KdImagePtr imagePtr;
  XvImagePtr pImage, pi;
  int numVisuals;
  VisualPtr pVisual;
  int i;

  pxvs->nAdaptors = 0;
  pxvs->pAdaptors = NULL;

  if(!(pAdaptor = xcalloc(number, sizeof(XvAdaptorRec)))) 
      return FALSE;

  for(pa = pAdaptor, na = 0, numAdaptor = 0; na < number; na++, adaptorPtr++) {
      adaptorPtr = infoPtr[na];

      if(!adaptorPtr->StopVideo || !adaptorPtr->SetPortAttribute ||
	 !adaptorPtr->GetPortAttribute || !adaptorPtr->QueryBestSize)
	   continue;

      /* client libs expect at least one encoding */
      if(!adaptorPtr->nEncodings || !adaptorPtr->pEncodings)
	   continue;

      pa->type = adaptorPtr->type; 

      if(!adaptorPtr->PutVideo && !adaptorPtr->GetVideo)
	 pa->type &= ~XvVideoMask;

      if(!adaptorPtr->PutStill && !adaptorPtr->GetStill)
	 pa->type &= ~XvStillMask;

      if(!adaptorPtr->PutImage || !adaptorPtr->QueryImageAttributes)
	 pa->type &= ~XvImageMask;

      if(!adaptorPtr->PutVideo && !adaptorPtr->PutImage && 
							  !adaptorPtr->PutStill)
	 pa->type &= ~XvInputMask;

      if(!adaptorPtr->GetVideo && !adaptorPtr->GetStill)
	 pa->type &= ~XvOutputMask;
	 
      if(!(adaptorPtr->type & (XvPixmapMask | XvWindowMask))) 
	  continue;
      if(!(adaptorPtr->type & (XvImageMask | XvVideoMask | XvStillMask))) 
	  continue;

      pa->pScreen = pScreen; 
      pa->ddAllocatePort = KdXVAllocatePort;
      pa->ddFreePort = KdXVFreePort;
      pa->ddPutVideo = KdXVPutVideo;
      pa->ddPutStill = KdXVPutStill;
      pa->ddGetVideo = KdXVGetVideo;
      pa->ddGetStill = KdXVGetStill;
      pa->ddStopVideo = KdXVStopVideo;
      pa->ddPutImage = KdXVPutImage;
      pa->ddSetPortAttribute = KdXVSetPortAttribute;
      pa->ddGetPortAttribute = KdXVGetPortAttribute;
      pa->ddQueryBestSize = KdXVQueryBestSize;
      pa->ddQueryImageAttributes = KdXVQueryImageAttributes;
      if((pa->name = xalloc(strlen(adaptorPtr->name) + 1)))
          strcpy(pa->name, adaptorPtr->name);

      if(adaptorPtr->nEncodings &&
	(pEncode = xcalloc(adaptorPtr->nEncodings, sizeof(XvEncodingRec)))) {

	for(pe = pEncode, encodingPtr = adaptorPtr->pEncodings, i = 0; 
	    i < adaptorPtr->nEncodings; pe++, i++, encodingPtr++) 
        {
	    pe->id = encodingPtr->id;
	    pe->pScreen = pScreen;
	    if((pe->name = xalloc(strlen(encodingPtr->name) + 1)))
                strcpy(pe->name, encodingPtr->name);
	    pe->width = encodingPtr->width;
	    pe->height = encodingPtr->height;
	    pe->rate.numerator = encodingPtr->rate.numerator;
	    pe->rate.denominator = encodingPtr->rate.denominator;
	}
	pa->nEncodings = adaptorPtr->nEncodings;
	pa->pEncodings = pEncode;  
      } 

      if(adaptorPtr->nImages &&
         (pImage = xcalloc(adaptorPtr->nImages, sizeof(XvImageRec)))) {

          for(i = 0, pi = pImage, imagePtr = adaptorPtr->pImages;
	      i < adaptorPtr->nImages; i++, pi++, imagePtr++) 
  	  {
	     pi->id = imagePtr->id;
	     pi->type = imagePtr->type;
	     pi->byte_order = imagePtr->byte_order;
	     memcpy(pi->guid, imagePtr->guid, 16);
	     pi->bits_per_pixel = imagePtr->bits_per_pixel;
	     pi->format = imagePtr->format;
	     pi->num_planes = imagePtr->num_planes;
	     pi->depth = imagePtr->depth;
	     pi->red_mask = imagePtr->red_mask;
	     pi->green_mask = imagePtr->green_mask;
	     pi->blue_mask = imagePtr->blue_mask;
	     pi->y_sample_bits = imagePtr->y_sample_bits;
	     pi->u_sample_bits = imagePtr->u_sample_bits;
	     pi->v_sample_bits = imagePtr->v_sample_bits;
	     pi->horz_y_period = imagePtr->horz_y_period;
	     pi->horz_u_period = imagePtr->horz_u_period;
	     pi->horz_v_period = imagePtr->horz_v_period;
	     pi->vert_y_period = imagePtr->vert_y_period;
	     pi->vert_u_period = imagePtr->vert_u_period;
	     pi->vert_v_period = imagePtr->vert_v_period;
	     memcpy(pi->component_order, imagePtr->component_order, 32);
	     pi->scanline_order = imagePtr->scanline_order;
          }
	  pa->nImages = adaptorPtr->nImages;
	  pa->pImages = pImage;
      }

      if(adaptorPtr->nAttributes &&
	(pAttribute = xcalloc(adaptorPtr->nAttributes, sizeof(XvAttributeRec))))
      {
	for(pat = pAttribute, attributePtr = adaptorPtr->pAttributes, i = 0; 
	    i < adaptorPtr->nAttributes; pat++, i++, attributePtr++) 
        {
	    pat->flags = attributePtr->flags;
	    pat->min_value = attributePtr->min_value;
	    pat->max_value = attributePtr->max_value;
	    if((pat->name = xalloc(strlen(attributePtr->name) + 1)))
                strcpy(pat->name, attributePtr->name);
	}
	pa->nAttributes = adaptorPtr->nAttributes;
	pa->pAttributes = pAttribute;  
      } 


      totFormat = adaptorPtr->nFormats;

      if(!(pFormat = xcalloc(totFormat, sizeof(XvFormatRec)))) {
          KdXVFreeAdaptor(pa);
          continue;
      }
      for(pf = pFormat, i = 0, numFormat = 0, formatPtr = adaptorPtr->pFormats; 
	  i < adaptorPtr->nFormats; i++, formatPtr++) 
      {
	  numVisuals = pScreen->numVisuals;
          pVisual = pScreen->visuals;

          while(numVisuals--) {
              if((pVisual->class == formatPtr->class) &&
                 (pVisual->nplanes == formatPtr->depth)) {

		   if(numFormat >= totFormat) {
			void *moreSpace; 
			totFormat *= 2;
			moreSpace = xrealloc(pFormat, 
					     totFormat * sizeof(XvFormatRec));
			if(!moreSpace) break;
			pFormat = moreSpace;
			pf = pFormat + numFormat;
		   }

                   pf->visual = pVisual->vid; 
		   pf->depth = formatPtr->depth;

		   pf++;
		   numFormat++;
              }
              pVisual++;
          }	
      }
      pa->nFormats = numFormat;
      pa->pFormats = pFormat;  
      if(!numFormat) {
          KdXVFreeAdaptor(pa);
          continue;
      }

      if(!(adaptorPriv = xcalloc(1, sizeof(XvAdaptorRecPrivate)))) {
          KdXVFreeAdaptor(pa);
          continue;
      }

      adaptorPriv->flags = adaptorPtr->flags;
      adaptorPriv->PutVideo = adaptorPtr->PutVideo;
      adaptorPriv->PutStill = adaptorPtr->PutStill;
      adaptorPriv->GetVideo = adaptorPtr->GetVideo;
      adaptorPriv->GetStill = adaptorPtr->GetStill;
      adaptorPriv->StopVideo = adaptorPtr->StopVideo;
      adaptorPriv->SetPortAttribute = adaptorPtr->SetPortAttribute;
      adaptorPriv->GetPortAttribute = adaptorPtr->GetPortAttribute;
      adaptorPriv->QueryBestSize = adaptorPtr->QueryBestSize;
      adaptorPriv->QueryImageAttributes = adaptorPtr->QueryImageAttributes;
      adaptorPriv->PutImage = adaptorPtr->PutImage;
      adaptorPriv->ReputImage = adaptorPtr->ReputImage;

      pa->devPriv.ptr = (pointer)adaptorPriv;

      if(!(pPort = xcalloc(adaptorPtr->nPorts, sizeof(XvPortRec)))) {
          KdXVFreeAdaptor(pa);
          continue;
      }
      for(pp = pPort, i = 0, numPort = 0; 
	  i < adaptorPtr->nPorts; i++) {

          if(!(pp->id = FakeClientID(0))) 
		continue;

	  if(!(portPriv = xcalloc(1, sizeof(XvPortRecPrivate)))) 
		continue;
	  
	  if(!AddResource(pp->id, PortResource, pp)) {
		xfree(portPriv);
		continue;
	  }

          pp->pAdaptor = pa;
          pp->pNotify = (XvPortNotifyPtr)NULL;
          pp->pDraw = (DrawablePtr)NULL;
          pp->client = (ClientPtr)NULL;
          pp->grab.client = (ClientPtr)NULL;
          pp->time = currentTime;
          pp->devPriv.ptr = portPriv;

	  portPriv->screen = screen;
	  portPriv->AdaptorRec = adaptorPriv;
          portPriv->DevPriv.ptr = adaptorPtr->pPortPrivates[i].ptr;
	
          pp++;
          numPort++;
      }
      pa->nPorts = numPort;
      pa->pPorts = pPort;
      if(!numPort) {
          KdXVFreeAdaptor(pa);
          continue;
      }

      pa->base_id = pPort->id;
      
      pa++;
      numAdaptor++;
  }

  if(numAdaptor) {
      pxvs->nAdaptors = numAdaptor;
      pxvs->pAdaptors = pAdaptor;
  } else {
     xfree(pAdaptor);
     return FALSE;
  }

  return TRUE;
}

/* Video should be clipped to the intersection of the window cliplist
   and the client cliplist specified in the GC for which the video was
   initialized.  When we need to reclip a window, the GC that started
   the video may not even be around anymore.  That's why we save the
   client clip from the GC when the video is initialized.  We then
   use KdXVUpdateCompositeClip to calculate the new composite clip
   when we need it.  This is different from what DEC did.  They saved
   the GC and used it's clip list when they needed to reclip the window,
   even if the client clip was different from the one the video was
   initialized with.  If the original GC was destroyed, they had to stop
   the video.  I like the new method better (MArk). 

   This function only works for windows.  Will need to rewrite when
   (if) we support pixmap rendering.
*/

static void  
KdXVUpdateCompositeClip(XvPortRecPrivatePtr portPriv)
{
   RegionPtr	pregWin, pCompositeClip;
   WindowPtr	pWin;
   Bool 	freeCompClip = FALSE;

   if(portPriv->pCompositeClip)
	return;

   pWin = (WindowPtr)portPriv->pDraw;

   /* get window clip list */
   if(portPriv->subWindowMode == IncludeInferiors) {
	pregWin = NotClippedByChildren(pWin);
	freeCompClip = TRUE;
   } else
	pregWin = &pWin->clipList;

   if(!portPriv->clientClip) {
	portPriv->pCompositeClip = pregWin;
	portPriv->FreeCompositeClip = freeCompClip;
	return;
   }

   pCompositeClip = REGION_CREATE(pWin->pScreen, NullBox, 1);
   REGION_COPY(pWin->pScreen, pCompositeClip, portPriv->clientClip);
   REGION_TRANSLATE(pWin->pScreen, pCompositeClip,
			portPriv->pDraw->x + portPriv->clipOrg.x,
			portPriv->pDraw->y + portPriv->clipOrg.y);
   REGION_INTERSECT(pWin->pScreen, pCompositeClip, pregWin, pCompositeClip);

   portPriv->pCompositeClip = pCompositeClip;
   portPriv->FreeCompositeClip = TRUE;

   if(freeCompClip) {
   	REGION_DESTROY(pWin->pScreen, pregWin);
   }    
}

/* Save the current clientClip and update the CompositeClip whenever
   we have a fresh GC */

static void
KdXVCopyClip(
   XvPortRecPrivatePtr portPriv, 
   GCPtr pGC
){
    /* copy the new clip if it exists */
    if((pGC->clientClipType == CT_REGION) && pGC->clientClip) {
	if(!portPriv->clientClip)
	    portPriv->clientClip = REGION_CREATE(pGC->pScreen, NullBox, 1);
	/* Note: this is in window coordinates */
	REGION_COPY(pGC->pScreen, portPriv->clientClip, pGC->clientClip);
    } else if(portPriv->clientClip) { /* free the old clientClip */
	REGION_DESTROY(pGC->pScreen, portPriv->clientClip);
	portPriv->clientClip = NULL;
    }

    /* get rid of the old clip list */
    if(portPriv->pCompositeClip && portPriv->FreeCompositeClip) {
	REGION_DESTROY(pWin->pScreen, portPriv->pCompositeClip);
    }

    portPriv->clipOrg = pGC->clipOrg;
    portPriv->pCompositeClip = pGC->pCompositeClip;
    portPriv->FreeCompositeClip = FALSE;
    portPriv->subWindowMode = pGC->subWindowMode;
}

static int
KdXVRegetVideo(XvPortRecPrivatePtr portPriv)
{
  RegionRec WinRegion;
  RegionRec ClipRegion;
  BoxRec WinBox;
  ScreenPtr pScreen = portPriv->pDraw->pScreen;
  int ret = Success;
  Bool clippedAway = FALSE;

  KdXVUpdateCompositeClip(portPriv);

  /* translate the video region to the screen */
  WinBox.x1 = portPriv->pDraw->x + portPriv->drw_x;
  WinBox.y1 = portPriv->pDraw->y + portPriv->drw_y;
  WinBox.x2 = WinBox.x1 + portPriv->drw_w;
  WinBox.y2 = WinBox.y1 + portPriv->drw_h;
  
  /* clip to the window composite clip */
  REGION_INIT(pScreen, &WinRegion, &WinBox, 1);
  REGION_INIT(pScreen, &ClipRegion, NullBox, 1);
  REGION_INTERSECT(Screen, &ClipRegion, &WinRegion, portPriv->pCompositeClip); 
  
  /* that's all if it's totally obscured */
  if(!REGION_NOTEMPTY(pScreen, &ClipRegion)) {
	clippedAway = TRUE;
	goto CLIP_VIDEO_BAILOUT;
  }

  if(portPriv->AdaptorRec->flags & VIDEO_INVERT_CLIPLIST) {
     REGION_SUBTRACT(pScreen, &ClipRegion, &WinRegion, &ClipRegion);
  }

  ret = (*portPriv->AdaptorRec->GetVideo)(portPriv->screen, 
			portPriv->vid_x, portPriv->vid_y, 
			WinBox.x1, WinBox.y1, 
			portPriv->vid_w, portPriv->vid_h, 
			portPriv->drw_w, portPriv->drw_h, 
			&ClipRegion, portPriv->DevPriv.ptr);

  if(ret == Success)
	portPriv->isOn = XV_ON;

CLIP_VIDEO_BAILOUT:

  if((clippedAway || (ret != Success)) && portPriv->isOn == XV_ON) {
	(*portPriv->AdaptorRec->StopVideo)(
		portPriv->screen, portPriv->DevPriv.ptr, FALSE);
	portPriv->isOn = XV_PENDING;
  }

  /* This clip was copied and only good for one shot */
  if(!portPriv->FreeCompositeClip)
     portPriv->pCompositeClip = NULL;

  REGION_UNINIT(pScreen, &WinRegion);
  REGION_UNINIT(pScreen, &ClipRegion);

  return ret;
}


static int
KdXVReputVideo(XvPortRecPrivatePtr portPriv)
{
  RegionRec WinRegion;
  RegionRec ClipRegion;
  BoxRec WinBox;
  ScreenPtr pScreen = portPriv->pDraw->pScreen;
  KdScreenPriv(pScreen);
  KdScreenInfo *screen=pScreenPriv->screen;
  int ret = Success;
  Bool clippedAway = FALSE;

  KdXVUpdateCompositeClip(portPriv);

  /* translate the video region to the screen */
  WinBox.x1 = portPriv->pDraw->x + portPriv->drw_x;
  WinBox.y1 = portPriv->pDraw->y + portPriv->drw_y;
  WinBox.x2 = WinBox.x1 + portPriv->drw_w;
  WinBox.y2 = WinBox.y1 + portPriv->drw_h;
  
  /* clip to the window composite clip */
  REGION_INIT(pScreen, &WinRegion, &WinBox, 1);
  REGION_INIT(pScreen, &ClipRegion, NullBox, 1);
  REGION_INTERSECT(Screen, &ClipRegion, &WinRegion, portPriv->pCompositeClip); 

  /* clip and translate to the viewport */
  if(portPriv->AdaptorRec->flags & VIDEO_CLIP_TO_VIEWPORT) {
     RegionRec VPReg;
     BoxRec VPBox;

     VPBox.x1 = 0;
     VPBox.y1 = 0;
     VPBox.x2 = screen->width;
     VPBox.y2 = screen->height;

     REGION_INIT(pScreen, &VPReg, &VPBox, 1);
     REGION_INTERSECT(Screen, &ClipRegion, &ClipRegion, &VPReg); 
     REGION_UNINIT(pScreen, &VPReg);
  }
  
  /* that's all if it's totally obscured */
  if(!REGION_NOTEMPTY(pScreen, &ClipRegion)) {
	clippedAway = TRUE;
	goto CLIP_VIDEO_BAILOUT;
  }

  /* bailout if we have to clip but the hardware doesn't support it */
  if(portPriv->AdaptorRec->flags & VIDEO_NO_CLIPPING) {
     BoxPtr clipBox = REGION_RECTS(&ClipRegion);
     if(  (REGION_NUM_RECTS(&ClipRegion) != 1) ||
	  (clipBox->x1 != WinBox.x1) || (clipBox->x2 != WinBox.x2) || 
	  (clipBox->y1 != WinBox.y1) || (clipBox->y2 != WinBox.y2)) 
     {
	    clippedAway = TRUE;
	    goto CLIP_VIDEO_BAILOUT;
     }
  }

  if(portPriv->AdaptorRec->flags & VIDEO_INVERT_CLIPLIST) {
     REGION_SUBTRACT(pScreen, &ClipRegion, &WinRegion, &ClipRegion);
  }

  ret = (*portPriv->AdaptorRec->PutVideo)(portPriv->screen, 
			portPriv->vid_x, portPriv->vid_y, 
			WinBox.x1, WinBox.y1,
			portPriv->vid_w, portPriv->vid_h, 
			portPriv->drw_w, portPriv->drw_h, 
			&ClipRegion, portPriv->DevPriv.ptr);

  if(ret == Success) portPriv->isOn = XV_ON;

CLIP_VIDEO_BAILOUT:

  if((clippedAway || (ret != Success)) && (portPriv->isOn == XV_ON)) {
	(*portPriv->AdaptorRec->StopVideo)(
		portPriv->screen, portPriv->DevPriv.ptr, FALSE);
	portPriv->isOn = XV_PENDING;
  }

  /* This clip was copied and only good for one shot */
  if(!portPriv->FreeCompositeClip)
     portPriv->pCompositeClip = NULL;

  REGION_UNINIT(pScreen, &WinRegion);
  REGION_UNINIT(pScreen, &ClipRegion);

  return ret;
}

static int
KdXVReputImage(XvPortRecPrivatePtr portPriv)
{
  RegionRec WinRegion;
  RegionRec ClipRegion;
  BoxRec WinBox;
  ScreenPtr pScreen = portPriv->pDraw->pScreen;
  KdScreenPriv(pScreen);
  KdScreenInfo *screen=pScreenPriv->screen;
  int ret = Success;
  Bool clippedAway = FALSE;

  KdXVUpdateCompositeClip(portPriv);

  /* translate the video region to the screen */
  WinBox.x1 = portPriv->pDraw->x + portPriv->drw_x;
  WinBox.y1 = portPriv->pDraw->y + portPriv->drw_y;
  WinBox.x2 = WinBox.x1 + portPriv->drw_w;
  WinBox.y2 = WinBox.y1 + portPriv->drw_h;
  
  /* clip to the window composite clip */
  REGION_INIT(pScreen, &WinRegion, &WinBox, 1);
  REGION_INIT(pScreen, &ClipRegion, NullBox, 1);
  REGION_INTERSECT(Screen, &ClipRegion, &WinRegion, portPriv->pCompositeClip); 

  /* clip and translate to the viewport */
  if(portPriv->AdaptorRec->flags & VIDEO_CLIP_TO_VIEWPORT) {
     RegionRec VPReg;
     BoxRec VPBox;

     VPBox.x1 = 0;
     VPBox.y1 = 0;
     VPBox.x2 = screen->width;
     VPBox.y2 = screen->height;

     REGION_INIT(pScreen, &VPReg, &VPBox, 1);
     REGION_INTERSECT(Screen, &ClipRegion, &ClipRegion, &VPReg); 
     REGION_UNINIT(pScreen, &VPReg);
  }
  
  /* that's all if it's totally obscured */
  if(!REGION_NOTEMPTY(pScreen, &ClipRegion)) {
	clippedAway = TRUE;
	goto CLIP_VIDEO_BAILOUT;
  }

  /* bailout if we have to clip but the hardware doesn't support it */
  if(portPriv->AdaptorRec->flags & VIDEO_NO_CLIPPING) {
     BoxPtr clipBox = REGION_RECTS(&ClipRegion);
     if(  (REGION_NUM_RECTS(&ClipRegion) != 1) ||
	  (clipBox->x1 != WinBox.x1) || (clipBox->x2 != WinBox.x2) || 
	  (clipBox->y1 != WinBox.y1) || (clipBox->y2 != WinBox.y2)) 
     {
	    clippedAway = TRUE;
	    goto CLIP_VIDEO_BAILOUT;
     }
  }

  if(portPriv->AdaptorRec->flags & VIDEO_INVERT_CLIPLIST) {
     REGION_SUBTRACT(pScreen, &ClipRegion, &WinRegion, &ClipRegion);
  }

  ret = (*portPriv->AdaptorRec->ReputImage)(portPriv->screen, 
			WinBox.x1, WinBox.y1,
			&ClipRegion, portPriv->DevPriv.ptr);

  portPriv->isOn = (ret == Success) ? XV_ON : XV_OFF;

CLIP_VIDEO_BAILOUT:

  if((clippedAway || (ret != Success)) && (portPriv->isOn == XV_ON)) {
	(*portPriv->AdaptorRec->StopVideo)(
		portPriv->screen, portPriv->DevPriv.ptr, FALSE);
	portPriv->isOn = XV_PENDING;
  }

  /* This clip was copied and only good for one shot */
  if(!portPriv->FreeCompositeClip)
     portPriv->pCompositeClip = NULL;

  REGION_UNINIT(pScreen, &WinRegion);
  REGION_UNINIT(pScreen, &ClipRegion);

  return ret;
}


static int
KdXVReputAllVideo(WindowPtr pWin, pointer data)
{
    KdXVWindowPtr WinPriv;
    
    if (pWin->drawable.type != DRAWABLE_WINDOW)
	return WT_DONTWALKCHILDREN;
    
    WinPriv = GET_KDXV_WINDOW(pWin);

    while(WinPriv) {
	if(WinPriv->PortRec->type == XvInputMask)
	    KdXVReputVideo(WinPriv->PortRec);
	else
	    KdXVRegetVideo(WinPriv->PortRec);
	WinPriv = WinPriv->next;
    }

    return WT_WALKCHILDREN;
}

static int
KdXVEnlistPortInWindow(WindowPtr pWin, XvPortRecPrivatePtr portPriv)
{
   KdXVWindowPtr winPriv, PrivRoot;    

   winPriv = PrivRoot = GET_KDXV_WINDOW(pWin);

  /* Enlist our port in the window private */
   while(winPriv) {
	if(winPriv->PortRec == portPriv) /* we're already listed */
	    break;
	winPriv = winPriv->next;
   }

   if(!winPriv) {
	winPriv = xalloc(sizeof(KdXVWindowRec));
	if(!winPriv) return BadAlloc;
	winPriv->PortRec = portPriv;
	winPriv->next = PrivRoot;
	pWin->devPrivates[KdXVWindowIndex].ptr = (pointer)winPriv;
   }   
   return Success;
}


static void
KdXVRemovePortFromWindow(WindowPtr pWin, XvPortRecPrivatePtr portPriv)
{
     KdXVWindowPtr winPriv, prevPriv = NULL;

     winPriv = GET_KDXV_WINDOW(pWin);

     while(winPriv) {
	if(winPriv->PortRec == portPriv) {
	    if(prevPriv) 
		prevPriv->next = winPriv->next;
	    else 
		pWin->devPrivates[KdXVWindowIndex].ptr = 
					(pointer)winPriv->next;
	    xfree(winPriv);
	    break;
	}
	prevPriv = winPriv; 
	winPriv = winPriv->next;
     }
     portPriv->pDraw = NULL;
}

/****  ScreenRec fields ****/


static Bool
KdXVCreateWindow(WindowPtr pWin)
{
  ScreenPtr pScreen = pWin->drawable.pScreen;
  KdXVScreenPtr ScreenPriv = GET_KDXV_SCREEN(pScreen);
  int ret;

  pScreen->CreateWindow = ScreenPriv->CreateWindow;
  ret = (*pScreen->CreateWindow)(pWin);
  pScreen->CreateWindow = KdXVCreateWindow;

  if(ret) pWin->devPrivates[KdXVWindowIndex].ptr = NULL;

  return ret;
}


static Bool
KdXVDestroyWindow(WindowPtr pWin)
{
  ScreenPtr pScreen = pWin->drawable.pScreen;
  KdXVScreenPtr ScreenPriv = GET_KDXV_SCREEN(pScreen);
  KdXVWindowPtr tmp, WinPriv = GET_KDXV_WINDOW(pWin);
  int ret;

  while(WinPriv) {
     XvPortRecPrivatePtr pPriv = WinPriv->PortRec;

     if(pPriv->isOn > XV_OFF) {
	(*pPriv->AdaptorRec->StopVideo)(
			pPriv->screen, pPriv->DevPriv.ptr, TRUE);
	pPriv->isOn = XV_OFF;
     }

     pPriv->pDraw = NULL;
     tmp = WinPriv;
     WinPriv = WinPriv->next;
     xfree(tmp);
  }

  pWin->devPrivates[KdXVWindowIndex].ptr = NULL;

  pScreen->DestroyWindow = ScreenPriv->DestroyWindow;
  ret = (*pScreen->DestroyWindow)(pWin);
  pScreen->DestroyWindow = KdXVDestroyWindow;

  return ret;
}


static void
KdXVWindowExposures(WindowPtr pWin, RegionPtr reg1, RegionPtr reg2)
{
  ScreenPtr pScreen = pWin->drawable.pScreen;
  KdXVScreenPtr ScreenPriv = GET_KDXV_SCREEN(pScreen);
  KdXVWindowPtr WinPriv = GET_KDXV_WINDOW(pWin);
  KdXVWindowPtr pPrev;
  XvPortRecPrivatePtr pPriv;
  Bool AreasExposed;

  AreasExposed = (WinPriv && reg1 && REGION_NOTEMPTY(pScreen, reg1));

  pScreen->WindowExposures = ScreenPriv->WindowExposures;
  (*pScreen->WindowExposures)(pWin, reg1, reg2);
  pScreen->WindowExposures = KdXVWindowExposures;

  /* filter out XClearWindow/Area */
  if (!pWin->valdata) return;
   
  pPrev = NULL;

  while(WinPriv) {
     pPriv = WinPriv->PortRec;

     /* Reput anyone with a reput function */

     switch(pPriv->type) {
     case XvInputMask:
	KdXVReputVideo(pPriv);
	break;	     
     case XvOutputMask:
	KdXVRegetVideo(pPriv);	
	break;     
     default:  /* overlaid still/image*/
	if (pPriv->AdaptorRec->ReputImage)
	   KdXVReputImage(pPriv);
	else if(AreasExposed) {
	    KdXVWindowPtr tmp;

	    if (pPriv->isOn == XV_ON) {
		(*pPriv->AdaptorRec->StopVideo)(
		    pPriv->screen, pPriv->DevPriv.ptr, FALSE);
		pPriv->isOn = XV_PENDING;
	    }
	    pPriv->pDraw = NULL;

	    if(!pPrev) 
	       pWin->devPrivates[KdXVWindowIndex].ptr = 		
						(pointer)(WinPriv->next);
	    else
	       pPrev->next = WinPriv->next;
	    tmp = WinPriv;
	    WinPriv = WinPriv->next;
	    xfree(tmp);
	    continue;
	}
	break;
     }
     pPrev = WinPriv;
     WinPriv = WinPriv->next;
  }
}


static void 
KdXVClipNotify(WindowPtr pWin, int dx, int dy)
{
  ScreenPtr pScreen = pWin->drawable.pScreen;
  KdXVScreenPtr ScreenPriv = GET_KDXV_SCREEN(pScreen);
  KdXVWindowPtr WinPriv = GET_KDXV_WINDOW(pWin);
  KdXVWindowPtr tmp, pPrev = NULL;
  XvPortRecPrivatePtr pPriv;
  Bool visible = (pWin->visibility == VisibilityUnobscured) ||
		 (pWin->visibility == VisibilityPartiallyObscured);

  while(WinPriv) {
     pPriv = WinPriv->PortRec;

     if(pPriv->pCompositeClip && pPriv->FreeCompositeClip)
	REGION_DESTROY(pScreen, pPriv->pCompositeClip);

     pPriv->pCompositeClip = NULL;

     /* Stop everything except images, but stop them too if the 
	window isn't visible.  But we only remove the images. */

     if(pPriv->type || !visible) {
	if(pPriv->isOn == XV_ON) {
	    (*pPriv->AdaptorRec->StopVideo)(
			pPriv->screen, pPriv->DevPriv.ptr, FALSE);
	    pPriv->isOn = XV_PENDING;
	}

	if(!pPriv->type) {  /* overlaid still/image */
	    pPriv->pDraw = NULL;

	    if(!pPrev) 
	       pWin->devPrivates[KdXVWindowIndex].ptr = 		
						(pointer)(WinPriv->next);
	    else
	       pPrev->next = WinPriv->next;
	    tmp = WinPriv;
	    WinPriv = WinPriv->next;
	    xfree(tmp);
	    continue;
	}
     }

     pPrev = WinPriv;
     WinPriv = WinPriv->next;
  }

  if(ScreenPriv->ClipNotify) {
      pScreen->ClipNotify = ScreenPriv->ClipNotify;
      (*pScreen->ClipNotify)(pWin, dx, dy);
      pScreen->ClipNotify = KdXVClipNotify;
  }
}



/**** Required XvScreenRec fields ****/

static Bool
KdXVCloseScreen(int i, ScreenPtr pScreen)
{
    KdScreenPriv(pScreen);
    KdScreenInfo *screen=pScreenPriv->screen;
  XvScreenPtr pxvs = GET_XV_SCREEN(pScreen);
  KdXVScreenPtr ScreenPriv = GET_KDXV_SCREEN(pScreen);
  XvAdaptorPtr pa;
  int c;

  if(!ScreenPriv) return TRUE;

  pScreen->CreateWindow = ScreenPriv->CreateWindow;
  pScreen->DestroyWindow = ScreenPriv->DestroyWindow;
  pScreen->WindowExposures = ScreenPriv->WindowExposures;
  pScreen->ClipNotify = ScreenPriv->ClipNotify;

/*   fprintf(stderr,"XV: Unwrapping screen funcs\n"); */

  for(c = 0, pa = pxvs->pAdaptors; c < pxvs->nAdaptors; c++, pa++) { 
       KdXVFreeAdaptor(pa);
  }

  if(pxvs->pAdaptors)
    xfree(pxvs->pAdaptors);

  xfree(ScreenPriv);


  return TRUE;
}


static int
KdXVQueryAdaptors(
   ScreenPtr pScreen,
   XvAdaptorPtr *p_pAdaptors,
   int *p_nAdaptors
){
  XvScreenPtr pxvs = GET_XV_SCREEN(pScreen);

  *p_nAdaptors = pxvs->nAdaptors;
  *p_pAdaptors = pxvs->pAdaptors;

  return (Success);
}

static Bool
KdXVRunning (ScreenPtr pScreen)
{
    return (KdXVGeneration == serverGeneration &&
	    GET_XV_SCREEN(pScreen) != 0);
}

Bool
KdXVEnable(ScreenPtr pScreen)
{
    KdXVScreenPtr ScreenPriv;
    
    if (!KdXVRunning (pScreen))
	return TRUE;
    
    WalkTree(pScreen, KdXVReputAllVideo, 0); 
 
    return TRUE;
}

void
KdXVDisable(ScreenPtr pScreen)
{
    XvScreenPtr pxvs;
    KdXVScreenPtr ScreenPriv;
    XvAdaptorPtr pAdaptor;
    XvPortPtr pPort;
    XvPortRecPrivatePtr pPriv;
    int i, j;

    if (!KdXVRunning (pScreen))
	return;

    pxvs = GET_XV_SCREEN(pScreen);
    ScreenPriv = GET_KDXV_SCREEN(pScreen);
    
    for(i = 0; i < pxvs->nAdaptors; i++) {
	pAdaptor = &pxvs->pAdaptors[i];
	for(j = 0; j < pAdaptor->nPorts; j++) {
	    pPort = &pAdaptor->pPorts[j];
	    pPriv = (XvPortRecPrivatePtr)pPort->devPriv.ptr;
	    if(pPriv->isOn > XV_OFF) {

		(*pPriv->AdaptorRec->StopVideo)(
			pPriv->screen, pPriv->DevPriv.ptr, TRUE);
		pPriv->isOn = XV_OFF;

		if(pPriv->pCompositeClip && pPriv->FreeCompositeClip)
		    REGION_DESTROY(pScreen, pPriv->pCompositeClip);

		pPriv->pCompositeClip = NULL;

		if(!pPriv->type && pPriv->pDraw) { /* still */
		    KdXVRemovePortFromWindow((WindowPtr)pPriv->pDraw, pPriv);
		}
	    }
	}
    }
}

/**** XvAdaptorRec fields ****/

static int
KdXVAllocatePort(
   unsigned long port,
   XvPortPtr pPort,
   XvPortPtr *ppPort
){
  *ppPort = pPort;
  return Success;
}

static int
KdXVFreePort(XvPortPtr pPort)
{
  return Success;
}

static int
KdXVPutVideo(
   ClientPtr client,
   DrawablePtr pDraw,
   XvPortPtr pPort,
   GCPtr pGC,
   INT16 vid_x, INT16 vid_y, 
   CARD16 vid_w, CARD16 vid_h, 
   INT16 drw_x, INT16 drw_y,
   CARD16 drw_w, CARD16 drw_h
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
  KdScreenPriv(portPriv->screen->pScreen);
  int result;

  /* No dumping video to pixmaps... For now anyhow */
  if(pDraw->type != DRAWABLE_WINDOW) {
      pPort->pDraw = (DrawablePtr)NULL;
      return BadAlloc;
  }
  
  /* If we are changing windows, unregister our port in the old window */
  if(portPriv->pDraw && (portPriv->pDraw != pDraw))
     KdXVRemovePortFromWindow((WindowPtr)(portPriv->pDraw), portPriv);

  /* Register our port with the new window */
  result =  KdXVEnlistPortInWindow((WindowPtr)pDraw, portPriv);
  if(result != Success) return result;

  portPriv->pDraw = pDraw;
  portPriv->type = XvInputMask;

  /* save a copy of these parameters */
  portPriv->vid_x = vid_x;  portPriv->vid_y = vid_y;
  portPriv->vid_w = vid_w;  portPriv->vid_h = vid_h;
  portPriv->drw_x = drw_x;  portPriv->drw_y = drw_y;
  portPriv->drw_w = drw_w;  portPriv->drw_h = drw_h;

  /* make sure we have the most recent copy of the clientClip */
  KdXVCopyClip(portPriv, pGC);

  /* To indicate to the DI layer that we were successful */
  pPort->pDraw = pDraw;

  if (!pScreenPriv->enabled) return Success;
  
  return(KdXVReputVideo(portPriv));
}

static int
KdXVPutStill(
   ClientPtr client,
   DrawablePtr pDraw,
   XvPortPtr pPort,
   GCPtr pGC,
   INT16 vid_x, INT16 vid_y, 
   CARD16 vid_w, CARD16 vid_h, 
   INT16 drw_x, INT16 drw_y,
   CARD16 drw_w, CARD16 drw_h
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
  ScreenPtr pScreen = pDraw->pScreen;  
  KdScreenPriv(pScreen);
  KdScreenInfo *screen=pScreenPriv->screen;
  RegionRec WinRegion;
  RegionRec ClipRegion;
  BoxRec WinBox;
  int ret = Success;
  Bool clippedAway = FALSE;

  if (pDraw->type != DRAWABLE_WINDOW)
      return BadAlloc;

  if (!pScreenPriv->enabled) return Success;

  WinBox.x1 = pDraw->x + drw_x;
  WinBox.y1 = pDraw->y + drw_y;
  WinBox.x2 = WinBox.x1 + drw_w;
  WinBox.y2 = WinBox.y1 + drw_h;
  
  REGION_INIT(pScreen, &WinRegion, &WinBox, 1);
  REGION_INIT(pScreen, &ClipRegion, NullBox, 1);
  REGION_INTERSECT(pScreen, &ClipRegion, &WinRegion, pGC->pCompositeClip);   

  if(portPriv->AdaptorRec->flags & VIDEO_CLIP_TO_VIEWPORT) {
     RegionRec VPReg;
     BoxRec VPBox;

     VPBox.x1 = 0;
     VPBox.y1 = 0;
     VPBox.x2 = screen->width;
     VPBox.y2 = screen->height;

     REGION_INIT(pScreen, &VPReg, &VPBox, 1);
     REGION_INTERSECT(Screen, &ClipRegion, &ClipRegion, &VPReg); 
     REGION_UNINIT(pScreen, &VPReg);
  }

  if(portPriv->pDraw) {
     KdXVRemovePortFromWindow((WindowPtr)(portPriv->pDraw), portPriv);
  }

  if(!REGION_NOTEMPTY(pScreen, &ClipRegion)) {
     clippedAway = TRUE;
     goto PUT_STILL_BAILOUT;
  }

  if(portPriv->AdaptorRec->flags & VIDEO_NO_CLIPPING) {
     BoxPtr clipBox = REGION_RECTS(&ClipRegion);
     if(  (REGION_NUM_RECTS(&ClipRegion) != 1) ||
	  (clipBox->x1 != WinBox.x1) || (clipBox->x2 != WinBox.x2) || 
	  (clipBox->y1 != WinBox.y1) || (clipBox->y2 != WinBox.y2))
     {
	  clippedAway = TRUE;
          goto PUT_STILL_BAILOUT;
     }
  }

  if(portPriv->AdaptorRec->flags & VIDEO_INVERT_CLIPLIST) {
     REGION_SUBTRACT(pScreen, &ClipRegion, &WinRegion, &ClipRegion);
  }

  ret = (*portPriv->AdaptorRec->PutStill)(portPriv->screen, 
		vid_x, vid_y, WinBox.x1, WinBox.y1,
		vid_w, vid_h, drw_w, drw_h,
		&ClipRegion, portPriv->DevPriv.ptr);

  if((ret == Success) &&
	(portPriv->AdaptorRec->flags & VIDEO_OVERLAID_STILLS)) {

     KdXVEnlistPortInWindow((WindowPtr)pDraw, portPriv);
     portPriv->isOn = XV_ON;
     portPriv->pDraw = pDraw;
     portPriv->drw_x = drw_x;  portPriv->drw_y = drw_y;
     portPriv->drw_w = drw_w;  portPriv->drw_h = drw_h;
     portPriv->type = 0;  /* no mask means it's transient and should
			     not be reput once it's removed */
     pPort->pDraw = pDraw;  /* make sure we can get stop requests */
  }

PUT_STILL_BAILOUT:

  if((clippedAway || (ret != Success)) && (portPriv->isOn == XV_ON)) {
        (*portPriv->AdaptorRec->StopVideo)(
                portPriv->screen, portPriv->DevPriv.ptr, FALSE);
        portPriv->isOn = XV_PENDING;
  }

  REGION_UNINIT(pScreen, &WinRegion);
  REGION_UNINIT(pScreen, &ClipRegion);

  return ret;
}

static int
KdXVGetVideo(
   ClientPtr client,
   DrawablePtr pDraw,
   XvPortPtr pPort,
   GCPtr pGC,
   INT16 vid_x, INT16 vid_y, 
   CARD16 vid_w, CARD16 vid_h, 
   INT16 drw_x, INT16 drw_y,
   CARD16 drw_w, CARD16 drw_h
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
  int result;
  KdScreenPriv(portPriv->screen->pScreen);

  /* No pixmaps... For now anyhow */
  if(pDraw->type != DRAWABLE_WINDOW) {
      pPort->pDraw = (DrawablePtr)NULL;
      return BadAlloc;
  }
  
  /* If we are changing windows, unregister our port in the old window */
  if(portPriv->pDraw && (portPriv->pDraw != pDraw))
     KdXVRemovePortFromWindow((WindowPtr)(portPriv->pDraw), portPriv);

  /* Register our port with the new window */
  result =  KdXVEnlistPortInWindow((WindowPtr)pDraw, portPriv);
  if(result != Success) return result;

  portPriv->pDraw = pDraw;
  portPriv->type = XvOutputMask;

  /* save a copy of these parameters */
  portPriv->vid_x = vid_x;  portPriv->vid_y = vid_y;
  portPriv->vid_w = vid_w;  portPriv->vid_h = vid_h;
  portPriv->drw_x = drw_x;  portPriv->drw_y = drw_y;
  portPriv->drw_w = drw_w;  portPriv->drw_h = drw_h;

  /* make sure we have the most recent copy of the clientClip */
  KdXVCopyClip(portPriv, pGC);

  /* To indicate to the DI layer that we were successful */
  pPort->pDraw = pDraw;
  
  if(!pScreenPriv->enabled) return Success;

  return(KdXVRegetVideo(portPriv));
}

static int
KdXVGetStill(
   ClientPtr client,
   DrawablePtr pDraw,
   XvPortPtr pPort,
   GCPtr pGC,
   INT16 vid_x, INT16 vid_y, 
   CARD16 vid_w, CARD16 vid_h, 
   INT16 drw_x, INT16 drw_y,
   CARD16 drw_w, CARD16 drw_h
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
  ScreenPtr pScreen = pDraw->pScreen;
  KdScreenPriv(pScreen);
  RegionRec WinRegion;
  RegionRec ClipRegion;
  BoxRec WinBox;
  int ret = Success;
  Bool clippedAway = FALSE;

  if (pDraw->type != DRAWABLE_WINDOW)
      return BadAlloc;

  if(!pScreenPriv->enabled) return Success;

  WinBox.x1 = pDraw->x + drw_x;
  WinBox.y1 = pDraw->y + drw_y;
  WinBox.x2 = WinBox.x1 + drw_w;
  WinBox.y2 = WinBox.y1 + drw_h;
  
  REGION_INIT(pScreen, &WinRegion, &WinBox, 1);
  REGION_INIT(pScreen, &ClipRegion, NullBox, 1);
  REGION_INTERSECT(pScreen, &ClipRegion, &WinRegion, pGC->pCompositeClip);   

  if(portPriv->pDraw) {
     KdXVRemovePortFromWindow((WindowPtr)(portPriv->pDraw), portPriv);
  }

  if(!REGION_NOTEMPTY(pScreen, &ClipRegion)) {
     clippedAway = TRUE;
     goto GET_STILL_BAILOUT;
  }

  if(portPriv->AdaptorRec->flags & VIDEO_INVERT_CLIPLIST) {
     REGION_SUBTRACT(pScreen, &ClipRegion, &WinRegion, &ClipRegion);
  }

  ret = (*portPriv->AdaptorRec->GetStill)(portPriv->screen,
		vid_x, vid_y, WinBox.x1, WinBox.y1,
		vid_w, vid_h, drw_w, drw_h,
		&ClipRegion, portPriv->DevPriv.ptr);

GET_STILL_BAILOUT:

  if((clippedAway || (ret != Success)) && (portPriv->isOn == XV_ON)) {
        (*portPriv->AdaptorRec->StopVideo)(
                portPriv->screen, portPriv->DevPriv.ptr, FALSE);
        portPriv->isOn = XV_PENDING;
  }

  REGION_UNINIT(pScreen, &WinRegion);
  REGION_UNINIT(pScreen, &ClipRegion);

  return ret;
}

 

static int
KdXVStopVideo(
   ClientPtr client,
   XvPortPtr pPort,
   DrawablePtr pDraw
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
  KdScreenPriv(portPriv->screen->pScreen);

  if(pDraw->type != DRAWABLE_WINDOW)
      return BadAlloc;
  
  KdXVRemovePortFromWindow((WindowPtr)pDraw, portPriv);

  if(!pScreenPriv->enabled) return Success;

  /* Must free resources. */

  if(portPriv->isOn > XV_OFF) {
	(*portPriv->AdaptorRec->StopVideo)(
		portPriv->screen, portPriv->DevPriv.ptr, TRUE);
	portPriv->isOn = XV_OFF;
  }

  return Success;
}

static int
KdXVSetPortAttribute(
   ClientPtr client,
   XvPortPtr pPort,
   Atom attribute,
   INT32 value
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
     
  return((*portPriv->AdaptorRec->SetPortAttribute)(portPriv->screen, 
		attribute, value, portPriv->DevPriv.ptr));
}


static int
KdXVGetPortAttribute(
   ClientPtr client,
   XvPortPtr pPort,
   Atom attribute,
   INT32 *p_value
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
     
  return((*portPriv->AdaptorRec->GetPortAttribute)(portPriv->screen, 
		attribute, (int *) p_value, portPriv->DevPriv.ptr));
}



static int
KdXVQueryBestSize(
   ClientPtr client,
   XvPortPtr pPort,
   CARD8 motion,
   CARD16 vid_w, CARD16 vid_h,
   CARD16 drw_w, CARD16 drw_h,
   unsigned int *p_w, unsigned int *p_h
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
     
  (*portPriv->AdaptorRec->QueryBestSize)(portPriv->screen, 
		(Bool)motion, vid_w, vid_h, drw_w, drw_h,
		p_w, p_h, portPriv->DevPriv.ptr);

  return Success;
}


static int 
KdXVPutImage(
   ClientPtr client, 
   DrawablePtr pDraw, 
   XvPortPtr pPort, 
   GCPtr pGC,
   INT16 src_x, INT16 src_y, 
   CARD16 src_w, CARD16 src_h, 
   INT16 drw_x, INT16 drw_y,
   CARD16 drw_w, CARD16 drw_h,
   XvImagePtr format,
   unsigned char* data,
   Bool sync,
   CARD16 width, CARD16 height
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);
  ScreenPtr pScreen = pDraw->pScreen;
  KdScreenPriv(pScreen);
  KdScreenInfo *screen=pScreenPriv->screen;
  RegionRec WinRegion;
  RegionRec ClipRegion;
  BoxRec WinBox;
  int ret = Success;
  Bool clippedAway = FALSE;

  if (pDraw->type != DRAWABLE_WINDOW)
      return BadAlloc;

  if(!pScreenPriv->enabled) return Success;

  WinBox.x1 = pDraw->x + drw_x;
  WinBox.y1 = pDraw->y + drw_y;
  WinBox.x2 = WinBox.x1 + drw_w;
  WinBox.y2 = WinBox.y1 + drw_h;
  
  REGION_INIT(pScreen, &WinRegion, &WinBox, 1);
  REGION_INIT(pScreen, &ClipRegion, NullBox, 1);
  REGION_INTERSECT(pScreen, &ClipRegion, &WinRegion, pGC->pCompositeClip);   

  if(portPriv->AdaptorRec->flags & VIDEO_CLIP_TO_VIEWPORT) {
     RegionRec VPReg;
     BoxRec VPBox;

     VPBox.x1 = 0;
     VPBox.y1 = 0;
     VPBox.x2 = pScreen->width;
     VPBox.y2 = pScreen->height;

     REGION_INIT(pScreen, &VPReg, &VPBox, 1);
     REGION_INTERSECT(Screen, &ClipRegion, &ClipRegion, &VPReg); 
     REGION_UNINIT(pScreen, &VPReg);
  }

  if(portPriv->pDraw) {
     KdXVRemovePortFromWindow((WindowPtr)(portPriv->pDraw), portPriv);
  }

  if(!REGION_NOTEMPTY(pScreen, &ClipRegion)) {
     clippedAway = TRUE;
     goto PUT_IMAGE_BAILOUT;
  }

  if(portPriv->AdaptorRec->flags & VIDEO_NO_CLIPPING) {
     BoxPtr clipBox = REGION_RECTS(&ClipRegion);
     if(  (REGION_NUM_RECTS(&ClipRegion) != 1) ||
	  (clipBox->x1 != WinBox.x1) || (clipBox->x2 != WinBox.x2) || 
	  (clipBox->y1 != WinBox.y1) || (clipBox->y2 != WinBox.y2))
     {
	  clippedAway = TRUE;
          goto PUT_IMAGE_BAILOUT;
     }
  }

  if(portPriv->AdaptorRec->flags & VIDEO_INVERT_CLIPLIST) {
     REGION_SUBTRACT(pScreen, &ClipRegion, &WinRegion, &ClipRegion);
  }

  ret = (*portPriv->AdaptorRec->PutImage)(portPriv->screen, 
		src_x, src_y, WinBox.x1, WinBox.y1,
		src_w, src_h, drw_w, drw_h, format->id, data, width, height,
		sync, &ClipRegion, portPriv->DevPriv.ptr);

  if((ret == Success) &&
	(portPriv->AdaptorRec->flags & VIDEO_OVERLAID_IMAGES)) {

     KdXVEnlistPortInWindow((WindowPtr)pDraw, portPriv);
     portPriv->isOn = XV_ON;
     portPriv->pDraw = pDraw;
     portPriv->drw_x = drw_x;  portPriv->drw_y = drw_y;
     portPriv->drw_w = drw_w;  portPriv->drw_h = drw_h;
     portPriv->type = 0;  /* no mask means it's transient and should
			     not be reput once it's removed */
     pPort->pDraw = pDraw;  /* make sure we can get stop requests */
  }

PUT_IMAGE_BAILOUT:

  if((clippedAway || (ret != Success)) && (portPriv->isOn == XV_ON)) {
        (*portPriv->AdaptorRec->StopVideo)(
                portPriv->screen, portPriv->DevPriv.ptr, FALSE);
        portPriv->isOn = XV_PENDING;
  }

  REGION_UNINIT(pScreen, &WinRegion);
  REGION_UNINIT(pScreen, &ClipRegion);

  return ret;
}


static  int 
KdXVQueryImageAttributes(
   ClientPtr client, 
   XvPortPtr pPort,
   XvImagePtr format, 
   CARD16 *width, 
   CARD16 *height, 
   int *pitches,
   int *offsets
){
  XvPortRecPrivatePtr portPriv = (XvPortRecPrivatePtr)(pPort->devPriv.ptr);

  return (*portPriv->AdaptorRec->QueryImageAttributes)(portPriv->screen, 
			format->id, width, height, pitches, offsets);
}


/****************  Offscreen surface stuff *******************/

typedef struct {
   KdOffscreenImagePtr images;
   int num;
} OffscreenImageRec;

static OffscreenImageRec OffscreenImages[MAXSCREENS];
static Bool offscreenInited = FALSE;

Bool 
KdXVRegisterOffscreenImages(
    ScreenPtr pScreen,
    KdOffscreenImagePtr images,
    int num
){
    if(!offscreenInited) {
	bzero(OffscreenImages, sizeof(OffscreenImages[MAXSCREENS]));
	offscreenInited = TRUE;
    }
  
    OffscreenImages[pScreen->myNum].num = num;
    OffscreenImages[pScreen->myNum].images = images;

    return TRUE;
}

KdOffscreenImagePtr
KdXVQueryOffscreenImages(
   ScreenPtr pScreen,
   int *num
){
   if(!offscreenInited) {
	*num = 0;
	return NULL;
   }

   *num = OffscreenImages[pScreen->myNum].num;
   return OffscreenImages[pScreen->myNum].images;
}
