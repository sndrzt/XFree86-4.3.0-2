/* $XFree86: xc/lib/GL/mesa/src/drv/radeon/radeon_vtxfmt_c.c,v 1.1 2002/10/30 12:51:58 alanh Exp $ */
/**************************************************************************

Copyright 2002 ATI Technologies Inc., Ontario, Canada, and
                     Tungsten Graphics Inc., Cedar Park, Texas.

All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
on the rights to use, copy, modify, merge, publish, distribute, sub
license, and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
ATI, TUNGSTEN GRAPHICS AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

/*
 * Authors:
 *   Keith Whitwell <keith@tungstengraphics.com>
 */
#include "mtypes.h"
#include "colormac.h"
#include "simple_list.h"
#include "api_noop.h"
#include "vtxfmt.h"

#include "radeon_vtxfmt.h"

/* Fallback versions of all the entrypoints for situations where
 * codegen isn't available.  This is still a lot faster than the
 * vb/pipeline implementation in Mesa.
 */
static void radeon_Vertex3f( GLfloat x, GLfloat y, GLfloat z )
{
   int i;

   *vb.dmaptr++ = *(int *)&x;
   *vb.dmaptr++ = *(int *)&y;
   *vb.dmaptr++ = *(int *)&z;

   for (i = 3; i < vb.vertex_size; i++)
      *vb.dmaptr++ = vb.vertex[i].i;
   
   if (--vb.counter == 0)
      vb.notify();
}


static void radeon_Vertex3fv( const GLfloat *v )
{
   int i;

   *vb.dmaptr++ = *(int *)&v[0];
   *vb.dmaptr++ = *(int *)&v[1];
   *vb.dmaptr++ = *(int *)&v[2];

   for (i = 3; i < vb.vertex_size; i++)
      *vb.dmaptr++ = vb.vertex[i].i;
   
   if (--vb.counter == 0)
      vb.notify();
}


static void radeon_Vertex2f( GLfloat x, GLfloat y )
{
   int i;

   *vb.dmaptr++ = *(int *)&x;
   *vb.dmaptr++ = *(int *)&y;
   *vb.dmaptr++ = 0;

   for (i = 3; i < vb.vertex_size; i++)
      *vb.dmaptr++ = *(int *)&vb.vertex[i];
   
   if (--vb.counter == 0)
      vb.notify();
}


static void radeon_Vertex2fv( const GLfloat *v )
{
   int i;

   *vb.dmaptr++ = *(int *)&v[0];
   *vb.dmaptr++ = *(int *)&v[1];
   *vb.dmaptr++ = 0;

   for (i = 3; i < vb.vertex_size; i++)
      *vb.dmaptr++ = vb.vertex[i].i;
   
   if (--vb.counter == 0)
      vb.notify();
}



/* Color for ubyte (packed) color formats:
 */
static void radeon_Color3ub_ub( GLubyte r, GLubyte g, GLubyte b )
{
   radeon_color_t *dest = vb.colorptr;
   dest->red	= r;
   dest->green	= g;
   dest->blue	= b;
   dest->alpha	= 0xff;
}

static void radeon_Color3ubv_ub( const GLubyte *v )
{
   radeon_color_t *dest = vb.colorptr;
   dest->red	= v[0];
   dest->green	= v[1];
   dest->blue	= v[2];
   dest->alpha	= 0xff;
}

static void radeon_Color4ub_ub( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   radeon_color_t *dest = vb.colorptr;
   dest->red	= r;
   dest->green	= g;
   dest->blue	= b;
   dest->alpha	= a;
}

static void radeon_Color4ubv_ub( const GLubyte *v )
{
   *(GLuint *)vb.colorptr = LE32_TO_CPU(*(GLuint *)v);
}


static void radeon_Color3f_ub( GLfloat r, GLfloat g, GLfloat b )
{
   radeon_color_t *dest = vb.colorptr;
   UNCLAMPED_FLOAT_TO_UBYTE( dest->red,   r );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->green, g );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->blue,  b );
   dest->alpha = 255;
}

static void radeon_Color3fv_ub( const GLfloat *v )
{
   radeon_color_t *dest = vb.colorptr;
   UNCLAMPED_FLOAT_TO_UBYTE( dest->red,   v[0] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->green, v[1] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->blue,  v[2] );
   dest->alpha = 255;
}

