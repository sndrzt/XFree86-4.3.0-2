/* $XFree86: xc/lib/GL/mesa/src/drv/r200/r200_state.c,v 1.3 2002/12/16 16:18:54 dawes Exp $ */
/*
Copyright (C) The Weather Channel, Inc.  2002.  All Rights Reserved.

The Weather Channel (TM) funded Tungsten Graphics to develop the
initial release of the Radeon 8500 driver under the XFree86 license.
This notice must be preserved.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * Authors:
 *   Keith Whitwell <keith@tungstengraphics.com>
 */

#include "r200_context.h"
#include "r200_ioctl.h"
#include "r200_state.h"
#include "r200_tcl.h"
#include "r200_tex.h"
#include "r200_swtcl.h"
#include "r200_vtxfmt.h"

#include "mem.h"
#include "mmath.h"
#include "enums.h"
#include "colormac.h"
#include "light.h"
#include "api_arrayelt.h"

#include "swrast/swrast.h"
#include "array_cache/acache.h"
#include "tnl/tnl.h"
#include "tnl/t_pipeline.h"
#include "swrast_setup/swrast_setup.h"


/* =============================================================
 * Alpha blending
 */

static void r200AlphaFunc( GLcontext *ctx, GLenum func, GLchan ref )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   int pp_misc = rmesa->hw.ctx.cmd[CTX_PP_MISC];

   R200_STATECHANGE( rmesa, ctx );

   pp_misc &= ~(R200_ALPHA_TEST_OP_MASK | R200_REF_ALPHA_MASK);
   pp_misc |= (ref & R200_REF_ALPHA_MASK);

   switch ( func ) {
   case GL_NEVER:
      pp_misc |= R200_ALPHA_TEST_FAIL; 
      break;
   case GL_LESS:
      pp_misc |= R200_ALPHA_TEST_LESS;
      break;
   case GL_EQUAL:
      pp_misc |= R200_ALPHA_TEST_EQUAL;
      break;
   case GL_LEQUAL:
      pp_misc |= R200_ALPHA_TEST_LEQUAL;
      break;
   case GL_GREATER:
      pp_misc |= R200_ALPHA_TEST_GREATER;
      break;
   case GL_NOTEQUAL:
      pp_misc |= R200_ALPHA_TEST_NEQUAL;
      break;
   case GL_GEQUAL:
      pp_misc |= R200_ALPHA_TEST_GEQUAL;
      break;
   case GL_ALWAYS:
      pp_misc |= R200_ALPHA_TEST_PASS;
      break;
   }
   
   rmesa->hw.ctx.cmd[CTX_PP_MISC] = pp_misc;
}

static void r200BlendEquation( GLcontext *ctx, GLenum mode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint b = rmesa->hw.ctx.cmd[CTX_RB3D_BLENDCNTL] & ~R200_COMB_FCN_MASK;

   switch ( mode ) {
   case GL_FUNC_ADD_EXT:
   case GL_LOGIC_OP:
      b |= R200_COMB_FCN_ADD_CLAMP;
      break;

   case GL_FUNC_SUBTRACT_EXT:
      b |= R200_COMB_FCN_SUB_CLAMP;
      break;

   case GL_FUNC_REVERSE_SUBTRACT_EXT:
      b |= R200_COMB_FCN_RSUB_CLAMP;
      break;

   case GL_MIN_EXT:
      b |= R200_COMB_FCN_MIN;
      break;

   case GL_MAX_EXT:
      b |= R200_COMB_FCN_MAX;
      break;

   default:
      break;
   }

   R200_STATECHANGE( rmesa, ctx );
   rmesa->hw.ctx.cmd[CTX_RB3D_BLENDCNTL] = b;
   if ( ctx->Color.ColorLogicOpEnabled ) {
      rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_ROP_ENABLE;
   } else {
      rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_ROP_ENABLE;
   }
}

static void r200BlendFunc( GLcontext *ctx, GLenum sfactor, GLenum dfactor )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint b = rmesa->hw.ctx.cmd[CTX_RB3D_BLENDCNTL] & 
      ~(R200_SRC_BLEND_MASK | R200_DST_BLEND_MASK);

   switch ( ctx->Color.BlendSrcRGB ) {
   case GL_ZERO:
      b |= R200_SRC_BLEND_GL_ZERO;
      break;
   case GL_ONE:
      b |= R200_SRC_BLEND_GL_ONE;
      break;
   case GL_DST_COLOR:
      b |= R200_SRC_BLEND_GL_DST_COLOR;
      break;
   case GL_ONE_MINUS_DST_COLOR:
      b |= R200_SRC_BLEND_GL_ONE_MINUS_DST_COLOR;
      break;
   case GL_SRC_ALPHA:
      b |= R200_SRC_BLEND_GL_SRC_ALPHA;
      break;
   case GL_ONE_MINUS_SRC_ALPHA:
      b |= R200_SRC_BLEND_GL_ONE_MINUS_SRC_ALPHA;
      break;
   case GL_DST_ALPHA:
      b |= R200_SRC_BLEND_GL_DST_ALPHA;
      break;
   case GL_ONE_MINUS_DST_ALPHA:
      b |= R200_SRC_BLEND_GL_ONE_MINUS_DST_ALPHA;
      break;
   case GL_SRC_ALPHA_SATURATE:
      b |= R200_SRC_BLEND_GL_SRC_ALPHA_SATURATE;
      break;
   case GL_CONSTANT_COLOR:
      b |= R200_SRC_BLEND_GL_CONST_COLOR;
      break;
   case GL_ONE_MINUS_CONSTANT_COLOR:
      b |= R200_SRC_BLEND_GL_ONE_MINUS_CONST_COLOR;
      break;
   case GL_CONSTANT_ALPHA:
      b |= R200_SRC_BLEND_GL_CONST_ALPHA;
      break;
   case GL_ONE_MINUS_CONSTANT_ALPHA:
      b |= R200_SRC_BLEND_GL_ONE_MINUS_CONST_ALPHA;
      break;
   default:
      break;
   }

   switch ( ctx->Color.BlendDstRGB ) {
   case GL_ZERO:
      b |= R200_DST_BLEND_GL_ZERO;
      break;
   case GL_ONE:
      b |= R200_DST_BLEND_GL_ONE;
      break;
   case GL_SRC_COLOR:
      b |= R200_DST_BLEND_GL_SRC_COLOR;
      break;
   case GL_ONE_MINUS_SRC_COLOR:
      b |= R200_DST_BLEND_GL_ONE_MINUS_SRC_COLOR;
      break;
   case GL_SRC_ALPHA:
      b |= R200_DST_BLEND_GL_SRC_ALPHA;
      break;
   case GL_ONE_MINUS_SRC_ALPHA:
      b |= R200_DST_BLEND_GL_ONE_MINUS_SRC_ALPHA;
      break;
   case GL_DST_ALPHA:
      b |= R200_DST_BLEND_GL_DST_ALPHA;
      break;
   case GL_ONE_MINUS_DST_ALPHA:
      b |= R200_DST_BLEND_GL_ONE_MINUS_DST_ALPHA;
      break;
   case GL_CONSTANT_COLOR:
      b |= R200_DST_BLEND_GL_CONST_COLOR;
      break;
   case GL_ONE_MINUS_CONSTANT_COLOR:
      b |= R200_DST_BLEND_GL_ONE_MINUS_CONST_COLOR;
      break;
   case GL_CONSTANT_ALPHA:
      b |= R200_DST_BLEND_GL_CONST_ALPHA;
      break;
   case GL_ONE_MINUS_CONSTANT_ALPHA:
      b |= R200_DST_BLEND_GL_ONE_MINUS_CONST_ALPHA;
      break;
   default:
      break;
   }

   R200_STATECHANGE( rmesa, ctx );
   rmesa->hw.ctx.cmd[CTX_RB3D_BLENDCNTL] = b;
}

static void r200BlendFuncSeparate( GLcontext *ctx,
				     GLenum sfactorRGB, GLenum dfactorRGB,
				     GLenum sfactorA, GLenum dfactorA )
{
   r200BlendFunc( ctx, sfactorRGB, dfactorRGB );
}


/* =============================================================
 * Depth testing
 */

static void r200DepthFunc( GLcontext *ctx, GLenum func )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   
   R200_STATECHANGE( rmesa, ctx );
   rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] &= ~R200_Z_TEST_MASK;

   switch ( ctx->Depth.Func ) {
   case GL_NEVER:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_NEVER;
      break;
   case GL_LESS:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_LESS;
      break;
   case GL_EQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_EQUAL;
      break;
   case GL_LEQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_LEQUAL;
      break;
   case GL_GREATER:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_GREATER;
      break;
   case GL_NOTEQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_NEQUAL;
      break;
   case GL_GEQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_GEQUAL;
      break;
   case GL_ALWAYS:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_Z_TEST_ALWAYS;
      break;
   }
}


static void r200DepthMask( GLcontext *ctx, GLboolean flag )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   R200_STATECHANGE( rmesa, ctx );

   if ( ctx->Depth.Mask ) {
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |=  R200_Z_WRITE_ENABLE;
   } else {
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] &= ~R200_Z_WRITE_ENABLE;
   }
}


/* =============================================================
 * Fog
 */


