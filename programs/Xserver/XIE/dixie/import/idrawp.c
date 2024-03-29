/* $Xorg: idrawp.c,v 1.4 2001/02/09 02:04:20 xorgcvs Exp $ */
/**** module idrawp.c ****/
/******************************************************************************

Copyright 1993, 1994, 1998  The Open Group

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


				NOTICE
                              
This software is being provided by AGE Logic, Inc. under the
following license.  By obtaining, using and/or copying this software,
you agree that you have read, understood, and will comply with these
terms and conditions:

     Permission to use, copy, modify, distribute and sell this
     software and its documentation for any purpose and without
     fee or royalty and to grant others any or all rights granted
     herein is hereby granted, provided that you agree to comply
     with the following copyright notice and statements, including
     the disclaimer, and that the same appears on all copies and
     derivative works of the software and documentation you make.
     
     "Copyright 1993, 1994 by AGE Logic, Inc."
     
     THIS SOFTWARE IS PROVIDED "AS IS".  AGE LOGIC MAKES NO
     REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By way of
     example, but not limitation, AGE LOGIC MAKE NO
     REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS
     FOR ANY PARTICULAR PURPOSE OR THAT THE SOFTWARE DOES NOT
     INFRINGE THIRD-PARTY PROPRIETARY RIGHTS.  AGE LOGIC 
     SHALL BEAR NO LIABILITY FOR ANY USE OF THIS SOFTWARE.  IN NO
     EVENT SHALL EITHER PARTY BE LIABLE FOR ANY INDIRECT,
     INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS
     OF PROFITS, REVENUE, DATA OR USE, INCURRED BY EITHER PARTY OR
     ANY THIRD PARTY, WHETHER IN AN ACTION IN CONTRACT OR TORT OR
     BASED ON A WARRANTY, EVEN IF AGE LOGIC LICENSEES
     HEREUNDER HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
     DAMAGES.
    
     The name of AGE Logic, Inc. may not be used in
     advertising or publicity pertaining to this software without
     specific, written prior permission from AGE Logic.

     Title to this software shall at all times remain with AGE
     Logic, Inc.
*****************************************************************************
  
	idrawp.c -- DIXIE routines for managing the ImportDrawablePlane element
  
	Dean Verheiden -- AGE Logic, Inc. June 1993
  
*****************************************************************************/
/* $XFree86: xc/programs/Xserver/XIE/dixie/import/idrawp.c,v 3.4 2001/01/17 22:12:50 dawes Exp $ */

#define _XIEC_IDRAWP

/*
 *  Include files
 */
  /*
   *  Core X Includes
   */
#define NEED_EVENTS
#include <X.h>
#include <Xproto.h>
  /*
   *  XIE Includes
   */
#include <dixie_i.h>
  /*
   *  more X server includes.
   */
#include <pixmapstr.h>
#include <scrnintstr.h>
#include <windowstr.h>
#include <window.h>
  /*
   *  Server XIE Includes
   */
#include <corex.h>
#include <error.h>
#include <macro.h>
#include <element.h>

/*
 *  routines internal to this module
 */
static Bool PrepIDrawP(floDefPtr flo, peDefPtr ped);

/*
 * dixie entry points
 */
static diElemVecRec iDrawPVec = {
    PrepIDrawP		/* prepare for analysis and execution	*/
    };


