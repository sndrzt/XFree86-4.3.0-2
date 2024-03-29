/*
 * Copyright (c) 2002 by The XFree86 Project, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE XFREE86 PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the XFree86 Project shall
 * not be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from the
 * XFree86 Project.
 *
 * Author: Paulo C�sar Pereira de Andrade
 */

/* $XFree86: xc/programs/xedit/lisp/package.h,v 1.6 2002/11/23 21:41:52 paulo Exp $ */

#ifndef Lisp_package_h
#define Lisp_package_h

#include "internal.h"

void LispPackageInit(void);
LispObj *LispFindPackage(LispObj*);
LispObj *LispFindPackageFromString(char*);
/* returns 1 if string can safely be read back */
int LispCheckAtomString(char*);

LispObj *Lisp_DoAllSymbols(LispBuiltin*);
LispObj *Lisp_DoExternalSymbols(LispBuiltin*);
LispObj *Lisp_DoSymbols(LispBuiltin*);
LispObj *Lisp_FindAllSymbols(LispBuiltin*);
LispObj *Lisp_FindPackage(LispBuiltin*);
LispObj *Lisp_FindSymbol(LispBuiltin*);
LispObj *Lisp_Export(LispBuiltin*);
LispObj *Lisp_Import(LispBuiltin*);
LispObj *Lisp_InPackage(LispBuiltin*);
LispObj *Lisp_Intern(LispBuiltin*);
LispObj *Lisp_ListAllPackages(LispBuiltin*);
LispObj *Lisp_MakePackage(LispBuiltin*);
LispObj *Lisp_Packagep(LispBuiltin*);
LispObj *Lisp_PackageName(LispBuiltin*);
LispObj *Lisp_PackageNicknames(LispBuiltin*);
LispObj *Lisp_PackageUseList(LispBuiltin*);
LispObj *Lisp_PackageUsedByList(LispBuiltin*);
LispObj *Lisp_Unexport(LispBuiltin*);

#endif /* Lisp_package_h */
