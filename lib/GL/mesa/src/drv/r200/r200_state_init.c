/* $XFree86: xc/lib/GL/mesa/src/drv/r200/r200_state_init.c,v 1.3 2003/01/29 23:00:41 dawes Exp $ */
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
 * State initialization
 */

void r200PrintDirty( r200ContextPtr rmesa, const char *msg )
{
   struct r200_state_atom *l;

   fprintf(stderr, msg);
   fprintf(stderr, ": ");

   foreach(l, &(rmesa->hw.dirty)) {
      fprintf(stderr, "%s, ", l->name);
   }

   fprintf(stderr, "\n");
}

static int cmdpkt( int id ) 
{
   drmRadeonCmdHeader h;
   h.i = 0;
   h.packet.cmd_type = RADEON_CMD_PACKET;
   h.packet.packet_id = id;
   return h.i;
}

static int cmdvec( int offset, int stride, int count ) 
{
   drmRadeonCmdHeader h;
   h.i = 0;
   h.vectors.cmd_type = RADEON_CMD_VECTORS;
   h.vectors.offset = offset;
   h.vectors.stride = stride;
   h.vectors.count = count;
   return h.i;
}

static int cmdscl( int offset, int stride, int count ) 
{
   drmRadeonCmdHeader h;
   h.i = 0;
   h.scalars.cmd_type = RADEON_CMD_SCALARS;
   h.scalars.offset = offset;
   h.scalars.stride = stride;
   h.scalars.count = count;
   return h.i;
}

static int cmdscl2( int offset, int stride, int count ) 
{
   drmRadeonCmdHeader h;
   h.i = 0;
   h.scalars.cmd_type = RADEON_CMD_SCALARS2;
   h.scalars.offset = offset - 0x100;
   h.scalars.stride = stride;
   h.scalars.count = count;
   return h.i;
}

#define CHECK( NM, FLAG )				\
static GLboolean check_##NM( GLcontext *ctx, int idx )	\
{							\
   (void) idx;						\
   return FLAG;						\
}

#define TCL_CHECK( NM, FLAG )				\
static GLboolean check_##NM( GLcontext *ctx, int idx )	\
{							\
   r200ContextPtr rmesa = R200_CONTEXT(ctx);		\
   (void) idx;						\
   return !rmesa->TclFallback && (FLAG);		\
}



CHECK( always, GL_TRUE )
CHECK( tex_any, ctx->Texture._ReallyEnabled )
CHECK( tex, ctx->Texture.Unit[idx]._ReallyEnabled )
CHECK( fog, ctx->Fog.Enabled )
TCL_CHECK( tcl, GL_TRUE )
TCL_CHECK( tcl_tex_any, ctx->Texture._ReallyEnabled )
TCL_CHECK( tcl_tex, ctx->Texture.Unit[idx]._ReallyEnabled )
TCL_CHECK( tcl_lighting, ctx->Light.Enabled )
TCL_CHECK( tcl_eyespace_or_lighting, ctx->_NeedEyeCoords || ctx->Light.Enabled )
TCL_CHECK( tcl_light, ctx->Light.Enabled && ctx->Light.Light[idx].Enabled )
TCL_CHECK( tcl_ucp, ctx->Transform.ClipEnabled[idx] )
/* TCL_CHECK( tcl_eyespace_or_fog, ctx->_NeedEyeCoords || ctx->Fog.Enabled )  */


static GLboolean check_tcl_eyespace_or_fog( GLcontext *ctx, int idx )
{
   r200ContextPtr rmesa = R200_CONTEXT(ctx);
   int res;
   (void) idx;
   res = !rmesa->TclFallback && (ctx->_NeedEyeCoords || ctx->Fog.Enabled);
   fprintf(stderr, "%s: %d\n", __FUNCTION__, res);
   return res;
}


/* Initialize the context's hardware state.
 */
