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
/* $XFree86$ */

/*
 * Original rewrite:
 *	Gareth Hughes <gareth@valinux.com>, 29 Sep - 1 Oct 2000
 *
 * Authors:
 *	Gareth Hughes <gareth@valinux.com>
 *	Brian Paul <brianp@valinux.com>
 *
 */

#ifndef _TDFX_TEX_H_
#define _TDFX_TEX_H_


#include "texutil.h"


#define tdfxDDIsCompressedFormatMacro(internalFormat) \
    (((internalFormat) == GL_COMPRESSED_RGB_FXT1_3DFX) || \
     ((internalFormat) == GL_COMPRESSED_RGBA_FXT1_3DFX))
#define tdfxDDIsCompressedGlideFormatMacro(internalFormat) \
    ((internalFormat) == GR_TEXFMT_ARGB_CMP_FXT1)



extern void
tdfxTexValidate(GLcontext * ctx, struct gl_texture_object *tObj);

extern void
tdfxDDBindTexture(GLcontext * ctx, GLenum target,
                  struct gl_texture_object *tObj);

extern void
tdfxDDDeleteTexture(GLcontext * ctx, struct gl_texture_object *tObj);

extern GLboolean
tdfxDDIsTextureResident(GLcontext *ctx, struct gl_texture_object *tObj);

extern void
tdfxDDTexturePalette(GLcontext * ctx, struct gl_texture_object *tObj);

#if 000 /* DEAD? */
extern void
fxDDTexUseGlobalPalette(GLcontext * ctx, GLboolean state);
#endif

extern void
tdfxDDTexEnv(GLcontext * ctx, GLenum target, GLenum pname,
             const GLfloat * param);

extern void
tdfxDDTexParameter(GLcontext * ctx, GLenum target,
                   struct gl_texture_object *tObj,
                   GLenum pname, const GLfloat * params);

extern const struct gl_texture_format *
tdfxDDChooseTextureFormat( GLcontext *ctx, GLint internalFormat,
                           GLenum srcFormat, GLenum srcType );

extern void
tdfxDDTexImage2D(GLcontext * ctx, GLenum target, GLint level,
                 GLint internalFormat, GLint width, GLint height,
                 GLint border,
                 GLenum format, GLenum type, const GLvoid * pixels,
                 const struct gl_pixelstore_attrib * packing,
                 struct gl_texture_object * texObj,
                 struct gl_texture_image * texImage);

extern void
tdfxDDTexSubImage2D(GLcontext *ctx, GLenum target, GLint level,
                    GLint xoffset, GLint yoffset,
                    GLsizei width, GLsizei height,
                    GLenum format, GLenum type,
                    const GLvoid *pixels,
                    const struct gl_pixelstore_attrib *packing,
                    struct gl_texture_object *texObj,
                    struct gl_texture_image *texImage );

#if 000
extern GLboolean
tdfxDDCompressedTexImage2D( GLcontext *ctx, GLenum target,
                            GLint level, GLsizei imageSize,
                            const GLvoid *data,
                            struct gl_texture_object *texObj,
                            struct gl_texture_image *texImage,
                            GLboolean *retainInternalCopy);

extern GLboolean
tdfxDDCompressedTexSubImage2D( GLcontext *ctx, GLenum target,
                               GLint level, GLint xoffset,
                               GLint yoffset, GLsizei width,
                               GLint height, GLenum format,
                               GLsizei imageSize, const GLvoid *data,
                               struct gl_texture_object *texObj,
                               struct gl_texture_image *texImage );
#endif

extern GLboolean
tdfxDDTestProxyTexImage(GLcontext *ctx, GLenum target,
                        GLint level, GLint internalFormat,
                        GLenum format, GLenum type,
                        GLint width, GLint height,
                        GLint depth, GLint border);

extern GLvoid *
tdfxDDGetTexImage(GLcontext * ctx, GLenum target, GLint level,
                  const struct gl_texture_object *texObj,
                  GLenum * formatOut, GLenum * typeOut,
                  GLboolean * freeImageOut);

extern void
tdfxDDGetCompressedTexImage( GLcontext *ctx, GLenum target,
                             GLint lod, void *image,
                             const struct gl_texture_object *texObj,
                             struct gl_texture_image *texImage );

extern GLint
tdfxDDSpecificCompressedTexFormat(GLcontext *ctx,
                                  GLint internalFormat,
                                  GLint numDimensions);

extern GLint
tdfxDDBaseCompressedTexFormat(GLcontext *ctx,
                              GLint internalFormat);

extern GLboolean
tdfxDDIsCompressedFormat(GLcontext *ctx, GLint internalFormat);

extern GLsizei
tdfxDDCompressedImageSize(GLcontext *ctx,
                          GLenum intFormat,
                          GLuint numDimensions,
                          GLuint width,
                          GLuint height,
                          GLuint depth);


#endif