static void radeon_Color4f_ub( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
   radeon_color_t *dest = vb.colorptr;
   UNCLAMPED_FLOAT_TO_UBYTE( dest->red,   r );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->green, g );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->blue,  b );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->alpha, a );
}

static void radeon_Color4fv_ub( const GLfloat *v )
{
   radeon_color_t *dest = vb.colorptr;
   UNCLAMPED_FLOAT_TO_UBYTE( dest->red,	  v[0] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->green, v[1] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->blue,  v[2] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->alpha, v[3] );
}


/* Color for float color+alpha formats:
 */
static void radeon_Color3ub_4f( GLubyte r, GLubyte g, GLubyte b )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(r);
   dest[1] = UBYTE_TO_FLOAT(g);
   dest[2] = UBYTE_TO_FLOAT(b);
   dest[3] = 1.0;
}

static void radeon_Color3ubv_4f( const GLubyte *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(v[0]);
   dest[1] = UBYTE_TO_FLOAT(v[1]);
   dest[2] = UBYTE_TO_FLOAT(v[2]);
   dest[3] = 1.0;
}

static void radeon_Color4ub_4f( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(r);
   dest[1] = UBYTE_TO_FLOAT(g);
   dest[2] = UBYTE_TO_FLOAT(b);
   dest[3] = UBYTE_TO_FLOAT(a);
}

static void radeon_Color4ubv_4f( const GLubyte *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(v[0]);
   dest[1] = UBYTE_TO_FLOAT(v[1]);
   dest[2] = UBYTE_TO_FLOAT(v[2]);
   dest[3] = UBYTE_TO_FLOAT(v[3]);
}


static void radeon_Color3f_4f( GLfloat r, GLfloat g, GLfloat b )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = r;
   dest[1] = g;
   dest[2] = b;
   dest[3] = 1.0;		
}

static void radeon_Color3fv_4f( const GLfloat *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = v[0];
   dest[1] = v[1];
   dest[2] = v[2];
   dest[3] = 1.0;
}

static void radeon_Color4f_4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = r;
   dest[1] = g;
   dest[2] = b;
   dest[3] = a;
}

static void radeon_Color4fv_4f( const GLfloat *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = v[0];
   dest[1] = v[1];
   dest[2] = v[2];
   dest[3] = v[3];
}


/* Color for float color formats:
 */
static void radeon_Color3ub_3f( GLubyte r, GLubyte g, GLubyte b )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(r);
   dest[1] = UBYTE_TO_FLOAT(g);
   dest[2] = UBYTE_TO_FLOAT(b);
}

static void radeon_Color3ubv_3f( const GLubyte *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(v[0]);
   dest[1] = UBYTE_TO_FLOAT(v[1]);
   dest[2] = UBYTE_TO_FLOAT(v[2]);
}

static void radeon_Color4ub_3f( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(r);
   dest[1] = UBYTE_TO_FLOAT(g);
   dest[2] = UBYTE_TO_FLOAT(b);
   vb.context->Current.Color[3] = UBYTE_TO_FLOAT(a);
}

static void radeon_Color4ubv_3f( const GLubyte *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = UBYTE_TO_FLOAT(v[0]);
   dest[1] = UBYTE_TO_FLOAT(v[1]);
   dest[2] = UBYTE_TO_FLOAT(v[2]);
   vb.context->Current.Color[3] = UBYTE_TO_FLOAT(v[3]);
}


static void radeon_Color3f_3f( GLfloat r, GLfloat g, GLfloat b )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = r;
   dest[1] = g;
   dest[2] = b;
}

static void radeon_Color3fv_3f( const GLfloat *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = v[0];
   dest[1] = v[1];
   dest[2] = v[2];
}

static void radeon_Color4f_3f( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = r;
   dest[1] = g;
   dest[2] = b;
   vb.context->Current.Color[3] = a;
}

static void radeon_Color4fv_3f( const GLfloat *v )
{
   GLfloat *dest = vb.floatcolorptr;
   dest[0] = v[0];
   dest[1] = v[1];
   dest[2] = v[2];
   vb.context->Current.Color[3] = v[3]; 
}


/* Secondary Color:
 */
static void radeon_SecondaryColor3ubEXT_ub( GLubyte r, GLubyte g, GLubyte b )
{
   radeon_color_t *dest = vb.specptr;
   dest->red	= r;
   dest->green	= g;
   dest->blue	= b;
   dest->alpha	= 0xff;
}

