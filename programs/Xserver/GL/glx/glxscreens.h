#ifndef _GLX_screens_h_
#define _GLX_screens_h_

/* $XFree86: xc/programs/Xserver/GL/glx/glxscreens.h,v 1.4 2001/03/21 16:29:37 dawes Exp $ */
/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
*/

/* XXX: should be defined somewhere globally */
#define CAPI

#include "GL/internal/glcore.h"

/*
** Screen dependent data.  These methods are the interface between the DIX
** and DDX layers of the GLX server extension.  The methods provide an
** interface for context management on a screen.
*/
typedef struct {
    /*
    ** Probe the screen and see if it supports GL rendering.  It will
    ** return GL_FALSE if it doesn't, GL_TRUE otherwise.
    */
    Bool (*screenProbe)(int screen);

    /*
    ** Create a context using configuration information from modes.
    ** Use imports as callbacks back to the OS. Return an opaque handle
    ** on the context (NULL if failure).
    */
    __GLinterface *(*createContext)(__GLimports *imports,
				    __GLcontextModes *modes,
				    __GLinterface *shareGC);

    /*
    ** Create a buffer using information from glxPriv.  This routine
    ** sets up any wrappers necessary to resize, swap or destroy the
    ** buffer.
    */
    void (*createBuffer)(__GLXdrawablePrivate *glxPriv);

    __GLXvisualConfig *pGlxVisual;
    void **pVisualPriv;
    GLint numVisuals;
    GLint numUsableVisuals;

    char *GLextensions;

    char *GLXvendor;
    char *GLXversion;
    char *GLXextensions;

    /*
    ** Things that are not statically set.
    */
    Bool (*WrappedPositionWindow)(WindowPtr pWin, int x, int y);

} __GLXscreenInfo;


extern void __glXScreenInit(GLint);
extern void __glXScreenReset(void);

#endif /* !__GLX_screens_h__ */
