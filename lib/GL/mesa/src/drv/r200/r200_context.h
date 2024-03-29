/* $XFree86: xc/lib/GL/mesa/src/drv/r200/r200_context.h,v 1.1 2002/10/30 12:51:52 alanh Exp $ */
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

#ifndef __R200_CONTEXT_H__
#define __R200_CONTEXT_H__

#ifdef GLX_DIRECT_RENDERING

#include <X11/Xlibint.h>
#include "dri_util.h"
#include "xf86drm.h"
#include "radeon_common.h"

#include "macros.h"
#include "mtypes.h"
#include "r200_reg.h"

struct r200_context;
typedef struct r200_context r200ContextRec;
typedef struct r200_context *r200ContextPtr;

#include "r200_lock.h"
#include "r200_screen.h"
#include "mm.h"

/* Flags for software fallback cases */
/* See correponding strings in r200_swtcl.c */
#define R200_FALLBACK_TEXTURE           0x1
#define R200_FALLBACK_DRAW_BUFFER       0x2
#define R200_FALLBACK_STENCIL           0x4
#define R200_FALLBACK_RENDER_MODE       0x8
#define R200_FALLBACK_BLEND_EQ          0x10
#define R200_FALLBACK_BLEND_FUNC        0x20
#define R200_FALLBACK_DISABLE           0x40

/* Use the templated vertex format:
 */
#define COLOR_IS_RGBA
#define TAG(x) r200##x
#include "tnl_dd/t_dd_vertex.h"
#undef TAG

typedef void (*r200_tri_func)( r200ContextPtr,
				 r200Vertex *,
				 r200Vertex *,
				 r200Vertex * );

typedef void (*r200_line_func)( r200ContextPtr,
				  r200Vertex *,
				  r200Vertex * );

typedef void (*r200_point_func)( r200ContextPtr,
				   r200Vertex * );


struct r200_colorbuffer_state {
   GLuint clear;
   GLint drawOffset, drawPitch;
};


struct r200_depthbuffer_state {
   GLfloat scale;
};

struct r200_pixel_state {
   GLint readOffset, readPitch;
};

struct r200_scissor_state {
   XF86DRIClipRectRec rect;
   GLboolean enabled;

   GLuint numClipRects;			/* Cliprects active */
   GLuint numAllocedClipRects;		/* Cliprects available */
   XF86DRIClipRectPtr pClipRects;
};

struct r200_stencilbuffer_state {
   GLboolean hwBuffer;
   GLuint clear;			/* rb3d_stencilrefmask value */
};

struct r200_stipple_state {
   GLuint mask[32];
};



#define TEX_0   0x1
#define TEX_1   0x2
#define TEX_ALL 0x3

typedef struct r200_tex_obj r200TexObj, *r200TexObjPtr;

/* Texture object in locally shared texture space.
 */
struct r200_tex_obj {
   r200TexObjPtr next, prev;

   struct gl_texture_object *tObj;	/* Mesa texture object */

   PMemBlock memBlock;			/* Memory block containing texture */
   GLuint bufAddr;			/* Offset to start of locally
					   shared texture block */

   GLuint dirty_images;			/* Flags for whether or not
					   images need to be uploaded to
					   local or AGP texture space */

   GLuint dirty_state;		        /* Flags (1 per texunit) for
					   whether or not this texobj
					   has dirty hardware state
					   (pp_*) that needs to be
					   brought into the
					   texunit. */

   GLint heap;				/* Texture heap currently stored in */

   drmRadeonTexImage image[RADEON_MAX_TEXTURE_LEVELS];

   GLint totalSize;			/* Total size of the texture
					   including all mipmap levels */

   GLuint pp_txfilter;		        /* hardware register values */
   GLuint pp_txformat;
   GLuint pp_txoffset;
   GLuint pp_txsize;		        /* npot only */
   GLuint pp_txpitch;		        /* npot only */
   GLuint pp_border_color;

   /* texObj->Image[firstLevel] through texObj->Image[lastLevel] are the
    * images to upload.
    */
   GLint firstLevel;     
   GLint lastLevel;      
};


struct r200_texture_env_state {
   r200TexObjPtr texobj;
   GLenum format;
   GLenum envMode;
};

#define R200_MAX_TEXTURE_UNITS 3

struct r200_texture_state {
   struct r200_texture_env_state unit[R200_MAX_TEXTURE_UNITS];
};


