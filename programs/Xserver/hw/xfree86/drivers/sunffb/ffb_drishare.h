/* $XFree86$ */

#ifndef _FFB_DRISHARE_H
#define _FFB_DRISHARE_H

typedef struct ffb_dri_state {
	int		flags;
#define FFB_DRI_FFB2		0x00000001
#define FFB_DRI_FFB2PLUS	0x00000002
#define FFB_DRI_PAC1		0x00000004
#define FFB_DRI_PAC2		0x00000008

	/* Indexed by DRI drawable id. */
#define FFB_DRI_NWIDS	64
	unsigned int	wid_table[FFB_DRI_NWIDS];
} ffb_dri_state_t;

#define FFB_DRISHARE(SAREA)	\
	((ffb_dri_state_t *) (((char *)(SAREA)) + sizeof(XF86DRISAREARec)))

typedef struct {
	drmHandle	hFbcRegs;
	drmSize		sFbcRegs;
	drmAddress	mFbcRegs;

	drmHandle	hDacRegs;
	drmSize		sDacRegs;
	drmAddress	mDacRegs;

	drmHandle	hSfb8r;
	drmSize		sSfb8r;
	drmAddress	mSfb8r;

	drmHandle	hSfb32;
	drmSize		sSfb32;
	drmAddress	mSfb32;

	drmHandle	hSfb64;
	drmSize		sSfb64;
	drmAddress	mSfb64;

	/* Fastfill/Pagefill parameters. */
	unsigned char	disable_pagefill;
	int		fastfill_small_area;
	int		pagefill_small_area;
	int		fastfill_height;
	int		fastfill_width;
	int		pagefill_height;
	int		pagefill_width;
	short		Pf_AlignTab[0x800];
} FFBDRIRec, *FFBDRIPtr;

#endif /* !(_FFB_DRISHARE_H) */
