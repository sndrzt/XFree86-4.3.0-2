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
/* $XFree86: xc/lib/GL/mesa/src/drv/tdfx/tdfx_tris.c,v 1.3 2002/02/22 21:45:04 dawes Exp $ */

/* Authors:
 *    Keith Whitwell <keith@tungstengraphics.com>
 */

#include <stdio.h>
#include <math.h>

#include "glheader.h"
#include "mtypes.h"
#include "macros.h"
#include "colormac.h"

#include "swrast/swrast.h"
#include "swrast_setup/swrast_setup.h"
#include "swrast_setup/ss_context.h"
#include "tnl/t_context.h"
#include "tnl/t_pipeline.h"

#include "tdfx_tris.h"
#include "tdfx_state.h"
#include "tdfx_vb.h"
#include "tdfx_lock.h"
#include "tdfx_render.h"


static void tdfxRasterPrimitive( GLcontext *ctx, GLenum prim );
static void tdfxRenderPrimitive( GLcontext *ctx, GLenum prim );

/***********************************************************************
 *          Macros for t_dd_tritmp.h to draw basic primitives          *
 ***********************************************************************/

#define TRI( a, b, c )				\
do {						\
   if (DO_FALLBACK)				\
      fxMesa->draw_triangle( fxMesa, a, b, c );	\
   else						\
      fxMesa->Glide.grDrawTriangle( a, b, c );	\
} while (0)					\

#define QUAD( a, b, c, d )			\
do {						\
   if (DO_FALLBACK) {				\
      fxMesa->draw_triangle( fxMesa, a, b, d );	\
      fxMesa->draw_triangle( fxMesa, b, c, d );	\
   } else {					\
      fxMesa->Glide.grDrawTriangle( a, b, d );	\
      fxMesa->Glide.grDrawTriangle( b, c, d );	\
   }						\
} while (0)

#define LINE( v0, v1 )				\
do {						\
   if (DO_FALLBACK)				\
      fxMesa->draw_line( fxMesa, v0, v1 );	\
   else {					\
      v0->v.x += LINE_X_OFFSET - TRI_X_OFFSET;	\
      v0->v.y += LINE_Y_OFFSET - TRI_Y_OFFSET;	\
      v1->v.x += LINE_X_OFFSET - TRI_X_OFFSET;	\
      v1->v.y += LINE_Y_OFFSET - TRI_Y_OFFSET;	\
      fxMesa->Glide.grDrawLine( v0, v1 );	\
      v0->v.x -= LINE_X_OFFSET - TRI_X_OFFSET;	\
      v0->v.y -= LINE_Y_OFFSET - TRI_Y_OFFSET;	\
      v1->v.x -= LINE_X_OFFSET - TRI_X_OFFSET;	\
      v1->v.y -= LINE_Y_OFFSET - TRI_Y_OFFSET;	\
   }						\
} while (0)

#define POINT( v0 )				\
do {						\
   if (DO_FALLBACK)				\
      fxMesa->draw_point( fxMesa, v0 );		\
   else {					\
      v0->v.x += PNT_X_OFFSET - TRI_X_OFFSET;	\
      v0->v.y += PNT_Y_OFFSET - TRI_Y_OFFSET;	\
      fxMesa->Glide.grDrawPoint( v0 );		\
      v0->v.x -= PNT_X_OFFSET - TRI_X_OFFSET;	\
      v0->v.y -= PNT_Y_OFFSET - TRI_Y_OFFSET;	\
   }						\
} while (0)


/***********************************************************************
 *              Fallback to swrast for basic primitives                *
 ***********************************************************************/

/* Build an SWvertex from a hardware vertex. 
 *
 * This code is hit only when a mix of accelerated and unaccelerated
 * primitives are being drawn, and only for the unaccelerated
 * primitives.  
 */