static void radeon_SecondaryColor3ubvEXT_ub( const GLubyte *v )
{
   radeon_color_t *dest = vb.specptr;
   dest->red	= v[0];
   dest->green	= v[1];
   dest->blue	= v[2];
   dest->alpha	= 0xff;
}

static void radeon_SecondaryColor3fEXT_ub( GLfloat r, GLfloat g, GLfloat b )
{
   radeon_color_t *dest = vb.specptr;
   UNCLAMPED_FLOAT_TO_UBYTE( dest->red,	  r );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->green, g );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->blue,  b );
   dest->alpha = 255;
}

static void radeon_SecondaryColor3fvEXT_ub( const GLfloat *v )
{
   radeon_color_t *dest = vb.specptr;
   UNCLAMPED_FLOAT_TO_UBYTE( dest->red,	  v[0] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->green, v[1] );
   UNCLAMPED_FLOAT_TO_UBYTE( dest->blue,  v[2] );
   dest->alpha = 255;
}

static void radeon_SecondaryColor3ubEXT_3f( GLubyte r, GLubyte g, GLubyte b )
{
   GLfloat *dest = vb.floatspecptr;
   dest[0] = UBYTE_TO_FLOAT(r);
   dest[1] = UBYTE_TO_FLOAT(g);
   dest[2] = UBYTE_TO_FLOAT(b);
   dest[3] = 1.0;
}

static void radeon_SecondaryColor3ubvEXT_3f( const GLubyte *v )
{
   GLfloat *dest = vb.floatspecptr;
   dest[0] = UBYTE_TO_FLOAT(v[0]);
   dest[1] = UBYTE_TO_FLOAT(v[1]);
   dest[2] = UBYTE_TO_FLOAT(v[2]);
   dest[3] = 1.0;
}

static void radeon_SecondaryColor3fEXT_3f( GLfloat r, GLfloat g, GLfloat b )
{
   GLfloat *dest = vb.floatspecptr;
   dest[0] = r;
   dest[1] = g;
   dest[2] = b;
   dest[3] = 1.0;
}

static void radeon_SecondaryColor3fvEXT_3f( const GLfloat *v )
{
   GLfloat *dest = vb.floatspecptr;
   dest[0] = v[0];
   dest[1] = v[1];
   dest[2] = v[2];
   dest[3] = 1.0;
}


/* Normal
 */
static void radeon_Normal3f( GLfloat n0, GLfloat n1, GLfloat n2 )
{
   GLfloat *dest = vb.normalptr;
   dest[0] = n0;
   dest[1] = n1;
   dest[2] = n2;
}

static void radeon_Normal3fv( const GLfloat *v )
{
   GLfloat *dest = vb.normalptr;
   dest[0] = v[0];
   dest[1] = v[1];
   dest[2] = v[2];
}


/* TexCoord
 */
static void radeon_TexCoord1f( GLfloat s )
{
   GLfloat *dest = vb.texcoordptr[0];
   dest[0] = s;
   dest[1] = 0;
}

static void radeon_TexCoord1fv( const GLfloat *v )
{
   GLfloat *dest = vb.texcoordptr[0];
   dest[0] = v[0];
   dest[1] = 0;
}

static void radeon_TexCoord2f( GLfloat s, GLfloat t )
{
   GLfloat *dest = vb.texcoordptr[0];
   dest[0] = s;
   dest[1] = t;
}

static void radeon_TexCoord2fv( const GLfloat *v )
{
   GLfloat *dest = vb.texcoordptr[0];
   dest[0] = v[0];
   dest[1] = v[1];
}


/* MultiTexcoord
 */
static void radeon_MultiTexCoord1fARB( GLenum target, GLfloat s  )
{
   GLfloat *dest = vb.texcoordptr[(target - GL_TEXTURE0_ARB)&1];
   dest[0] = s;
   dest[1] = 0;
}

static void radeon_MultiTexCoord1fvARB( GLenum target, const GLfloat *v )
{
   GLfloat *dest = vb.texcoordptr[(target - GL_TEXTURE0_ARB)&1];
   dest[0] = v[0];
   dest[1] = 0;
}

