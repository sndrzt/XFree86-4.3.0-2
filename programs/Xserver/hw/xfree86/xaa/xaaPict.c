/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xaaPict.c,v 1.16 2002/11/08 12:39:47 alanh Exp $
 *
 * Copyright � 2000 Keith Packard, member of The XFree86 Project, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "misc.h"
#include "xf86.h"
#include "xf86_ansic.h"
#include "xf86_OSproc.h"

#include "X.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "xf86str.h"
#include "mi.h"
#include "picturestr.h"
#include "glyphstr.h"
#include "picture.h"
#include "mipict.h"
#include "xaa.h"
#include "xaalocal.h"
#include "xaawrap.h"
#include "xaacexp.h"
#include "xf86fbman.h"
#include "servermd.h"

Bool
XAAGetPixelFromRGBA (
    CARD32 *pixel,
    CARD16 red,
    CARD16 green,
    CARD16 blue,
    CARD16 alpha,
    CARD32 format
){
    int rbits, bbits, gbits, abits;
    int rshift, bshift, gshift, ashift;

    *pixel = 0;

    if(!PICT_FORMAT_COLOR(format))
    	return FALSE;
	
    rbits = PICT_FORMAT_R(format);
    gbits = PICT_FORMAT_G(format);
    bbits = PICT_FORMAT_B(format);
    abits = PICT_FORMAT_A(format);
    
    if(PICT_FORMAT_TYPE(format) == PICT_TYPE_ARGB) {
        bshift = 0;
        gshift = bbits;
	rshift = gshift + gbits;
	ashift = rshift + rbits;
    } else {  /* PICT_TYPE_ABGR */
        rshift = 0;
	gshift = rbits;
	bshift = gshift + gbits;
	ashift = bshift + bbits;
    }
    
    *pixel |=  ( blue >> (16 - bbits)) << bshift;
    *pixel |=  (  red >> (16 - rbits)) << rshift;
    *pixel |=  (green >> (16 - gbits)) << gshift;
    *pixel |=  (alpha >> (16 - abits)) << ashift;

    return TRUE;
}


Bool
XAAGetRGBAFromPixel(
    CARD32 pixel,
    CARD16 *red,
    CARD16 *green,
    CARD16 *blue,
    CARD16 *alpha,
    CARD32 format
){
    int rbits, bbits, gbits, abits;
    int rshift, bshift, gshift, ashift;
    
    if(!PICT_FORMAT_COLOR(format))
    	return FALSE;
	
    rbits = PICT_FORMAT_R(format);
    gbits = PICT_FORMAT_G(format);
    bbits = PICT_FORMAT_B(format);
    abits = PICT_FORMAT_A(format);
    
    if(PICT_FORMAT_TYPE(format) == PICT_TYPE_ARGB) {
        bshift = 0;
        gshift = bbits;
	rshift = gshift + gbits;
	ashift = rshift + rbits;
    } else {  /* PICT_TYPE_ABGR */
        rshift = 0;
	gshift = rbits;
	bshift = gshift + gbits;
	ashift = bshift + bbits;
    }
 
    *red = ((pixel >> rshift ) & ((1 << rbits) - 1)) << (16 - rbits);
    while(rbits < 16) {
       *red |= *red >> rbits;
       rbits <<= 1;
    }
    
    *green = ((pixel >> gshift ) & ((1 << gbits) - 1)) << (16 - gbits);
    while(gbits < 16) {
       *green |= *green >> gbits;
       gbits <<= 1;
    }
        
    *blue = ((pixel >> bshift ) & ((1 << bbits) - 1)) << (16 - bbits);
    while(bbits < 16) {
       *blue |= *blue >> bbits;
       bbits <<= 1;
    }  
    
    if(abits) {
       *alpha = ((pixel >> ashift ) & ((1 << abits) - 1)) << (16 - abits);
       while(abits < 16) {
          *alpha |= *alpha >> abits;
          abits <<= 1;
       }     
    } else *alpha = 0xffff;
      
    return TRUE;
}

/* 8:8:8 + PICT_a8 -> 8:8:8:8 texture */

void
XAA_888_plus_PICT_a8_to_8888 (
    CARD32 color,
    CARD8  *alphaPtr,   /* in bytes */
    int    alphaPitch,
    CARD32  *dstPtr,
    int    dstPitch,	/* in dwords */
    int    width,
    int    height
){
    int x;

    color &= 0x00ffffff;

    while(height--) {
	for(x = 0; x < width; x++)
	   dstPtr[x] = color | (alphaPtr[x] << 24);
	dstPtr += dstPitch;
	alphaPtr += alphaPitch;
    } 
}

