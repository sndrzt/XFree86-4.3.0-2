/* $Xorg: StCol.c,v 1.3 2000/08/17 19:44:55 cpqbld Exp $ */

/*
 * Code and supporting documentation (c) Copyright 1990 1991 Tektronix, Inc.
 * 	All Rights Reserved
 * 
 * This file is a component of an X Window System-specific implementation
 * of Xcms based on the TekColor Color Management System.  Permission is
 * hereby granted to use, copy, modify, sell, and otherwise distribute this
 * software and its documentation for any purpose and without fee, provided
 * that this copyright, permission, and disclaimer notice is reproduced in
 * all copies of this software and in supporting documentation.  TekColor
 * is a trademark of Tektronix, Inc.
 * 
 * Tektronix makes no representation about the suitability of this software
 * for any purpose.  It is provided "as is" and with all faults.
 * 
 * TEKTRONIX DISCLAIMS ALL WARRANTIES APPLICABLE TO THIS SOFTWARE,
 * INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL TEKTRONIX BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA, OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR THE PERFORMANCE OF THIS SOFTWARE.
 *
 *
 *	NAME
 *		XcmsStCol.c
 *
 *	DESCRIPTION
 *		Source for XcmsStoreColor
 *
 *
 */
/* $XFree86: xc/lib/X11/StCol.c,v 1.2 1999/05/09 10:50:13 dawes Exp $ */

#include "Xlibint.h"
#include "Xcmsint.h"

/* EXTERN */

/* SetGetCols.c */
extern Status _XcmsSetGetColors();


/************************************************************************
 *									*
 *			PUBLIC ROUTINES					*
 *									*
 ************************************************************************/

/*
 *	NAME
 *		XcmsStoreColor - Store Color
 *
 *	SYNOPSIS
 */
Status
XcmsStoreColor(dpy, colormap, pColor_in)
    Display *dpy;
    Colormap colormap;
    XcmsColor *pColor_in;
/*
 *	DESCRIPTION
 *		Given a device-dependent or device-independent color
 *		specification, this routine will convert it to X RGB
 *		values then use it in a call to XStoreColor.
 *
 *	RETURNS
 *		XcmsFailure if failed;
 *		XcmsSuccess if it succeeded without gamut compression;
 *		XcmsSuccessWithCompression if it succeeded with gamut
 *			compression;
 *
 *		Since XStoreColor has no return value this routine
 *		does not return the color specification of the color actually
 *		stored.
 */
{
    XcmsColor tmpColor;

    tmpColor = *pColor_in;
    return(_XcmsSetGetColors (XStoreColor, dpy, colormap,
			      &tmpColor, 1, XcmsRGBFormat, (Bool *) NULL));
}