static void r200Fogfv( GLcontext *ctx, GLenum pname, const GLfloat *param )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   union { int i; float f; } c, d;
   GLchan col[4];
   GLuint i;

   c.i = rmesa->hw.fog.cmd[FOG_C];
   d.i = rmesa->hw.fog.cmd[FOG_D];

   switch (pname) {
   case GL_FOG_MODE:
      if (!ctx->Fog.Enabled)
	 return;
      R200_STATECHANGE(rmesa, tcl);
      rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] &= ~R200_TCL_FOG_MASK;
      switch (ctx->Fog.Mode) {
      case GL_LINEAR:
	 rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] |= R200_TCL_FOG_LINEAR;
	 if (ctx->Fog.Start == ctx->Fog.End) {
	    c.f = 1.0F;
	    d.f = 1.0F;
	 }
	 else {
	    c.f = ctx->Fog.End/(ctx->Fog.End-ctx->Fog.Start);
	    d.f = -1.0/(ctx->Fog.End-ctx->Fog.Start);
	 }
	 break;
      case GL_EXP:
	 rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] |= R200_TCL_FOG_EXP;
	 c.f = 0.0;
	 d.f = -ctx->Fog.Density;
	 break;
      case GL_EXP2:
	 rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] |= R200_TCL_FOG_EXP2;
	 c.f = 0.0;
	 d.f = -(ctx->Fog.Density * ctx->Fog.Density);
	 break;
      default:
	 return;
      }
      break;
   case GL_FOG_DENSITY:
      switch (ctx->Fog.Mode) {
      case GL_EXP:
	 c.f = 0.0;
	 d.f = -ctx->Fog.Density;
	 break;
      case GL_EXP2:
	 c.f = 0.0;
	 d.f = -(ctx->Fog.Density * ctx->Fog.Density);
	 break;
      default:
	 break;
      }
      break;
   case GL_FOG_START:
   case GL_FOG_END:
      if (ctx->Fog.Mode == GL_LINEAR) {
	 if (ctx->Fog.Start == ctx->Fog.End) {
	    c.f = 1.0F;
	    d.f = 1.0F;
	 } else {
	    c.f = ctx->Fog.End/(ctx->Fog.End-ctx->Fog.Start);
	    d.f = -1.0/(ctx->Fog.End-ctx->Fog.Start);
	 }
      }
      break;
   case GL_FOG_COLOR: 
      R200_STATECHANGE( rmesa, ctx );
      UNCLAMPED_FLOAT_TO_RGB_CHAN( col, ctx->Fog.Color );
      i = r200PackColor( 4, col[0], col[1], col[2], 0 );
      rmesa->hw.ctx.cmd[CTX_PP_FOG_COLOR] &= ~R200_FOG_COLOR_MASK;
      rmesa->hw.ctx.cmd[CTX_PP_FOG_COLOR] |= i;
      break;
   case GL_FOG_COORDINATE_SOURCE_EXT: 
      /* What to do?
       */
      break;
   default:
      return;
   }

   if (c.i != rmesa->hw.fog.cmd[FOG_C] || d.i != rmesa->hw.fog.cmd[FOG_D]) {
      R200_STATECHANGE( rmesa, fog );
      rmesa->hw.fog.cmd[FOG_C] = c.i;
      rmesa->hw.fog.cmd[FOG_D] = d.i;
   }
}


/* =============================================================
 * Scissoring
 */


static GLboolean intersect_rect( XF86DRIClipRectPtr out,
				 XF86DRIClipRectPtr a,
				 XF86DRIClipRectPtr b )
{
   *out = *a;
   if ( b->x1 > out->x1 ) out->x1 = b->x1;
   if ( b->y1 > out->y1 ) out->y1 = b->y1;
   if ( b->x2 < out->x2 ) out->x2 = b->x2;
   if ( b->y2 < out->y2 ) out->y2 = b->y2;
   if ( out->x1 >= out->x2 ) return GL_FALSE;
   if ( out->y1 >= out->y2 ) return GL_FALSE;
   return GL_TRUE;
}


void r200RecalcScissorRects( r200ContextPtr rmesa )
{
   XF86DRIClipRectPtr out;
   int i;

   /* Grow cliprect store?
    */
   if (rmesa->state.scissor.numAllocedClipRects < rmesa->numClipRects) {
      while (rmesa->state.scissor.numAllocedClipRects < rmesa->numClipRects) {
	 rmesa->state.scissor.numAllocedClipRects += 1;	/* zero case */
	 rmesa->state.scissor.numAllocedClipRects *= 2;
      }

      if (rmesa->state.scissor.pClipRects)
	 FREE(rmesa->state.scissor.pClipRects);

      rmesa->state.scissor.pClipRects = 
	 MALLOC( rmesa->state.scissor.numAllocedClipRects * 
		 sizeof(XF86DRIClipRectRec) );

      if (!rmesa->state.scissor.numAllocedClipRects) {
	 rmesa->state.scissor.numAllocedClipRects = 0;
	 return;
      }
   }
   
   out = rmesa->state.scissor.pClipRects;
   rmesa->state.scissor.numClipRects = 0;

   for ( i = 0 ; i < rmesa->numClipRects ;  i++ ) {
      if ( intersect_rect( out, 
			   &rmesa->pClipRects[i], 
			   &rmesa->state.scissor.rect ) ) {
	 rmesa->state.scissor.numClipRects++;
	 out++;
      }
   }
}


static void r200UpdateScissor( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   if ( rmesa->dri.drawable ) {
      __DRIdrawablePrivate *dPriv = rmesa->dri.drawable;

      int x = ctx->Scissor.X;
      int y = dPriv->h - ctx->Scissor.Y - ctx->Scissor.Height;
      int w = ctx->Scissor.X + ctx->Scissor.Width - 1;
      int h = dPriv->h - ctx->Scissor.Y - 1;

      rmesa->state.scissor.rect.x1 = x + dPriv->x;
      rmesa->state.scissor.rect.y1 = y + dPriv->y;
      rmesa->state.scissor.rect.x2 = w + dPriv->x + 1;
      rmesa->state.scissor.rect.y2 = h + dPriv->y + 1;

      r200RecalcScissorRects( rmesa );
   }
}


static void r200Scissor( GLcontext *ctx,
			   GLint x, GLint y, GLsizei w, GLsizei h )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   if ( ctx->Scissor.Enabled ) {
      R200_FIREVERTICES( rmesa );	/* don't pipeline cliprect changes */
      r200UpdateScissor( ctx );
   }

}


/* =============================================================
 * Culling
 */

static void r200CullFace( GLcontext *ctx, GLenum unused )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint s = rmesa->hw.set.cmd[SET_SE_CNTL];
   GLuint t = rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL];

   s |= R200_FFACE_SOLID | R200_BFACE_SOLID;
   t &= ~(R200_CULL_FRONT | R200_CULL_BACK);

   if ( ctx->Polygon.CullFlag ) {
      switch ( ctx->Polygon.CullFaceMode ) {
      case GL_FRONT:
	 s &= ~R200_FFACE_SOLID;
	 t |= R200_CULL_FRONT;
	 break;
      case GL_BACK:
	 s &= ~R200_BFACE_SOLID;
	 t |= R200_CULL_BACK;
	 break;
      case GL_FRONT_AND_BACK:
	 s &= ~(R200_FFACE_SOLID | R200_BFACE_SOLID);
	 t |= (R200_CULL_FRONT | R200_CULL_BACK);
	 break;
      }
   }

   if ( rmesa->hw.set.cmd[SET_SE_CNTL] != s ) {
      R200_STATECHANGE(rmesa, set );
      rmesa->hw.set.cmd[SET_SE_CNTL] = s;
   }

   if ( rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] != t ) {
      R200_STATECHANGE(rmesa, tcl );
      rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] = t;
   }
}

static void r200FrontFace( GLcontext *ctx, GLenum mode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   R200_STATECHANGE( rmesa, set );
   rmesa->hw.set.cmd[SET_SE_CNTL] &= ~R200_FFACE_CULL_DIR_MASK;

   R200_STATECHANGE( rmesa, tcl );
   rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] &= ~R200_CULL_FRONT_IS_CCW;

   switch ( mode ) {
   case GL_CW:
      rmesa->hw.set.cmd[SET_SE_CNTL] |= R200_FFACE_CULL_CW;
      break;
   case GL_CCW:
      rmesa->hw.set.cmd[SET_SE_CNTL] |= R200_FFACE_CULL_CCW;
      rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] |= R200_CULL_FRONT_IS_CCW;
      break;
   }
}

/* =============================================================
 * Point state
 */
static void r200PointSize( GLcontext *ctx, GLfloat size )
{
   if (R200_DEBUG & DEBUG_STATE)
      fprintf(stderr, "%s: %f\n", __FUNCTION__, size );
}

/* =============================================================
 * Line state
 */
static void r200LineWidth( GLcontext *ctx, GLfloat widthf )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   R200_STATECHANGE( rmesa, lin );
   R200_STATECHANGE( rmesa, set );

   /* Line width is stored in U6.4 format.
    */
   rmesa->hw.lin.cmd[LIN_SE_LINE_WIDTH] &= ~0xffff;
   rmesa->hw.lin.cmd[LIN_SE_LINE_WIDTH] |= (GLuint)(ctx->Line._Width * 16.0);

   if ( widthf > 1.0 ) {
      rmesa->hw.set.cmd[SET_SE_CNTL] |=  R200_WIDELINE_ENABLE;
   } else {
      rmesa->hw.set.cmd[SET_SE_CNTL] &= ~R200_WIDELINE_ENABLE;
   }
}

static void r200LineStipple( GLcontext *ctx, GLint factor, GLushort pattern )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   R200_STATECHANGE( rmesa, lin );
   rmesa->hw.lin.cmd[LIN_RE_LINE_PATTERN] = 
      ((((GLuint)factor & 0xff) << 16) | ((GLuint)pattern));
}