static void 
tdfx_translate_vertex( GLcontext *ctx, const tdfxVertex *src, SWvertex *dst)
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);

   if (fxMesa->vertexFormat == TDFX_LAYOUT_TINY) {
      dst->win[0] = src->tv.x - fxMesa->x_offset;
      dst->win[1] = src->tv.y - (fxMesa->screen_height - fxMesa->height - fxMesa->y_offset);
      dst->win[2] = src->tv.z;
      dst->win[3] = 1.0;

      dst->color[0] = src->tv.color.red;
      dst->color[1] = src->tv.color.green;
      dst->color[2] = src->tv.color.blue;
      dst->color[3] = src->tv.color.alpha;
   } 
   else {
      GLfloat w = 1.0 / src->v.rhw;

      dst->win[0] = src->v.x - fxMesa->x_offset;
      dst->win[1] = fxMesa->screen_height - fxMesa->y_offset - src->v.y;
      dst->win[2] = src->v.z;
      dst->win[3] = src->v.rhw;

      dst->color[0] = src->v.color.red;
      dst->color[1] = src->v.color.green;
      dst->color[2] = src->v.color.blue;
      dst->color[3] = src->v.color.alpha;

      if (fxMesa->vertexFormat == TDFX_LAYOUT_PROJECT) {
	 dst->texcoord[0][0] = fxMesa->sScale0 * w * src->pv.tu0;
	 dst->texcoord[0][1] = fxMesa->tScale0 * w * src->pv.tv0;
	 dst->texcoord[0][3] = w * src->pv.tq0;

	 if (fxMesa->SetupIndex & TDFX_TEX1_BIT) {
	    dst->texcoord[1][0] = fxMesa->sScale1 * w * src->pv.tu1;
	    dst->texcoord[1][1] = fxMesa->tScale1 * w * src->pv.tv1;
	    dst->texcoord[1][3] = w * src->pv.tq1;
	 }
      } else if (fxMesa->SetupIndex & TDFX_TEX0_BIT) {
	 dst->texcoord[0][0] = fxMesa->sScale0 * w * src->v.tu0;
	 dst->texcoord[0][1] = fxMesa->tScale0 * w * src->v.tv0;
	 dst->texcoord[0][3] = 1.0;

	 if (fxMesa->SetupIndex & TDFX_TEX1_BIT) {
	    dst->texcoord[1][0] = fxMesa->sScale1 * w * src->v.tu1;
	    dst->texcoord[1][1] = fxMesa->tScale1 * w * src->v.tv1;
	    dst->texcoord[1][3] = 1.0;
	 }
      }
   }

   dst->pointSize = ctx->Point._Size;
}


static void 
tdfx_fallback_tri( tdfxContextPtr fxMesa, 
		   tdfxVertex *v0, 
		   tdfxVertex *v1, 
		   tdfxVertex *v2 )
{
   GLcontext *ctx = fxMesa->glCtx;
   SWvertex v[3];
   tdfx_translate_vertex( ctx, v0, &v[0] );
   tdfx_translate_vertex( ctx, v1, &v[1] );
   tdfx_translate_vertex( ctx, v2, &v[2] );
   _swrast_Triangle( ctx, &v[0], &v[1], &v[2] );
}


static void 
tdfx_fallback_line( tdfxContextPtr fxMesa,
		    tdfxVertex *v0,
		    tdfxVertex *v1 )
{
   GLcontext *ctx = fxMesa->glCtx;
   SWvertex v[2];
   tdfx_translate_vertex( ctx, v0, &v[0] );
   tdfx_translate_vertex( ctx, v1, &v[1] );
   _swrast_Line( ctx, &v[0], &v[1] );
}


static void 
tdfx_fallback_point( tdfxContextPtr fxMesa, 
		     tdfxVertex *v0 )
{
   GLcontext *ctx = fxMesa->glCtx;
   SWvertex v[1];
   tdfx_translate_vertex( ctx, v0, &v[0] );
   _swrast_Point( ctx, &v[0] );
}

/***********************************************************************
 *                 Functions to draw basic primitives                  *
 ***********************************************************************/

static void tdfx_print_vertex( GLcontext *ctx, const tdfxVertex *v )
{
   tdfxContextPtr imesa = TDFX_CONTEXT( ctx );

   fprintf(stderr, "vertex at %p\n", v);

   if (imesa->vertexFormat == TDFX_LAYOUT_TINY) {
      fprintf(stderr, "x %f y %f z %f\n", v->v.x, v->v.y, v->v.z);
      fprintf(stderr, "r %d g %d b %d a %d\n", 
	      v->tv.color.red,
	      v->tv.color.green,
	      v->tv.color.blue,
	      v->tv.color.alpha);
   } 
   else {
      fprintf(stderr, "x %f y %f z %f oow %f\n", 
	      v->v.x, v->v.y, v->v.z, v->v.rhw);
      fprintf(stderr, "r %d g %d b %d a %d\n", 
	      v->v.color.red,
	      v->v.color.green,
	      v->v.color.blue,
	      v->v.color.alpha);
   }
   
   fprintf(stderr, "\n");
}

#define DO_FALLBACK 0

/* Need to do clip loop at each triangle when mixing swrast and hw
 * rendering.  These functions are only used when mixed-mode rendering
 * is occurring.
 */
static void tdfx_draw_quad( tdfxContextPtr fxMesa,
			    tdfxVertexPtr v0,
			    tdfxVertexPtr v1,
			    tdfxVertexPtr v2,
			    tdfxVertexPtr v3 )
{
/*     fprintf(stderr, "%s\n", __FUNCTION__); */
   BEGIN_CLIP_LOOP_LOCKED(fxMesa) {
      QUAD( v0, v1, v2, v3 );
   } END_CLIP_LOOP_LOCKED(fxMesa);
}

