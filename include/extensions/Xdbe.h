/* $Xorg: Xdbe.h,v 1.3 2000/08/18 04:05:45 coskrey Exp $ */
/******************************************************************************
 * 
 * Copyright (c) 1994, 1995  Hewlett-Packard Company
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL HEWLETT-PACKARD COMPANY BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the Hewlett-Packard
 * Company shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Hewlett-Packard Company.
 * 
 *     Header file for Xlib-related DBE
 *
 *****************************************************************************/
/* $XFree86$ */

#ifndef XDBE_H
#define XDBE_H


/* INCLUDES */

#include <X11/Xfuncproto.h>
#include <X11/extensions/Xdbeproto.h>


/* DEFINES */

/* Errors */
#define XdbeBadBuffer    0


/* TYPEDEFS */

typedef Drawable XdbeBackBuffer;

typedef unsigned char XdbeSwapAction;

typedef struct
{
    Window		swap_window;    /* window for which to swap buffers   */
    XdbeSwapAction	swap_action;    /* swap action to use for swap_window */
}
XdbeSwapInfo;

typedef struct
{
    Window	window;			/* window that buffer belongs to */
}
XdbeBackBufferAttributes;

typedef struct
{
    int			type;
    Display		*display;	/* display the event was read from */
    XdbeBackBuffer	buffer;		/* resource id                     */
    unsigned long	serial;		/* serial number of failed request */
    unsigned char	error_code;	/* error base + XdbeBadBuffer      */
    unsigned char	request_code;	/* major opcode of failed request  */
    unsigned char	minor_code;	/* minor opcode of failed request  */
}
XdbeBufferError;

/* _XFUNCPROTOBEGIN and _XFUNCPROTOEND are defined as noops
 * (for non-C++ builds) in X11/Xfuncproto.h.
 */
_XFUNCPROTOBEGIN

extern Status XdbeQueryExtension(
#if NeedFunctionPrototypes
    Display*		/* dpy                  */,
    int*		/* major_version_return */,
    int*		/* minor_version_return */
#endif
);

extern XdbeBackBuffer XdbeAllocateBackBufferName(
#if NeedFunctionPrototypes
    Display*		/* dpy         */,
    Window		/* window      */,
    XdbeSwapAction	/* swap_action */
#endif
);

extern Status XdbeDeallocateBackBufferName(
#if NeedFunctionPrototypes
    Display*		/* dpy    */,
    XdbeBackBuffer	/* buffer */
#endif
);

extern Status XdbeSwapBuffers(
#if NeedFunctionPrototypes
    Display*		/* dpy         */,
    XdbeSwapInfo*	/* swap_info   */,
    int			/* num_windows */
#endif
);

extern Status XdbeBeginIdiom(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

extern Status XdbeEndIdiom(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

extern XdbeScreenVisualInfo *XdbeGetVisualInfo(
#if NeedFunctionPrototypes
    Display*		/* dpy               */,
    Drawable*		/* screen_specifiers */,
    int*		/* num_screens       */
#endif
);

extern void XdbeFreeVisualInfo(
#if NeedFunctionPrototypes
    XdbeScreenVisualInfo*	/* visual_info */
#endif
);

extern XdbeBackBufferAttributes *XdbeGetBackBufferAttributes(
#if NeedFunctionPrototypes
    Display*		/* dpy    */,
    XdbeBackBuffer	/* buffer */
#endif
);

_XFUNCPROTOEND

#endif /* XDBE_H */

