/* $Xorg: Xlocale.h,v 1.4 2001/02/09 02:03:38 xorgcvs Exp $ */
/*

Copyright 1991, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: xc/lib/X11/Xlocale.h,v 1.3 2001/07/29 05:01:11 tsi Exp $ */

#ifndef _XLOCALE_H_
#define _XLOCALE_H_

#include <X11/Xfuncproto.h>
#include <X11/Xosdefs.h>

#ifndef X_LOCALE
#include <locale.h>
#else

#define LC_ALL      0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5

_XFUNCPROTOBEGIN
extern char *_Xsetlocale(
#if NeedFunctionPrototypes
    int /* category */,
    _Xconst char* /* name */
#endif
);
_XFUNCPROTOEND

#define setlocale _Xsetlocale

#include <stddef.h>

#endif /* X_LOCALE */

#endif /* _XLOCALE_H_ */