static void tdfx_draw_triangle( tdfxContextPtr fxMesa,
				tdfxVertexPtr v0,
				tdfxVertexPtr v1,
				tdfxVertexPtr v2 )
{
/*     fprintf(stderr, "%s\n", __FUNCTION__); */
/*     tdfx_print_vertex( fxMesa->glCtx, v0 ); */
/*     tdfx_print_vertex( fxMesa->glCtx, v1 ); */
/*     tdfx_print_vertex( fxMesa->glCtx, v2 ); */
   BEGIN_CLIP_LOOP_LOCKED(fxMesa) {
      TRI( v0, v1, v2 );
   } END_CLIP_LOOP_LOCKED(fxMesa);
}

static void tdfx_draw_line( tdfxContextPtr fxMesa,
			    tdfxVertexPtr v0,
			    tdfxVertexPtr v1 )
{
   /* No support for wide lines (avoid wide/aa line fallback).
    */
   BEGIN_CLIP_LOOP_LOCKED(fxMesa) {
      LINE(v0, v1);
   } END_CLIP_LOOP_LOCKED(fxMesa);
}

static void tdfx_draw_point( tdfxContextPtr fxMesa,
			     tdfxVertexPtr v0 )
{
   /* No support for wide points.
    */
   BEGIN_CLIP_LOOP_LOCKED(fxMesa) {
      POINT( v0 );
   } END_CLIP_LOOP_LOCKED(fxMesa);
}

#undef DO_FALLBACK


#define TDFX_UNFILLED_BIT    0x1
#define TDFX_OFFSET_BIT	     0x2
#define TDFX_TWOSIDE_BIT     0x4
#define TDFX_FLAT_BIT        0x8
#define TDFX_FALLBACK_BIT    0x10
#define TDFX_MAX_TRIFUNC     0x20

static struct {
   points_func	        points;
   line_func		line;
   triangle_func	triangle;
   quad_func		quad;
} rast_tab[TDFX_MAX_TRIFUNC];

#define DO_FALLBACK (IND & TDFX_FALLBACK_BIT)
#define DO_OFFSET   (IND & TDFX_OFFSET_BIT)
#define DO_UNFILLED (IND & TDFX_UNFILLED_BIT)
#define DO_TWOSIDE  (IND & TDFX_TWOSIDE_BIT)
#define DO_FLAT     (IND & TDFX_FLAT_BIT)
#define DO_TRI       1
#define DO_QUAD      1
#define DO_LINE      1
#define DO_POINTS    1
#define DO_FULL_QUAD 1

#define HAVE_RGBA   1
#define HAVE_SPEC   0
#define HAVE_HW_FLATSHADE 0
#define HAVE_BACK_COLORS  0
#define VERTEX tdfxVertex
#define TAB rast_tab

#define TDFX_COLOR( dst, src )			\
do {						\
   dst[0] = src[2];				\
   dst[1] = src[1];				\
   dst[2] = src[0];				\
   dst[3] = src[3];				\
} while (0)

#define DEPTH_SCALE 1.0
#define UNFILLED_TRI unfilled_tri
#define UNFILLED_QUAD unfilled_quad
#define VERT_X(_v) _v->v.x
#define VERT_Y(_v) _v->v.y
#define VERT_Z(_v) _v->v.z
#define AREA_IS_CCW( a ) (a < 0)
#define GET_VERTEX(e) (fxMesa->verts + (e<<fxMesa->vertex_stride_shift))

#define VERT_SET_RGBA( v, c )    TDFX_COLOR( v->ub4[coloroffset], c )
#define VERT_COPY_RGBA( v0, v1 ) v0->ui[coloroffset] = v1->ui[coloroffset]
#define VERT_SAVE_RGBA( idx )    color[idx] = v[idx]->ui[coloroffset]
#define VERT_RESTORE_RGBA( idx ) v[idx]->ui[coloroffset] = color[idx]   

#define LOCAL_VARS(n)					\
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);		\
   GLuint coloroffset = (fxMesa->vertexFormat ==	\
			 TDFX_LAYOUT_TINY) ? 3 : 4;	\
   GLuint color[n];					\
   (void) color; (void)coloroffset



/***********************************************************************
 *            Functions to draw basic unfilled primitives              *
 ***********************************************************************/

#define RASTERIZE(x) if (fxMesa->raster_primitive != x) \
                        tdfxRasterPrimitive( ctx, x )
#define RENDER_PRIMITIVE fxMesa->render_primitive
#define IND TDFX_FALLBACK_BIT
#define TAG(x) x
#include "tnl_dd/t_dd_unfilled.h"
#undef IND

/***********************************************************************
 *                 Functions to draw GL primitives                     *
 ***********************************************************************/