/* =============================================================
 * Masks
 */
static void r200ColorMask( GLcontext *ctx,
			   GLboolean r, GLboolean g,
			   GLboolean b, GLboolean a )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint mask = r200PackColor( rmesa->r200Screen->cpp,
				ctx->Color.ColorMask[RCOMP],
				ctx->Color.ColorMask[GCOMP],
				ctx->Color.ColorMask[BCOMP],
				ctx->Color.ColorMask[ACOMP] );

   GLuint flag = rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] & ~R200_PLANE_MASK_ENABLE;

   if (!(r && g && b && a))
      flag |= R200_PLANE_MASK_ENABLE;

   if ( rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] != flag ) { 
      R200_STATECHANGE( rmesa, ctx ); 
      rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] = flag; 
   } 

   if ( rmesa->hw.msk.cmd[MSK_RB3D_PLANEMASK] != mask ) {
      R200_STATECHANGE( rmesa, msk );
      rmesa->hw.msk.cmd[MSK_RB3D_PLANEMASK] = mask;
   }
}


/* =============================================================
 * Polygon state
 */

static void r200PolygonOffset( GLcontext *ctx,
			       GLfloat factor, GLfloat units )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLfloat constant = units * rmesa->state.depth.scale;

/*    factor *= 2; */
/*    constant *= 2; */
   
/*    fprintf(stderr, "%s f:%f u:%f\n", __FUNCTION__, factor, constant); */

   R200_STATECHANGE( rmesa, zbs );
   rmesa->hw.zbs.cmd[ZBS_SE_ZBIAS_FACTOR]   = *(GLuint *)&factor;
   rmesa->hw.zbs.cmd[ZBS_SE_ZBIAS_CONSTANT] = *(GLuint *)&constant;
}

static void r200PolygonStipple( GLcontext *ctx, const GLubyte *mask )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint i;
   drmRadeonStipple stipple;

   /* Must flip pattern upside down.
    */
   for ( i = 0 ; i < 32 ; i++ ) {
      rmesa->state.stipple.mask[31 - i] = ((GLuint *) mask)[i];
   }

   /* TODO: push this into cmd mechanism
    */
   R200_FIREVERTICES( rmesa );
   LOCK_HARDWARE( rmesa );

   /* FIXME: Use window x,y offsets into stipple RAM.
    */
   stipple.mask = rmesa->state.stipple.mask;
   drmCommandWrite( rmesa->dri.fd, DRM_RADEON_STIPPLE, 
                    &stipple, sizeof(drmRadeonStipple) );
   UNLOCK_HARDWARE( rmesa );
}

static void r200PolygonMode( GLcontext *ctx, GLenum face, GLenum mode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLboolean flag = (ctx->_TriangleCaps & DD_TRI_UNFILLED) != 0;

   /* Can't generally do unfilled via tcl, but some good special
    * cases work. 
    */
   TCL_FALLBACK( ctx, R200_TCL_FALLBACK_UNFILLED, flag);
   if (rmesa->TclFallback) {
      r200ChooseRenderState( ctx );
      r200ChooseVertexState( ctx );
   }
}


/* =============================================================
 * Rendering attributes
 *
 * We really don't want to recalculate all this every time we bind a
 * texture.  These things shouldn't change all that often, so it makes
 * sense to break them out of the core texture state update routines.
 */

/* Examine lighting and texture state to determine if separate specular
 * should be enabled.
 */
static void r200UpdateSpecular( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   CARD32 p = rmesa->hw.ctx.cmd[CTX_PP_CNTL];

   R200_STATECHANGE( rmesa, tcl );
   R200_STATECHANGE( rmesa, vtx );

   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] &= ~(3<<R200_VTX_COLOR_0_SHIFT);
   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] &= ~(3<<R200_VTX_COLOR_1_SHIFT);
   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] &= ~R200_OUTPUT_COLOR_0;
   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] &= ~R200_OUTPUT_COLOR_1;
   rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= ~R200_LIGHTING_ENABLE;
   p &= ~R200_SPECULAR_ENABLE;

   rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |= R200_DIFFUSE_SPECULAR_COMBINE;


   if (ctx->Light.Enabled &&
       ctx->Light.Model.ColorControl == GL_SEPARATE_SPECULAR_COLOR) {
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] |= 
	 ((R200_VTX_FP_RGBA << R200_VTX_COLOR_0_SHIFT) |
	  (R200_VTX_FP_RGBA << R200_VTX_COLOR_1_SHIFT));	
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] |= R200_OUTPUT_COLOR_0;
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] |= R200_OUTPUT_COLOR_1;
      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |= R200_LIGHTING_ENABLE;
      p |=  R200_SPECULAR_ENABLE;
      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= 
	 ~R200_DIFFUSE_SPECULAR_COMBINE;
   }
   else if (ctx->Light.Enabled) {
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] |= 
	 ((R200_VTX_FP_RGBA << R200_VTX_COLOR_0_SHIFT));	
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] |= R200_OUTPUT_COLOR_0;
      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |= R200_LIGHTING_ENABLE;
   } else if (ctx->Fog.ColorSumEnabled ) {
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] |= 
	 ((R200_VTX_FP_RGBA << R200_VTX_COLOR_0_SHIFT) |
	  (R200_VTX_FP_RGBA << R200_VTX_COLOR_1_SHIFT));	
      p |=  R200_SPECULAR_ENABLE;
   } else {
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] |= 
	 ((R200_VTX_FP_RGBA << R200_VTX_COLOR_0_SHIFT));	
   }

   if (ctx->Fog.Enabled) {
       rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] |= 
	  ((R200_VTX_FP_RGBA << R200_VTX_COLOR_1_SHIFT));	
       rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] |= R200_OUTPUT_COLOR_1;
   }

   if ( rmesa->hw.ctx.cmd[CTX_PP_CNTL] != p ) {
      R200_STATECHANGE( rmesa, ctx );
      rmesa->hw.ctx.cmd[CTX_PP_CNTL] = p;
   }

   /* Update vertex/render formats
    */
   if (rmesa->TclFallback) { 
      r200ChooseRenderState( ctx );
      r200ChooseVertexState( ctx );
   }
}


/* =============================================================
 * Materials
 */


/* Update on colormaterial, material emmissive/ambient, 
 * lightmodel.globalambient
 */
static void update_global_ambient( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   float *fcmd = (float *)R200_DB_STATE( glt );

   /* Need to do more if both emmissive & ambient are PREMULT:
    */
   if ((rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_1] &
       ((3 << R200_FRONT_EMISSIVE_SOURCE_SHIFT) |
	(3 << R200_FRONT_AMBIENT_SOURCE_SHIFT))) == 0) 
   {
      COPY_3V( &fcmd[GLT_RED], 
	       ctx->Light.Material[0].Emission);
      ACC_SCALE_3V( &fcmd[GLT_RED],
		   ctx->Light.Model.Ambient,
		   ctx->Light.Material[0].Ambient);
   } 
   else
   {
      COPY_3V( &fcmd[GLT_RED], ctx->Light.Model.Ambient );
   }
   
   R200_DB_STATECHANGE(rmesa, &rmesa->hw.glt);
}

/* Update on change to 
 *    - light[p].colors
 *    - light[p].enabled
 *    - material,
 *    - colormaterial enabled
 *    - colormaterial bitmask
 */
static void update_light_colors( GLcontext *ctx, GLuint p )
{
   struct gl_light *l = &ctx->Light.Light[p];

/*     fprintf(stderr, "%s\n", __FUNCTION__); */

   if (l->Enabled) {
      r200ContextPtr rmesa = R200_CONTEXT(ctx);
      float *fcmd = (float *)R200_DB_STATE( lit[p] );
      GLuint bitmask = ctx->Light.ColorMaterialBitmask;
      struct gl_material *mat = &ctx->Light.Material[0];

      COPY_4V( &fcmd[LIT_AMBIENT_RED], l->Ambient );	 
      COPY_4V( &fcmd[LIT_DIFFUSE_RED], l->Diffuse );
      COPY_4V( &fcmd[LIT_SPECULAR_RED], l->Specular );
      
      if (!ctx->Light.ColorMaterialEnabled)
	 bitmask = 0;

      if ((bitmask & FRONT_AMBIENT_BIT) == 0) 
	 SELF_SCALE_3V( &fcmd[LIT_AMBIENT_RED], mat->Ambient );

      if ((bitmask & FRONT_DIFFUSE_BIT) == 0) 
	 SELF_SCALE_3V( &fcmd[LIT_DIFFUSE_RED], mat->Diffuse );
      
      if ((bitmask & FRONT_SPECULAR_BIT) == 0) 
	 SELF_SCALE_3V( &fcmd[LIT_SPECULAR_RED], mat->Specular );

      R200_DB_STATECHANGE( rmesa, &rmesa->hw.lit[p] );
   }
}

/* Also fallback for asym colormaterial mode in twoside lighting...
 */
static void check_twoside_fallback( GLcontext *ctx )
{
   GLboolean fallback = GL_FALSE;

   if (ctx->Light.Enabled && ctx->Light.Model.TwoSide) {
      if (memcmp( &ctx->Light.Material[0],
		  &ctx->Light.Material[1],
		  sizeof(struct gl_material)) != 0)
	 fallback = GL_TRUE;  
      else if (ctx->Light.ColorMaterialEnabled &&
	       (ctx->Light.ColorMaterialBitmask & BACK_MATERIAL_BITS) != 
	       ((ctx->Light.ColorMaterialBitmask & FRONT_MATERIAL_BITS)<<1))
	 fallback = GL_TRUE;
   }

   TCL_FALLBACK( ctx, R200_TCL_FALLBACK_LIGHT_TWOSIDE, fallback );
}

