/* $XFree86: xc/programs/Xserver/Xext/dpmsproc.h,v 1.2 1998/07/25 08:48:37 dawes Exp $ */

/* Prototypes for functions that the DDX must provide */

#ifndef _DPMSPROC_H_
#define _DPMSPROC_H_

void DPMSSet(int level);
int  DPMSGet(int *level);
Bool DPMSSupported(void);

#endif