#define IND (0)
#define TAG(x) x
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT)
#define TAG(x) x##_offset
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT)
#define TAG(x) x##_twoside
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT)
#define TAG(x) x##_twoside_offset
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_UNFILLED_BIT)
#define TAG(x) x##_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT)
#define TAG(x) x##_offset_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_UNFILLED_BIT)
#define TAG(x) x##_twoside_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT)
#define TAG(x) x##_twoside_offset_unfilled
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_FALLBACK_BIT)
#define TAG(x) x##_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_FALLBACK_BIT)
#define TAG(x) x##_offset_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_FALLBACK_BIT)
#define TAG(x) x##_twoside_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_FALLBACK_BIT)
#define TAG(x) x##_twoside_offset_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_UNFILLED_BIT|TDFX_FALLBACK_BIT)
#define TAG(x) x##_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT|TDFX_FALLBACK_BIT)
#define TAG(x) x##_offset_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_UNFILLED_BIT|TDFX_FALLBACK_BIT)
#define TAG(x) x##_twoside_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT| \
	     TDFX_FALLBACK_BIT)
#define TAG(x) x##_twoside_offset_unfilled_fallback
#include "tnl_dd/t_dd_tritmp.h"


/* Tdfx doesn't support provoking-vertex flat-shading?
 */
#define IND (TDFX_FLAT_BIT)
#define TAG(x) x##_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_offset_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_UNFILLED_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_offset_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_UNFILLED_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_unfilled_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_offset_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_UNFILLED_BIT|TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT|TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_offset_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_UNFILLED_BIT|TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"

#define IND (TDFX_TWOSIDE_BIT|TDFX_OFFSET_BIT|TDFX_UNFILLED_BIT| \
	     TDFX_FALLBACK_BIT|TDFX_FLAT_BIT)
#define TAG(x) x##_twoside_offset_unfilled_fallback_flat
#include "tnl_dd/t_dd_tritmp.h"


static void init_rast_tab( void )
{
   init();
   init_offset();
   init_twoside();
   init_twoside_offset();
   init_unfilled();
   init_offset_unfilled();
   init_twoside_unfilled();
   init_twoside_offset_unfilled();
   init_fallback();
   init_offset_fallback();
   init_twoside_fallback();
   init_twoside_offset_fallback();
   init_unfilled_fallback();
   init_offset_unfilled_fallback();
   init_twoside_unfilled_fallback();
   init_twoside_offset_unfilled_fallback();

   init_flat();
   init_offset_flat();
   init_twoside_flat();
   init_twoside_offset_flat();
   init_unfilled_flat();
   init_offset_unfilled_flat();
   init_twoside_unfilled_flat();
   init_twoside_offset_unfilled_flat();
   init_fallback_flat();
   init_offset_fallback_flat();
   init_twoside_fallback_flat();
   init_twoside_offset_fallback_flat();
   init_unfilled_fallback_flat();
   init_offset_unfilled_fallback_flat();
   init_twoside_unfilled_fallback_flat();
   init_twoside_offset_unfilled_fallback_flat();
}


/**********************************************************************/
/*                 Render whole begin/end objects                     */
/**********************************************************************/


/* Accelerate vertex buffer rendering when renderindex == 0 and
 * there is no clipping.
 */

static void tdfx_render_vb_points( GLcontext *ctx,
				      GLuint start,
				      GLuint count,
				      GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   int stride = 1<<shift;
   char *tmp;
   GLint i;
   (void) flags;

   /* Adjust point coords */
   for (i = start, tmp = fxVB; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x += PNT_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y += PNT_Y_OFFSET - TRI_Y_OFFSET;
   }

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_POINTS, count-start,
                                              fxVB, stride);
   /* restore point coords */
   for (i = start, tmp = fxVB; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x -= PNT_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y -= PNT_Y_OFFSET - TRI_Y_OFFSET;
   }
}

static void tdfx_render_vb_line_strip( GLcontext *ctx,
				      GLuint start,
				      GLuint count,
				      GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   int stride = 1<<shift;
   char *tmp;
   GLint i;
   (void) flags;

   /* adjust line coords */
   for (i = start, tmp = fxVB; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x += LINE_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y += LINE_Y_OFFSET - TRI_Y_OFFSET;
   }

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_LINE_STRIP, count-start,
                                              fxVB, 1<<shift);

   /* restore line coords */
   for (i = start, tmp = fxVB; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x -= LINE_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y -= LINE_Y_OFFSET - TRI_Y_OFFSET;
   }
}

static void tdfx_render_vb_line_loop( GLcontext *ctx,
				      GLuint start,
				      GLuint count,
				      GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   int stride = 1<<shift;
   char *tmp, *tmp2 = fxVB;
   GLint i;
   GLint j = start;
   (void) flags;

   if (!(flags & PRIM_BEGIN)) {
      fxVB += (1 << shift);
      j++;
   }

   /* adjust line coords */
   for (i = start, tmp = tmp2; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x += LINE_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y += LINE_Y_OFFSET - TRI_Y_OFFSET;
   }

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_LINE_STRIP, count-j,
                                              fxVB, 1<<shift);

   if (flags & PRIM_END) 
      fxMesa->Glide.grDrawLine( fxMesa->verts + ((count - 1)<<shift), 
                                fxMesa->verts + (start<<shift) );

   /* restore line coords */
   for (i = start, tmp = tmp2; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x -= LINE_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y -= LINE_Y_OFFSET - TRI_Y_OFFSET;
   }
}