struct r200_state_atom {
   struct r200_state_atom *next, *prev;
   const char *name;		         /* for debug */
   int cmd_size;		         /* size in bytes */
   GLuint idx;
   int *cmd;			         /* one or more cmd's */
   int *lastcmd;			 /* one or more cmd's */
   GLboolean (*check)( GLcontext *, int );    /* is this state active? */
};
   


/* Trying to keep these relatively short as the variables are becoming
 * extravagently long.  Drop the R200_ off the front of everything -
 * I think we know we're in the r200 driver by now, and keep the
 * prefix to 3 letters unless absolutely impossible.  
 */

#define CTX_CMD_0             0
#define CTX_PP_MISC           1
#define CTX_PP_FOG_COLOR      2
#define CTX_RE_SOLID_COLOR    3
#define CTX_RB3D_BLENDCNTL    4
#define CTX_RB3D_DEPTHOFFSET  5
#define CTX_RB3D_DEPTHPITCH   6
#define CTX_RB3D_ZSTENCILCNTL 7
#define CTX_CMD_1             8
#define CTX_PP_CNTL           9
#define CTX_RB3D_CNTL         10
#define CTX_RB3D_COLOROFFSET  11
#define CTX_CMD_2             12 /* why */
#define CTX_RB3D_COLORPITCH   13 /* why */
#define CTX_STATE_SIZE        14

#define SET_CMD_0               0
#define SET_SE_CNTL             1
#define SET_RE_CNTL             2 /* replace se_coord_fmt */
#define SET_STATE_SIZE          3

#define VTE_CMD_0               0
#define VTE_SE_VTE_CNTL         1
#define VTE_STATE_SIZE          2

#define LIN_CMD_0               0
#define LIN_RE_LINE_PATTERN     1
#define LIN_RE_LINE_STATE       2
#define LIN_CMD_1               3
#define LIN_SE_LINE_WIDTH       4
#define LIN_STATE_SIZE          5

#define MSK_CMD_0               0
#define MSK_RB3D_STENCILREFMASK 1
#define MSK_RB3D_ROPCNTL        2
#define MSK_RB3D_PLANEMASK      3
#define MSK_STATE_SIZE          4

#define VPT_CMD_0           0
#define VPT_SE_VPORT_XSCALE          1
#define VPT_SE_VPORT_XOFFSET         2
#define VPT_SE_VPORT_YSCALE          3
#define VPT_SE_VPORT_YOFFSET         4
#define VPT_SE_VPORT_ZSCALE          5
#define VPT_SE_VPORT_ZOFFSET         6
#define VPT_STATE_SIZE      7

#define ZBS_CMD_0              0
#define ZBS_SE_ZBIAS_FACTOR             1
#define ZBS_SE_ZBIAS_CONSTANT           2
#define ZBS_STATE_SIZE         3

#define MSC_CMD_0               0
#define MSC_RE_MISC             1
#define MSC_STATE_SIZE          2

#define TAM_CMD_0               0
#define TAM_DEBUG3              1
#define TAM_STATE_SIZE          2

#define TEX_CMD_0                   0
#define TEX_PP_TXFILTER             1
#define TEX_PP_TXFORMAT             2
#define TEX_PP_TXFORMAT_X           3
#define TEX_PP_TXSIZE               4
#define TEX_PP_TXPITCH              5
#define TEX_PP_BORDER_COLOR         6
#define TEX_CMD_1                   7
#define TEX_PP_TXOFFSET             8
#define TEX_STATE_SIZE              9

#define PIX_CMD_0                   0
#define PIX_PP_TXCBLEND             1
#define PIX_PP_TXCBLEND2            2
#define PIX_PP_TXABLEND             3
#define PIX_PP_TXABLEND2            4
#define PIX_STATE_SIZE              5

#define TF_CMD_0                    0
#define TF_TFACTOR_0                1
#define TF_TFACTOR_1                2
#define TF_TFACTOR_2                3
#define TF_TFACTOR_3                4
#define TF_TFACTOR_4                5
#define TF_TFACTOR_5                6
#define TF_STATE_SIZE               7

