/* $XFree86$ */

/* OEM Data for 310/325 series */

const UCHAR SiS310_CRT2DelayCompensation1 = 0x04;   /* 301 */

const UCHAR SiS310_CRT2DelayCompensation2 = 0x00;   /* 301B */

const UCHAR SiS310_CRT2DelayCompensation3 = 0x00;   /* LVDS */

const UCHAR SiS310_LCDDelayCompensation1[] =	    /* 301 */
{
		 0x00,0x00,0x00,    /*   800x600 */
		 0x0b,0x0b,0x0b,    /*  1024x768 */
		 0x08,0x08,0x08,    /* 1280x1024 */
		 0x00,0x00,0x00,    /*   640x480 (unknown) */
		 0x00,0x00,0x00,    /*  1024x600 (unknown) */
		 0x00,0x00,0x00,    /*  1152x864 (unknown) */
		 0x08,0x08,0x08,    /*  1280x960 (guessed) */
		 0x00,0x00,0x00,    /*  1152x768 (unknown) */
		 0x08,0x08,0x08,    /* 1400x1050 */
		 0x08,0x08,0x08,    /*  1280x768  (guessed) */
		 0x00,0x00,0x00,    /* 1600x1200 */
		 0x00,0x00,0x00,    /*   320x480 (unknown) */
		 0x00,0x00,0x00,
		 0x00,0x00,0x00,
		 0x00,0x00,0x00
};

UCHAR SiS310_LCDDelayCompensation2[] =		   /* 30xB,LV,LVX */
{
		 0x01,0x01,0x01,    /*   800x600 */
		 0x01,0x01,0x01,    /*  1024x768 */
		 0x01,0x01,0x01,    /* 1280x1024 */
                 0x01,0x01,0x01,    /*   640x480 (unknown) */
		 0x01,0x01,0x01,    /*  1024x600 (unknown) */
		 0x01,0x01,0x01,    /*  1152x864 (unknown) */
		 0x01,0x01,0x01,    /*  1280x960 (guessed) */
		 0x01,0x01,0x01,    /*  1152x768 (unknown) */
		 0x01,0x01,0x01,    /* 1400x1050 */
		 0x01,0x01,0x01,    /*  1280x768  (guessed) */
		 0x01,0x01,0x01,    /* 1600x1200 */
		 0x02,0x02,0x02,
		 0x02,0x02,0x02,
		 0x02,0x02,0x02,
		 0x02,0x02,0x02
};

const UCHAR SiS310_LCDDelayCompensation3[] =	   /* LVDS */
{
                 0x00,0x00,0x00,    /*   800x600 */
		 0x00,0x00,0x00,    /*  1024x768 */
		 0x00,0x00,0x00,    /* 1280x1024 */
		 0x00,0x00,0x00,    /*   640x480 (unknown) */
		 0x00,0x00,0x00,    /*  1024x600 (unknown) */
		 0x00,0x00,0x00,    /*  1152x864 (unknown) */
		 0x00,0x00,0x00,    /*  1280x960 (guessed) */
		 0x00,0x00,0x00,    /*  1152x768 (unknown) */
		 0x00,0x00,0x00,    /* 1400x1050 */
		 0x00,0x00,0x00,    /*  1280x768  (guessed) */
		 0x00,0x00,0x00,    /* 1600x1200 */
		 0x00,0x00,0x00,
		 0x00,0x00,0x00,
		 0x00,0x00,0x00,
		 0x00,0x00,0x00
};

const UCHAR SiS310_LCDDelayCompensation4[] =	   /* 650 */
{
                 0x01,0x01,0x01,    /*   800x600 (guessed)*/
		 0x01,0x01,0x01,    /*  1024x768 */
		 0x01,0x01,0x01,    /* 1280x1024 */
		 0x01,0x01,0x01,    /*   640x480 (unknown) */
		 0x01,0x01,0x01,    /*  1024x600 (unknown) */
		 0x01,0x01,0x01,    /*  1152x864 (unknown) */
		 0x01,0x01,0x01,    /*  1280x960 (guessed) */
		 0x01,0x01,0x01,    /*  1152x768 (unknown) */
		 0x01,0x01,0x01,    /* 1400x1050 */
		 0x01,0x01,0x01,    /*  1280x768  (guessed) */
		 0x01,0x01,0x01,    /* 1600x1200 */
		 0x01,0x01,0x01,
		 0x01,0x01,0x01,
		 0x01,0x01,0x01,
		 0x01,0x01,0x01
};

