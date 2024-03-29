/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86xvmc.h,v 1.4 2001/04/08 23:00:27 alanh Exp $ */

#ifndef _XF86XVMC_H
#define _XF86XVMC_H

#include "xvmcext.h"
#include "xf86xv.h"

typedef struct {
  int num_xvimages;
  int *xvimage_ids;  /* reference the subpictures in the XF86MCAdaptorRec */
} XF86MCImageIDList; 

typedef struct {
  int surface_type_id;  /* Driver generated.  Must be unique on the port */
  int chroma_format;
  int color_description;  /* no longer used */
  unsigned short max_width;       
  unsigned short max_height;   
  unsigned short subpicture_max_width;
  unsigned short subpicture_max_height;
  int mc_type;         
  int flags;
  XF86MCImageIDList *compatible_subpictures; /* can be null, if none */
} XF86MCSurfaceInfoRec, *XF86MCSurfaceInfoPtr;


/*
   xf86XvMCCreateContextProc 

   DIX will fill everything out in the context except the driver_priv.
   The port_priv holds the private data specified for the port when
   Xv was initialized by the driver.
   The driver may store whatever it wants in driver_priv and edit
   the width, height and flags.  If the driver wants to return something
   to the client it can allocate space in priv and specify the number
   of 32 bit words in num_priv.  This must be dynamically allocated
   space because DIX will free it after it passes it to the client.
*/
   

typedef int (*xf86XvMCCreateContextProcPtr) (
  ScrnInfoPtr pScrn,
  XvMCContextPtr context,
  int *num_priv,
  CARD32 **priv 
);

typedef void (*xf86XvMCDestroyContextProcPtr) (
  ScrnInfoPtr pScrn,
  XvMCContextPtr context
);

/*
   xf86XvMCCreateSurfaceProc 

   DIX will fill everything out in the surface except the driver_priv.
   The driver may store whatever it wants in driver_priv.  The driver
   may pass data back to the client in the same manner as the
   xf86XvMCCreateContextProc.
*/


typedef int (*xf86XvMCCreateSurfaceProcPtr) (
  ScrnInfoPtr pScrn,
  XvMCSurfacePtr surface,
  int *num_priv,
  CARD32 **priv
);

typedef void (*xf86XvMCDestroySurfaceProcPtr) (
  ScrnInfoPtr pScrn,
  XvMCSurfacePtr surface
);

/*
   xf86XvMCCreateSubpictureProc 

   DIX will fill everything out in the subpicture except the driver_priv,
   num_palette_entries, entry_bytes and component_order.  The driver may
   store whatever it wants in driver_priv and edit the width and height.
   If it is a paletted subpicture the driver needs to fill out the
   num_palette_entries, entry_bytes and component_order.  These are
   not communicated to the client until the time the surface is
   created.

   The driver may pass data back to the client in the same manner as the
   xf86XvMCCreateContextProc.
*/


typedef int (*xf86XvMCCreateSubpictureProcPtr) (
  ScrnInfoPtr pScrn,
  XvMCSubpicturePtr subpicture,
  int *num_priv,
  CARD32 **priv
);

typedef void (*xf86XvMCDestroySubpictureProcPtr) (
  ScrnInfoPtr pScrn,
  XvMCSubpicturePtr subpicture
);


typedef struct {
  char *name;
  int num_surfaces;
  XF86MCSurfaceInfoPtr *surfaces;
  int num_subpictures;
  XF86ImagePtr *subpictures;
  xf86XvMCCreateContextProcPtr 		CreateContext; 
  xf86XvMCDestroyContextProcPtr		DestroyContext; 
  xf86XvMCCreateSurfaceProcPtr 		CreateSurface; 
  xf86XvMCDestroySurfaceProcPtr		DestroySurface; 
  xf86XvMCCreateSubpictureProcPtr	CreateSubpicture; 
  xf86XvMCDestroySubpictureProcPtr	DestroySubpicture; 
} XF86MCAdaptorRec, *XF86MCAdaptorPtr;

/* 
   xf86XvMCScreenInit 

   Unlike Xv, the adaptor data is not copied from this structure.
   This structure's data is used so it must stick around for the
   life of the server.  Note that it's an array of pointers not
   an array of structures.
*/

Bool xf86XvMCScreenInit(
  ScreenPtr pScreen, 
  int num_adaptors,
  XF86MCAdaptorPtr *adaptors
);

#endif /* _XF86XVMC_H */
