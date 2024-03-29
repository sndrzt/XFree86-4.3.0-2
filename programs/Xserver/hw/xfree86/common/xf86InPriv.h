/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86InPriv.h,v 1.3 1999/05/16 06:55:49 dawes Exp $ */

#ifndef _xf86InPriv_h
#define _xf86InPriv_h

/* xf86Globals.c */
#ifdef XFree86LOADER
extern InputDriverPtr *xf86InputDriverList;
#else
extern InputDriverPtr xf86InputDriverList[];
#endif
extern int xf86NumInputDrivers;

/* xf86Xinput.c */
void xf86ActivateDevice(InputInfoPtr pInfo);

#endif /* _xf86InPriv_h */