static void r200ColorMaterial( GLcontext *ctx, GLenum face, GLenum mode )
{
   if (ctx->Light.ColorMaterialEnabled) {
      r200ContextPtr rmesa = R200_CONTEXT(ctx);
      GLuint light_model_ctl1 = rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_1];
      GLuint mask = ctx->Light.ColorMaterialBitmask;

      /* Default to PREMULT:
       */
      light_model_ctl1 &= ~((0xf << R200_FRONT_EMISSIVE_SOURCE_SHIFT) |
			   (0xf << R200_FRONT_AMBIENT_SOURCE_SHIFT) |
			   (0xf << R200_FRONT_DIFFUSE_SOURCE_SHIFT) |
			   (0xf << R200_FRONT_SPECULAR_SOURCE_SHIFT)); 
   
      if (mask & FRONT_EMISSION_BIT) {
	 light_model_ctl1 |= (R200_LM1_SOURCE_VERTEX_COLOR_0 <<
			     R200_FRONT_EMISSIVE_SOURCE_SHIFT);
      }

      if (mask & FRONT_AMBIENT_BIT) {
	 light_model_ctl1 |= (R200_LM1_SOURCE_VERTEX_COLOR_0 <<
			     R200_FRONT_AMBIENT_SOURCE_SHIFT);
      }
	 
      if (mask & FRONT_DIFFUSE_BIT) {
	 light_model_ctl1 |= (R200_LM1_SOURCE_VERTEX_COLOR_0 <<
			     R200_FRONT_DIFFUSE_SOURCE_SHIFT);
      }
   
      if (mask & FRONT_SPECULAR_BIT) {
	 light_model_ctl1 |= (R200_LM1_SOURCE_VERTEX_COLOR_0 <<
			     R200_FRONT_SPECULAR_SOURCE_SHIFT);
      }
   
      if (light_model_ctl1 != rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_1]) {
	 GLuint p;

	 R200_STATECHANGE( rmesa, tcl );
	 rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_1] = light_model_ctl1;      

	 for (p = 0 ; p < MAX_LIGHTS; p++) 
	    update_light_colors( ctx, p );
	 update_global_ambient( ctx );
      }
   }
   
   check_twoside_fallback( ctx );
}

void r200UpdateMaterial( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLfloat *fcmd = (GLfloat *)R200_DB_STATE( mtl[0] );
   GLuint p;
   GLuint mask = ~0;
   
   if (ctx->Light.ColorMaterialEnabled)
      mask &= ~ctx->Light.ColorMaterialBitmask;

   if (R200_DEBUG & DEBUG_STATE)
      fprintf(stderr, "%s\n", __FUNCTION__);

      
   if (mask & FRONT_EMISSION_BIT) {
      fcmd[MTL_EMMISSIVE_RED]   = ctx->Light.Material[0].Emission[0];
      fcmd[MTL_EMMISSIVE_GREEN] = ctx->Light.Material[0].Emission[1];
      fcmd[MTL_EMMISSIVE_BLUE]  = ctx->Light.Material[0].Emission[2];
      fcmd[MTL_EMMISSIVE_ALPHA] = ctx->Light.Material[0].Emission[3];
   }
   if (mask & FRONT_AMBIENT_BIT) {
      fcmd[MTL_AMBIENT_RED]     = ctx->Light.Material[0].Ambient[0];
      fcmd[MTL_AMBIENT_GREEN]   = ctx->Light.Material[0].Ambient[1];
      fcmd[MTL_AMBIENT_BLUE]    = ctx->Light.Material[0].Ambient[2];
      fcmd[MTL_AMBIENT_ALPHA]   = ctx->Light.Material[0].Ambient[3];
   }
   if (mask & FRONT_DIFFUSE_BIT) {
      fcmd[MTL_DIFFUSE_RED]     = ctx->Light.Material[0].Diffuse[0];
      fcmd[MTL_DIFFUSE_GREEN]   = ctx->Light.Material[0].Diffuse[1];
      fcmd[MTL_DIFFUSE_BLUE]    = ctx->Light.Material[0].Diffuse[2];
      fcmd[MTL_DIFFUSE_ALPHA]   = ctx->Light.Material[0].Diffuse[3];
   }
   if (mask & FRONT_SPECULAR_BIT) {
      fcmd[MTL_SPECULAR_RED]    = ctx->Light.Material[0].Specular[0];
      fcmd[MTL_SPECULAR_GREEN]  = ctx->Light.Material[0].Specular[1];
      fcmd[MTL_SPECULAR_BLUE]   = ctx->Light.Material[0].Specular[2];
      fcmd[MTL_SPECULAR_ALPHA]  = ctx->Light.Material[0].Specular[3];
   }
   if (mask & FRONT_SHININESS_BIT) {
      fcmd[MTL_SHININESS]       = ctx->Light.Material[0].Shininess;
   }

   if (R200_DB_STATECHANGE( rmesa, &rmesa->hw.mtl[0] )) {
      for (p = 0 ; p < MAX_LIGHTS; p++) 
	 update_light_colors( ctx, p );

      check_twoside_fallback( ctx );
      update_global_ambient( ctx );
   }
   else if (R200_DEBUG & (DEBUG_PRIMS|DEBUG_STATE))
      fprintf(stderr, "%s: Elided noop material call\n", __FUNCTION__);
}

/* _NEW_LIGHT
 * _NEW_MODELVIEW
 * _MESA_NEW_NEED_EYE_COORDS
 *
 * Uses derived state from mesa:
 *       _VP_inf_norm
 *       _h_inf_norm
 *       _Position
 *       _NormDirection
 *       _ModelViewInvScale
 *       _NeedEyeCoords
 *       _EyeZDir
 *
 * which are calculated in light.c and are correct for the current
 * lighting space (model or eye), hence dependencies on _NEW_MODELVIEW
 * and _MESA_NEW_NEED_EYE_COORDS.  
 */
static void update_light( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   /* Have to check these, or have an automatic shortcircuit mechanism
    * to remove noop statechanges. (Or just do a better job on the
    * front end).
    */
   {
      GLuint tmp = rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0];

      if (ctx->_NeedEyeCoords)
	 tmp &= ~R200_LIGHT_IN_MODELSPACE;
      else
	 tmp |= R200_LIGHT_IN_MODELSPACE;
      
      if (tmp != rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0]) 
      {
	 R200_STATECHANGE( rmesa, tcl );
	 rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] = tmp;
      }
   }

   {
      GLfloat *fcmd = (GLfloat *)R200_DB_STATE( eye );
      fcmd[EYE_X] = ctx->_EyeZDir[0];
      fcmd[EYE_Y] = ctx->_EyeZDir[1];
      fcmd[EYE_Z] = - ctx->_EyeZDir[2];
      fcmd[EYE_RESCALE_FACTOR] = ctx->_ModelViewInvScale;
      R200_DB_STATECHANGE( rmesa, &rmesa->hw.eye );
   }


/*     R200_STATECHANGE( rmesa, glt ); */

   if (ctx->Light.Enabled) {
      GLint p;
      for (p = 0 ; p < MAX_LIGHTS; p++) {
	 if (ctx->Light.Light[p].Enabled) {
	    struct gl_light *l = &ctx->Light.Light[p];
	    GLfloat *fcmd = (GLfloat *)R200_DB_STATE( lit[p] );
	    
	    if (l->EyePosition[3] == 0.0) {
	       COPY_3FV( &fcmd[LIT_POSITION_X], l->_VP_inf_norm ); 
	       COPY_3FV( &fcmd[LIT_DIRECTION_X], l->_h_inf_norm ); 
	       fcmd[LIT_POSITION_W] = 0;
	       fcmd[LIT_DIRECTION_W] = 0;
	    } else {
	       COPY_4V( &fcmd[LIT_POSITION_X], l->_Position );
	       fcmd[LIT_DIRECTION_X] = -l->_NormDirection[0];
	       fcmd[LIT_DIRECTION_Y] = -l->_NormDirection[1];
	       fcmd[LIT_DIRECTION_Z] = -l->_NormDirection[2];
	       fcmd[LIT_DIRECTION_W] = 0;
	    }

	    R200_DB_STATECHANGE( rmesa, &rmesa->hw.lit[p] );
	 }
      }
   }
}