static void tdfx_render_vb_lines( GLcontext *ctx,
				      GLuint start,
				      GLuint count,
				      GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   int stride = 1<<shift;
   char *tmp;
   GLint i;
   (void) flags;

   /* adjust line coords */
   for (i = start, tmp = fxVB; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x += LINE_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y += LINE_Y_OFFSET - TRI_Y_OFFSET;
   }

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_LINES, count-start,
                                              fxVB, 1<<shift);

   /* restore line coords */
   for (i = start, tmp = fxVB; i < count; i++, tmp += stride) {
      ((tdfxVertexPtr)tmp)->v.x -= LINE_X_OFFSET - TRI_X_OFFSET;
      ((tdfxVertexPtr)tmp)->v.y -= LINE_Y_OFFSET - TRI_Y_OFFSET;
   }
}

static void tdfx_render_vb_triangles( GLcontext *ctx,
				      GLuint start,
				      GLuint count,
				      GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   (void) flags;

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_TRIANGLES, count-start,
                                              fxVB, 1<<shift);
}


static void tdfx_render_vb_tri_strip( GLcontext *ctx,
				      GLuint start,
				      GLuint count,
				      GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   int mode;
   (void) flags;

/*     fprintf(stderr, "%s/%d\n", __FUNCTION__, 1<<shift); */
/*     if(!prevLockLine) abort(); */

   if (flags & PRIM_PARITY) 
      mode = GR_TRIANGLE_STRIP_CONTINUE;
   else
      mode = GR_TRIANGLE_STRIP;

   fxMesa->Glide.grDrawVertexArrayContiguous( mode, count-start,
                                              fxVB, 1<<shift);
}


static void tdfx_render_vb_tri_fan( GLcontext *ctx,
				    GLuint start,
				    GLuint count,
				    GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   (void) flags;

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_TRIANGLE_FAN, count-start,
                                              fxVB, 1<<shift );
}

static void tdfx_render_vb_quads( GLcontext *ctx,
				       GLuint start,
				       GLuint count,
				       GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts;
   GLuint i;
   (void) flags;
   
   for (i = start ; i < count-3 ; i += 4 ) {
#define VERT(x) (fxVB + ((x)<<shift))
      fxMesa->Glide.grDrawTriangle( VERT(i),   VERT(i+1), VERT(i+3) );
      fxMesa->Glide.grDrawTriangle( VERT(i+1), VERT(i+2), VERT(i+3) );
#undef VERT
   }
}

static void tdfx_render_vb_quad_strip( GLcontext *ctx,
				       GLuint start,
				       GLuint count,
				       GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   (void) flags;

   count -= (count-start)&1;

   fxMesa->Glide.grDrawVertexArrayContiguous( GR_TRIANGLE_STRIP,
                                              count-start, fxVB, 1<<shift);
}

static void tdfx_render_vb_poly( GLcontext *ctx,
				 GLuint start,
				 GLuint count,
				 GLuint flags )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint shift = fxMesa->vertex_stride_shift;
   char *fxVB = fxMesa->verts + (start << shift);
   (void) flags;
   
   fxMesa->Glide.grDrawVertexArrayContiguous( GR_POLYGON, count-start,
                                              fxVB, 1<<shift);
}

static void tdfx_render_vb_noop( GLcontext *ctx,
				 GLuint start,
				 GLuint count,
				 GLuint flags )
{
   (void) (ctx && start && count && flags);
}

static void (*tdfx_render_tab_verts[GL_POLYGON+2])(GLcontext *,
						   GLuint,
						   GLuint,
						   GLuint) = 
{
   tdfx_render_vb_points,
   tdfx_render_vb_lines,
   tdfx_render_vb_line_loop,
   tdfx_render_vb_line_strip,
   tdfx_render_vb_triangles,
   tdfx_render_vb_tri_strip,
   tdfx_render_vb_tri_fan,
   tdfx_render_vb_quads,
   tdfx_render_vb_quad_strip,
   tdfx_render_vb_poly,
   tdfx_render_vb_noop,
};


/**********************************************************************/
/*            Render whole (indexed) begin/end objects                */
/**********************************************************************/


#define VERT(x) (tdfxVertex *)(vertptr + ((x)<<vertshift))

#define RENDER_POINTS( start, count )		\
   for ( ; start < count ; start++)		\
      fxMesa->Glide.grDrawPoint( VERT(ELT(start)) );

#define RENDER_LINE( v0, v1 ) \
   fxMesa->Glide.grDrawLine( VERT(v0), VERT(v1) )

#define RENDER_TRI( v0, v1, v2 )  \
   fxMesa->Glide.grDrawTriangle( VERT(v0), VERT(v1), VERT(v2) )

#define RENDER_QUAD( v0, v1, v2, v3 ) \
   tdfx_draw_quad( fxMesa, VERT(v0), VERT(v1), VERT(v2), VERT(v3) )