#define TCL_CMD_0                 0
#define TCL_LIGHT_MODEL_CTL_0     1
#define TCL_LIGHT_MODEL_CTL_1     2
#define TCL_PER_LIGHT_CTL_0       3
#define TCL_PER_LIGHT_CTL_1       4
#define TCL_PER_LIGHT_CTL_2       5
#define TCL_PER_LIGHT_CTL_3       6
#define TCL_CMD_1                 7
#define TCL_UCP_VERT_BLEND_CTL    8
#define TCL_STATE_SIZE            9

#define MSL_CMD_0                     0
#define MSL_MATRIX_SELECT_0           1
#define MSL_MATRIX_SELECT_1           2
#define MSL_MATRIX_SELECT_2           3
#define MSL_MATRIX_SELECT_3           4
#define MSL_MATRIX_SELECT_4           5
#define MSL_STATE_SIZE                6

#define TCG_CMD_0                 0
#define TCG_TEX_PROC_CTL_2            1
#define TCG_TEX_PROC_CTL_3            2
#define TCG_TEX_PROC_CTL_0            3
#define TCG_TEX_PROC_CTL_1            4
#define TCG_TEX_CYL_WRAP_CTL      5
#define TCG_STATE_SIZE            6

#define MTL_CMD_0            0	
#define MTL_EMMISSIVE_RED    1	
#define MTL_EMMISSIVE_GREEN  2	
#define MTL_EMMISSIVE_BLUE   3	
#define MTL_EMMISSIVE_ALPHA  4	
#define MTL_AMBIENT_RED      5
#define MTL_AMBIENT_GREEN    6
#define MTL_AMBIENT_BLUE     7
#define MTL_AMBIENT_ALPHA    8
#define MTL_DIFFUSE_RED      9
#define MTL_DIFFUSE_GREEN    10
#define MTL_DIFFUSE_BLUE     11
#define MTL_DIFFUSE_ALPHA    12
#define MTL_SPECULAR_RED     13
#define MTL_SPECULAR_GREEN   14
#define MTL_SPECULAR_BLUE    15
#define MTL_SPECULAR_ALPHA   16
#define MTL_CMD_1            17
#define MTL_SHININESS        18
#define MTL_STATE_SIZE       19

#define VAP_CMD_0                   0
#define VAP_SE_VAP_CNTL             1
#define VAP_STATE_SIZE              2

/* Replaces a lot of packet info from radeon
 */
#define VTX_CMD_0                   0
#define VTX_VTXFMT_0            1
#define VTX_VTXFMT_1            2
#define VTX_TCL_OUTPUT_VTXFMT_0 3
#define VTX_TCL_OUTPUT_VTXFMT_1 4
#define VTX_CMD_1               5
#define VTX_TCL_OUTPUT_COMPSEL  6
#define VTX_CMD_2               7
#define VTX_STATE_CNTL          8
#define VTX_STATE_SIZE          9


#define VTX_COLOR(v,n)   (((v)>>(R200_VTX_COLOR_0_SHIFT+(n)*2))&\
                         R200_VTX_COLOR_MASK)

#define MAT_CMD_0              0
#define MAT_ELT_0              1
#define MAT_STATE_SIZE         17

#define GRD_CMD_0                  0
#define GRD_VERT_GUARD_CLIP_ADJ    1
#define GRD_VERT_GUARD_DISCARD_ADJ 2
#define GRD_HORZ_GUARD_CLIP_ADJ    3
#define GRD_HORZ_GUARD_DISCARD_ADJ 4
#define GRD_STATE_SIZE             5

/* position changes frequently when lighting in modelpos - separate
 * out to new state item?  
 */
#define LIT_CMD_0                  0
#define LIT_AMBIENT_RED            1
#define LIT_AMBIENT_GREEN          2
#define LIT_AMBIENT_BLUE           3
#define LIT_AMBIENT_ALPHA          4
#define LIT_DIFFUSE_RED            5
#define LIT_DIFFUSE_GREEN          6
#define LIT_DIFFUSE_BLUE           7
#define LIT_DIFFUSE_ALPHA          8
#define LIT_SPECULAR_RED           9
#define LIT_SPECULAR_GREEN         10
#define LIT_SPECULAR_BLUE          11
#define LIT_SPECULAR_ALPHA         12
#define LIT_POSITION_X             13
#define LIT_POSITION_Y             14
#define LIT_POSITION_Z             15
#define LIT_POSITION_W             16
#define LIT_DIRECTION_X            17
#define LIT_DIRECTION_Y            18
#define LIT_DIRECTION_Z            19
#define LIT_DIRECTION_W            20
#define LIT_ATTEN_CONST            21
#define LIT_ATTEN_LINEAR           22
#define LIT_ATTEN_QUADRATIC        23
#define LIT_ATTEN_XXX              24
#define LIT_CMD_1                  25
#define LIT_SPOT_DCD               26
#define LIT_SPOT_DCM               27
#define LIT_SPOT_EXPONENT          28
#define LIT_SPOT_CUTOFF            29
#define LIT_SPECULAR_THRESH        30
#define LIT_RANGE_CUTOFF           31 /* ? */
#define LIT_RANGE_ATTEN            32 /* ? */
#define LIT_STATE_SIZE             33