static void r200Lightfv( GLcontext *ctx, GLenum light,
			   GLenum pname, const GLfloat *params )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLint p = light - GL_LIGHT0;
   struct gl_light *l = &ctx->Light.Light[p];
   GLfloat *fcmd = (GLfloat *)rmesa->hw.lit[p].cmd;
   

   switch (pname) {
   case GL_AMBIENT:		
   case GL_DIFFUSE:
   case GL_SPECULAR:
      update_light_colors( ctx, p );
      break;

   case GL_SPOT_DIRECTION: 
      /* picked up in update_light */	
      break;

   case GL_POSITION: {
      /* positions picked up in update_light, but can do flag here */	
      GLuint flag = (p&1)? R200_LIGHT_1_IS_LOCAL : R200_LIGHT_0_IS_LOCAL;
      GLuint idx = TCL_PER_LIGHT_CTL_0 + p/2;

      R200_STATECHANGE(rmesa, tcl);
      if (l->EyePosition[3] != 0.0F)
	 rmesa->hw.tcl.cmd[idx] |= flag;
      else
	 rmesa->hw.tcl.cmd[idx] &= ~flag;
      break;
   }

   case GL_SPOT_EXPONENT:
      R200_STATECHANGE(rmesa, lit[p]);
      fcmd[LIT_SPOT_EXPONENT] = params[0];
      break;

   case GL_SPOT_CUTOFF: {
      GLuint flag = (p&1) ? R200_LIGHT_1_IS_SPOT : R200_LIGHT_0_IS_SPOT;
      GLuint idx = TCL_PER_LIGHT_CTL_0 + p/2;

      R200_STATECHANGE(rmesa, lit[p]);
      fcmd[LIT_SPOT_CUTOFF] = l->_CosCutoff;

      R200_STATECHANGE(rmesa, tcl);
      if (l->SpotCutoff != 180.0F)
	 rmesa->hw.tcl.cmd[idx] |= flag;
      else
	 rmesa->hw.tcl.cmd[idx] &= ~flag;

      break;
   }

   case GL_CONSTANT_ATTENUATION:
      R200_STATECHANGE(rmesa, lit[p]);
      fcmd[LIT_ATTEN_CONST] = params[0];
      break;
   case GL_LINEAR_ATTENUATION:
      R200_STATECHANGE(rmesa, lit[p]);
      fcmd[LIT_ATTEN_LINEAR] = params[0];
      break;
   case GL_QUADRATIC_ATTENUATION:
      R200_STATECHANGE(rmesa, lit[p]);
      fcmd[LIT_ATTEN_QUADRATIC] = params[0];
      break;
   default:
      return;
   }

}

		  


static void r200LightModelfv( GLcontext *ctx, GLenum pname,
				const GLfloat *param )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   switch (pname) {
      case GL_LIGHT_MODEL_AMBIENT: 
	 update_global_ambient( ctx );
	 break;

      case GL_LIGHT_MODEL_LOCAL_VIEWER:
	 R200_STATECHANGE( rmesa, tcl );
	 if (ctx->Light.Model.LocalViewer)
	    rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |= R200_LOCAL_VIEWER;
	 else
	    rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= ~R200_LOCAL_VIEWER;
         break;

      case GL_LIGHT_MODEL_TWO_SIDE:
	 R200_STATECHANGE( rmesa, tcl );
	 if (ctx->Light.Model.TwoSide)
	    rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |= R200_LIGHT_TWOSIDE;
	 else
	    rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= ~R200_LIGHT_TWOSIDE;

	 check_twoside_fallback( ctx );

	 if (rmesa->TclFallback) {
	    r200ChooseRenderState( ctx );
	    r200ChooseVertexState( ctx );
	 }
         break;

      case GL_LIGHT_MODEL_COLOR_CONTROL:
	 r200UpdateSpecular(ctx);
         break;

      default:
         break;
   }
}

static void r200ShadeModel( GLcontext *ctx, GLenum mode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint s = rmesa->hw.set.cmd[SET_SE_CNTL];

   s &= ~(R200_DIFFUSE_SHADE_MASK |
	  R200_ALPHA_SHADE_MASK |
	  R200_SPECULAR_SHADE_MASK |
	  R200_FOG_SHADE_MASK);

   switch ( mode ) {
   case GL_FLAT:
      s |= (R200_DIFFUSE_SHADE_FLAT |
	    R200_ALPHA_SHADE_FLAT |
	    R200_SPECULAR_SHADE_FLAT |
	    R200_FOG_SHADE_FLAT);
      break;
   case GL_SMOOTH:
      s |= (R200_DIFFUSE_SHADE_GOURAUD |
	    R200_ALPHA_SHADE_GOURAUD |
	    R200_SPECULAR_SHADE_GOURAUD |
	    R200_FOG_SHADE_GOURAUD);
      break;
   default:
      return;
   }

   if ( rmesa->hw.set.cmd[SET_SE_CNTL] != s ) {
      R200_STATECHANGE( rmesa, set );
      rmesa->hw.set.cmd[SET_SE_CNTL] = s;
   }
}


/* =============================================================
 * User clip planes
 */

static void r200ClipPlane( GLcontext *ctx, GLenum plane, const GLfloat *eq )
{
   GLint p = (GLint) plane - (GLint) GL_CLIP_PLANE0;
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLint *ip = (GLint *)ctx->Transform._ClipUserPlane[p];

   R200_STATECHANGE( rmesa, ucp[p] );
   rmesa->hw.ucp[p].cmd[UCP_X] = ip[0];
   rmesa->hw.ucp[p].cmd[UCP_Y] = ip[1];
   rmesa->hw.ucp[p].cmd[UCP_Z] = ip[2];
   rmesa->hw.ucp[p].cmd[UCP_W] = ip[3];
}

static void r200UpdateClipPlanes( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint p;

   for (p = 0; p < ctx->Const.MaxClipPlanes; p++) {
      if (ctx->Transform.ClipEnabled[p]) {
	 GLint *ip = (GLint *)ctx->Transform._ClipUserPlane[p];

	 R200_STATECHANGE( rmesa, ucp[p] );
	 rmesa->hw.ucp[p].cmd[UCP_X] = ip[0];
	 rmesa->hw.ucp[p].cmd[UCP_Y] = ip[1];
	 rmesa->hw.ucp[p].cmd[UCP_Z] = ip[2];
	 rmesa->hw.ucp[p].cmd[UCP_W] = ip[3];
      }
   }
}


/* =============================================================
 * Stencil
 */

static void r200StencilFunc( GLcontext *ctx, GLenum func,
			       GLint ref, GLuint mask )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint refmask = ((ctx->Stencil.Ref << R200_STENCIL_REF_SHIFT) |
		     (ctx->Stencil.ValueMask << R200_STENCIL_MASK_SHIFT));

   R200_STATECHANGE( rmesa, ctx );
   R200_STATECHANGE( rmesa, msk );

   rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] &= ~R200_STENCIL_TEST_MASK;
   rmesa->hw.msk.cmd[MSK_RB3D_STENCILREFMASK] &= ~(R200_STENCIL_REF_MASK|
						   R200_STENCIL_VALUE_MASK);

   switch ( ctx->Stencil.Function ) {
   case GL_NEVER:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_NEVER;
      break;
   case GL_LESS:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_LESS;
      break;
   case GL_EQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_EQUAL;
      break;
   case GL_LEQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_LEQUAL;
      break;
   case GL_GREATER:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_GREATER;
      break;
   case GL_NOTEQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_NEQUAL;
      break;
   case GL_GEQUAL:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_GEQUAL;
      break;
   case GL_ALWAYS:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_TEST_ALWAYS;
      break;
   }

   rmesa->hw.msk.cmd[MSK_RB3D_STENCILREFMASK] |= refmask;
}

static void r200StencilMask( GLcontext *ctx, GLuint mask )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   R200_STATECHANGE( rmesa, msk );
   rmesa->hw.msk.cmd[MSK_RB3D_STENCILREFMASK] &= ~R200_STENCIL_WRITE_MASK;
   rmesa->hw.msk.cmd[MSK_RB3D_STENCILREFMASK] |=
      (ctx->Stencil.WriteMask << R200_STENCIL_WRITEMASK_SHIFT);
}

static void r200StencilOp( GLcontext *ctx, GLenum fail,
			     GLenum zfail, GLenum zpass )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   R200_STATECHANGE( rmesa, ctx );
   rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] &= ~(R200_STENCIL_FAIL_MASK |
					       R200_STENCIL_ZFAIL_MASK |
					       R200_STENCIL_ZPASS_MASK);

   switch ( ctx->Stencil.FailFunc ) {
   case GL_KEEP:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_KEEP;
      break;
   case GL_ZERO:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_ZERO;
      break;
   case GL_REPLACE:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_REPLACE;
      break;
   case GL_INCR:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_INC;
      break;
   case GL_DECR:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_DEC;
      break;
   case GL_INCR_WRAP_EXT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_INC_WRAP;
      break;
   case GL_DECR_WRAP_EXT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_DEC_WRAP;
      break;
   case GL_INVERT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_FAIL_INVERT;
      break;
   }

   switch ( ctx->Stencil.ZFailFunc ) {
   case GL_KEEP:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_KEEP;
      break;
   case GL_ZERO:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_ZERO;
      break;
   case GL_REPLACE:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_REPLACE;
      break;
   case GL_INCR:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_INC;
      break;
   case GL_DECR:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_DEC;
      break;
   case GL_INCR_WRAP_EXT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_INC_WRAP;
      break;
   case GL_DECR_WRAP_EXT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_DEC_WRAP;
      break;
   case GL_INVERT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZFAIL_INVERT;
      break;
   }

   switch ( ctx->Stencil.ZPassFunc ) {
   case GL_KEEP:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_KEEP;
      break;
   case GL_ZERO:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_ZERO;
      break;
   case GL_REPLACE:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_REPLACE;
      break;
   case GL_INCR:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_INC;
      break;
   case GL_DECR:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_DEC;
      break;
   case GL_INCR_WRAP_EXT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_INC_WRAP;
      break;
   case GL_DECR_WRAP_EXT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_DEC_WRAP;
      break;
   case GL_INVERT:
      rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] |= R200_STENCIL_ZPASS_INVERT;
      break;
   }
}