static void radeon_MultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t )
{
   GLfloat *dest = vb.texcoordptr[(target - GL_TEXTURE0_ARB)&1];
   dest[0] = s;
   dest[1] = t;
}

static void radeon_MultiTexCoord2fvARB( GLenum target, const GLfloat *v )
{
   GLfloat *dest = vb.texcoordptr[(target - GL_TEXTURE0_ARB)&1];
   dest[0] = v[0];
   dest[1] = v[1];
}

static struct dynfn *lookup( struct dynfn *l, int key )
{
   struct dynfn *f;

   foreach( f, l ) {
      if (f->key == key) 
	 return f;
   }

   return 0;
}

/* Can't use the loopback template for this:
 */

#define CHOOSE(FN, FNTYPE, MASK, ACTIVE, ARGS1, ARGS2 )			\
static void choose_##FN ARGS1						\
{									\
   radeonContextPtr rmesa = RADEON_CONTEXT(vb.context);			\
   int key = rmesa->vb.vertex_format & (MASK|ACTIVE);			\
   struct dynfn *dfn = lookup( &rmesa->vb.dfn_cache.FN, key );		\
									\
   if (dfn == 0)							\
      dfn = rmesa->vb.codegen.FN( vb.context, key );			\
   else if (RADEON_DEBUG & DEBUG_CODEGEN)				\
      fprintf(stderr, "%s -- cached codegen\n", __FUNCTION__ );		\
									\
   if (dfn)								\
      vb.context->Exec->FN = (FNTYPE)(dfn->code);			\
   else {								\
      if (RADEON_DEBUG & DEBUG_CODEGEN)					\
	 fprintf(stderr, "%s -- generic version\n", __FUNCTION__ );	\
      vb.context->Exec->FN = radeon_##FN;				\
   }									\
									\
   vb.context->Driver.NeedFlush |= FLUSH_UPDATE_CURRENT;		\
   vb.context->Exec->FN ARGS2;						\
}



/* For the _3f case, only allow one color function to be hooked in at
 * a time.  Eventually, use a similar mechanism to allow selecting the
 * color component of the vertex format based on client behaviour.  
 *
 * Note:  Perform these actions even if there is a codegen or cached 
 * codegen version of the chosen function.
 */
#define CHOOSE_COLOR(FN, FNTYPE, NR, MASK, ACTIVE, ARGS1, ARGS2 )	\
static void choose_##FN ARGS1						\
{									\
   GLcontext *ctx = vb.context;						\
   radeonContextPtr rmesa = RADEON_CONTEXT(vb.context);			\
   int key = rmesa->vb.vertex_format & (MASK|ACTIVE);			\
   struct dynfn *dfn;							\
									\
   if (rmesa->vb.vertex_format & ACTIVE_PKCOLOR) {			\
      ctx->Exec->FN = radeon_##FN##_ub;					\
   }									\
   else if ((rmesa->vb.vertex_format &					\
            (ACTIVE_FPCOLOR|ACTIVE_FPALPHA)) == ACTIVE_FPCOLOR) {	\
									\
      if (rmesa->vb.installed_color_3f_sz != NR) {			\
         rmesa->vb.installed_color_3f_sz = NR;				\
         if (NR == 3) ctx->Current.Color[3] = 1.0;			\
         if (ctx->Driver.NeedFlush & FLUSH_UPDATE_CURRENT) {		\
            radeon_copy_to_current( ctx );				\
            _mesa_install_exec_vtxfmt( ctx, &rmesa->vb.vtxfmt );	\
            ctx->Exec->FN ARGS2;					\
            return;							\
         }								\
      }									\
									\
      ctx->Exec->FN = radeon_##FN##_3f;					\
   }									\
   else {								\
      ctx->Exec->FN = radeon_##FN##_4f;					\
   }									\
									\
									\
   dfn = lookup( &rmesa->vb.dfn_cache.FN, key );			\
   if (!dfn) dfn = rmesa->vb.codegen.FN( ctx, key );			\
									\
   if (dfn) {								\
      if (RADEON_DEBUG & DEBUG_CODEGEN)					\
         fprintf(stderr, "%s -- codegen version\n", __FUNCTION__ );	\
      ctx->Exec->FN = (FNTYPE)dfn->code;				\
   }									\
   else if (RADEON_DEBUG & DEBUG_CODEGEN)				\
         fprintf(stderr, "%s -- 'c' version\n", __FUNCTION__ );		\
									\
   ctx->Driver.NeedFlush |= FLUSH_UPDATE_CURRENT;			\
   ctx->Exec->FN ARGS2;							\
}