#define INIT(x) tdfxRenderPrimitive( ctx, x )

#undef LOCAL_VARS
#define LOCAL_VARS						\
    tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);			\
    GLubyte *vertptr = (GLubyte *)fxMesa->verts;		\
    const GLuint vertshift = fxMesa->vertex_stride_shift;	\
    const GLuint * const elt = TNL_CONTEXT(ctx)->vb.Elts;	\
    (void) elt;

#define RESET_STIPPLE 
#define RESET_OCCLUSION 
#define PRESERVE_VB_DEFS

/* Elts, no clipping.
 */
#undef ELT
#undef TAG
#define TAG(x) tdfx_##x##_elts
#define ELT(x) elt[x]
#include "tnl_dd/t_dd_rendertmp.h"



/**********************************************************************/
/*                   Render clipped primitives                        */
/**********************************************************************/



static void tdfxRenderClippedPoly( GLcontext *ctx, const GLuint *elts, 
				   GLuint n )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   struct vertex_buffer *VB = &tnl->vb;
   GLuint prim = fxMesa->render_primitive;

   /* Render the new vertices as an unclipped polygon. 
    */
   {
      GLuint *tmp = VB->Elts;
      VB->Elts = (GLuint *)elts;
      tnl->Driver.Render.PrimTabElts[GL_POLYGON]( ctx, 0, n, PRIM_BEGIN|PRIM_END );
      VB->Elts = tmp;
   }

   /* Restore the render primitive
    */
   if (prim != GL_POLYGON)
      tnl->Driver.Render.PrimitiveNotify( ctx, prim );
}

static void tdfxRenderClippedLine( GLcontext *ctx, GLuint ii, GLuint jj )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   tnl->Driver.Render.Line( ctx, ii, jj );
}

static void tdfxFastRenderClippedPoly( GLcontext *ctx, const GLuint *elts, 
				       GLuint n )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT( ctx );
   GLubyte *vertptr = (GLubyte *)fxMesa->verts;			
   const GLuint vertshift = fxMesa->vertex_stride_shift;       	
   const GLuint *start = (const GLuint *)VERT(elts[0]);
   int i;

   for (i = 2 ; i < n ; i++) {
      fxMesa->Glide.grDrawTriangle( VERT(elts[i-1]), VERT(elts[i]), start );
   }
}

/**********************************************************************/
/*                    Choose render functions                         */
/**********************************************************************/


#define POINT_FALLBACK (DD_POINT_SMOOTH)
#define LINE_FALLBACK (DD_LINE_STIPPLE)
#define TRI_FALLBACK (DD_TRI_SMOOTH)
#define ANY_FALLBACK_FLAGS (POINT_FALLBACK|LINE_FALLBACK|TRI_FALLBACK|DD_TRI_STIPPLE)
#define ANY_RASTER_FLAGS (DD_FLATSHADE|DD_TRI_LIGHT_TWOSIDE|DD_TRI_OFFSET| \
			  DD_TRI_UNFILLED)


/* All state referenced below:
 */
#define _TDFX_NEW_RENDERSTATE (_DD_NEW_POINT_SMOOTH |		\
                               _DD_NEW_LINE_STIPPLE |		\
                               _DD_NEW_TRI_SMOOTH |		\
			       _DD_NEW_FLATSHADE |		\
			       _DD_NEW_TRI_UNFILLED |		\
			       _DD_NEW_TRI_LIGHT_TWOSIDE |	\
			       _DD_NEW_TRI_OFFSET |		\
			       _DD_NEW_TRI_STIPPLE |		\
			       _NEW_POLYGONSTIPPLE)