/* Fog
 */
#define FOG_CMD_0      0
#define FOG_R          1
#define FOG_C          2
#define FOG_D          3
#define FOG_PAD        4
#define FOG_STATE_SIZE 5

/* UCP
 */
#define UCP_CMD_0      0
#define UCP_X          1
#define UCP_Y          2
#define UCP_Z          3
#define UCP_W          4
#define UCP_STATE_SIZE 5

/* GLT - Global ambient
 */
#define GLT_CMD_0      0
#define GLT_RED        1
#define GLT_GREEN      2
#define GLT_BLUE       3
#define GLT_ALPHA      4
#define GLT_STATE_SIZE 5

/* EYE
 */
#define EYE_CMD_0          0
#define EYE_X              1
#define EYE_Y              2
#define EYE_Z              3
#define EYE_RESCALE_FACTOR 4
#define EYE_STATE_SIZE     5

/* CST - constant state
 */
#define CST_CMD_0                             0
#define CST_PP_CNTL_X                         1
#define CST_CMD_1                             2
#define CST_RB3D_DEPTHXY_OFFSET               3
#define CST_CMD_2                             4
#define CST_RE_AUX_SCISSOR_CNTL               5
#define CST_CMD_3                             6
#define CST_RE_SCISSOR_TL_0                   7
#define CST_RE_SCISSOR_BR_0                   8
#define CST_CMD_4                             9
#define CST_SE_VAP_CNTL_STATUS                10
#define CST_CMD_5                             11
#define CST_RE_POINTSIZE                      12
#define CST_CMD_6                             13
#define CST_SE_TCL_INPUT_VTX_0                14
#define CST_SE_TCL_INPUT_VTX_1                15
#define CST_SE_TCL_INPUT_VTX_2                16
#define CST_SE_TCL_INPUT_VTX_3                17
#define CST_STATE_SIZE                        18




struct r200_hw_state {
   /* All state should be on one of these lists:
    */
   struct r200_state_atom dirty; /* dirty list head placeholder */
   struct r200_state_atom clean; /* clean list head placeholder */

   /* Hardware state, stored as cmdbuf commands:  
    *   -- Need to doublebuffer for
    *           - reviving state after loss of context
    *           - eliding noop statechange loops? (except line stipple count)
    */
   struct r200_state_atom ctx;
   struct r200_state_atom set;
   struct r200_state_atom vte;
   struct r200_state_atom lin;
   struct r200_state_atom msk;
   struct r200_state_atom vpt;
   struct r200_state_atom vap;
   struct r200_state_atom vtx;
   struct r200_state_atom tcl;
   struct r200_state_atom msl;
   struct r200_state_atom tcg;
   struct r200_state_atom msc;
   struct r200_state_atom cst;
   struct r200_state_atom tam;
   struct r200_state_atom tf;
   struct r200_state_atom tex[2];
   struct r200_state_atom zbs;
   struct r200_state_atom mtl[2]; 
   struct r200_state_atom mat[5]; 
   struct r200_state_atom lit[8]; /* includes vec, scl commands */
   struct r200_state_atom ucp[6];
   struct r200_state_atom pix[6]; /* pixshader stages */
   struct r200_state_atom eye; /* eye pos */
   struct r200_state_atom grd; /* guard band clipping */
   struct r200_state_atom fog; 
   struct r200_state_atom glt; 
};

struct r200_state {
   /* Derived state for internal purposes:
    */
   struct r200_colorbuffer_state color;
   struct r200_depthbuffer_state depth;
   struct r200_pixel_state pixel;
   struct r200_scissor_state scissor;
   struct r200_stencilbuffer_state stencil;
   struct r200_stipple_state stipple;
   struct r200_texture_state texture;
};