const UCHAR SiS310_LCDDelayCompensation5[] =	   /* 650 LVX */
{
                 0x01,0x01,0x01,    /*   800x600 (guessed) */
		 0x01,0x01,0x01,    /*  1024x768 */
		 0x01,0x01,0x01,    /* 1280x1024 */
		 0x01,0x01,0x01,    /*   640x480 (unknown) */
		 0x01,0x01,0x01,    /*  1024x600 (unknown) */
		 0x01,0x01,0x01,    /*  1152x864 (unknown) */
		 0x01,0x01,0x01,    /*  1280x960 (guessed) */
		 0x01,0x01,0x01,    /*  1152x768 (unknown) */
		 0x01,0x01,0x01,    /* 1400x1050 */
		 0x01,0x01,0x01,    /*  1280x768  (guessed) */
		 0x01,0x01,0x01,    /* 1600x1200 */
		 0x01,0x01,0x01,
		 0x01,0x01,0x01,
		 0x01,0x01,0x01,
		 0x01,0x01,0x01
};

const UCHAR SiS310_LCDDelayCompensation6[] =	   /* M650/651 */
{
                 0x33,0x33,0x33,    /*   800x600 (guessed) */
		 0x33,0x33,0x33,    /*  1024x768 */
		 0x33,0x33,0x33,    /* 1280x1024 */
		 0x33,0x33,0x33,    /*   640x480 (unknown) */
		 0x33,0x33,0x33,    /*  1024x600 (unknown) */
		 0x33,0x33,0x33,    /*  1152x864 (unknown) */
		 0x33,0x33,0x33,    /*  1280x960 (guessed) */
		 0x33,0x33,0x33,    /*  1152x768 (unknown) */
		 0x33,0x33,0x33,    /* 1400x1050 */
		 0x33,0x33,0x33,    /*  1280x768  (guessed) */
		 0x33,0x33,0x33,    /* 1600x1200 */
		 0x33,0x33,0x33,
		 0x33,0x33,0x33,
		 0x33,0x33,0x33,
		 0x33,0x33,0x33
};

const UCHAR SiS310_LCDDelayCompensation7[] =	   /* M650/651 301LVX */
{
                 0x33,0x33,0x33,    /*   800x600 (guessed) */
		 0x33,0x33,0x33,    /*  1024x768 */
		 0x33,0x33,0x33,    /* 1280x1024 */
		 0x33,0x33,0x33,    /*   640x480 (unknown) */
		 0x33,0x33,0x33,    /*  1024x600 (unknown) */
		 0x33,0x33,0x33,    /*  1152x864 (unknown) */
		 0x33,0x33,0x33,    /*  1280x960 (guessed) */
		 0x33,0x33,0x33,    /*  1152x768 (unknown) */
		 0x33,0x33,0x33,    /* 1400x1050 */
		 0x33,0x33,0x33,    /*  1280x768  (guessed) */
		 0x33,0x33,0x33,    /* 1600x1200 */
		 0x33,0x33,0x33,
		 0x33,0x33,0x33,
		 0x33,0x33,0x33,
		 0x33,0x33,0x33
};

const UCHAR SiS310_TVDelayCompensation1[] = 		/* 301 */
{
		 0x02,0x02,    /* NTSC Enhanced, Standard */
                 0x02,0x02,    /* PAL */
		 0x08,0x0b     /* HiVision */
};

const UCHAR SiS310_TVDelayCompensation2[] =		/* 301B;LV */
{
		 0x03,0x03,
		 0x03,0x03,
		 0x03,0x03
};

const UCHAR SiS310_TVDelayCompensation3[] =		/* LVDS */
{
		 0x0a,0x0a,
		 0x0a,0x0a,
		 0x0a,0x0a
};

const UCHAR SiS310_TVDelayCompensation4[] =		/* 650 */
{
		 0x03,0x03,
		 0x03,0x03,
		 0x03,0x03
};

const UCHAR SiS310_TVDelayCompensation5[] =		/* 650 LVX */
{
		 0x03,0x03,
		 0x03,0x03,
		 0x03,0x03
};