static void tdfxChooseRenderState(GLcontext *ctx)
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint flags = ctx->_TriangleCaps;
   GLuint index = 0;

   if (0) {
      fxMesa->draw_point = tdfx_draw_point;
      fxMesa->draw_line = tdfx_draw_line;
      fxMesa->draw_triangle = tdfx_draw_triangle;
      index |= TDFX_FALLBACK_BIT;
   }

   if (flags & (ANY_FALLBACK_FLAGS|ANY_RASTER_FLAGS)) {
      if (flags & ANY_RASTER_FLAGS) {
	 if (flags & DD_TRI_LIGHT_TWOSIDE)    index |= TDFX_TWOSIDE_BIT;
	 if (flags & DD_TRI_OFFSET)	      index |= TDFX_OFFSET_BIT;
	 if (flags & DD_TRI_UNFILLED)	      index |= TDFX_UNFILLED_BIT;
	 if (flags & DD_FLATSHADE)	      index |= TDFX_FLAT_BIT;
      }

      fxMesa->draw_point = tdfx_draw_point;
      fxMesa->draw_line = tdfx_draw_line;
      fxMesa->draw_triangle = tdfx_draw_triangle;

      /* Hook in fallbacks for specific primitives.
       *
       * DD_TRI_UNFILLED is here because the unfilled_tri functions use
       * fxMesa->draw_tri *always*, and thus can't use the multipass
       * approach to cliprects.
       *
       */
      if (flags & (POINT_FALLBACK|
		   LINE_FALLBACK|
		   TRI_FALLBACK|
		   DD_TRI_STIPPLE|
		   DD_TRI_UNFILLED))
      {
	 if (flags & POINT_FALLBACK)
	    fxMesa->draw_point = tdfx_fallback_point;

	 if (flags & LINE_FALLBACK)
	    fxMesa->draw_line = tdfx_fallback_line;

	 if (flags & TRI_FALLBACK)
	    fxMesa->draw_triangle = tdfx_fallback_tri;

	 if ((flags & DD_TRI_STIPPLE) && !fxMesa->haveHwStipple)
	    fxMesa->draw_triangle = tdfx_fallback_tri;

	 index |= TDFX_FALLBACK_BIT;
      }
   }

   if (fxMesa->RenderIndex != index) {
      fxMesa->RenderIndex = index;

      tnl->Driver.Render.Points = rast_tab[index].points;
      tnl->Driver.Render.Line = rast_tab[index].line;
      tnl->Driver.Render.Triangle = rast_tab[index].triangle;
      tnl->Driver.Render.Quad = rast_tab[index].quad;

      if (index == 0) {
	 tnl->Driver.Render.PrimTabVerts = tdfx_render_tab_verts;
	 tnl->Driver.Render.PrimTabElts = tdfx_render_tab_elts;
	 tnl->Driver.Render.ClippedLine = line; /* from tritmp.h */
	 tnl->Driver.Render.ClippedPolygon = tdfxFastRenderClippedPoly;
      } else {
	 tnl->Driver.Render.PrimTabVerts = _tnl_render_tab_verts;
	 tnl->Driver.Render.PrimTabElts = _tnl_render_tab_elts;
	 tnl->Driver.Render.ClippedLine = tdfxRenderClippedLine;
	 tnl->Driver.Render.ClippedPolygon = tdfxRenderClippedPoly;
      }
   }
}

/**********************************************************************/
/*                Use multipass rendering for cliprects               */
/**********************************************************************/



/* TODO: Benchmark this.
 * TODO: Use single back-buffer cliprect where possible.  
 * NOTE: <pass> starts at 1, not zero!
 */
static GLboolean multipass_cliprect( GLcontext *ctx, GLuint pass )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   if (pass >= fxMesa->numClipRects)
      return GL_FALSE;
   else {   
      fxMesa->Glide.grClipWindow(fxMesa->pClipRects[pass].x1,
		   fxMesa->screen_height - fxMesa->pClipRects[pass].y2,
		   fxMesa->pClipRects[pass].x2,
		   fxMesa->screen_height - fxMesa->pClipRects[pass].y1);
      
      return GL_TRUE;
   }
}


/**********************************************************************/
/*                Runtime render state and callbacks                  */
/**********************************************************************/

static void tdfxRunPipeline( GLcontext *ctx )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);

   if (fxMesa->new_state) {
      tdfxDDUpdateHwState( ctx );
   }

   if (!fxMesa->Fallback && fxMesa->new_gl_state) {
      if (fxMesa->new_gl_state & _TDFX_NEW_RASTERSETUP)
	 tdfxChooseVertexState( ctx );
      
      if (fxMesa->new_gl_state & _TDFX_NEW_RENDERSTATE)
	 tdfxChooseRenderState( ctx );
      
      fxMesa->new_gl_state = 0;
   }

   _tnl_run_pipeline( ctx );
}


static void tdfxRenderStart( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);

   tdfxCheckTexSizes( ctx );

   LOCK_HARDWARE(fxMesa);

   /* Make sure vertex format changes get uploaded before we start
    * sending triangles.  
    */
   if (fxMesa->dirty) {
      tdfxEmitHwStateLocked( fxMesa );
   }

   if (fxMesa->numClipRects && !(fxMesa->RenderIndex & TDFX_FALLBACK_BIT)) {
      fxMesa->Glide.grClipWindow(fxMesa->pClipRects[0].x1,
		   fxMesa->screen_height - fxMesa->pClipRects[0].y2,
		   fxMesa->pClipRects[0].x2,
		   fxMesa->screen_height - fxMesa->pClipRects[0].y1);
      if (fxMesa->numClipRects > 1)
         tnl->Driver.Render.Multipass = multipass_cliprect;
      else
         tnl->Driver.Render.Multipass = NULL;
   }
   else
      tnl->Driver.Render.Multipass = NULL;
}


static GLenum reduced_prim[GL_POLYGON+1] = {
   GL_POINTS,
   GL_LINES,
   GL_LINES,
   GL_LINES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES,
   GL_TRIANGLES
};



/* Always called between RenderStart and RenderFinish --> We already
 * hold the lock.
 */