void r200InitState( r200ContextPtr rmesa )
{
   GLcontext *ctx = rmesa->glCtx;
   GLuint color_fmt, depth_fmt, i;

   switch ( rmesa->r200Screen->cpp ) {
   case 2:
      color_fmt = R200_COLOR_FORMAT_RGB565;
      break;
   case 4:
      color_fmt = R200_COLOR_FORMAT_ARGB8888;
      break;
   default:
      fprintf( stderr, "Error: Unsupported pixel depth... exiting\n" );
      exit( -1 );
   }

   rmesa->state.color.clear = 0x00000000;

   switch ( ctx->Visual.depthBits ) {
   case 16:
      rmesa->state.depth.scale = 1.0 / (GLfloat)0xffff;
      depth_fmt = R200_DEPTH_FORMAT_16BIT_INT_Z;
      rmesa->state.stencil.clear = 0x00000000;
      break;
   case 24:
      rmesa->state.depth.scale = 1.0 / (GLfloat)0xffffff;
      depth_fmt = R200_DEPTH_FORMAT_24BIT_INT_Z;
      rmesa->state.stencil.clear = 0xff000000;
      break;
   default:
      fprintf( stderr, "Error: Unsupported depth %d... exiting\n",
	       ctx->Visual.depthBits );
      exit( -1 );
   }

   /* Only have hw stencil when depth buffer is 24 bits deep */
   rmesa->state.stencil.hwBuffer = ( ctx->Visual.stencilBits > 0 &&
				     ctx->Visual.depthBits == 24 );

   rmesa->Fallback = 0;

   if ( ctx->Visual.doubleBufferMode && rmesa->sarea->pfCurrentPage == 0 ) {
      rmesa->state.color.drawOffset = rmesa->r200Screen->backOffset;
      rmesa->state.color.drawPitch  = rmesa->r200Screen->backPitch;
   } else {
      rmesa->state.color.drawOffset = rmesa->r200Screen->frontOffset;
      rmesa->state.color.drawPitch  = rmesa->r200Screen->frontPitch;
   }

   rmesa->state.pixel.readOffset = rmesa->state.color.drawOffset;
   rmesa->state.pixel.readPitch  = rmesa->state.color.drawPitch;

   /* Initialize lists:
    */
   make_empty_list(&(rmesa->hw.dirty)); rmesa->hw.dirty.name = "DIRTY";
   make_empty_list(&(rmesa->hw.clean)); rmesa->hw.clean.name = "CLEAN";


#define ALLOC_STATE( ATOM, CHK, SZ, NM, IDX )				\
   do {								\
      rmesa->hw.ATOM.cmd_size = SZ;				\
      rmesa->hw.ATOM.cmd = (int *)CALLOC(SZ * sizeof(int));	\
      rmesa->hw.ATOM.lastcmd = (int *)CALLOC(SZ * sizeof(int));	\
      rmesa->hw.ATOM.name = NM;					\
      rmesa->hw.ATOM.idx = IDX;					\
      rmesa->hw.ATOM.check = check_##CHK;				\
      insert_at_head(&(rmesa->hw.dirty), &(rmesa->hw.ATOM));	\
   } while (0)
      
      
   /* Allocate state buffers:
    */
   ALLOC_STATE( ctx, always, CTX_STATE_SIZE, "CTX/context", 0 );
   ALLOC_STATE( set, always, SET_STATE_SIZE, "SET/setup", 0 );
   ALLOC_STATE( lin, always, LIN_STATE_SIZE, "LIN/line", 0 );
   ALLOC_STATE( msk, always, MSK_STATE_SIZE, "MSK/mask", 0 );
   ALLOC_STATE( vpt, always, VPT_STATE_SIZE, "VPT/viewport", 0 );
   ALLOC_STATE( vtx, always, VTX_STATE_SIZE, "VTX/vertex", 0 );
   ALLOC_STATE( vap, always, VAP_STATE_SIZE, "VAP/vap", 0 );
   ALLOC_STATE( vte, always, VTE_STATE_SIZE, "VTE/vte", 0 );
   ALLOC_STATE( msc, always, MSC_STATE_SIZE, "MSC/misc", 0 );
   ALLOC_STATE( cst, always, CST_STATE_SIZE, "CST/constant", 0 );
   ALLOC_STATE( zbs, always, ZBS_STATE_SIZE, "ZBS/zbias", 0 );
   ALLOC_STATE( tam, tex_any, TAM_STATE_SIZE, "TAM/tam", 0 );
   ALLOC_STATE( tf, tex_any, TF_STATE_SIZE, "TF/tfactor", 0 );
   ALLOC_STATE( tex[0], tex_any, TEX_STATE_SIZE, "TEX/tex-0", 0 );
   ALLOC_STATE( tex[1], tex_any, TEX_STATE_SIZE, "TEX/tex-1", 1 );

   ALLOC_STATE( tcl, tcl, TCL_STATE_SIZE, "TCL/tcl", 0 );
   ALLOC_STATE( msl, tcl, MSL_STATE_SIZE, "MSL/matrix-select", 0 );
   ALLOC_STATE( tcg, tcl, TCG_STATE_SIZE, "TCG/texcoordgen", 0 );
   ALLOC_STATE( mtl[0], tcl_lighting, MTL_STATE_SIZE, "MTL0/material0", 0 );
   ALLOC_STATE( grd, tcl, GRD_STATE_SIZE, "GRD/guard-band", 0 );
   ALLOC_STATE( fog, fog, FOG_STATE_SIZE, "FOG/fog", 0 );
   ALLOC_STATE( glt, tcl_lighting, GLT_STATE_SIZE, "GLT/light-global", 0 );
   ALLOC_STATE( eye, tcl_lighting, EYE_STATE_SIZE, "EYE/eye-vector", 0 );
   ALLOC_STATE( mat[R200_MTX_MV], tcl, MAT_STATE_SIZE, "MAT/modelview", 0 );
   ALLOC_STATE( mat[R200_MTX_IMV], tcl, MAT_STATE_SIZE, "MAT/it-modelview", 0 );
   ALLOC_STATE( mat[R200_MTX_MVP], tcl, MAT_STATE_SIZE, "MAT/modelproject", 0 );
   ALLOC_STATE( mat[R200_MTX_TEX0], tcl_tex, MAT_STATE_SIZE, "MAT/texmat0", 0 );
   ALLOC_STATE( mat[R200_MTX_TEX1], tcl_tex, MAT_STATE_SIZE, "MAT/texmat1", 1 );
   ALLOC_STATE( ucp[0], tcl_ucp, UCP_STATE_SIZE, "UCP/userclip-0", 0 );
   ALLOC_STATE( ucp[1], tcl_ucp, UCP_STATE_SIZE, "UCP/userclip-1", 1 );
   ALLOC_STATE( ucp[2], tcl_ucp, UCP_STATE_SIZE, "UCP/userclip-2", 2 );
   ALLOC_STATE( ucp[3], tcl_ucp, UCP_STATE_SIZE, "UCP/userclip-3", 3 );
   ALLOC_STATE( ucp[4], tcl_ucp, UCP_STATE_SIZE, "UCP/userclip-4", 4 );
   ALLOC_STATE( ucp[5], tcl_ucp, UCP_STATE_SIZE, "UCP/userclip-5", 5 );
   ALLOC_STATE( lit[0], tcl_light, LIT_STATE_SIZE, "LIT/light-0", 0 );
   ALLOC_STATE( lit[1], tcl_light, LIT_STATE_SIZE, "LIT/light-1", 1 );
   ALLOC_STATE( lit[2], tcl_light, LIT_STATE_SIZE, "LIT/light-2", 2 );
   ALLOC_STATE( lit[3], tcl_light, LIT_STATE_SIZE, "LIT/light-3", 3 );
   ALLOC_STATE( lit[4], tcl_light, LIT_STATE_SIZE, "LIT/light-4", 4 );
   ALLOC_STATE( lit[5], tcl_light, LIT_STATE_SIZE, "LIT/light-5", 5 );
   ALLOC_STATE( lit[6], tcl_light, LIT_STATE_SIZE, "LIT/light-6", 6 );
   ALLOC_STATE( lit[7], tcl_light, LIT_STATE_SIZE, "LIT/light-7", 7 );
   ALLOC_STATE( pix[0], always, PIX_STATE_SIZE, "PIX/pixstage-0", 0 );
   ALLOC_STATE( pix[1], tex, PIX_STATE_SIZE, "PIX/pixstage-1", 1 );


   /* Fill in the packet headers:
    */
   rmesa->hw.ctx.cmd[CTX_CMD_0] = cmdpkt(RADEON_EMIT_PP_MISC);
   rmesa->hw.ctx.cmd[CTX_CMD_1] = cmdpkt(RADEON_EMIT_PP_CNTL);
   rmesa->hw.ctx.cmd[CTX_CMD_2] = cmdpkt(RADEON_EMIT_RB3D_COLORPITCH);
   rmesa->hw.lin.cmd[LIN_CMD_0] = cmdpkt(RADEON_EMIT_RE_LINE_PATTERN);
   rmesa->hw.lin.cmd[LIN_CMD_1] = cmdpkt(RADEON_EMIT_SE_LINE_WIDTH);
   rmesa->hw.msk.cmd[MSK_CMD_0] = cmdpkt(RADEON_EMIT_RB3D_STENCILREFMASK);
   rmesa->hw.vpt.cmd[VPT_CMD_0] = cmdpkt(RADEON_EMIT_SE_VPORT_XSCALE);
   rmesa->hw.set.cmd[SET_CMD_0] = cmdpkt(RADEON_EMIT_SE_CNTL);
   rmesa->hw.msc.cmd[MSC_CMD_0] = cmdpkt(RADEON_EMIT_RE_MISC);
   rmesa->hw.cst.cmd[CST_CMD_0] = cmdpkt(R200_EMIT_PP_CNTL_X);
   rmesa->hw.cst.cmd[CST_CMD_1] = cmdpkt(R200_EMIT_RB3D_DEPTHXY_OFFSET);
   rmesa->hw.cst.cmd[CST_CMD_2] = cmdpkt(R200_EMIT_RE_AUX_SCISSOR_CNTL);
   rmesa->hw.cst.cmd[CST_CMD_3] = cmdpkt(R200_EMIT_RE_SCISSOR_TL_0);
   rmesa->hw.cst.cmd[CST_CMD_4] = cmdpkt(R200_EMIT_SE_VAP_CNTL_STATUS);
   rmesa->hw.cst.cmd[CST_CMD_5] = cmdpkt(R200_EMIT_RE_POINTSIZE);
   rmesa->hw.cst.cmd[CST_CMD_6] = cmdpkt(R200_EMIT_TCL_INPUT_VTX_VECTOR_ADDR_0);
   rmesa->hw.tam.cmd[TAM_CMD_0] = cmdpkt(R200_EMIT_PP_TAM_DEBUG3);
   rmesa->hw.tf.cmd[TF_CMD_0]   = cmdpkt(R200_EMIT_TFACTOR_0);
   rmesa->hw.tex[0].cmd[TEX_CMD_0] = cmdpkt(R200_EMIT_PP_TXFILTER_0);
   rmesa->hw.tex[0].cmd[TEX_CMD_1] = cmdpkt(R200_EMIT_PP_TXOFFSET_0);
   rmesa->hw.tex[1].cmd[TEX_CMD_0] = cmdpkt(R200_EMIT_PP_TXFILTER_1);
   rmesa->hw.tex[1].cmd[TEX_CMD_1] = cmdpkt(R200_EMIT_PP_TXOFFSET_1);
   rmesa->hw.pix[0].cmd[PIX_CMD_0] = cmdpkt(R200_EMIT_PP_TXCBLEND_0);
   rmesa->hw.pix[1].cmd[PIX_CMD_0] = cmdpkt(R200_EMIT_PP_TXCBLEND_1);
   rmesa->hw.zbs.cmd[ZBS_CMD_0] = cmdpkt(RADEON_EMIT_SE_ZBIAS_FACTOR);
   rmesa->hw.tcl.cmd[TCL_CMD_0] = cmdpkt(R200_EMIT_TCL_LIGHT_MODEL_CTL_0);
   rmesa->hw.tcl.cmd[TCL_CMD_1] = cmdpkt(R200_EMIT_TCL_UCP_VERT_BLEND_CTL);
   rmesa->hw.tcg.cmd[TCG_CMD_0] = cmdpkt(R200_EMIT_TEX_PROC_CTL_2);
   rmesa->hw.msl.cmd[MSL_CMD_0] = cmdpkt(R200_EMIT_MATRIX_SELECT_0);
   rmesa->hw.vap.cmd[VAP_CMD_0] = cmdpkt(R200_EMIT_VAP_CTL);
   rmesa->hw.vtx.cmd[VTX_CMD_0] = cmdpkt(R200_EMIT_VTX_FMT_0);
   rmesa->hw.vtx.cmd[VTX_CMD_1] = cmdpkt(R200_EMIT_OUTPUT_VTX_COMP_SEL);
   rmesa->hw.vtx.cmd[VTX_CMD_2] = cmdpkt(R200_EMIT_SE_VTX_STATE_CNTL);
   rmesa->hw.vte.cmd[VTE_CMD_0] = cmdpkt(R200_EMIT_VTE_CNTL);
   rmesa->hw.mtl[0].cmd[MTL_CMD_0] = 
      cmdvec( R200_VS_MAT_0_EMISS, 1, 16 );
   rmesa->hw.mtl[0].cmd[MTL_CMD_1] = 
      cmdscl2( R200_SS_MAT_0_SHININESS, 1, 1 );
   rmesa->hw.grd.cmd[GRD_CMD_0] = 
      cmdscl( R200_SS_VERT_GUARD_CLIP_ADJ_ADDR, 1, 4 );
   rmesa->hw.fog.cmd[FOG_CMD_0] = 
      cmdvec( R200_VS_FOG_PARAM_ADDR, 1, 4 );
   rmesa->hw.glt.cmd[GLT_CMD_0] = 
      cmdvec( R200_VS_GLOBAL_AMBIENT_ADDR, 1, 4 );
   rmesa->hw.eye.cmd[EYE_CMD_0] = 
      cmdvec( R200_VS_EYE_VECTOR_ADDR, 1, 4 );

   rmesa->hw.mat[R200_MTX_MV].cmd[MAT_CMD_0] = 
      cmdvec( R200_VS_MATRIX_0_MV, 1, 16);
   rmesa->hw.mat[R200_MTX_IMV].cmd[MAT_CMD_0] = 
      cmdvec( R200_VS_MATRIX_1_INV_MV, 1, 16);
   rmesa->hw.mat[R200_MTX_MVP].cmd[MAT_CMD_0] = 
      cmdvec( R200_VS_MATRIX_2_MVP, 1, 16);
   rmesa->hw.mat[R200_MTX_TEX0].cmd[MAT_CMD_0] = 
      cmdvec( R200_VS_MATRIX_3_TEX0, 1, 16);
   rmesa->hw.mat[R200_MTX_TEX1].cmd[MAT_CMD_0] = 
      cmdvec( R200_VS_MATRIX_4_TEX1, 1, 16);

   for (i = 0 ; i < 8; i++) {
      rmesa->hw.lit[i].cmd[LIT_CMD_0] = 
	 cmdvec( R200_VS_LIGHT_AMBIENT_ADDR + i, 8, 24 );
      rmesa->hw.lit[i].cmd[LIT_CMD_1] = 
	 cmdscl( R200_SS_LIGHT_DCD_ADDR + i, 8, 7 );
   }

   for (i = 0 ; i < 6; i++) {
      rmesa->hw.ucp[i].cmd[UCP_CMD_0] = 
	 cmdvec( R200_VS_UCP_ADDR + i, 1, 4 );
   }

   /* Initial Harware state:
    */
   rmesa->hw.ctx.cmd[CTX_PP_MISC] = (R200_ALPHA_TEST_PASS |
				     R200_RIGHT_HAND_CUBE_OGL);

   rmesa->hw.ctx.cmd[CTX_PP_FOG_COLOR] = (R200_FOG_VERTEX |
					  R200_FOG_USE_SPEC_ALPHA);

   rmesa->hw.ctx.cmd[CTX_RE_SOLID_COLOR] = 0x00000000;

   rmesa->hw.ctx.cmd[CTX_RB3D_BLENDCNTL] = (R200_COMB_FCN_ADD_CLAMP |
					    R200_SRC_BLEND_GL_ONE |
					    R200_DST_BLEND_GL_ZERO );

   rmesa->hw.ctx.cmd[CTX_RB3D_DEPTHOFFSET] =
      rmesa->r200Screen->depthOffset;

   rmesa->hw.ctx.cmd[CTX_RB3D_DEPTHPITCH] = 
      ((rmesa->r200Screen->depthPitch &
	R200_DEPTHPITCH_MASK) |
       R200_DEPTH_ENDIAN_NO_SWAP);

   rmesa->hw.ctx.cmd[CTX_RB3D_ZSTENCILCNTL] = (depth_fmt |
  					       R200_Z_TEST_LESS |  
					       R200_STENCIL_TEST_ALWAYS |
					       R200_STENCIL_FAIL_KEEP |
					       R200_STENCIL_ZPASS_KEEP |
					       R200_STENCIL_ZFAIL_KEEP |
					       R200_Z_WRITE_ENABLE);

   rmesa->hw.ctx.cmd[CTX_PP_CNTL] = (R200_ANTI_ALIAS_NONE 
 				     | R200_TEX_BLEND_0_ENABLE);

   rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] = color_fmt;
   rmesa->hw.ctx.cmd[CTX_RB3D_CNTL] |=  R200_DITHER_ENABLE;

   rmesa->hw.ctx.cmd[CTX_RB3D_COLOROFFSET] = (rmesa->state.color.drawOffset &
					      R200_COLOROFFSET_MASK);

   rmesa->hw.ctx.cmd[CTX_RB3D_COLORPITCH] = ((rmesa->state.color.drawPitch &
					      R200_COLORPITCH_MASK) |
					     R200_COLOR_ENDIAN_NO_SWAP);

   rmesa->hw.set.cmd[SET_SE_CNTL] = (R200_FFACE_CULL_CCW |
				     R200_BFACE_SOLID |
				     R200_FFACE_SOLID |
				     R200_FLAT_SHADE_VTX_LAST |
				     R200_DIFFUSE_SHADE_GOURAUD |
				     R200_ALPHA_SHADE_GOURAUD |
				     R200_SPECULAR_SHADE_GOURAUD |
				     R200_FOG_SHADE_GOURAUD |
				     R200_VTX_PIX_CENTER_OGL |
				     R200_ROUND_MODE_TRUNC |
				     R200_ROUND_PREC_8TH_PIX);

   rmesa->hw.set.cmd[SET_RE_CNTL] = (R200_PERSPECTIVE_ENABLE |
				     R200_SCISSOR_ENABLE);

   rmesa->hw.lin.cmd[LIN_RE_LINE_PATTERN] = ((1 << 16) | 0xffff);

   rmesa->hw.lin.cmd[LIN_RE_LINE_STATE] = 
      ((0 << R200_LINE_CURRENT_PTR_SHIFT) |
       (1 << R200_LINE_CURRENT_COUNT_SHIFT));

   rmesa->hw.lin.cmd[LIN_SE_LINE_WIDTH] = (1 << 4);

   rmesa->hw.msk.cmd[MSK_RB3D_STENCILREFMASK] = 
      ((0x00 << R200_STENCIL_REF_SHIFT) |
       (0xff << R200_STENCIL_MASK_SHIFT) |
       (0xff << R200_STENCIL_WRITEMASK_SHIFT));

   rmesa->hw.msk.cmd[MSK_RB3D_ROPCNTL] = R200_ROP_COPY;
   rmesa->hw.msk.cmd[MSK_RB3D_PLANEMASK] = 0xffffffff;

   rmesa->hw.tam.cmd[TAM_DEBUG3] = 0;

   rmesa->hw.msc.cmd[MSC_RE_MISC] = 
      ((0 << R200_STIPPLE_X_OFFSET_SHIFT) |
       (0 << R200_STIPPLE_Y_OFFSET_SHIFT) |
       R200_STIPPLE_BIG_BIT_ORDER);


   rmesa->hw.cst.cmd[CST_PP_CNTL_X] = 0;
   rmesa->hw.cst.cmd[CST_RB3D_DEPTHXY_OFFSET] = 0;
   rmesa->hw.cst.cmd[CST_RE_AUX_SCISSOR_CNTL] = 0x0;
   rmesa->hw.cst.cmd[CST_RE_SCISSOR_TL_0] = 0;
   rmesa->hw.cst.cmd[CST_RE_SCISSOR_BR_0] = 0;
   rmesa->hw.cst.cmd[CST_SE_VAP_CNTL_STATUS] =