static void r200ClearStencil( GLcontext *ctx, GLint s )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   rmesa->state.stencil.clear = 
      ((GLuint) ctx->Stencil.Clear |
       (0xff << R200_STENCIL_MASK_SHIFT) |
       (ctx->Stencil.WriteMask << R200_STENCIL_WRITEMASK_SHIFT));
}


/* =============================================================
 * Window position and viewport transformation
 */

/*
 * To correctly position primitives:
 */
#define SUBPIXEL_X 0.125
#define SUBPIXEL_Y 0.125

void r200UpdateWindow( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   __DRIdrawablePrivate *dPriv = rmesa->dri.drawable;
   GLfloat xoffset = (GLfloat)dPriv->x;
   GLfloat yoffset = (GLfloat)dPriv->y + dPriv->h;
   const GLfloat *v = ctx->Viewport._WindowMap.m;

   GLfloat sx = v[MAT_SX];
   GLfloat tx = v[MAT_TX] + xoffset + SUBPIXEL_X;
   GLfloat sy = - v[MAT_SY];
   GLfloat ty = (- v[MAT_TY]) + yoffset + SUBPIXEL_Y;
   GLfloat sz = v[MAT_SZ] * rmesa->state.depth.scale;
   GLfloat tz = v[MAT_TZ] * rmesa->state.depth.scale;

   R200_FIREVERTICES( rmesa );
   R200_STATECHANGE( rmesa, vpt );

   rmesa->hw.vpt.cmd[VPT_SE_VPORT_XSCALE]  = *(GLuint *)&sx;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_XOFFSET] = *(GLuint *)&tx;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_YSCALE]  = *(GLuint *)&sy;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_YOFFSET] = *(GLuint *)&ty;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_ZSCALE]  = *(GLuint *)&sz;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_ZOFFSET] = *(GLuint *)&tz;
}



static void r200Viewport( GLcontext *ctx, GLint x, GLint y,
			    GLsizei width, GLsizei height )
{
   /* Don't pipeline viewport changes, conflict with window offset
    * setting below.  Could apply deltas to rescue pipelined viewport
    * values, or keep the originals hanging around.
    */
   R200_FIREVERTICES( R200_CONTEXT(ctx) );
   r200UpdateWindow( ctx );
}

static void r200DepthRange( GLcontext *ctx, GLclampd nearval,
			      GLclampd farval )
{
   r200UpdateWindow( ctx );
}

void r200UpdateViewportOffset( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   __DRIdrawablePrivate *dPriv = rmesa->dri.drawable;
   GLfloat xoffset = (GLfloat)dPriv->x;
   GLfloat yoffset = (GLfloat)dPriv->y + dPriv->h;
   const GLfloat *v = ctx->Viewport._WindowMap.m;

   GLfloat tx = v[MAT_TX] + xoffset;
   GLfloat ty = (- v[MAT_TY]) + yoffset;

   if ( rmesa->hw.vpt.cmd[VPT_SE_VPORT_XOFFSET] != *(GLuint *)&tx ||
	rmesa->hw.vpt.cmd[VPT_SE_VPORT_YOFFSET] != *(GLuint *)&ty )
   {
      /* Note: this should also modify whatever data the context reset
       * code uses...
       */
      rmesa->hw.vpt.cmd[VPT_SE_VPORT_XOFFSET] = *(GLuint *)&tx;
      rmesa->hw.vpt.cmd[VPT_SE_VPORT_YOFFSET] = *(GLuint *)&ty;
      
      /* update polygon stipple x/y screen offset */
      {
         GLuint stx, sty;
         GLuint m = rmesa->hw.msc.cmd[MSC_RE_MISC];

         m &= ~(R200_STIPPLE_X_OFFSET_MASK |
                R200_STIPPLE_Y_OFFSET_MASK);

         /* add magic offsets, then invert */
         stx = 31 - ((rmesa->dri.drawable->x - 1) & R200_STIPPLE_COORD_MASK);
         sty = 31 - ((rmesa->dri.drawable->y + rmesa->dri.drawable->h - 1)
                     & R200_STIPPLE_COORD_MASK);

         m |= ((stx << R200_STIPPLE_X_OFFSET_SHIFT) |
               (sty << R200_STIPPLE_Y_OFFSET_SHIFT));

         if ( rmesa->hw.msc.cmd[MSC_RE_MISC] != m ) {
            R200_STATECHANGE( rmesa, msc );
	    rmesa->hw.msc.cmd[MSC_RE_MISC] = m;
         }
      }
   }

   r200UpdateScissor( ctx );
}



/* =============================================================
 * Miscellaneous
 */

static void r200ClearColor( GLcontext *ctx, const GLchan c[4] )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   rmesa->state.color.clear = r200PackColor( rmesa->r200Screen->cpp,
					       c[0], c[1], c[2], c[3] );
}


static void r200RenderMode( GLcontext *ctx, GLenum mode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   FALLBACK( rmesa, R200_FALLBACK_RENDER_MODE, (mode != GL_RENDER) );
}


static GLuint r200_rop_tab[] = {
   R200_ROP_CLEAR,
   R200_ROP_AND,
   R200_ROP_AND_REVERSE,
   R200_ROP_COPY,
   R200_ROP_AND_INVERTED,
   R200_ROP_NOOP,
   R200_ROP_XOR,
   R200_ROP_OR,
   R200_ROP_NOR,
   R200_ROP_EQUIV,
   R200_ROP_INVERT,
   R200_ROP_OR_REVERSE,
   R200_ROP_COPY_INVERTED,
   R200_ROP_OR_INVERTED,
   R200_ROP_NAND,
   R200_ROP_SET,
};

static void r200LogicOpCode( GLcontext *ctx, GLenum opcode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint rop = (GLuint)opcode - GL_CLEAR;

   ASSERT( rop < 16 );

   R200_STATECHANGE( rmesa, msk );
   rmesa->hw.msk.cmd[MSK_RB3D_ROPCNTL] = r200_rop_tab[rop];
}


void r200SetCliprects( r200ContextPtr rmesa, GLenum mode )
{
   __DRIdrawablePrivate *dPriv = rmesa->dri.drawable;

   switch ( mode ) {
   case GL_FRONT_LEFT:
      rmesa->numClipRects = dPriv->numClipRects;
      rmesa->pClipRects = (XF86DRIClipRectPtr)dPriv->pClipRects;
      break;
   case GL_BACK_LEFT:
      /* Can't ignore 2d windows if we are page flipping.
       */
      if ( dPriv->numBackClipRects == 0 || rmesa->doPageFlip ) {
	 rmesa->numClipRects = dPriv->numClipRects;
	 rmesa->pClipRects = (XF86DRIClipRectPtr)dPriv->pClipRects;
      }
      else {
	 rmesa->numClipRects = dPriv->numBackClipRects;
	 rmesa->pClipRects = (XF86DRIClipRectPtr)dPriv->pBackClipRects;
      }
      break;
   default:
      fprintf(stderr, "bad mode in r200SetCliprects\n");
      return;
   }

   if (rmesa->state.scissor.enabled)
      r200RecalcScissorRects( rmesa );
}


static void r200SetDrawBuffer( GLcontext *ctx, GLenum mode )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);

   if (R200_DEBUG & DEBUG_DRI)
      fprintf(stderr, "%s %s\n", __FUNCTION__,
	      _mesa_lookup_enum_by_nr( mode ));

   R200_FIREVERTICES(rmesa);	/* don't pipeline cliprect changes */

   switch ( mode ) {
   case GL_FRONT_LEFT:
      FALLBACK( rmesa, R200_FALLBACK_DRAW_BUFFER, GL_FALSE );
      if ( rmesa->doPageFlip && rmesa->sarea->pfCurrentPage == 1 ) {
        rmesa->state.color.drawOffset = rmesa->r200Screen->backOffset;
        rmesa->state.color.drawPitch  = rmesa->r200Screen->backPitch;
      } else {
      	rmesa->state.color.drawOffset = rmesa->r200Screen->frontOffset;
      	rmesa->state.color.drawPitch  = rmesa->r200Screen->frontPitch;
      }
      r200SetCliprects( rmesa, GL_FRONT_LEFT );
      break;
   case GL_BACK_LEFT:
      FALLBACK( rmesa, R200_FALLBACK_DRAW_BUFFER, GL_FALSE );
      if ( rmesa->doPageFlip && rmesa->sarea->pfCurrentPage == 1 ) {
      	rmesa->state.color.drawOffset = rmesa->r200Screen->frontOffset;
      	rmesa->state.color.drawPitch  = rmesa->r200Screen->frontPitch;
      } else {
        rmesa->state.color.drawOffset = rmesa->r200Screen->backOffset;
        rmesa->state.color.drawPitch  = rmesa->r200Screen->backPitch;
      }
      r200SetCliprects( rmesa, GL_BACK_LEFT );
      break;
   default:
      FALLBACK( rmesa, R200_FALLBACK_DRAW_BUFFER, GL_TRUE );
      return;
   }

   R200_STATECHANGE( rmesa, ctx );
   rmesa->hw.ctx.cmd[CTX_RB3D_COLOROFFSET] = (rmesa->state.color.drawOffset &
					    R200_COLOROFFSET_MASK);
   rmesa->hw.ctx.cmd[CTX_RB3D_COLORPITCH] = rmesa->state.color.drawPitch;
}


/* =============================================================
 * State enable/disable
 */