/* Right now there are both _ub and _3f versions of the secondary color
 * functions.  Currently, we only set-up the hardware to use the _ub versions.
 * The _3f versions are needed for the cases where secondary color isn't used
 * in the vertex format, but it still needs to be stored in the context
 * state vector.
 */
#define CHOOSE_SECONDARY_COLOR(FN, FNTYPE, MASK, ACTIVE, ARGS1, ARGS2 )	\
static void choose_##FN ARGS1						\
{									\
   GLcontext *ctx = vb.context;						\
   radeonContextPtr rmesa = RADEON_CONTEXT(vb.context);			\
   int key = rmesa->vb.vertex_format & (MASK|ACTIVE);			\
   struct dynfn *dfn = lookup( &rmesa->vb.dfn_cache.FN, key );		\
									\
   if (dfn == 0)							\
      dfn = rmesa->vb.codegen.FN( vb.context, key );			\
   else  if (RADEON_DEBUG & DEBUG_CODEGEN)				\
      fprintf(stderr, "%s -- cached version\n", __FUNCTION__ );		\
									\
   if (dfn)								\
      vb.context->Exec->FN = (FNTYPE)(dfn->code);			\
   else {								\
      if (RADEON_DEBUG & DEBUG_CODEGEN)					\
         fprintf(stderr, "%s -- generic version\n", __FUNCTION__ );	\
      vb.context->Exec->FN = ((rmesa->vb.vertex_format & ACTIVE_PKSPEC) != 0) \
	  ? radeon_##FN##_ub : radeon_##FN##_3f;			\
   }									\
									\
   ctx->Driver.NeedFlush |= FLUSH_UPDATE_CURRENT;			\
   ctx->Exec->FN ARGS2;							\
}





/* Shorthands
 */
#define ACTIVE_XYZW (RADEON_CP_VC_FRMT_W0|RADEON_CP_VC_FRMT_Z)
#define ACTIVE_NORM RADEON_CP_VC_FRMT_N0

#define ACTIVE_PKCOLOR RADEON_CP_VC_FRMT_PKCOLOR
#define ACTIVE_FPCOLOR RADEON_CP_VC_FRMT_FPCOLOR
#define ACTIVE_FPALPHA RADEON_CP_VC_FRMT_FPALPHA
#define ACTIVE_COLOR (ACTIVE_FPCOLOR|ACTIVE_PKCOLOR)

#define ACTIVE_PKSPEC RADEON_CP_VC_FRMT_PKSPEC
#define ACTIVE_FPSPEC RADEON_CP_VC_FRMT_FPSPEC
#define ACTIVE_SPEC   (ACTIVE_FPSPEC|ACTIVE_PKSPEC)

#define ACTIVE_ST0 RADEON_CP_VC_FRMT_ST0
#define ACTIVE_ST1 RADEON_CP_VC_FRMT_ST1
#define ACTIVE_ST_ALL (RADEON_CP_VC_FRMT_ST1|RADEON_CP_VC_FRMT_ST0)

/* Each codegen function should be able to be fully specified by a
 * subsetted version of rmesa->vb.vertex_format.
 */
#define MASK_NORM    (ACTIVE_XYZW)
#define MASK_COLOR   (MASK_NORM|ACTIVE_NORM)
#define MASK_SPEC    (MASK_COLOR|ACTIVE_COLOR)
#define MASK_ST0     (MASK_SPEC|ACTIVE_SPEC)
#define MASK_ST1     (MASK_ST0|ACTIVE_ST0)
#define MASK_ST_ALL  (MASK_ST1|ACTIVE_ST1)
#define MASK_VERTEX  (MASK_ST_ALL|ACTIVE_FPALPHA) 


typedef void (*p4f)( GLfloat, GLfloat, GLfloat, GLfloat );
typedef void (*p3f)( GLfloat, GLfloat, GLfloat );
typedef void (*p2f)( GLfloat, GLfloat );
typedef void (*p1f)( GLfloat );
typedef void (*pe2f)( GLenum, GLfloat, GLfloat );
typedef void (*pe1f)( GLenum, GLfloat );
typedef void (*p4ub)( GLubyte, GLubyte, GLubyte, GLubyte );
typedef void (*p3ub)( GLubyte, GLubyte, GLubyte );
typedef void (*pfv)( const GLfloat * );
typedef void (*pefv)( GLenum, const GLfloat * );
typedef void (*pubv)( const GLubyte * );


