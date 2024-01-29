/* $XFree86$ */

/* OEM Data for 300 series */

const UCHAR SiS300_OEMTVDelay301[8][4] =
{
	{0x08,0x08,0x08,0x08},
	{0x08,0x08,0x08,0x08},
	{0x08,0x08,0x08,0x08},
	{0x2c,0x2c,0x2c,0x2c},
	{0x08,0x08,0x08,0x08},
	{0x08,0x08,0x08,0x08},
	{0x08,0x08,0x08,0x08},
	{0x20,0x20,0x20,0x20}
};

const UCHAR SiS300_OEMTVDelayLVDS[8][4] =
{
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20}
};

const UCHAR SiS300_OEMTVFlicker[8][4] =
{
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00}
};

#if 0   /* TW: Not used */
const UCHAR SiS300_OEMLCDDelay1[12][4]={
	{0x2c,0x2c,0x2c,0x2c},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x2c,0x2c,0x2c,0x2c},
	{0x2c,0x2c,0x2c,0x2c},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x24,0x24,0x24,0x24},
	{0x24,0x24,0x24,0x24},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x24,0x24,0x24,0x24}
};
#endif

/* TW: From 630/301B BIOS */
const UCHAR SiS300_OEMLCDDelay2[64][4] =		 /* for 301/301b/302b/301LV/302LV */
{
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20}
};

/* TW: From 300/301LV BIOS */
const UCHAR SiS300_OEMLCDDelay4[12][4] =
{
	{0x2c,0x2c,0x2c,0x2c},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x2c,0x2c,0x2c,0x2c},
	{0x2c,0x2c,0x2c,0x2c},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x24,0x24,0x24,0x24},
	{0x24,0x24,0x24,0x24},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x24,0x24,0x24,0x24}
};

/* TW: From 300/301LV BIOS */
const UCHAR SiS300_OEMLCDDelay5[32][4] =
{
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
};

/* TW: Added for LVDS */
const UCHAR SiS300_OEMLCDDelay3[64][4] = {	/* For LVDS */
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20},
	{0x20,0x20,0x20,0x20}
};

const UCHAR SiS300_Phase1[8][6][4] =
{
    {
	{0x21,0xed,0x00,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x21,0xed,0x00,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    }
};


const UCHAR SiS300_Phase2[8][6][4] =
{
    {
        {0x21,0xed,0x00,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0xff,0xff,0xff,0xff}
    },
    {
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x21,0xed,0x00,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0x21,0xed,0x8a,0x08},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    },
    {
        {0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0x2a,0x05,0xd3,0x00},
	{0xff,0xff,0xff,0xff}
    }
};

const UCHAR SiS300_Filter1[10][16][4] =
{
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
    },
    {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x10,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xfc,0xfb,0x14,0x2a},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32}
    },
    {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x10,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xfc,0xfb,0x14,0x2a},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32}
    },
    {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x10,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xfc,0xfb,0x14,0x2a},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32}
    },
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
    },
    {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x10,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xfc,0xfb,0x14,0x2a},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32}
    },
    {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x10,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xfc,0xfb,0x14,0x2a},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32}
    },
    {
	{0x00,0xf4,0x10,0x38},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x10,0x32},
	{0xf3,0x00,0x1d,0x20},
	{0x00,0xf4,0x10,0x38},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xfc,0xfb,0x14,0x2a},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32},
	{0xf1,0xf7,0x1f,0x32}
    },
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
    },
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
    },
};

const UCHAR SiS300_Filter2[10][9][7] =
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
    }
};

const UCHAR SiS300_LCDHData[24][11][5] = {
    {
        {0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x65,0xef,0x83,0x5c,0x00},
	{0x65,0xef,0x83,0x5c,0x00},
	{0x8a,0x14,0x00,0x80,0x00},
	{0x8a,0x14,0x00,0x80,0x00}
    },
    {
        {0x4e,0x18,0x90,0x38,0x00},
	{0x4e,0x18,0x90,0x38,0x00},
	{0x8e,0x18,0x28,0x78,0x00},
	{0x8e,0x18,0x28,0x78,0x00},
	{0x8e,0x18,0x28,0x78,0x00},
	{0x4e,0x18,0x90,0x38,0x00},
	{0x4e,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9a,0x56,0x00},
        {0x67,0x11,0x9a,0x56,0x00},
	{0x8a,0x14,0x00,0x80,0x00},
	{0x8a,0x14,0x00,0x80,0x00}
    },
    {
        {0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x65,0xef,0x83,0x5c,0x00},
	{0x65,0xef,0x83,0x5c,0x00},
	{0x8a,0x14,0x00,0x80,0x00},
	{0x8a,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4e,0x18,0x90,0x38,0x00},
	{0x4e,0x18,0x90,0x38,0x00},
	{0x8e,0x18,0x28,0x78,0x00},
	{0x8e,0x18,0x28,0x78,0x00},
	{0x8e,0x18,0x28,0x78,0x00},
	{0x4e,0x18,0x90,0x38,0x00},
	{0x4e,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9a,0x56,0x00},
	{0x67,0x11,0x9a,0x56,0x00},
	{0x8a,0x14,0x00,0x80,0x00},
	{0x8a,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x67,0x91,0x84,0x5e,0x00},
	{0x65,0xef,0x83,0x5c,0x00},
	{0x65,0xef,0x83,0x5c,0x00},
	{0x8a,0x14,0x00,0x80,0x00},
	{0x8a,0x14,0x00,0x80,0x00}
    },
    {
        {0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
    	{0x67,0x91,0x84,0x5E,0x00},
    	{0x67,0x91,0x84,0x5E,0x00},
    	{0x67,0x91,0x84,0x5E,0x00},
    	{0x67,0x91,0x84,0x5E,0x00},
    	{0x65,0xEF,0x83,0x5C,0x00},
    	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x67,0x91,0x84,0x5E,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x65,0xEF,0x83,0x5C,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    },
    {
    	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x8E,0x18,0x28,0x78,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x4E,0x18,0x90,0x38,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x67,0x11,0x9A,0x56,0x00},
	{0x8A,0x14,0x00,0x80,0x00},
	{0x8A,0x14,0x00,0x80,0x00}
    }
};

#if 0
const UCHAR SiS300_LCDVData[24][11][6] = {
    {
        {
    },
};
#endif