const UCHAR SiS310_TVDelayCompensation6[] =		/* M650, 651 */
{
		 0x33,0x33,
		 0x33,0x33,
		 0x33,0x33
};

const UCHAR SiS310_TVDelayCompensation7[] =		/* M650, 651, LVX */
{
		 0x33,0x33,
		 0x33,0x33,
		 0x33,0x33
};

const UCHAR SiS310_TVAntiFlick1[3][2] =
{
            {0x4,0x0},
	    {0x4,0x8},
	    {0x0,0x0}
};

const UCHAR SiS310_TVEdge1[3][2] =
{
            {0x0,0x4},
	    {0x0,0x4},
	    {0x0,0x0}
};

const UCHAR SiS310_TVYFilter1[3][8][4] =
{
 {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x25,0x18},
	{0xf1,0x04,0x1f,0x18},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x25,0x18},
	{0xee,0x0c,0x22,0x08},
	{0xeb,0x15,0x25,0xf6}
 },
 {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0xfc,0xfb,0x14,0x2a}
 },
 {
	{0x00,0x00,0x00,0x00},
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x25,0x18},
	{0xf7,0x06,0x19,0x14},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x25,0x18},
	{0xee,0x0c,0x22,0x08}
 }
};

const UCHAR SiS310_TVYFilter2[3][9][7] =
{
 {
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0x01,0x01,0xFC,0xF8,0x08,0x26,0x38},
	{0xFF,0xFF,0xFC,0x00,0x0F,0x22,0x28}
 },
 {
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},   
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0x01,0x01,0xFC,0xF8,0x08,0x26,0x38},
	{0xFF,0xFF,0xFC,0x00,0x0F,0x22,0x28}
 },
 {
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22},
	{0x00,0x00,0x00,0xF4,0xFF,0x1C,0x22}
 }
};

const UCHAR SiS310_PALMFilter[16][4] =
{
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x10,0x18},
	{0xf7,0x06,0x19,0x14},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x15,0x25,0xf6},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18}
};

const UCHAR SiS310_PALNFilter[16][4] =
{
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x10,0x18},
	{0xf7,0x06,0x19,0x14},
	{0x00,0xf4,0x10,0x38},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x15,0x25,0xf6},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18},
	{0xeb,0x04,0x25,0x18}
};


const UCHAR SiS310_PALMFilter2[9][7] =
{
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0x01,0x01,0xFC,0xF8,0x08,0x26,0x38},
	{0xFF,0xFF,0xFC,0x00,0x0F,0x22,0x28}
};

const UCHAR SiS310_PALNFilter2[9][7] =
{
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0xFF,0x03,0x02,0xF6,0xFC,0x27,0x46},
	{0x01,0x02,0xFE,0xF7,0x03,0x27,0x3C},
	{0x01,0x01,0xFC,0xF8,0x08,0x26,0x38},
	{0xFF,0xFF,0xFC,0x00,0x0F,0x22,0x28}
};

const UCHAR SiS310_TVPhaseIncr1[3][2][4] =
{
 {
	{0x21,0xed,0xba,0x08},
	{0x21,0xed,0xba,0x08}
 },
 {
	{0x2a,0x05,0xe3,0x00},
	{0x2a,0x05,0xe3,0x00}
 },
 {
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00}
 }
};

const UCHAR SiS310_TVPhaseIncr2[3][2][4] =
{
 {
	{0x21,0xf0,0x7b,0xd6},   /* 1.10.7w;  1.10.6s: {0x1e,0x8b,0xda,0xa7},   old: {0x21,0xF1,0x37,0x56} */
	{0x21,0xf0,0x7b,0xd6}    /* 1.10.7w;  1.10.6s: {0x1e,0x8b,0xda,0xa7}    old: {0x21,0xF1,0x37,0x56} */
 },
 {
	{0x2a,0x0a,0x41,0xe9},   /* 1.10.7w, 1.10.6s. old: {0x2a,0x09,0x86,0xe9}, */
	{0x2a,0x0a,0x41,0xe9}    /* 1.10.7w, 1.10.6s. old: {0x2a,0x09,0x86,0xe9}  */
 },
 {
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00}
 }
};