CHOOSE(Normal3f, p3f, MASK_NORM, ACTIVE_NORM, 
       (GLfloat a,GLfloat b,GLfloat c), (a,b,c))
CHOOSE(Normal3fv, pfv, MASK_NORM, ACTIVE_NORM, 
       (const GLfloat *v), (v))

CHOOSE_COLOR(Color4ub, p4ub, 4, MASK_COLOR, ACTIVE_COLOR,
	(GLubyte a,GLubyte b, GLubyte c, GLubyte d), (a,b,c,d))
CHOOSE_COLOR(Color4ubv, pubv, 4, MASK_COLOR, ACTIVE_COLOR, 
	(const GLubyte *v), (v))
CHOOSE_COLOR(Color3ub, p3ub, 3, MASK_COLOR, ACTIVE_COLOR, 
	(GLubyte a,GLubyte b, GLubyte c), (a,b,c))
CHOOSE_COLOR(Color3ubv, pubv, 3, MASK_COLOR, ACTIVE_COLOR, 
	(const GLubyte *v), (v))

CHOOSE_COLOR(Color4f, p4f, 4, MASK_COLOR, ACTIVE_COLOR, 
	(GLfloat a,GLfloat b, GLfloat c, GLfloat d), (a,b,c,d))
CHOOSE_COLOR(Color4fv, pfv, 4, MASK_COLOR, ACTIVE_COLOR, 
	(const GLfloat *v), (v))
CHOOSE_COLOR(Color3f, p3f, 3, MASK_COLOR, ACTIVE_COLOR,
	(GLfloat a,GLfloat b, GLfloat c), (a,b,c))
CHOOSE_COLOR(Color3fv, pfv, 3, MASK_COLOR, ACTIVE_COLOR,
	(const GLfloat *v), (v))


CHOOSE_SECONDARY_COLOR(SecondaryColor3ubEXT, p3ub, MASK_SPEC, ACTIVE_SPEC,
	(GLubyte a,GLubyte b, GLubyte c), (a,b,c))
CHOOSE_SECONDARY_COLOR(SecondaryColor3ubvEXT, pubv, MASK_SPEC, ACTIVE_SPEC,
	(const GLubyte *v), (v))
CHOOSE_SECONDARY_COLOR(SecondaryColor3fEXT, p3f, MASK_SPEC, ACTIVE_SPEC,
	(GLfloat a,GLfloat b, GLfloat c), (a,b,c))
CHOOSE_SECONDARY_COLOR(SecondaryColor3fvEXT, pfv, MASK_SPEC, ACTIVE_SPEC,
	(const GLfloat *v), (v))

CHOOSE(TexCoord2f, p2f, MASK_ST0, ACTIVE_ST0, 
       (GLfloat a,GLfloat b), (a,b))
CHOOSE(TexCoord2fv, pfv, MASK_ST0, ACTIVE_ST0, 
       (const GLfloat *v), (v))
CHOOSE(TexCoord1f, p1f, MASK_ST0, ACTIVE_ST0, 
       (GLfloat a), (a))
CHOOSE(TexCoord1fv, pfv, MASK_ST0, ACTIVE_ST0, 
       (const GLfloat *v), (v))

CHOOSE(MultiTexCoord2fARB, pe2f, MASK_ST_ALL, ACTIVE_ST_ALL,
	 (GLenum u,GLfloat a,GLfloat b), (u,a,b))
CHOOSE(MultiTexCoord2fvARB, pefv, MASK_ST_ALL, ACTIVE_ST_ALL,
	(GLenum u,const GLfloat *v), (u,v))
CHOOSE(MultiTexCoord1fARB, pe1f, MASK_ST_ALL, ACTIVE_ST_ALL,
	 (GLenum u,GLfloat a), (u,a))
CHOOSE(MultiTexCoord1fvARB, pefv, MASK_ST_ALL, ACTIVE_ST_ALL,
	(GLenum u,const GLfloat *v), (u,v))