struct r200_texture {
   r200TexObj objects[R200_NR_TEX_HEAPS];
   r200TexObj swapped;

   memHeap_t *heap[R200_NR_TEX_HEAPS];
   GLint age[R200_NR_TEX_HEAPS];

   GLint numHeaps;
};

/* Need refcounting on dma buffers:
 */
struct r200_dma_buffer {
   int refcount;		/* the number of retained regions in buf */
   drmBufPtr buf;
};

#define GET_START(rvb) (rmesa->r200Screen->agp_buffer_offset +		\
			(rvb)->address - rmesa->dma.buf0_address +	\
			(rvb)->start)

/* A retained region, eg vertices for indexed vertices.
 */
struct r200_dma_region {
   struct r200_dma_buffer *buf;
   char *address;		/* == buf->address */
   int start, end, ptr;		/* offsets from start of buf */
   int aos_start;
   int aos_stride;
   int aos_size;
};


struct r200_dma {
   /* Active dma region.  Allocations for vertices and retained
    * regions come from here.  Also used for emitting random vertices,
    * these may be flushed by calling flush_current();
    */
   struct r200_dma_region current;
   
   void (*flush)( r200ContextPtr );

   char *buf0_address;		/* start of buf[0], for index calcs */
   GLuint nr_released_bufs;	/* flush after so many buffers released */
};

struct r200_dri_mirror {
   Display *display;			/* X server display */

   __DRIcontextPrivate	*context;	/* DRI context */
   __DRIscreenPrivate	*screen;	/* DRI screen */
   __DRIdrawablePrivate	*drawable;	/* DRI drawable bound to this ctx */

   drmContext hwContext;
   drmLock *hwLock;
   int fd;
   int drmMinor;
};


#define R200_CMD_BUF_SZ  (8*1024) 

struct r200_store {
   GLuint statenr;
   GLuint primnr;
   char cmd_buf[R200_CMD_BUF_SZ];
   int cmd_used;   
   int elts_start;
};


/* r200_tcl.c
 */
struct r200_tcl_info {
   GLuint vertex_format;
   GLint last_offset;
   GLuint hw_primitive;

   struct r200_dma_region *aos_components[8];
   GLuint nr_aos_components;

   GLuint *Elts;

   struct r200_dma_region indexed_verts;
   struct r200_dma_region obj;
   struct r200_dma_region rgba;
   struct r200_dma_region spec;
   struct r200_dma_region fog;
   struct r200_dma_region tex[R200_MAX_TEXTURE_UNITS];
   struct r200_dma_region norm;
};


/* r200_swtcl.c
 */
struct r200_swtcl_info {
   GLuint SetupIndex;
   GLuint SetupNewInputs;
   GLuint RenderIndex;
   GLuint vertex_size;
   GLuint vertex_stride_shift;
   GLuint vertex_format;
   char *verts;

   /* Fallback rasterization functions
    */
   r200_point_func draw_point;
   r200_line_func draw_line;
   r200_tri_func draw_tri;

   GLuint hw_primitive;
   GLenum render_primitive;
   GLuint numverts;

   struct r200_dma_region indexed_verts;
};


struct r200_ioctl {
   GLuint vertex_offset;
   GLuint vertex_size;
};



#define R200_MAX_PRIMS 64


/* Want to keep a cache of these around.  Each is parameterized by
 * only a single value which has only a small range.  Only expect a
 * few, so just rescan the list each time?
 */
struct dynfn {
   struct dynfn *next, *prev;
   int key[2];
   char *code;
};

struct dfn_lists {
   struct dynfn Vertex2f;
   struct dynfn Vertex2fv;
   struct dynfn Vertex3f;
   struct dynfn Vertex3fv;
   struct dynfn Color4ub;
   struct dynfn Color4ubv;
   struct dynfn Color3ub;
   struct dynfn Color3ubv;
   struct dynfn Color4f;
   struct dynfn Color4fv;
   struct dynfn Color3f;
   struct dynfn Color3fv;
   struct dynfn SecondaryColor3ubEXT;
   struct dynfn SecondaryColor3ubvEXT;
   struct dynfn SecondaryColor3fEXT;
   struct dynfn SecondaryColor3fvEXT;
   struct dynfn Normal3f;
   struct dynfn Normal3fv;
   struct dynfn TexCoord2f;
   struct dynfn TexCoord2fv;
   struct dynfn TexCoord1f;
   struct dynfn TexCoord1fv;
   struct dynfn MultiTexCoord2fARB;
   struct dynfn MultiTexCoord2fvARB;
   struct dynfn MultiTexCoord1fARB;
   struct dynfn MultiTexCoord1fvARB;
};

