/* $Xorg: LookupCol.c,v 1.4 2001/02/09 02:03:34 xorgcvs Exp $ */
/*

Copyright 1985, 1998  The Open Group

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
/* $XFree86: xc/lib/X11/LookupCol.c,v 1.3 2001/01/17 19:41:39 dawes Exp $ */

#define NEED_REPLIES
#include <stdio.h>
#include "Xlibint.h"
#include "Xcmsint.h"

extern Status _XcmsResolveColorString();

extern void _XcmsRGB_to_XColor();
extern void _XUnresolveColor();

#if NeedFunctionPrototypes
Status XLookupColor (
	register Display *dpy,
        Colormap cmap,
	_Xconst char *spec,
	XColor *def,
	XColor *scr)
#else
Status XLookupColor (dpy, cmap, spec, def, scr)
	register Display *dpy;
        Colormap cmap;
	char *spec;
	XColor *def, *scr;
#endif
{
	register int n;
	xLookupColorReply reply;
	register xLookupColorReq *req;
	XcmsCCC ccc;
	XcmsColor cmsColor_exact;

	/*
	 * Let's Attempt to use Xcms and i18n approach to Parse Color
	 */
	if ((ccc = XcmsCCCOfColormap(dpy, cmap)) != (XcmsCCC)NULL) {
	    const char *tmpName = spec;

	    switch (_XcmsResolveColorString(ccc, &tmpName, &cmsColor_exact,
					    XcmsRGBFormat)) {
	    case XcmsSuccess:
	    case XcmsSuccessWithCompression:
		_XcmsRGB_to_XColor(&cmsColor_exact, def, 1);
		memcpy((char *)scr, (char *)def, sizeof(XColor));
		_XUnresolveColor(ccc, scr);
		return(1);
	    case XcmsFailure:
	    case _XCMS_NEWNAME:
		/*
		 * if the result was _XCMS_NEWNAME tmpName points to
		 * a string in cmsColNm.c:pairs table, for example,
		 * gray70 would become tekhvc:0.0/70.0/0.0
		 */
		break;
	    }
	}

	/*
	 * Xcms and i18n methods failed, so lets pass it to the server
	 * for parsing.
	 */

	n = strlen (spec);
	LockDisplay(dpy);
	GetReq (LookupColor, req);
	req->cmap = cmap;
	req->nbytes = n;
	req->length += (n + 3) >> 2;
	Data (dpy, spec, (long)n);
	if (!_XReply (dpy, (xReply *) &reply, 0, xTrue)) {
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return (0);
	    }
	def->red   = reply.exactRed;
	def->green = reply.exactGreen;
	def->blue  = reply.exactBlue;

	scr->red   = reply.screenRed;
	scr->green = reply.screenGreen;
	scr->blue  = reply.screenBlue;

	UnlockDisplay(dpy);
	SyncHandle();
	return (1);
}