#ifdef MESA_BIG_ENDIAN
						R200_VC_32BIT_SWAP;
#else
						R200_VC_NO_SWAP;
#endif
   rmesa->hw.cst.cmd[CST_RE_POINTSIZE] = 0x100010;
   rmesa->hw.cst.cmd[CST_SE_TCL_INPUT_VTX_0] =
      (0x0 << R200_VERTEX_POSITION_ADDR__SHIFT);
   rmesa->hw.cst.cmd[CST_SE_TCL_INPUT_VTX_1] =
      (0x02 << R200_VTX_COLOR_0_ADDR__SHIFT) |
      (0x03 << R200_VTX_COLOR_1_ADDR__SHIFT);
   rmesa->hw.cst.cmd[CST_SE_TCL_INPUT_VTX_2] =
      (0x06 << R200_VTX_TEX_0_ADDR__SHIFT) |
      (0x07 << R200_VTX_TEX_1_ADDR__SHIFT) |
      (0x08 << R200_VTX_TEX_2_ADDR__SHIFT) |
      (0x09 << R200_VTX_TEX_3_ADDR__SHIFT);
   rmesa->hw.cst.cmd[CST_SE_TCL_INPUT_VTX_3] =
      (0x0A << R200_VTX_TEX_4_ADDR__SHIFT) |
      (0x0B << R200_VTX_TEX_5_ADDR__SHIFT);
  

   rmesa->hw.vpt.cmd[VPT_SE_VPORT_XSCALE]  = 0x00000000;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_XOFFSET] = 0x00000000;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_YSCALE]  = 0x00000000;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_YOFFSET] = 0x00000000;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_ZSCALE]  = 0x00000000;
   rmesa->hw.vpt.cmd[VPT_SE_VPORT_ZOFFSET] = 0x00000000;

   rmesa->hw.tex[0].cmd[TEX_PP_TXFILTER] = R200_BORDER_MODE_OGL;
   rmesa->hw.tex[0].cmd[TEX_PP_TXFORMAT] = 
      (R200_TXFORMAT_ST_ROUTE_STQ0 |
       (2 << R200_TXFORMAT_WIDTH_SHIFT) |
       (2 << R200_TXFORMAT_HEIGHT_SHIFT));
   rmesa->hw.tex[0].cmd[TEX_PP_TXOFFSET] = 0;
   rmesa->hw.tex[0].cmd[TEX_PP_BORDER_COLOR] = 0;
   rmesa->hw.tex[0].cmd[TEX_PP_TXFORMAT_X] =
      (/* R200_TEXCOORD_PROJ | */
       0x100000);	/* Small default bias */

   rmesa->hw.pix[0].cmd[PIX_PP_TXCBLEND] =  
      (R200_TXC_ARG_A_ZERO |
       R200_TXC_ARG_B_ZERO |
       R200_TXC_ARG_C_DIFFUSE_COLOR |
       R200_TXC_OP_MADD);

   rmesa->hw.pix[0].cmd[PIX_PP_TXCBLEND2] =  
      ((0 << R200_TXC_TFACTOR_SEL_SHIFT) | 
       R200_TXC_SCALE_1X |
       R200_TXC_CLAMP_0_1 |
       R200_TXC_OUTPUT_REG_R0);

   rmesa->hw.pix[0].cmd[PIX_PP_TXABLEND] =  
      (R200_TXA_ARG_A_ZERO |
       R200_TXA_ARG_B_ZERO |
       R200_TXA_ARG_C_DIFFUSE_ALPHA |
       R200_TXA_OP_MADD);

   rmesa->hw.pix[0].cmd[PIX_PP_TXABLEND2] =  
      ((0 << R200_TXA_TFACTOR_SEL_SHIFT) | 
       R200_TXA_SCALE_1X |
       R200_TXA_CLAMP_0_1 |
       R200_TXA_OUTPUT_REG_R0);

   rmesa->hw.tex[1].cmd[TEX_PP_TXFILTER] = R200_BORDER_MODE_OGL;
   rmesa->hw.tex[1].cmd[TEX_PP_TXFORMAT] = 
      (R200_TXFORMAT_ST_ROUTE_STQ1 |
       (2 << R200_TXFORMAT_WIDTH_SHIFT) |
       (2 << R200_TXFORMAT_HEIGHT_SHIFT));
   rmesa->hw.tex[1].cmd[TEX_PP_TXOFFSET] = 0;
   rmesa->hw.tex[1].cmd[TEX_PP_BORDER_COLOR] = 0;
   rmesa->hw.tex[1].cmd[TEX_PP_TXFORMAT_X] = 
      (/* R200_TEXCOORD_PROJ | */
       0x100000);	/* Small default bias */

   rmesa->hw.pix[1].cmd[PIX_PP_TXCBLEND] =  
      (R200_TXC_ARG_A_ZERO |
       R200_TXC_ARG_B_ZERO |
       R200_TXC_ARG_C_DIFFUSE_COLOR |
       R200_TXC_OP_MADD);

   rmesa->hw.pix[1].cmd[PIX_PP_TXCBLEND2] =  
      ((0 << R200_TXC_TFACTOR_SEL_SHIFT) | 
       R200_TXC_SCALE_1X |
       R200_TXC_CLAMP_0_1 |
       R200_TXC_OUTPUT_REG_R0);

   rmesa->hw.pix[1].cmd[PIX_PP_TXABLEND] =  
      (R200_TXA_ARG_A_ZERO |
       R200_TXA_ARG_B_ZERO |
       R200_TXA_ARG_C_DIFFUSE_ALPHA |
       R200_TXA_OP_MADD);

   rmesa->hw.pix[1].cmd[PIX_PP_TXABLEND2] =  
      ((0 << R200_TXA_TFACTOR_SEL_SHIFT) | 
       R200_TXA_SCALE_1X |
       R200_TXA_CLAMP_0_1 |
       R200_TXA_OUTPUT_REG_R0);

   rmesa->hw.tf.cmd[TF_TFACTOR_0] = 0;
   rmesa->hw.tf.cmd[TF_TFACTOR_1] = 0;
   rmesa->hw.tf.cmd[TF_TFACTOR_2] = 0;
   rmesa->hw.tf.cmd[TF_TFACTOR_3] = 0;
   rmesa->hw.tf.cmd[TF_TFACTOR_4] = 0;
   rmesa->hw.tf.cmd[TF_TFACTOR_5] = 0;

   rmesa->hw.vap.cmd[VAP_SE_VAP_CNTL] = 
      (R200_VAP_TCL_ENABLE | 
       (0x9 << R200_VAP_VF_MAX_VTX_NUM__SHIFT));

   rmesa->hw.vte.cmd[VTE_SE_VTE_CNTL] = 
      (R200_VPORT_X_SCALE_ENA |
       R200_VPORT_Y_SCALE_ENA |
       R200_VPORT_Z_SCALE_ENA |
       R200_VPORT_X_OFFSET_ENA |
       R200_VPORT_Y_OFFSET_ENA |
       R200_VPORT_Z_OFFSET_ENA |
/* FIXME: Turn on for tex rect only */
       R200_VTX_ST_DENORMALIZED |  
       R200_VTX_W0_FMT); 


   rmesa->hw.vtx.cmd[VTX_VTXFMT_0] = 0;
   rmesa->hw.vtx.cmd[VTX_VTXFMT_1] = 0;
   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_0] = 
      ((R200_VTX_Z0 | R200_VTX_W0 |
       (R200_VTX_FP_RGBA << R200_VTX_COLOR_0_SHIFT)));	
   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_VTXFMT_1] = 0;
   rmesa->hw.vtx.cmd[VTX_TCL_OUTPUT_COMPSEL] = (R200_OUTPUT_XYZW);
   rmesa->hw.vtx.cmd[VTX_STATE_CNTL] = R200_VSC_UPDATE_USER_COLOR_0_ENABLE;
						   

   /* Matrix selection */
   rmesa->hw.msl.cmd[MSL_MATRIX_SELECT_0] = 
      (R200_MTX_MV << R200_MODELVIEW_0_SHIFT);
   
   rmesa->hw.msl.cmd[MSL_MATRIX_SELECT_1] = 
       (R200_MTX_IMV << R200_IT_MODELVIEW_0_SHIFT);

   rmesa->hw.msl.cmd[MSL_MATRIX_SELECT_2] = 
      (R200_MTX_MVP << R200_MODELPROJECT_0_SHIFT);

   rmesa->hw.msl.cmd[MSL_MATRIX_SELECT_3] = 
      ((R200_MTX_TEX0 << R200_TEXMAT_0_SHIFT) |
       (R200_MTX_TEX1 << R200_TEXMAT_1_SHIFT) |
       (R200_MTX_TEX2 << R200_TEXMAT_2_SHIFT) |
       (R200_MTX_TEX3 << R200_TEXMAT_3_SHIFT));

   rmesa->hw.msl.cmd[MSL_MATRIX_SELECT_4] = 
      ((R200_MTX_TEX4 << R200_TEXMAT_4_SHIFT) |
       (R200_MTX_TEX5 << R200_TEXMAT_5_SHIFT));


   /* General TCL state */
   rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_0] = 
      (R200_SPECULAR_LIGHTS |
       R200_DIFFUSE_SPECULAR_COMBINE |
       R200_LOCAL_LIGHT_VEC_GL);

   rmesa->hw.tcl.cmd[TCL_LIGHT_MODEL_CTL_1] = 
      ((R200_LM1_SOURCE_LIGHT_PREMULT << R200_FRONT_EMISSIVE_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_FRONT_AMBIENT_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_FRONT_DIFFUSE_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_FRONT_SPECULAR_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_BACK_EMISSIVE_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_BACK_AMBIENT_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_BACK_DIFFUSE_SOURCE_SHIFT) |
       (R200_LM1_SOURCE_LIGHT_PREMULT << R200_BACK_SPECULAR_SOURCE_SHIFT)); 

   rmesa->hw.tcl.cmd[TCL_PER_LIGHT_CTL_0] = 0; /* filled in via callbacks */
   rmesa->hw.tcl.cmd[TCL_PER_LIGHT_CTL_1] = 0;
   rmesa->hw.tcl.cmd[TCL_PER_LIGHT_CTL_2] = 0;
   rmesa->hw.tcl.cmd[TCL_PER_LIGHT_CTL_3] = 0;
   
   rmesa->hw.tcl.cmd[TCL_UCP_VERT_BLEND_CTL] = 
      (R200_UCP_IN_CLIP_SPACE |
       R200_CULL_FRONT_IS_CCW);

   /* Texgen/Texmat state */
   rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_2] = 0x0; /* masks??? */
   rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_3] = 
      ((0 << R200_TEXGEN_0_INPUT_TEX_SHIFT) |
       (1 << R200_TEXGEN_1_INPUT_TEX_SHIFT) |
       (2 << R200_TEXGEN_2_INPUT_TEX_SHIFT) |
       (3 << R200_TEXGEN_3_INPUT_TEX_SHIFT) |
       (4 << R200_TEXGEN_4_INPUT_TEX_SHIFT) |
       (5 << R200_TEXGEN_5_INPUT_TEX_SHIFT)); 
   rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_0] = 0; 
   rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_1] =  
      ((0 << R200_TEXGEN_0_INPUT_SHIFT) |
       (1 << R200_TEXGEN_1_INPUT_SHIFT) |
       (2 << R200_TEXGEN_2_INPUT_SHIFT) |
       (3 << R200_TEXGEN_3_INPUT_SHIFT) |
       (4 << R200_TEXGEN_4_INPUT_SHIFT) |
       (5 << R200_TEXGEN_5_INPUT_SHIFT)); 
   rmesa->hw.tcg.cmd[TCG_TEX_CYL_WRAP_CTL] = 0;

   rmesa->TexGenInputs = rmesa->hw.tcg.cmd[TCG_TEX_PROC_CTL_1];


   for (i = 0 ; i < 8; i++) {
      struct gl_light *l = &ctx->Light.Light[i];
      GLenum p = GL_LIGHT0 + i;
      *(float *)&(rmesa->hw.lit[i].cmd[LIT_RANGE_CUTOFF]) = FLT_MAX;

      ctx->Driver.Lightfv( ctx, p, GL_AMBIENT, l->Ambient );
      ctx->Driver.Lightfv( ctx, p, GL_DIFFUSE, l->Diffuse );
      ctx->Driver.Lightfv( ctx, p, GL_SPECULAR, l->Specular );
      ctx->Driver.Lightfv( ctx, p, GL_POSITION, 0 );
      ctx->Driver.Lightfv( ctx, p, GL_SPOT_DIRECTION, 0 );
      ctx->Driver.Lightfv( ctx, p, GL_SPOT_EXPONENT, &l->SpotExponent );
      ctx->Driver.Lightfv( ctx, p, GL_SPOT_CUTOFF, &l->SpotCutoff );
      ctx->Driver.Lightfv( ctx, p, GL_CONSTANT_ATTENUATION,
			   &l->ConstantAttenuation );
      ctx->Driver.Lightfv( ctx, p, GL_LINEAR_ATTENUATION, 
			   &l->LinearAttenuation );
      ctx->Driver.Lightfv( ctx, p, GL_QUADRATIC_ATTENUATION, 
		     &l->QuadraticAttenuation );
   }

   ctx->Driver.LightModelfv( ctx, GL_LIGHT_MODEL_AMBIENT, 
			     ctx->Light.Model.Ambient );

   TNL_CONTEXT(ctx)->Driver.NotifyMaterialChange( ctx );

   for (i = 0 ; i < 6; i++) {
      ctx->Driver.ClipPlane( ctx, GL_CLIP_PLANE0 + i, NULL );
   }

   ctx->Driver.Fogfv( ctx, GL_FOG_MODE, 0 );
   ctx->Driver.Fogfv( ctx, GL_FOG_DENSITY, &ctx->Fog.Density );
   ctx->Driver.Fogfv( ctx, GL_FOG_START, &ctx->Fog.Start );
   ctx->Driver.Fogfv( ctx, GL_FOG_END, &ctx->Fog.End );
   ctx->Driver.Fogfv( ctx, GL_FOG_COLOR, ctx->Fog.Color );
   ctx->Driver.Fogfv( ctx, GL_FOG_COORDINATE_SOURCE_EXT, 0 );
   
   rmesa->hw.grd.cmd[GRD_VERT_GUARD_CLIP_ADJ] = IEEE_ONE;
   rmesa->hw.grd.cmd[GRD_VERT_GUARD_DISCARD_ADJ] = IEEE_ONE;
   rmesa->hw.grd.cmd[GRD_HORZ_GUARD_CLIP_ADJ] = IEEE_ONE;
   rmesa->hw.grd.cmd[GRD_HORZ_GUARD_DISCARD_ADJ] = IEEE_ONE;

   rmesa->hw.eye.cmd[EYE_X] = 0;
   rmesa->hw.eye.cmd[EYE_Y] = 0;
   rmesa->hw.eye.cmd[EYE_Z] = IEEE_ONE;
   rmesa->hw.eye.cmd[EYE_RESCALE_FACTOR] = IEEE_ONE;

   r200LightingSpaceChange( ctx );
   
   rmesa->lost_context = 1;
}