CHOOSE(Vertex3f, p3f, MASK_VERTEX, MASK_VERTEX, 
       (GLfloat a,GLfloat b,GLfloat c), (a,b,c))
CHOOSE(Vertex3fv, pfv, MASK_VERTEX, MASK_VERTEX, 
       (const GLfloat *v), (v))
CHOOSE(Vertex2f, p2f, MASK_VERTEX, MASK_VERTEX, 
       (GLfloat a,GLfloat b), (a,b))
CHOOSE(Vertex2fv, pfv, MASK_VERTEX, MASK_VERTEX, 
       (const GLfloat *v), (v))





void radeonVtxfmtInitChoosers( GLvertexformat *vfmt )
{
   vfmt->Color3f = choose_Color3f;
   vfmt->Color3fv = choose_Color3fv;
   vfmt->Color3ub = choose_Color3ub;
   vfmt->Color3ubv = choose_Color3ubv;
   vfmt->Color4f = choose_Color4f;
   vfmt->Color4fv = choose_Color4fv;
   vfmt->Color4ub = choose_Color4ub;
   vfmt->Color4ubv = choose_Color4ubv;
   vfmt->SecondaryColor3fEXT = choose_SecondaryColor3fEXT;
   vfmt->SecondaryColor3fvEXT = choose_SecondaryColor3fvEXT;
   vfmt->SecondaryColor3ubEXT = choose_SecondaryColor3ubEXT;
   vfmt->SecondaryColor3ubvEXT = choose_SecondaryColor3ubvEXT;
   vfmt->MultiTexCoord1fARB = choose_MultiTexCoord1fARB;
   vfmt->MultiTexCoord1fvARB = choose_MultiTexCoord1fvARB;
   vfmt->MultiTexCoord2fARB = choose_MultiTexCoord2fARB;
   vfmt->MultiTexCoord2fvARB = choose_MultiTexCoord2fvARB;
   vfmt->Normal3f = choose_Normal3f;
   vfmt->Normal3fv = choose_Normal3fv;
   vfmt->TexCoord1f = choose_TexCoord1f;
   vfmt->TexCoord1fv = choose_TexCoord1fv;
   vfmt->TexCoord2f = choose_TexCoord2f;
   vfmt->TexCoord2fv = choose_TexCoord2fv;
   vfmt->Vertex2f = choose_Vertex2f;
   vfmt->Vertex2fv = choose_Vertex2fv;
   vfmt->Vertex3f = choose_Vertex3f;
   vfmt->Vertex3fv = choose_Vertex3fv;
}


static struct dynfn *codegen_noop( GLcontext *ctx, int key )
{
   (void) ctx; (void) key;
   return 0;
}

void radeonInitCodegen( struct dfn_generators *gen )
{
   gen->Vertex3f = codegen_noop;
   gen->Vertex3fv = codegen_noop;
   gen->Color4ub = codegen_noop;
   gen->Color4ubv = codegen_noop;
   gen->Normal3f = codegen_noop;
   gen->Normal3fv = codegen_noop;
   gen->TexCoord2f = codegen_noop;
   gen->TexCoord2fv = codegen_noop;
   gen->MultiTexCoord2fARB = codegen_noop;
   gen->MultiTexCoord2fvARB = codegen_noop;
   gen->Vertex2f = codegen_noop;
   gen->Vertex2fv = codegen_noop;
   gen->Color3ub = codegen_noop;
   gen->Color3ubv = codegen_noop;
   gen->Color4f = codegen_noop;
   gen->Color4fv = codegen_noop;
   gen->Color3f = codegen_noop;
   gen->Color3fv = codegen_noop;
   gen->SecondaryColor3fEXT = codegen_noop;
   gen->SecondaryColor3fvEXT = codegen_noop;
   gen->SecondaryColor3ubEXT = codegen_noop;
   gen->SecondaryColor3ubvEXT = codegen_noop;
   gen->TexCoord1f = codegen_noop;
   gen->TexCoord1fv = codegen_noop;
   gen->MultiTexCoord1fARB = codegen_noop;
   gen->MultiTexCoord1fvARB = codegen_noop;

   if (!getenv("RADEON_NO_CODEGEN")) {
#if defined(USE_X86_ASM)
      radeonInitX86Codegen( gen );
#endif

#if defined(USE_SSE_ASM)
      radeonInitSSECodegen( gen );
#endif
   }
}