Bool
XAADoComposite (
    CARD8      op,
    PicturePtr pSrc,
    PicturePtr pMask,
    PicturePtr pDst,
    INT16      xSrc,
    INT16      ySrc,
    INT16      xMask,
    INT16      yMask,
    INT16      xDst,
    INT16      yDst,
    CARD16     width,
    CARD16     height
){
    ScreenPtr pScreen = pDst->pDrawable->pScreen;
    XAAInfoRecPtr infoRec = GET_XAAINFORECPTR_FROM_SCREEN(pScreen);
    RegionRec region;
    CARD32 *formats;
    int flags = 0;
    BoxPtr pbox;
    int nbox, w, h;

    if(!REGION_NUM_RECTS(pDst->pCompositeClip))
        return TRUE;

    if(!infoRec->pScrn->vtSema || 
      ((pDst->pDrawable->type != DRAWABLE_WINDOW) &&
	!IS_OFFSCREEN_PIXMAP(pDst->pDrawable)))
	return FALSE;

    if((pSrc->pDrawable->type != DRAWABLE_PIXMAP) ||
        IS_OFFSCREEN_PIXMAP(pSrc->pDrawable))
	return FALSE;

    if (pSrc->transform || (pMask && pMask->transform))
	return FALSE;

    if (pDst->alphaMap || pSrc->alphaMap || (pMask && pMask->alphaMap))
	return FALSE;
	
    xDst += pDst->pDrawable->x;
    yDst += pDst->pDrawable->y;
    xSrc += pSrc->pDrawable->x;
    ySrc += pSrc->pDrawable->y;

    if(pMask) {
	/* for now we only do it if there is a 1x1 (solid) source */

	if((pSrc->pDrawable->width == 1) && (pSrc->pDrawable->height == 1)) {
	   CARD16 red, green, blue, alpha;
           CARD32 pixel =
                *((CARD32*)(((PixmapPtr)(pSrc->pDrawable))->devPrivate.ptr));

	   if(!XAAGetRGBAFromPixel(pixel,&red,&green,&blue,&alpha,pSrc->format))
		return FALSE;

	   xMask += pMask->pDrawable->x;
	   yMask += pMask->pDrawable->y;	
		
	   /* pull out color expandable operations here */
	   if((pMask->format == PICT_a1) && (alpha == 0xffff) &&
	       (op == PictOpOver) && infoRec->WriteBitmap && !pMask->repeat &&
	       !(infoRec->WriteBitmapFlags & NO_TRANSPARENCY) &&
	       (!(infoRec->WriteBitmapFlags & RGB_EQUAL) || 
	         ((red == green) && (green == blue))))
	   {
	        PixmapPtr pPix = (PixmapPtr)(pMask->pDrawable);
		int skipleft;
		        
	  	if (!miComputeCompositeRegion (&region, pSrc, pMask, pDst,
                                   xSrc, ySrc, xMask, yMask, xDst, yDst,
                                   width, height))
		      return TRUE;
		      
	  	nbox = REGION_NUM_RECTS(&region);
	  	pbox = REGION_RECTS(&region);   
		
	        if(!nbox)
		    return TRUE;	
		    
	        XAAGetPixelFromRGBA(&pixel, red, green, blue, 0, pDst->format);
		    
	   	xMask -= xDst;
	   	yMask -= yDst;

	   	while(nbox--) {
		    skipleft = pbox->x1 + xMask;
		    
	            (*infoRec->WriteBitmap)(infoRec->pScrn,
			        pbox->x1, pbox->y1, 
			        pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
			        (unsigned char*)(pPix->devPrivate.ptr) + 
				  (pPix->devKind * (pbox->y1 + yMask)) + 
			          ((skipleft >> 3) & ~3), pPix->devKind, 
				skipleft & 31, pixel, -1, GXcopy, ~0);
	            pbox++;
	   	}
				  
		/* WriteBitmap sets the Sync flag */		  
	        REGION_UNINIT(pScreen, &region);
		return TRUE;
	  }

	  if(!(formats = infoRec->CPUToScreenAlphaTextureFormats))
		return FALSE;

	  w = pMask->pDrawable->width;
	  h = pMask->pDrawable->height;

	  if(pMask->repeat) {
	      if((infoRec->CPUToScreenAlphaTextureFlags & XAA_RENDER_NO_TILE) ||
		   ((infoRec->CPUToScreenAlphaTextureFlags & 
                                   XAA_RENDER_POWER_OF_2_TILE_ONLY) && 
				((h & (h - 1)) || (w & (w - 1)))))
	      {
		 return FALSE;
	      }
	      flags |= XAA_RENDER_REPEAT;
	  } 

	  if((alpha != 0xffff) &&
              (infoRec->CPUToScreenAlphaTextureFlags & XAA_RENDER_NO_SRC_ALPHA))
		return FALSE;

	  while(*formats != pMask->format) {
		if(!(*formats)) return FALSE;
		formats++;
          }

	  if (!miComputeCompositeRegion (&region, pSrc, pMask, pDst,
                                   xSrc, ySrc, xMask, yMask, xDst, yDst,
                                   width, height))
		return TRUE;

	  nbox = REGION_NUM_RECTS(&region);
	  pbox = REGION_RECTS(&region);   
	     
	  if(!nbox) {
                REGION_UNINIT(pScreen, &region);
		return TRUE;
	  }

	  if(!(infoRec->SetupForCPUToScreenAlphaTexture)(infoRec->pScrn,
			op, red, green, blue, alpha, pMask->format, 
			((PixmapPtr)(pMask->pDrawable))->devPrivate.ptr,
			((PixmapPtr)(pMask->pDrawable))->devKind, 
			w, h, flags))
	  {
                REGION_UNINIT(pScreen, &region);
		return FALSE;
	  }

	   xMask -= xDst;
	   yMask -= yDst;
	
	   while(nbox--) {
	      (*infoRec->SubsequentCPUToScreenAlphaTexture)(infoRec->pScrn,
			pbox->x1, pbox->y1, 
			pbox->x1 + xMask, pbox->y1 + yMask,
			pbox->x2 - pbox->x1, pbox->y2 - pbox->y1);
	      pbox++;
	   }

	   SET_SYNC_FLAG(infoRec);
	   REGION_UNINIT(pScreen, &region);
	   return TRUE;
	}
    } else {	
	if(!(formats = infoRec->CPUToScreenTextureFormats))
	    return FALSE;

        w = pSrc->pDrawable->width;
        h = pSrc->pDrawable->height;

        if(pSrc->repeat) {
              if((infoRec->CPUToScreenTextureFlags & XAA_RENDER_NO_TILE) ||
                   ((infoRec->CPUToScreenTextureFlags &
                                   XAA_RENDER_POWER_OF_2_TILE_ONLY) &&
                                ((h & (h - 1)) || (w & (w - 1)))))
              {
                 return FALSE;
              }
              flags |= XAA_RENDER_REPEAT;
        }


	while(*formats != pSrc->format) {
	    if(!(*formats)) return FALSE;
	    formats++;
	}

	if (!miComputeCompositeRegion (&region, pSrc, pMask, pDst,
                                   xSrc, ySrc, xMask, yMask, xDst, yDst,
                                   width, height))
		return TRUE;

	nbox = REGION_NUM_RECTS(&region);
	pbox = REGION_RECTS(&region);   
	     
        if(!nbox) {
             REGION_UNINIT(pScreen, &region);
             return TRUE;
        }

	if(!(infoRec->SetupForCPUToScreenTexture)(infoRec->pScrn,
			op, pSrc->format, 
			((PixmapPtr)(pSrc->pDrawable))->devPrivate.ptr,
			((PixmapPtr)(pSrc->pDrawable))->devKind, 
			w, h, flags))
        {
              REGION_UNINIT(pScreen, &region);
              return FALSE;
        }


	xSrc -= xDst;
	ySrc -= yDst;
	
	while(nbox--) {
	    (*infoRec->SubsequentCPUToScreenTexture)(infoRec->pScrn,
			pbox->x1, pbox->y1, 
			pbox->x1 + xSrc, pbox->y1 + ySrc,
			pbox->x2 - pbox->x1, pbox->y2 - pbox->y1);
	    pbox++;
	 }

	SET_SYNC_FLAG(infoRec);
	REGION_UNINIT(pScreen, &region);
	return TRUE;
    }


    return FALSE;
}


