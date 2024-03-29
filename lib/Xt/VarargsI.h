/* $Xorg: VarargsI.h,v 1.4 2001/02/09 02:03:59 xorgcvs Exp $ */

/*

Copyright 1985, 1986, 1987, 1988, 1989, 1998  The Open Group

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
/* $XFree86: xc/lib/Xt/VarargsI.h,v 3.5 2001/07/29 05:01:12 tsi Exp $ */

#ifndef _VarargsI_h_ 
#define _VarargsI_h_ 

#include <stdarg.h>
 
/* private routines */

extern void _XtCountVaList(
#if NeedFunctionPrototypes
    va_list /*var*/, int* /*total_count*/, int* /*typed_count*/
#endif
);

extern void _XtVaToArgList(
#if NeedFunctionPrototypes
   Widget /*widget*/, va_list /*var*/, int /*max_count*/, ArgList* /*args_return*/, Cardinal* /*num_args_return*/
#endif
);

extern void _XtVaToTypedArgList(
#if NeedFunctionPrototypes
    va_list /*var*/, int /*count*/, XtTypedArgList* /*args_return*/, Cardinal* /*num_args_return*/
#endif
);

extern XtTypedArgList _XtVaCreateTypedArgList(
#if NeedFunctionPrototypes
    va_list /*var*/, int /*count*/
#endif
);

extern void _XtFreeArgList(
#if NeedFunctionPrototypes
    ArgList /*args*/, int /*total_count*/, int /*typed_count*/
#endif
);

extern void _XtGetApplicationResources(
#if NeedFunctionPrototypes
    Widget /*w*/, XtPointer /*base*/, XtResourceList /*resources*/, Cardinal /*num_resources*/, ArgList /*args*/, Cardinal /*num_args*/, XtTypedArgList /*typed_args*/, Cardinal /*num_typed_args*/
#endif
);

extern void _XtGetSubresources(
#if NeedFunctionPrototypes
    Widget /*w*/, XtPointer /*base*/, const char* /*name*/, const char* /*class*/, XtResourceList /*resources*/, Cardinal /*num_resources*/, ArgList /*args*/, Cardinal /*num_args*/, XtTypedArgList /*typed_args*/, Cardinal /*num_typed_args*/
#endif
);

#endif /* _VarargsI_h_ */
