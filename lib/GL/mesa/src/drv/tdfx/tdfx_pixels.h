/* -*- mode: c; c-basic-offset: 3 -*-
 *
 * Copyright 2000 VA Linux Systems Inc., Fremont, California.
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* $XFree86: xc/lib/GL/mesa/src/drv/tdfx/tdfx_pixels.h,v 1.1 2001/03/21 16:14:28 dawes Exp $ */

/*
 * Original rewrite:
 *	Gareth Hughes <gareth@valinux.com>, 29 Sep - 1 Oct 2000
 *
 * Authors:
 *	Gareth Hughes <gareth@valinux.com>
 *	Brian Paul <brianp@valinux.com>
 *	Nathan Hand <nhand@valinux.com>
 *
 */

#ifndef __TDFX_PIXELS_H__
#define __TDFX_PIXELS_H__

#ifdef GLX_DIRECT_RENDERING

#include "context.h"

extern void
tdfx_bitmap_R5G6B5( GLcontext *ctx, GLint px, GLint py,
		    GLsizei width, GLsizei height,
		    const struct gl_pixelstore_attrib *unpack,
		    const GLubyte *bitmap );

extern void
tdfx_bitmap_R8G8B8A8( GLcontext *ctx, GLint px, GLint py,
		      GLsizei width, GLsizei height,
		      const struct gl_pixelstore_attrib *unpack,
		      const GLubyte *bitmap );

extern void
tdfx_readpixels_R5G6B5( GLcontext *ctx, GLint x, GLint y,
			GLsizei width, GLsizei height,
			GLenum format, GLenum type,
			const struct gl_pixelstore_attrib *packing,
			GLvoid *dstImage );

extern void
tdfx_readpixels_R8G8B8A8( GLcontext *ctx, GLint x, GLint y,
			  GLsizei width, GLsizei height,
			  GLenum format, GLenum type,
			  const struct gl_pixelstore_attrib *packing,
			  GLvoid *dstImage );

extern void
tdfx_drawpixels_R8G8B8A8( GLcontext *ctx, GLint x, GLint y,
			  GLsizei width, GLsizei height,
			  GLenum format, GLenum type,
			  const struct gl_pixelstore_attrib *unpack,
			  const GLvoid *pixels );

#endif
#endif