void
XAAComposite (CARD8      op,
	      PicturePtr pSrc,
	      PicturePtr pMask,
	      PicturePtr pDst,
	      INT16      xSrc,
	      INT16      ySrc,
	      INT16      xMask,
	      INT16      yMask,
	      INT16      xDst,
	      INT16      yDst,
	      CARD16     width,
	      CARD16     height)
{
    ScreenPtr	pScreen = pDst->pDrawable->pScreen;
    XAAInfoRecPtr infoRec = GET_XAAINFORECPTR_FROM_SCREEN(pScreen);
    XAA_RENDER_PROLOGUE(pScreen, Composite);

    if(!infoRec->Composite ||
       !(*infoRec->Composite)(op, pSrc, pMask, pDst,
                       xSrc, ySrc, xMask, yMask, xDst, yDst,
                       width, height))
    {
        if(pSrc->pDrawable->type == DRAWABLE_WINDOW ||
           pDst->pDrawable->type == DRAWABLE_WINDOW ||
           IS_OFFSCREEN_PIXMAP(pSrc->pDrawable) ||
           IS_OFFSCREEN_PIXMAP(pDst->pDrawable)) {
            SYNC_CHECK(pDst->pDrawable);
        }
        (*GetPictureScreen(pScreen)->Composite) (op,
		       pSrc,
		       pMask,
		       pDst,
		       xSrc,
		       ySrc,
		       xMask,
		       yMask,
		       xDst,
		       yDst,
		       width,
		       height);    
    }

    if(pDst->pDrawable->type == DRAWABLE_PIXMAP)
	(XAA_GET_PIXMAP_PRIVATE((PixmapPtr)(pDst->pDrawable)))->flags |= DIRTY;

    XAA_RENDER_EPILOGUE(pScreen, Composite, XAAComposite);
}