static void r200Enable( GLcontext *ctx, GLenum cap, GLboolean state )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint p, flag;

   if ( R200_DEBUG & DEBUG_STATE )
      fprintf( stderr, "%s( %s = %s )\n", __FUNCTION__,
	       _mesa_lookup_enum_by_nr( cap ),
	       state ? "GL_TRUE" : "GL_FALSE" );

   switch ( cap ) {
      /* Fast track this one...
       */
   case GL_TEXTURE_1D:
   case GL_TEXTURE_2D:
   case GL_TEXTURE_3D:
      break;

   case GL_ALPHA_TEST:
      R200_STATECHANGE( rmesa, ctx );
      if (state) {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] |= R200_ALPHA_TEST_ENABLE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] &= ~R200_ALPHA_TEST_ENABLE;
      }
      break;

   case GL_BLEND:
      R200_STATECHANGE( rmesa, ctx );
      if (state) {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_ALPHA_BLEND_ENABLE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_ALPHA_BLEND_ENABLE;
      }
      if ( ctx->Color.ColorLogicOpEnabled ) {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_ROP_ENABLE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_ROP_ENABLE;
      }
      break;

   case GL_CLIP_PLANE0:
   case GL_CLIP_PLANE1:
   case GL_CLIP_PLANE2:
   case GL_CLIP_PLANE3:
   case GL_CLIP_PLANE4:
   case GL_CLIP_PLANE5: 
      p = cap-GL_CLIP_PLANE0;
      R200_STATECHANGE( rmesa, tcl );
      if (state) {
	 rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] |= (R200_UCP_ENABLE_0<<p);
	 r200ClipPlane( ctx, cap, NULL );
      }
      else {
	 rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] &= ~(R200_UCP_ENABLE_0<<p);
      }
      break;

   case GL_COLOR_MATERIAL:
      r200ColorMaterial( ctx, 0, 0 );
      if (!state) 
	 r200UpdateMaterial( ctx );
      break;

   case GL_CULL_FACE:
      r200CullFace( ctx, 0 );
      break;

   case GL_DEPTH_TEST:
      R200_STATECHANGE(rmesa, ctx );
      if ( state ) {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_Z_ENABLE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_Z_ENABLE;
      }
      break;

   case GL_DITHER:
      R200_STATECHANGE(rmesa, ctx );
      if ( state ) {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_DITHER_ENABLE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_DITHER_ENABLE;
      }
      break;

   case GL_FOG:
      R200_STATECHANGE(rmesa, ctx );
      if ( state ) {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] |= R200_FOG_ENABLE;
	 r200Fogfv( ctx, GL_FOG_MODE, 0 );
      } else {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] &= ~R200_FOG_ENABLE;
	 R200_STATECHANGE(rmesa, tcl);
	 rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] &= ~R200_TCL_FOG_MASK;
      }
      r200UpdateSpecular( ctx ); /* for PK_SPEC */
      break;

   case GL_LIGHT0:
   case GL_LIGHT1:
   case GL_LIGHT2:
   case GL_LIGHT3:
   case GL_LIGHT4:
   case GL_LIGHT5:
   case GL_LIGHT6:
   case GL_LIGHT7:
      R200_STATECHANGE(rmesa, tcl);
      p = cap - GL_LIGHT0;
      if (p&1) 
	 flag = (R200_LIGHT_1_ENABLE |
		 R200_LIGHT_1_ENABLE_AMBIENT | 
		 R200_LIGHT_1_ENABLE_SPECULAR);
      else
	 flag = (R200_LIGHT_0_ENABLE |
		 R200_LIGHT_0_ENABLE_AMBIENT | 
		 R200_LIGHT_0_ENABLE_SPECULAR);

      if (state)
	 rmesa->hw.tcl.cmd[p/2 + TCL_PER_LIGHT_CTL_0] |= flag;
      else
	 rmesa->hw.tcl.cmd[p/2 + TCL_PER_LIGHT_CTL_0] &= ~flag;

      /* 
       */
      update_light_colors( ctx, p );
      break;

   case GL_LIGHTING:
      r200UpdateSpecular(ctx);
      check_twoside_fallback( ctx );
      break;

   case GL_LINE_SMOOTH:
      R200_STATECHANGE( rmesa, ctx );
      if ( state ) {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] |=  R200_ANTI_ALIAS_LINE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] &= ~R200_ANTI_ALIAS_LINE;
      }
      break;

   case GL_LINE_STIPPLE:
      R200_STATECHANGE( rmesa, set );
      if ( state ) {
	 rmesa->hw.set.cmd[SET_RE_CNTL] |=  R200_PATTERN_ENABLE;
      } else {
	 rmesa->hw.set.cmd[SET_RE_CNTL] &= ~R200_PATTERN_ENABLE;
      }
      break;

   case GL_COLOR_LOGIC_OP:
      R200_STATECHANGE( rmesa, ctx );
      if ( state ) {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_ROP_ENABLE;
      } else {
	 rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_ROP_ENABLE;
      }
      break;
      
   case GL_NORMALIZE:
      R200_STATECHANGE( rmesa, tcl );
      if ( state ) {
	 rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |=  R200_NORMALIZE_NORMALS;
      } else {
	 rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= ~R200_NORMALIZE_NORMALS;
      }
      break;

      /* Pointsize registers on r200 don't seem to do anything.  Maybe
       * have to pass pointsizes as vertex parameters?  In any case,
       * setting pointmin == pointsizemax == 1.0, and doing nothing
       * for aa is enough to satisfy conform.
       */
   case GL_POINT_SMOOTH:
      break;

      /* These don't really do anything, as we don't use the 3vtx
       * primitives yet.
       */
#if 0
   case GL_POLYGON_OFFSET_POINT:
      R200_STATECHANGE( rmesa, set );
      if ( state ) {
	 rmesa->hw.set.cmd[SET_SE_CNTL] |=  R200_ZBIAS_ENABLE_POINT;
      } else {
	 rmesa->hw.set.cmd[SET_SE_CNTL] &= ~R200_ZBIAS_ENABLE_POINT;
      }
      break;

   case GL_POLYGON_OFFSET_LINE:
      R200_STATECHANGE( rmesa, set );
      if ( state ) {
	 rmesa->hw.set.cmd[SET_SE_CNTL] |=  R200_ZBIAS_ENABLE_LINE;
      } else {
	 rmesa->hw.set.cmd[SET_SE_CNTL] &= ~R200_ZBIAS_ENABLE_LINE;
      }
      break;
#endif

   case GL_POLYGON_OFFSET_FILL:
      R200_STATECHANGE( rmesa, set );
      if ( state ) {
	 rmesa->hw.set.cmd[SET_SE_CNTL] |=  R200_ZBIAS_ENABLE_TRI;
      } else {
	 rmesa->hw.set.cmd[SET_SE_CNTL] &= ~R200_ZBIAS_ENABLE_TRI;
      }
      break;

   case GL_POLYGON_SMOOTH:
      R200_STATECHANGE( rmesa, ctx );
      if ( state ) {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] |=  R200_ANTI_ALIAS_POLY;
      } else {
	 rmesa->hw.ctx.cmd[CTX_PP_CNTL] &= ~R200_ANTI_ALIAS_POLY;
      }
      break;

   case GL_POLYGON_STIPPLE:
      R200_STATECHANGE(rmesa, set );
      if ( state ) {
	 rmesa->hw.set.cmd[SET_RE_CNTL] |=  R200_STIPPLE_ENABLE;
      } else {
	 rmesa->hw.set.cmd[SET_RE_CNTL] &= ~R200_STIPPLE_ENABLE;
      }
      break;

   case GL_RESCALE_NORMAL_EXT: {
      GLboolean tmp = ctx->_NeedEyeCoords ? state : !state;
      R200_STATECHANGE( rmesa, tcl );
      if ( tmp ) {
	 rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |=  R200_RESCALE_NORMALS;
      } else {
	 rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= ~R200_RESCALE_NORMALS;
      }
      break;
   }

   case GL_SCISSOR_TEST:
      R200_FIREVERTICES( rmesa );
      rmesa->state.scissor.enabled = state;
      r200UpdateScissor( ctx );
      break;

   case GL_STENCIL_TEST:
      if ( rmesa->state.stencil.hwBuffer ) {
	 R200_STATECHANGE( rmesa, ctx );
	 if ( state ) {
	    rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_STENCIL_ENABLE;
	 } else {
	    rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] &= ~R200_STENCIL_ENABLE;
	 }
      } else {
	 FALLBACK( rmesa, R200_FALLBACK_STENCIL, state );
      }
      break;

   case GL_TEXTURE_GEN_Q:
   case GL_TEXTURE_GEN_R:
   case GL_TEXTURE_GEN_S:
   case GL_TEXTURE_GEN_T:
      /* Picked up in r200UpdateTextureState.
       */
      rmesa->recheck_texgen[ctx->Texture.CurrentUnit] = GL_TRUE; 
      break;

   case GL_COLOR_SUM_EXT:
      r200UpdateSpecular ( ctx );
      break;

   default:
      return;
   }
}


void r200LightingSpaceChange( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLboolean tmp;

   if (R200_DEBUG & DEBUG_STATE) 
      fprintf(stderr, "%s %d BEFORE %x\n", __FUNCTION__, ctx->_NeedEyeCoords,
	      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0]);

   if (ctx->_NeedEyeCoords)
      tmp = ctx->Transform.RescaleNormals;
   else
      tmp = !ctx->Transform.RescaleNormals;

   R200_STATECHANGE( rmesa, tcl );
   if ( tmp ) {
      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] |=  R200_RESCALE_NORMALS;
   } else {
      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] &= ~R200_RESCALE_NORMALS;
   }

   if (R200_DEBUG & DEBUG_STATE) 
      fprintf(stderr, "%s %d AFTER %x\n", __FUNCTION__, ctx->_NeedEyeCoords,
	      rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0]);
}