struct _vb;

struct dfn_generators {
   struct dynfn *(*Vertex2f)( GLcontext *, const int * );
   struct dynfn *(*Vertex2fv)( GLcontext *, const int * );
   struct dynfn *(*Vertex3f)( GLcontext *, const int * );
   struct dynfn *(*Vertex3fv)( GLcontext *, const int * );
   struct dynfn *(*Color4ub)( GLcontext *, const int * );
   struct dynfn *(*Color4ubv)( GLcontext *, const int * );
   struct dynfn *(*Color3ub)( GLcontext *, const int * );
   struct dynfn *(*Color3ubv)( GLcontext *, const int * );
   struct dynfn *(*Color4f)( GLcontext *, const int * );
   struct dynfn *(*Color4fv)( GLcontext *, const int * );
   struct dynfn *(*Color3f)( GLcontext *, const int * );
   struct dynfn *(*Color3fv)( GLcontext *, const int * );
   struct dynfn *(*SecondaryColor3ubEXT)( GLcontext *, const int * );
   struct dynfn *(*SecondaryColor3ubvEXT)( GLcontext *, const int * );
   struct dynfn *(*SecondaryColor3fEXT)( GLcontext *, const int * );
   struct dynfn *(*SecondaryColor3fvEXT)( GLcontext *, const int * );
   struct dynfn *(*Normal3f)( GLcontext *, const int * );
   struct dynfn *(*Normal3fv)( GLcontext *, const int * );
   struct dynfn *(*TexCoord2f)( GLcontext *, const int * );
   struct dynfn *(*TexCoord2fv)( GLcontext *, const int * );
   struct dynfn *(*TexCoord1f)( GLcontext *, const int * );
   struct dynfn *(*TexCoord1fv)( GLcontext *, const int * );
   struct dynfn *(*MultiTexCoord2fARB)( GLcontext *, const int * );
   struct dynfn *(*MultiTexCoord2fvARB)( GLcontext *, const int * );
   struct dynfn *(*MultiTexCoord1fARB)( GLcontext *, const int * );
   struct dynfn *(*MultiTexCoord1fvARB)( GLcontext *, const int * );
};


struct r200_vb {
   /* Keep these first: referenced from codegen templates:
    */
   GLint counter, initial_counter;
   GLint *dmaptr;
   void (*notify)( void );
   GLint vertex_size;

   /* A maximum total of 15 elements per vertex:  3 floats for position, 3
    * floats for normal, 4 floats for color, 4 bytes for secondary color,
    * 2 floats for each texture unit (4 floats total).
    * 
    * As soon as the 3rd TMU is supported or cube maps (or 3D textures) are
    * supported, this value will grow.
    * 
    * The position data is never actually stored here, so 3 elements could be
    * trimmed out of the buffer.
    */
   union { float f; int i; r200_color_t color; } vertex[15];

   GLfloat *normalptr;
   GLfloat *floatcolorptr;
   r200_color_t *colorptr;
   GLfloat *floatspecptr;
   r200_color_t *specptr;
   GLfloat *texcoordptr[2];

   GLcontext *context;		/* current context : Single thread only! */
};

struct r200_prim {
   GLuint start;
   GLuint end;
   GLuint prim;
};

struct r200_vbinfo {
   GLenum *prim;		/* &ctx->Driver.CurrentExecPrimitive */
   GLuint primflags;
   GLboolean enabled;		/* R200_NO_VTXFMT//R200_NO_TCL env vars */
   GLboolean installed;
   GLboolean fell_back;
   GLboolean recheck;
   GLint initial_counter;
   GLint nrverts;
   GLuint vtxfmt_0, vtxfmt_1;

   GLuint installed_vertex_format;
   GLuint installed_color_3f_sz;

   struct r200_prim primlist[R200_MAX_PRIMS];
   int nrprims;

   struct dfn_lists dfn_cache;
   struct dfn_generators codegen;
   GLvertexformat vtxfmt;
};