Bool
XAADoGlyphs (CARD8         op,
	   PicturePtr    pSrc,
	   PicturePtr    pDst,
	   PictFormatPtr maskFormat,
	   INT16         xSrc,
	   INT16         ySrc,
	   int           nlist,
	   GlyphListPtr  list,
	   GlyphPtr      *glyphs)
{
    ScreenPtr	pScreen = pDst->pDrawable->pScreen;
    XAAInfoRecPtr infoRec = GET_XAAINFORECPTR_FROM_SCREEN(pScreen);

    if(!REGION_NUM_RECTS(pDst->pCompositeClip))
	return TRUE;

    if(!infoRec->pScrn->vtSema || 
      ((pDst->pDrawable->type != DRAWABLE_WINDOW) &&
	!IS_OFFSCREEN_PIXMAP(pDst->pDrawable)))
	return FALSE;

    if((pSrc->pDrawable->type != DRAWABLE_PIXMAP) ||
        IS_OFFSCREEN_PIXMAP(pSrc->pDrawable))
        return FALSE;

    if(maskFormat && (maskFormat->depth == 1) && 
       (pSrc->pDrawable->width == 1) && (pSrc->pDrawable->height == 1) &&
       (op == PictOpOver) && infoRec->WriteBitmap &&
       !(infoRec->WriteBitmapFlags & NO_TRANSPARENCY))
    {
	CARD16 red, green, blue, alpha;
	CARD32 pixel =
                *((CARD32*)(((PixmapPtr)(pSrc->pDrawable))->devPrivate.ptr));
	CARD32 *bits, *pntr, *pnt;
	int x, y, i, n, left, top, right, bottom, width, height, pitch;
	int L, T, R, B, X, Y, h, w, dwords, row, column, nbox;
	int leftEdge, rightEdge, topLine, botLine;
	BoxPtr pbox;
	GlyphPtr glyph;
	
	if(!XAAGetRGBAFromPixel(pixel,&red,&green,&blue,&alpha,pSrc->format))
		return FALSE;

	if(alpha != 0xffff) return FALSE;

	XAAGetPixelFromRGBA(&pixel, red, green, blue, 0, pDst->format);

	if((infoRec->WriteBitmapFlags & RGB_EQUAL) && !((red == green) && (green == blue)))
	   return FALSE;

	x = pDst->pDrawable->x;
	y = pDst->pDrawable->y;

	while(nlist--) {
	    x += list->xOff;
	    y += list->yOff;
	    left = right = X = x;
	    top = bottom = Y = y;
	    for(i = 0; i < list->len; i++) {
		glyph = glyphs[i];

		L = X - glyph->info.x;
		if(L < left) left = L;
		R = L + glyph->info.width;
		if(R > right) right = R;

		T = Y - glyph->info.y;
		if(T < top) top = T;
		B = T + glyph->info.height;
		if(B > bottom) bottom = B;

		X += glyph->info.xOff;
		Y += glyph->info.yOff;
	    }

	    width = right - left;
	    height = bottom - top;

	    if(width && height) {
		pitch = (((width + 31) & ~31) >> 5) + 1;
		pntr = (CARD32*)xalloc(sizeof(CARD32) * pitch * height);
		if(!pntr) 
		    return TRUE;
		bzero(pntr, sizeof(CARD32) * pitch * height);
		n = list->len;

		X = x; Y = y;
		while(n--) {
		    glyph = *glyphs++;
		    h = glyph->info.height;
		    w = glyph->info.width;
		    if(h && w) {
			row = y - top - glyph->info.y;
			column = x - left - glyph->info.x;
			pnt = pntr + (row * pitch) + (column >> 5);
			column &= 31;
			dwords = ((w + 31) >> 5) - 1;
			bits = (CARD32*)(glyph + 1);
			if(dwords) {
			  while(h--) {
			    for(i = 0; i <= dwords; i++) {
				if(column) {
				    pnt[i] |= SHIFT_L(*bits, column);
				    pnt[i + 1] |= SHIFT_R(*bits, 32 - column);
				} else
				    pnt[i] |= *bits;

				if(i != dwords) bits++;
			    }
			    bits++;
			    pnt += pitch;
			  } 
			} else {
			  if(column) {
			     while(h--) {
				pnt[0] |= SHIFT_L(*bits, column);
				pnt[0 + 1] |= SHIFT_R(*bits, 32 - column);
				bits++;
				pnt += pitch;
			     }
			  } else {
			     while(h--) {
				*pnt |= *bits++;
				pnt += pitch;
			     }			  
			  }	  
			}
		    }
		    x += glyph->info.xOff;
		    y += glyph->info.yOff;
		}
		
		nbox = REGION_NUM_RECTS(pDst->pCompositeClip);
		pbox = REGION_RECTS(pDst->pCompositeClip);
		
		while(nbox && (top >= pbox->y2)) {
		    pbox++; nbox--;
		}
		
		while(nbox && (bottom > pbox->y1)) {		
		    leftEdge = max(left, pbox->x1);
		    rightEdge = min(right, pbox->x2);
		    
		    if(rightEdge > leftEdge) {
		    	column = leftEdge - left;
			topLine = max(top, pbox->y1);
			botLine = min(bottom, pbox->y2);
			h = botLine - topLine;
			
			if(h > 0) {
			  (*infoRec->WriteBitmap)(infoRec->pScrn, 
			  	leftEdge, topLine, rightEdge - leftEdge, h,
				(unsigned char*)(pntr + 
				  ((topLine - top) * pitch) + (column >> 5)),
				pitch << 2, column & 31, pixel, -1, GXcopy, ~0);
			}
		    }	
		    nbox--; pbox++;
	   	}
		xfree(pntr);
	    } else {
		x = X; y = Y;
	    }
	    list++;
	}

	return TRUE;
    }

    /*
     * If it looks like we have a chance of being able to draw these
     * glyphs with an accelerated Composite, do that now to avoid
     * unneeded and costly syncs.
     */
    if(maskFormat) {
        if(!infoRec->CPUToScreenAlphaTextureFormats)
            return FALSE;
    } else {
        if(!infoRec->CPUToScreenTextureFormats)
            return FALSE;
    }

    miGlyphs(op, pSrc, pDst, maskFormat, xSrc, ySrc, nlist, list, glyphs);

    return TRUE;
}	   
	 
	
void
XAAGlyphs (CARD8         op,
	   PicturePtr    pSrc,
	   PicturePtr    pDst,
	   PictFormatPtr maskFormat,
	   INT16         xSrc,
	   INT16         ySrc,
	   int           nlist,
	   GlyphListPtr  list,
	   GlyphPtr      *glyphs)
{
    ScreenPtr	pScreen = pDst->pDrawable->pScreen;
    XAAInfoRecPtr infoRec = GET_XAAINFORECPTR_FROM_SCREEN(pScreen);
    XAA_RENDER_PROLOGUE(pScreen, Glyphs);

    if(!infoRec->Glyphs ||
       !(*infoRec->Glyphs)(op, pSrc, pDst, maskFormat,
                                          xSrc, ySrc, nlist, list, glyphs))
    {
       if((pSrc->pDrawable->type == DRAWABLE_WINDOW) ||
          (pDst->pDrawable->type == DRAWABLE_WINDOW) ||
          IS_OFFSCREEN_PIXMAP(pSrc->pDrawable) ||
          IS_OFFSCREEN_PIXMAP(pDst->pDrawable)) {
           SYNC_CHECK(pDst->pDrawable);
       }
       (*GetPictureScreen(pScreen)->Glyphs) (op, pSrc, pDst, maskFormat,
					  xSrc, ySrc, nlist, list, glyphs);
    }

    if(pDst->pDrawable->type == DRAWABLE_PIXMAP)
	(XAA_GET_PIXMAP_PRIVATE((PixmapPtr)(pDst->pDrawable)))->flags |= DIRTY;

    XAA_RENDER_EPILOGUE(pScreen, Glyphs, XAAGlyphs);
}
