/* $Xorg: XawInit.h,v 1.5 2001/02/09 02:03:47 xorgcvs Exp $
 *
Copyright 1989, 1994, 1998  The Open Group

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
/* $XFree86: xc/lib/Xaw/XawInit.h,v 1.12 2001/01/17 19:42:36 dawes Exp $ */

#ifndef _XawInit_h
#define _XawInit_h

#define XawVendor	"XFree86"

#ifdef OLDXAW
#define XawVersion	6700002L
#else
#define	XawVersion	7000002L

typedef struct _XawDL XawDisplayList;
#endif /* OLDXAW */

#include <X11/Xfuncproto.h>

_XFUNCPROTOBEGIN

void XawInitializeWidgetSet(void);
#ifndef OLDXAW
void XawInitializeDefaultConverters(void);
#endif

_XFUNCPROTOEND

#endif /* _XawInit_h */