struct r200_context {
   GLcontext *glCtx;			/* Mesa context */

   /* Driver and hardware state management
    */
   struct r200_hw_state hw;
   struct r200_state state;

   /* Texture object bookkeeping
    */
   struct r200_texture texture;


   /* Rasterization and vertex state:
    */
   GLuint TclFallback;
   GLuint Fallback;
   GLuint NewGLState;

   
   /* Temporaries for translating away float colors:
    */
   struct gl_client_array UbyteColor;
   struct gl_client_array UbyteSecondaryColor;

   /* Vertex buffers
    */
   struct r200_ioctl ioctl;
   struct r200_dma dma;
   struct r200_store store;

   /* Page flipping
    */
   GLuint doPageFlip;

   /* Busy waiting
    */
   GLuint do_usleeps;
   GLuint do_irqs;
   GLuint irqsEmitted;
   drmRadeonIrqWait iw;

   /* Clientdata textures;
    */
   GLuint prefer_agp_client_texturing;

   /* Drawable, cliprect and scissor information
    */
   GLuint numClipRects;			/* Cliprects for the draw buffer */
   XF86DRIClipRectPtr pClipRects;
   unsigned int lastStamp;
   GLboolean lost_context;
   r200ScreenPtr r200Screen;	/* Screen private DRI data */
   RADEONSAREAPrivPtr sarea;		/* Private SAREA data */

   /* TCL stuff
    */
   GLmatrix TexGenMatrix[R200_MAX_TEXTURE_UNITS];
   GLboolean recheck_texgen[R200_MAX_TEXTURE_UNITS];
   GLboolean TexGenNeedNormals[R200_MAX_TEXTURE_UNITS];
   GLuint TexMatEnabled;
   GLuint TexMatCompSel;
   GLuint TexGenEnabled;
   GLuint TexGenInputs;
   GLuint TexGenCompSel;
   GLmatrix tmpmat;

   /* VBI
    */
   GLuint vbl_seq;

   /* r200_tcl.c
    */
   struct r200_tcl_info tcl;

   /* r200_swtcl.c
    */
   struct r200_swtcl_info swtcl;

   /* r200_vtxfmt.c
    */
   struct r200_vbinfo vb;

   /* Mirrors of some DRI state
    */
   struct r200_dri_mirror dri;
};

#define R200_CONTEXT(ctx)		((r200ContextPtr)(ctx->DriverCtx))


static __inline GLuint r200PackColor( GLuint cpp,
					GLubyte r, GLubyte g,
					GLubyte b, GLubyte a )
{
   switch ( cpp ) {
   case 2:
      return PACK_COLOR_565( r, g, b );
   case 4:
      return PACK_COLOR_8888( a, r, g, b );
   default:
      return 0;
   }
}

#define R200_OLD_PACKETS 0


extern void r200DestroyContext( __DRIcontextPrivate *driContextPriv );
extern GLboolean r200CreateContext( Display *dpy, 
				    const __GLcontextModes *glVisual,
				    __DRIcontextPrivate *driContextPriv,
				    void *sharedContextPrivate);
extern void r200SwapBuffers(Display *dpy, void *drawablePrivate);
extern GLboolean r200MakeCurrent( __DRIcontextPrivate *driContextPriv,
				  __DRIdrawablePrivate *driDrawPriv,
				  __DRIdrawablePrivate *driReadPriv );
extern GLboolean r200UnbindContext( __DRIcontextPrivate *driContextPriv );

/* ================================================================
 * Debugging:
 */
#define DO_DEBUG		1

#if DO_DEBUG
extern int R200_DEBUG;
#else
#define R200_DEBUG		0
#endif

#define DEBUG_TEXTURE	0x001
#define DEBUG_STATE	0x002
#define DEBUG_IOCTL	0x004
#define DEBUG_PRIMS	0x008
#define DEBUG_VERTS	0x010
#define DEBUG_FALLBACKS	0x020
#define DEBUG_VFMT	0x040
#define DEBUG_CODEGEN	0x080
#define DEBUG_VERBOSE	0x100
#define DEBUG_DRI       0x200
#define DEBUG_DMA       0x400
#define DEBUG_SANITY    0x800
#define DEBUG_SYNC      0x1000
#define DEBUG_PIXEL     0x2000

#endif
#endif /* __R200_CONTEXT_H__ */
