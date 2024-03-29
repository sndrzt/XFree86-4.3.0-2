/*
 * Copyright 2001 by Alan Hourihane, Sychdyn, North Wales, UK.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors:  Alan Hourihane, <alanh@fairlite.demon.co.uk>
 */
/* $XFree86: xc/programs/Xserver/hw/kdrive/pcmcia/pcmcia.h,v 1.1 2001/05/23 08:56:09 alanh Exp $ */

#ifndef _PCMCIA_H_
#define _PCMCIA_H_

#include <fbdev.h>

/*
 * offset from ioport beginning 
 */

#define PCMCIA_COP_BASE(c)	0x20000000
#define PCMCIA_COP_SIZE(c)	0x10000
#define TRIDENT_COP_OFF(c)	0x2100

typedef volatile CARD8	VOL8;
typedef volatile CARD16	VOL16;
typedef volatile CARD32	VOL32;

typedef struct _cop {
    VOL32	src_start_xy;	    /* 0x00 */
    VOL32	src_end_xy;	    /* 0x04 */
    VOL32	dst_start_xy;	    /* 0x08 */
    VOL32	dst_end_xy;	    /* 0x0c */
    VOL32	alpha;		    /* 0x10 */
    CARD8	pad14[0xc];	    /* 0x14 */
    VOL32	multi;		    /* 0x20 */

#define COP_MULTI_CLIP_TOP_LEFT	    0x10000000
#define COP_MULTI_DEPTH		    0x40000000
#define COP_MULTI_COLOR_KEY	    0x70000000
#define COP_MULTI_STYLE		    0x50000000
#define COP_MULTI_PATTERN	    0x80000000
#define COP_MULTI_ROP		    0x90000000
#define COP_MULTI_STRIDE	    0x60000000
#define COP_MULTI_Z		    0xa0000000
#define COP_MULTI_ALPHA		    0xb0000000
#define COP_MULTI_TEXTURE	    0xd0000000
#define COP_MULTI_TEXTURE_BOUND	    0xe0000000
#define COP_MULTI_TEXTURE_ADVANCED  0x20000000
#define COP_MULTI_MASK		    0xf0000000
    
#define COP_DEPTH_8		    0x00000000
#define COP_DEPTH_16		    0x00000001
#define COP_DEPTH_24_32		    0x00000002
#define COP_DEPTH_15		    0x00000005
#define COP_DEPTH_DITHER_DISABLE    0x00000008
    

#define COP_ALPHA_SRC_BLEND_0	    0x00000000
#define COP_ALPHA_SRC_BLEND_1	    0x00000001
#define COP_ALPHA_SRC_BLEND_SRC_C   0x00000002
#define COP_ALPHA_SRC_BLEND_1_SRC_C 0x00000003
#define COP_ALPHA_SRC_BLEND_SRC_A   0x00000004
#define COP_ALPHA_SRC_BLEND_1_SRC_A 0x00000005
#define COP_ALPHA_SRC_BLEND_DST_A   0x00000006
#define COP_ALPHA_SRC_BLEND_1_DST_A 0x00000007
#define COP_ALPHA_SRC_BLEND_DST_C   0x00000008
#define COP_ALPHA_SRC_BLEND_1_DST_C 0x00000009
#define COP_ALPHA_SRC_BLEND_SAT     0x0000000A
#define COP_ALPHA_SRC_BLEND_BG      0x0000000B

#define COP_ALPHA_DST_BLEND_0	    0x00000000
#define COP_ALPHA_DST_BLEND_1	    0x00000010
#define COP_ALPHA_DST_BLEND_SRC_C   0x00000020
#define COP_ALPHA_DST_BLEND_1_SRC_C 0x00000030
#define COP_ALPHA_DST_BLEND_SRC_A   0x00000040
#define COP_ALPHA_DST_BLEND_1_SRC_A 0x00000050
#define COP_ALPHA_DST_BLEND_DST_A   0x00000060
#define COP_ALPHA_DST_BLEND_1_DST_A 0x00000070
#define COP_ALPHA_DST_BLEND_DST_C   0x00000080
#define COP_ALPHA_DST_BLEND_1_DST_C 0x00000090
#define COP_ALPHA_DST_BLEND_OTHER   0x000000A0

#define COP_ALPHA_RESULT_ALPHA	    0x00100000
#define COP_ALPHA_DEST_ALPHA	    0x00200000
#define COP_ALPHA_SOURCE_ALPHA	    0x00400000
#define COP_ALPHA_WRITE_ENABLE	    0x00800000
#define COP_ALPHA_TEST_ENABLE	    0x01000000
#define COP_ALPHA_BLEND_ENABLE	    0x02000000
#define COP_ALPHA_DEST_VALUE	    0x04000000
#define COP_ALPHA_SOURCE_VALUE	    0x08000000

    VOL32	command;	    /* 0x24 */
#define COP_OP_NULL	    0x00000000
#define COP_OP_LINE	    0x20000000
#define COP_OP_BLT	    0x80000000
#define COP_OP_TEXT	    0x90000000
#define COP_OP_POLY	    0xb0000000
#define COP_OP_POLY2	    0xe0000000
#define COP_SCL_EXPAND	    0x00800000
#define COP_SCL_OPAQUE	    0x00400000
#define COP_SCL_REVERSE	    0x00200000
#define COP_SCL_MONO_OFF    0x001c0000
#define COP_LIT_TEXTURE	    0x00004000
#define COP_BILINEAR	    0x00002000
#define COP_OP_ZBUF	    0x00000800
#define COP_OP_ROP	    0x00000400
#define COP_OP_FG	    0x00000200
#define COP_OP_FB	    0x00000080
#define COP_X_REVERSE	    0x00000004
#define COP_CLIP	    0x00000001
    VOL32	texture_format;	    /* 0x28 */
    CARD8	pad2c[0x4];	    /* 0x2c */
    
    VOL32	clip_bottom_right;  /* 0x30 */
    VOL32	dataIII;	    /* 0x34 */
    VOL32	dataIV;		    /* 0x38 */
    CARD8	pad3c[0x8];	    /* 0x3c */
    
    VOL32	fg;		    /* 0x44 */
    VOL32	bg;		    /* 0x48 */
    CARD8	pad4c[0x4];	    /* 0x4c */
    
    VOL32	pattern_fg;	    /* 0x50 */
    VOL32	pattern_bg;	    /* 0x54 */
    CARD8	pad58[0xc];	    /* 0x58 */

    VOL32	status;		    /* 0x64 */
#define COP_STATUS_BE_BUSY	0x80000000
#define COP_STATUS_DPE_BUSY	0x20000000
#define COP_STATUS_MI_BUSY	0x10000000
#define COP_STATUS_FIFO_BUSY	0x08000000
#define COP_STATUS_WB_BUSY	0x00800000
#define COP_STATUS_Z_FAILED	0x00400000
#define COP_STATUS_EFFECTIVE	0x00200000
#define COP_STATUS_LEFT_VIEW	0x00080000
    
    CARD8	pad68[0x4];	    /* 0x68 */
    
    VOL32	src_offset;	    /* 0x6c */
    VOL32	z_offset;	    /* 0x70 */
    CARD8	pad74[0x4];	    /* 0x74 */
    
    VOL32	display_offset;	    /* 0x78 */
    VOL32	dst_offset;	    /* 0x7c */
    CARD8	pad80[0x34];	    /* 0x80 */
    
    VOL32	semaphore;	    /* 0xb4 */
} Cop;