/* =============================================================
 * Deferred state management - matrices, textures, other?
 */




static void upload_matrix( r200ContextPtr rmesa, GLfloat *src, int idx )
{
   float *dest = ((float *)R200_DB_STATE( mat[idx] ))+MAT_ELT_0;
   int i;


   for (i = 0 ; i < 4 ; i++) {
      *dest++ = src[i];
      *dest++ = src[i+4];
      *dest++ = src[i+8];
      *dest++ = src[i+12];
   }

   R200_DB_STATECHANGE( rmesa, &rmesa->hw.mat[idx] );
}

static void upload_matrix_t( r200ContextPtr rmesa, GLfloat *src, int idx )
{
   float *dest = ((float *)R200_DB_STATE( mat[idx] ))+MAT_ELT_0;
   memcpy(dest, src, 16*sizeof(float));
   R200_DB_STATECHANGE( rmesa, &rmesa->hw.mat[idx] );
}


static void update_texturematrix( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT( ctx );
   GLuint tpc = rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_0];
   GLuint compsel = rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL];
   int unit;

   if (R200_DEBUG & DEBUG_STATE) 
      fprintf(stderr, "%s before COMPSEL: %x\n", __FUNCTION__,
	      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL]);

   rmesa->TexMatEnabled = 0;
   rmesa->TexMatCompSel = 0;

   for (unit = 0 ; unit < 2; unit++) {
      if (!ctx->Texture.Unit[unit]._ReallyEnabled) 
	 continue;

      if (ctx->TextureMatrix[unit].type != MATRIX_IDENTITY) {
	 rmesa->TexMatEnabled |= (R200_TEXGEN_TEXMAT_0_ENABLE|
				  R200_TEXMAT_0_ENABLE) << unit;

	 rmesa->TexMatCompSel |= R200_OUTPUT_TEX_0 << unit;

	 if (rmesa->TexGenEnabled & (R200_TEXMAT_0_ENABLE << unit)) {
	    /* Need to preconcatenate any active texgen 
	     * obj/eyeplane matrices:
	     */
	    _math_matrix_mul_matrix( &rmesa->tmpmat, 
				     &rmesa->TexGenMatrix[unit],
				     &ctx->TextureMatrix[unit] );
	    upload_matrix( rmesa, rmesa->tmpmat.m, R200_MTX_TEX0+unit );
	 } 
	 else {
	    upload_matrix( rmesa, ctx->TextureMatrix[unit].m, 
			   R200_MTX_TEX0+unit );
	 }
      }
      else if (rmesa->TexGenEnabled & (R200_TEXMAT_0_ENABLE << unit)) {
	 upload_matrix( rmesa, rmesa->TexGenMatrix[unit].m, 
			R200_MTX_TEX0+unit );
      }
   }

   tpc = (rmesa->TexMatEnabled | rmesa->TexGenEnabled);
   if (tpc != rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_0] ||
       rmesa->TexGenInputs != rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_1]) {
      R200_STATECHANGE(rmesa, tcg);
      rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_0] = tpc;
      rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_1] = rmesa->TexGenInputs;
   }

   compsel &= ~R200_OUTPUT_TEX_MASK;
   compsel |= rmesa->TexMatCompSel | rmesa->TexGenCompSel;
   if (compsel != rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL]) {
      R200_STATECHANGE(rmesa, vtx);
      rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] = compsel;
   }
}



void r200ValidateState( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   GLuint new_state = rmesa->NewGLState;

   if (new_state & _NEW_TEXTURE) {
      r200UpdateTextureState( ctx );
      new_state |= rmesa->NewGLState; /* may add TEXTURE_MATRIX */
   }

   /* Need an event driven matrix update?
    */
   if (new_state & (_NEW_MODELVIEW|_NEW_PROJECTION)) 
      upload_matrix( rmesa, ctx->_ModelProjectMatrix.m, R200_MTX_MVP );

   /* Need these for lighting (shouldn't upload otherwise)
    */
   if (new_state & (_NEW_MODELVIEW)) {
      upload_matrix( rmesa, ctx->ModelView.m, R200_MTX_MV );
      upload_matrix_t( rmesa, ctx->ModelView.inv, R200_MTX_IMV );
   }

   /* Does this need to be triggered on eg. modelview for
    * texgen-derived objplane/eyeplane matrices?
    */
   if (new_state & (_NEW_TEXTURE|_NEW_TEXTURE_MATRIX)) {
      update_texturematrix( ctx );
   }      

   if (new_state & (_NEW_LIGHT|_NEW_MODELVIEW|_MESA_NEW_NEED_EYE_COORDS)) {
      update_light( ctx );
   }

   /* emit all active clip planes if projection matrix changes.
    */
   if (new_state & (_NEW_PROJECTION)) {
      if (ctx->Transform._AnyClip) 
	 r200UpdateClipPlanes( ctx );
   }


   rmesa->NewGLState = 0;
}


static void r200InvalidateState( GLcontext *ctx, GLuint new_state )
{
   _swrast_InvalidateState( ctx, new_state );
   _swsetup_InvalidateState( ctx, new_state );
   _ac_InvalidateState( ctx, new_state );
   _tnl_InvalidateState( ctx, new_state );
   _ae_invalidate_state( ctx, new_state );
   R200_CONTEXT(ctx)->NewGLState |= new_state;
   r200VtxfmtInvalidate( ctx );
}

static void r200WrapRunPipeline( GLcontext *ctx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   TNLcontext *tnl = TNL_CONTEXT(ctx);

   if (0)
      fprintf(stderr, "%s, newstate: %x\n", __FUNCTION__, rmesa->NewGLState);

   /* Validate state:
    */
   if (rmesa->NewGLState)
      r200ValidateState( ctx );

   if (tnl->vb.Material) {
      TCL_FALLBACK( ctx, R200_TCL_FALLBACK_MATERIAL, GL_TRUE );
   }

   /* Run the pipeline.
    */ 
   _tnl_run_pipeline( ctx );

   if (tnl->vb.Material) {
      TCL_FALLBACK( ctx, R200_TCL_FALLBACK_MATERIAL, GL_FALSE );
      r200UpdateMaterial( ctx ); /* not needed any more? */
   }
}


/* Initialize the driver's state functions.
 */
void r200InitStateFuncs( GLcontext *ctx )
{
   ctx->Driver.UpdateState		= r200InvalidateState;
   ctx->Driver.LightingSpaceChange      = r200LightingSpaceChange;

   ctx->Driver.SetDrawBuffer		= r200SetDrawBuffer;

   ctx->Driver.AlphaFunc		= r200AlphaFunc;
   ctx->Driver.BlendEquation		= r200BlendEquation;
   ctx->Driver.BlendFunc		= r200BlendFunc;
   ctx->Driver.BlendFuncSeparate	= r200BlendFuncSeparate;
   ctx->Driver.ClearColor		= r200ClearColor;
   ctx->Driver.ClearDepth		= NULL;
   ctx->Driver.ClearIndex		= NULL;
   ctx->Driver.ClearStencil		= r200ClearStencil;
   ctx->Driver.ClipPlane		= r200ClipPlane;
   ctx->Driver.ColorMask		= r200ColorMask;
   ctx->Driver.CullFace			= r200CullFace;
   ctx->Driver.DepthFunc		= r200DepthFunc;
   ctx->Driver.DepthMask		= r200DepthMask;
   ctx->Driver.DepthRange		= r200DepthRange;
   ctx->Driver.Enable			= r200Enable;
   ctx->Driver.Fogfv			= r200Fogfv;
   ctx->Driver.FrontFace		= r200FrontFace;
   ctx->Driver.Hint			= NULL;
   ctx->Driver.IndexMask		= NULL;
   ctx->Driver.LightModelfv		= r200LightModelfv;
   ctx->Driver.Lightfv			= r200Lightfv;
   ctx->Driver.LineStipple              = r200LineStipple;
   ctx->Driver.LineWidth                = r200LineWidth;
   ctx->Driver.LogicOpcode		= r200LogicOpCode;
   ctx->Driver.PolygonMode		= r200PolygonMode;
   ctx->Driver.PolygonOffset		= r200PolygonOffset;
   ctx->Driver.PolygonStipple		= r200PolygonStipple;
   ctx->Driver.PointSize                = r200PointSize;
   ctx->Driver.RenderMode		= r200RenderMode;
   ctx->Driver.Scissor			= r200Scissor;
   ctx->Driver.ShadeModel		= r200ShadeModel;
   ctx->Driver.StencilFunc		= r200StencilFunc;
   ctx->Driver.StencilMask		= r200StencilMask;
   ctx->Driver.StencilOp		= r200StencilOp;
   ctx->Driver.Viewport			= r200Viewport;

   /* Swrast hooks for imaging extensions:
    */
   ctx->Driver.CopyColorTable		= _swrast_CopyColorTable;
   ctx->Driver.CopyColorSubTable	= _swrast_CopyColorSubTable;
   ctx->Driver.CopyConvolutionFilter1D	= _swrast_CopyConvolutionFilter1D;
   ctx->Driver.CopyConvolutionFilter2D	= _swrast_CopyConvolutionFilter2D;

   TNL_CONTEXT(ctx)->Driver.NotifyMaterialChange = r200UpdateMaterial;
   TNL_CONTEXT(ctx)->Driver.RunPipeline = r200WrapRunPipeline;
}
