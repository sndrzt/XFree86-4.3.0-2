/* $Xorg: SetPntMap.c,v 1.4 2001/02/09 02:03:36 xorgcvs Exp $ */
/*

Copyright 1986, 1998  The Open Group

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
/* $XFree86: xc/lib/X11/SetPntMap.c,v 1.3 2001/01/17 19:41:44 dawes Exp $ */

#define NEED_REPLIES

#include "Xlibint.h"
/* returns either  DeviceMappingSuccess or DeviceMappingBusy  */

#if NeedFunctionPrototypes
int XSetPointerMapping (
    register Display *dpy,
    _Xconst unsigned char *map,
    int nmaps)
#else
int XSetPointerMapping (dpy, map, nmaps)
    register Display *dpy;
    unsigned char *map;
    int nmaps;
#endif
    {
    register xSetPointerMappingReq *req;
    xSetPointerMappingReply rep;

    LockDisplay(dpy);
    GetReq (SetPointerMapping, req);
    req->nElts = nmaps;
    req->length += (nmaps + 3)>>2;
    Data (dpy, (char *)map, (long) nmaps);
    if (_XReply (dpy, (xReply *)&rep, 0, xFalse) == 0) 
	rep.success = MappingSuccess;
    UnlockDisplay(dpy);
    SyncHandle();
    return ((int) rep.success);
    }

int
XChangeKeyboardMapping (dpy, first_keycode, keysyms_per_keycode, 
		     keysyms, nkeycodes)
    register Display *dpy;
    int first_keycode;
    int keysyms_per_keycode;
    KeySym *keysyms;
    int nkeycodes;
    {
    register long nbytes;
    register xChangeKeyboardMappingReq *req;

    LockDisplay(dpy);
    GetReq (ChangeKeyboardMapping, req);
    req->firstKeyCode = first_keycode;
    req->keyCodes = nkeycodes;
    req->keySymsPerKeyCode = keysyms_per_keycode;
    req->firstKeyCode = first_keycode;
    req->length += nkeycodes * keysyms_per_keycode;
    nbytes = keysyms_per_keycode * nkeycodes * 4;
    Data32 (dpy, (long *)keysyms, nbytes);
    UnlockDisplay(dpy);
    SyncHandle();
    return 0;
    }
    