#define TRI_XY(x,y) ((y) << 16 | (x))

typedef struct _pcmciaCardInfo {
    CARD8		*fb;
    Bool		HP;
    CARD32		memory;
    CARD8		*cop_base;
    Cop			*cop;
    CARD32		*window;
    CARD32		cop_depth;
    CARD32		cop_stride;
} pcmciaCardInfo;
    
#define getpcmciaCardInfo(kd)  ((pcmciaCardInfo *) ((kd)->card->driver))
#define pcmciaCardInfo(kd)	    pcmciaCardInfo	*pcmciac = getpcmciaCardInfo(kd)

typedef struct _pcmciaCursor {
    int		width, height;
    int		xhot, yhot;
    Bool	has_cursor;
    CursorPtr	pCursor;
    Pixel	source, mask;
} pcmciaCursor;

#define PCMCIA_CURSOR_WIDTH	64
#define PCMCIA_CURSOR_HEIGHT	64

typedef struct _pcmciaScreenInfo {
    CARD8	    *cursor_base;
    CARD8	    *screen;
    CARD8	    *off_screen;
    int		    off_screen_size;
    int	            randr;
    LayerPtr        pLayer;
    pcmciaCursor    cursor;
} pcmciaScreenInfo;

#define getpcmciaScreenInfo(kd) ((pcmciaScreenInfo *) ((kd)->screen->driver))
#define pcmciaScreenInfo(kd)    pcmciaScreenInfo *pcmcias = getpcmciaScreenInfo(kd)

Bool
pcmciaDrawInit (ScreenPtr pScreen);

void
pcmciaDrawEnable (ScreenPtr pScreen);

void
pcmciaDrawSync (ScreenPtr pScreen);

void
pcmciaDrawDisable (ScreenPtr pScreen);

void
pcmciaDrawFini (ScreenPtr pScreen);

CARD8
pcmciaReadIndex (pcmciaCardInfo *pcmciac, CARD16 port, CARD8 index);

void
pcmciaWriteIndex (pcmciaCardInfo *pcmciac, CARD16 port, CARD8 index, CARD8 value);

void
pcmciaWriteReg (pcmciaCardInfo *pcmciac, CARD16 port, CARD8 value);

Bool
pcmciaCursorInit (ScreenPtr pScreen);

void
pcmciaCursorEnable (ScreenPtr pScreen);

void
pcmciaCursorDisable (ScreenPtr pScreen);

void
pcmciaCursorFini (ScreenPtr pScreen);

void
pcmciaRecolorCursor (ScreenPtr pScreen, int ndef, xColorItem *pdef);

void
pcmciaUpdateRotatePacked (ScreenPtr	pScreen,
			  shadowBufPtr	pBuf);

typedef struct _pcmciaDisplayModeRec {
    int				Width;
    int				Height;
    int 			Refresh;
    int				Clock;		/* pixel clock freq */
    int				HDisplay;	/* horizontal timing */
    int				HSyncStart;
    int				HSyncEnd;
    int				HTotal;
    int				HSkew;
    int				VDisplay;	/* vertical timing */
    int				VSyncStart;
    int				VSyncEnd;
    int				VTotal;
    int				VScan;
    int				Flags;
} pcmciaDisplayModeRec, *pcmciaDisplayModePtr;

extern KdCardFuncs  pcmciaFuncs;

#endif /* _PCMCIA_H_ */
