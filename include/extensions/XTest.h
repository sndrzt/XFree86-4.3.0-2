/* $Xorg: XTest.h,v 1.5 2001/02/09 02:03:24 xorgcvs Exp $ */
/*

Copyright 1992, 1998  The Open Group

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

*/
/* $XFree86: xc/include/extensions/XTest.h,v 3.2 2001/01/17 17:53:19 dawes Exp $ */

#ifndef _XTEST_H_
#define _XTEST_H_

#include <X11/Xfuncproto.h>

#define X_XTestGetVersion	0
#define X_XTestCompareCursor	1
#define X_XTestFakeInput	2
#define X_XTestGrabControl	3

#define XTestNumberEvents	0

#define XTestNumberErrors	0

#define XTestMajorVersion	2
#define XTestMinorVersion	2

#define XTestExtensionName	"XTEST"

#ifndef _XTEST_SERVER_

#include <X11/extensions/XInput.h>

_XFUNCPROTOBEGIN

Bool XTestQueryExtension(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    int*		/* event_basep */,
    int*		/* error_basep */,
    int*		/* majorp */,
    int*		/* minorp */
#endif
);

Bool XTestCompareCursorWithWindow(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    Window		/* window */,
    Cursor		/* cursor */
#endif
);

Bool XTestCompareCurrentCursorWithWindow(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    Window		/* window */
#endif
);

extern int XTestFakeKeyEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    unsigned int	/* keycode */,
    Bool		/* is_press */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeButtonEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    unsigned int	/* button */,
    Bool		/* is_press */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeMotionEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    int			/* screen */,
    int			/* x */,
    int			/* y */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeRelativeMotionEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    int			/* x */,
    int			/* y */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeDeviceKeyEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XDevice*		/* dev */,
    unsigned int	/* keycode */,
    Bool		/* is_press */,
    int*		/* axes */,
    int			/* n_axes */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeDeviceButtonEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XDevice*		/* dev */,
    unsigned int	/* button */,
    Bool		/* is_press */,
    int*		/* axes */,
    int			/* n_axes */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeProximityEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XDevice*		/* dev */,
    Bool		/* in_prox */,
    int*		/* axes */,
    int			/* n_axes */,
    unsigned long	/* delay */
#endif
);

extern int XTestFakeDeviceMotionEvent(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XDevice*		/* dev */,
    Bool		/* is_relative */,
    int			/* first_axis */,
    int*		/* axes */,
    int			/* n_axes */,
    unsigned long	/* delay */
#endif
);

extern int XTestGrabControl(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    Bool		/* impervious */
#endif
);

void XTestSetGContextOfGC(
#if NeedFunctionPrototypes
    GC			/* gc */,
    GContext		/* gid */
#endif
);

void XTestSetVisualIDOfVisual(
#if NeedFunctionPrototypes
    Visual*		/* visual */,
    VisualID		/* visualid */
#endif
);

Status XTestDiscard(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

_XFUNCPROTOEND

#endif /* _XTEST_SERVER_ */

#endif