static void tdfxRasterPrimitive( GLcontext *ctx, GLenum prim )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT( ctx );

   FLUSH_BATCH( fxMesa );

   fxMesa->raster_primitive = prim;

   tdfxUpdateCull(ctx);
   if ( fxMesa->dirty & TDFX_UPLOAD_CULL ) {
      fxMesa->Glide.grCullMode( fxMesa->CullMode );
      fxMesa->dirty &= ~TDFX_UPLOAD_CULL;
   }

   tdfxUpdateStipple(ctx);
   if ( fxMesa->dirty & TDFX_UPLOAD_STIPPLE ) {
      fxMesa->Glide.grStipplePattern ( fxMesa->Stipple.Pattern );
      fxMesa->Glide.grStippleMode ( fxMesa->Stipple.Mode );
      fxMesa->dirty &= ~TDFX_UPLOAD_STIPPLE;
   }
}



/* Determine the rasterized primitive when not drawing unfilled 
 * polygons.
 *
 * Used only for the default render stage which always decomposes
 * primitives to trianges/lines/points.  For the accelerated stage,
 * which renders strips as strips, the equivalent calculations are
 * performed in tdfx_render.c.
 */
static void tdfxRenderPrimitive( GLcontext *ctx, GLenum prim )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint rprim = reduced_prim[prim];

   fxMesa->render_primitive = prim;

   if (rprim == GL_TRIANGLES && (ctx->_TriangleCaps & DD_TRI_UNFILLED))
      return;
       
   if (fxMesa->raster_primitive != rprim) {
      tdfxRasterPrimitive( ctx, rprim );
   }
}

static void tdfxRenderFinish( GLcontext *ctx )
{
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);

   if (fxMesa->RenderIndex & TDFX_FALLBACK_BIT)
      _swrast_flush( ctx );

   UNLOCK_HARDWARE(fxMesa);
}


/**********************************************************************/
/*               Manage total rasterization fallbacks                 */
/**********************************************************************/

static char *fallbackStrings[] = {
   "1D/3D Texture map",
   "glDrawBuffer(GL_FRONT_AND_BACK)",
   "Separate specular color",
   "glEnable/Disable(GL_STENCIL_TEST)",
   "glRenderMode(selection or feedback)",
   "glLogicOp()",
   "Texture env mode",
   "Texture border",
   "glColorMask",
   "blend mode",
   "line stipple"
};


static char *getFallbackString(GLuint bit)
{
   int i = 0;
   while (bit > 1) {
      i++;
      bit >>= 1;
   }
   return fallbackStrings[i];
}


void tdfxFallback( GLcontext *ctx, GLuint bit, GLboolean mode )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   GLuint oldfallback = fxMesa->Fallback;

   if (mode) {
      fxMesa->Fallback |= bit;
      if (oldfallback == 0) {
         /*printf("Go to software rendering, bit = 0x%x\n", bit);*/
	 FLUSH_BATCH(fxMesa);
	 _swsetup_Wakeup( ctx );
	 fxMesa->RenderIndex = ~0;
         if (fxMesa->debugFallbacks) {
            fprintf(stderr, "Tdfx begin software fallback: 0x%x %s\n",
                    bit, getFallbackString(bit));
         }
      }
   }
   else {
      fxMesa->Fallback &= ~bit;
      if (oldfallback == bit) {
         /*printf("Go to hardware rendering, bit = 0x%x\n", bit);*/
	 _swrast_flush( ctx );
	 tnl->Driver.Render.Start = tdfxRenderStart;
	 tnl->Driver.Render.PrimitiveNotify = tdfxRenderPrimitive;
	 tnl->Driver.Render.Finish = tdfxRenderFinish;
	 tnl->Driver.Render.BuildVertices = tdfxBuildVertices;
	 fxMesa->new_gl_state |= (_TDFX_NEW_RENDERSTATE|
				  _TDFX_NEW_RASTERSETUP);
         if (fxMesa->debugFallbacks) {
            fprintf(stderr, "Tdfx end software fallback: 0x%x %s\n",
                    bit, getFallbackString(bit));
         }
      }
   }
}


void tdfxDDInitTriFuncs( GLcontext *ctx )
{
   TNLcontext *tnl = TNL_CONTEXT(ctx);
   tdfxContextPtr fxMesa = TDFX_CONTEXT(ctx);
   static int firsttime = 1;

   if (firsttime) {
      init_rast_tab();
      firsttime = 0;
   }

   fxMesa->RenderIndex = ~0;
	
   tnl->Driver.RunPipeline              = tdfxRunPipeline;
   tnl->Driver.Render.Start             = tdfxRenderStart;
   tnl->Driver.Render.Finish            = tdfxRenderFinish; 
   tnl->Driver.Render.PrimitiveNotify   = tdfxRenderPrimitive;
   tnl->Driver.Render.ResetLineStipple  = _swrast_ResetLineStipple;
   tnl->Driver.Render.BuildVertices     = tdfxBuildVertices;
   tnl->Driver.Render.Multipass		= NULL;

   (void) tdfx_print_vertex;
}