/*------------------------------------------------------------------------
----------------- routine: make an ImportDrawablePlane element ----------------
------------------------------------------------------------------------*/
peDefPtr MakeIDrawP(floDefPtr flo, xieTypPhototag tag, xieFlo *pe)
{
  peDefPtr ped;
  ELEMENT(xieFloImportDrawablePlane);
  ELEMENT_SIZE_MATCH(xieFloImportDrawablePlane);
  
  if(!(ped = MakePEDef(1, (CARD32)stuff->elemLength<<2, sizeof(iDrawDefRec)))) 
    FloAllocError(flo,tag,xieElemImportDrawablePlane, return(NULL));
  
  ped->diVec	    = &iDrawPVec;
  ped->phototag     = tag;
  ped->flags.import = TRUE;
  raw = (xieFloImportDrawablePlane *)ped->elemRaw;
  /*
   * copy the client element parameters (swap if necessary)
   */
  if( flo->reqClient->swapped ) {
    raw->elemType   = stuff->elemType;
    raw->elemLength = stuff->elemLength;
    raw->notify     = stuff->notify; 
    cpswapl(stuff->drawable, raw->drawable);
    cpswaps(stuff->srcX, raw->srcX);
    cpswaps(stuff->srcY, raw->srcY);
    cpswaps(stuff->width, raw->width);
    cpswaps(stuff->height, raw->height);
    cpswapl(stuff->fill, raw->fill);
    cpswapl(stuff->bitPlane, raw->bitPlane);
  }
  else
    memcpy((char *)raw, (char *)stuff, sizeof(xieFloImportDrawablePlane));
  
  return(ped);
}                               /* end MakeIDrawP */


/*------------------------------------------------------------------------
---------------- routine: prepare for analysis and execution -------------
------------------------------------------------------------------------*/
static Bool PrepIDrawP(floDefPtr flo, peDefPtr ped)
{
  xieFloImportDrawablePlane *raw = (xieFloImportDrawablePlane *) ped->elemRaw;
  iDrawDefPtr pvt 	    = (iDrawDefPtr) ped->elemPvt;
  inFloPtr    inf 	    = &ped->inFloLst[IMPORT];
  outFloPtr   dst 	    = &ped->outFlo; 
  formatPtr   fmt           = &inf->format[0]; 
  DrawablePtr pd; 
  CARD32   f, padmask;

  if(!(pd = pvt->pDraw = ((DrawablePtr)
			  LookupIDByClass(raw->drawable, RC_DRAWABLE))))
    DrawableError(flo,ped,raw->drawable,return(FALSE));

  if(!((pd->type == DRAWABLE_WINDOW && ((WindowPtr)pd)->realized) ||
        pd->type == DRAWABLE_PIXMAP))
    DrawableError(flo,ped,raw->drawable, return(FALSE));

  if(raw->srcX < 0) {
    ValueError(flo,ped,raw->srcX, return(FALSE));
  } else if (raw->srcY < 0) {
    ValueError(flo,ped,raw->srcY, return(FALSE));
  } else if (raw->srcX + raw->width  > pd->width) {
    ValueError(flo,ped,raw->width, return(FALSE));
  } else if (raw->srcY + raw->height > pd->height) {
    ValueError(flo,ped,raw->height, return(FALSE));
  } else if(!raw->bitPlane || raw->bitPlane & (raw->bitPlane - 1) ||
      raw->bitPlane >= (1<<pd->depth))
    ValueError(flo,ped,raw->bitPlane, return(FALSE));

  /* find the screen format that matches this drawable and fill in the format
   */
  for(f = 0; f < screenInfo.numPixmapFormats
      && pd->depth != screenInfo.formats[f].depth; ++f);
  if(f == screenInfo.numPixmapFormats)
    DrawableError(flo,ped,raw->drawable, return(FALSE));
  padmask = screenInfo.formats[f].scanlinePad - 1;
  fmt->interleaved = FALSE;
  fmt->band   = 0;
  fmt->depth  = pd->depth;
  fmt->width  = raw->width;
  fmt->height = raw->height;
  fmt->levels = 1<<pd->depth;
  fmt->stride = screenInfo.formats[f].bitsPerPixel;
  fmt->pitch  = (fmt->stride * raw->width + padmask) & ~padmask;
  /*
   * set output attributes from input format (stride and pitch may differ)
   */
  dst->bands     = inf->bands = 1;
  dst->format[0] = inf->format[0];
  dst->format[0].levels = 2;
  if(!UpdateFormatfromLevels(ped))
    MatchError(flo,ped, return(FALSE));

  return(TRUE);
}                               /* end PrepIDrawP */
/* end module idrawp.c */
