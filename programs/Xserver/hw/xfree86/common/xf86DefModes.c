/* $XFree86: $ */

/* THIS FILE IS AUTOMATICALLY GENERATED -- DO NOT EDIT -- LOOK at
 * modeline2c.pl */

/*
 * Copyright 1999 by The XFree86 Project, Inc.
 *
 * Author: Dirk Hohndel <hohndel@XFree86.Org>
 */

#include "xf86.h"
#include "xf86Config.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"

#include "globals.h"

#define MODEPREFIX(name) NULL, NULL, name, 0,M_T_DEFAULT
#define MODESUFFIX       0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,FALSE,FALSE,0,NULL,0,0.0,0.0

DisplayModeRec xf86DefaultModes [] = {
/* 640x350 @ 85Hz (VESA) hsync: 37.9kHz */
	{MODEPREFIX("640x350"),31500, 640,672,736,832,0, 350,382,385,445,0, V_PHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("320x175"),15750, 320,336,368,416,0, 175,191,192,222,0, V_PHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 640x400 @ 85Hz (VESA) hsync: 37.9kHz */
	{MODEPREFIX("640x400"),31500, 640,672,736,832,0, 400,401,404,445,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("320x200"),15750, 320,336,368,416,0, 200,200,202,222,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 720x400 @ 85Hz (VESA) hsync: 37.9kHz */
	{MODEPREFIX("720x400"),35500, 720,756,828,936,0, 400,401,404,446,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("360x200"),17750, 360,378,414,468,0, 200,200,202,223,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 640x480 @ 60Hz (Industry standard) hsync: 31.5kHz */
	{MODEPREFIX("640x480"),25200, 640,656,752,800,0, 480,490,492,525,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("320x240"),12600, 320,328,376,400,0, 240,245,246,262,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 640x480 @ 72Hz (VESA) hsync: 37.9kHz */
	{MODEPREFIX("640x480"),31500, 640,664,704,832,0, 480,489,491,520,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("320x240"),15750, 320,332,352,416,0, 240,244,245,260,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 640x480 @ 75Hz (VESA) hsync: 37.5kHz */
	{MODEPREFIX("640x480"),31500, 640,656,720,840,0, 480,481,484,500,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("320x240"),15750, 320,328,360,420,0, 240,240,242,250,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 640x480 @ 85Hz (VESA) hsync: 43.3kHz */
	{MODEPREFIX("640x480"),36000, 640,696,752,832,0, 480,481,484,509,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("320x240"),18000, 320,348,376,416,0, 240,240,242,254,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 800x600 @ 56Hz (VESA) hsync: 35.2kHz */
	{MODEPREFIX("800x600"),36000, 800,824,896,1024,0, 600,601,603,625,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("400x300"),18000, 400,412,448,512,0, 300,300,301,312,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 800x600 @ 60Hz (VESA) hsync: 37.9kHz */
	{MODEPREFIX("800x600"),40000, 800,840,968,1056,0, 600,601,605,628,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("400x300"),20000, 400,420,484,528,0, 300,300,302,314,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 800x600 @ 72Hz (VESA) hsync: 48.1kHz */
	{MODEPREFIX("800x600"),50000, 800,856,976,1040,0, 600,637,643,666,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("400x300"),25000, 400,428,488,520,0, 300,318,321,333,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 800x600 @ 75Hz (VESA) hsync: 46.9kHz */
	{MODEPREFIX("800x600"),49500, 800,816,896,1056,0, 600,601,604,625,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("400x300"),24750, 400,408,448,528,0, 300,300,302,312,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 800x600 @ 85Hz (VESA) hsync: 53.7kHz */
	{MODEPREFIX("800x600"),56300, 800,832,896,1048,0, 600,601,604,631,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("400x300"),28150, 400,416,448,524,0, 300,300,302,315,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1024x768i @ 43Hz (industry standard) hsync: 35.5kHz */
	{MODEPREFIX("1024x768"),44900, 1024,1032,1208,1264,0, 768,768,776,817,0, V_PHSYNC | V_PVSYNC | V_INTERLACE, MODESUFFIX},
	{MODEPREFIX("512x384"),22450, 512,516,604,632,0, 384,384,388,408,0, V_PHSYNC | V_PVSYNC | V_INTERLACE | V_DBLSCAN, MODESUFFIX},
/* 1024x768 @ 60Hz (VESA) hsync: 48.4kHz */
	{MODEPREFIX("1024x768"),65000, 1024,1048,1184,1344,0, 768,771,777,806,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("512x384"),32500, 512,524,592,672,0, 384,385,388,403,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1024x768 @ 70Hz (VESA) hsync: 56.5kHz */
	{MODEPREFIX("1024x768"),75000, 1024,1048,1184,1328,0, 768,771,777,806,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("512x384"),37500, 512,524,592,664,0, 384,385,388,403,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1024x768 @ 75Hz (VESA) hsync: 60.0kHz */
	{MODEPREFIX("1024x768"),78800, 1024,1040,1136,1312,0, 768,769,772,800,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("512x384"),39400, 512,520,568,656,0, 384,384,386,400,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1024x768 @ 85Hz (VESA) hsync: 68.7kHz */
	{MODEPREFIX("1024x768"),94500, 1024,1072,1168,1376,0, 768,769,772,808,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("512x384"),47250, 512,536,584,688,0, 384,384,386,404,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1152x864 @ 75Hz (VESA) hsync: 67.5kHz */
	{MODEPREFIX("1152x864"),108000, 1152,1216,1344,1600,0, 864,865,868,900,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("576x432"),54000, 576,608,672,800,0, 432,432,434,450,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1280x960 @ 60Hz (VESA) hsync: 60.0kHz */
	{MODEPREFIX("1280x960"),108000, 1280,1376,1488,1800,0, 960,961,964,1000,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("640x480"),54000, 640,688,744,900,0, 480,480,482,500,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1280x960 @ 85Hz (VESA) hsync: 85.9kHz */
	{MODEPREFIX("1280x960"),148500, 1280,1344,1504,1728,0, 960,961,964,1011,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("640x480"),74250, 640,672,752,864,0, 480,480,482,505,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1280x1024 @ 60Hz (VESA) hsync: 64.0kHz */
	{MODEPREFIX("1280x1024"),108000, 1280,1328,1440,1688,0, 1024,1025,1028,1066,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("640x512"),54000, 640,664,720,844,0, 512,512,514,533,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1280x1024 @ 75Hz (VESA) hsync: 80.0kHz */
	{MODEPREFIX("1280x1024"),135000, 1280,1296,1440,1688,0, 1024,1025,1028,1066,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("640x512"),67500, 640,648,720,844,0, 512,512,514,533,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1280x1024 @ 85Hz (VESA) hsync: 91.1kHz */
	{MODEPREFIX("1280x1024"),157500, 1280,1344,1504,1728,0, 1024,1025,1028,1072,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("640x512"),78750, 640,672,752,864,0, 512,512,514,536,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1600x1200 @ 60Hz (VESA) hsync: 75.0kHz */
	{MODEPREFIX("1600x1200"),162000, 1600,1664,1856,2160,0, 1200,1201,1204,1250,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("800x600"),81000, 800,832,928,1080,0, 600,600,602,625,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1600x1200 @ 65Hz (VESA) hsync: 81.3kHz */
	{MODEPREFIX("1600x1200"),175500, 1600,1664,1856,2160,0, 1200,1201,1204,1250,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("800x600"),87750, 800,832,928,1080,0, 600,600,602,625,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1600x1200 @ 70Hz (VESA) hsync: 87.5kHz */
	{MODEPREFIX("1600x1200"),189000, 1600,1664,1856,2160,0, 1200,1201,1204,1250,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("800x600"),94500, 800,832,928,1080,0, 600,600,602,625,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1600x1200 @ 75Hz (VESA) hsync: 93.8kHz */
	{MODEPREFIX("1600x1200"),202500, 1600,1664,1856,2160,0, 1200,1201,1204,1250,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("800x600"),101250, 800,832,928,1080,0, 600,600,602,625,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1600x1200 @ 85Hz (VESA) hsync: 106.3kHz */
	{MODEPREFIX("1600x1200"),229500, 1600,1664,1856,2160,0, 1200,1201,1204,1250,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("800x600"),114750, 800,832,928,1080,0, 600,600,602,625,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1792x1344 @ 60Hz (VESA) hsync: 83.6kHz */
	{MODEPREFIX("1792x1344"),204800, 1792,1920,2120,2448,0, 1344,1345,1348,1394,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("896x672"),102400, 896,960,1060,1224,0, 672,672,674,697,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1792x1344 @ 75Hz (VESA) hsync: 106.3kHz */
	{MODEPREFIX("1792x1344"),261000, 1792,1888,2104,2456,0, 1344,1345,1348,1417,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("896x672"),130500, 896,944,1052,1228,0, 672,672,674,708,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1856x1392 @ 60Hz (VESA) hsync: 86.3kHz */
	{MODEPREFIX("1856x1392"),218300, 1856,1952,2176,2528,0, 1392,1393,1396,1439,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("928x696"),109150, 928,976,1088,1264,0, 696,696,698,719,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1856x1392 @ 75Hz (VESA) hsync: 112.5kHz */
	{MODEPREFIX("1856x1392"),288000, 1856,1984,2208,2560,0, 1392,1393,1396,1500,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("928x696"),144000, 928,992,1104,1280,0, 696,696,698,750,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1920x1440 @ 60Hz (VESA) hsync: 90.0kHz */
	{MODEPREFIX("1920x1440"),234000, 1920,2048,2256,2600,0, 1440,1441,1444,1500,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("960x720"),117000, 960,1024,1128,1300,0, 720,720,722,750,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1920x1440 @ 75Hz (VESA) hsync: 112.5kHz */
	{MODEPREFIX("1920x1440"),297000, 1920,2064,2288,2640,0, 1440,1441,1444,1500,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("960x720"),148500, 960,1032,1144,1320,0, 720,720,722,750,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 832x624 @ 75Hz (74.55Hz) (fix if the official/Apple spec is different) hsync: 49.725kHz */
	{MODEPREFIX("832x624"),57284, 832,864,928,1152,0, 624,625,628,667,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("416x312"),28642, 416,432,464,576,0, 312,312,314,333,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1152x768 @ 54.8Hz (Titanium PowerBook) hsync: 44.2kHz */
	{MODEPREFIX("1152x768"),64995, 1152,1178,1314,1472,0, 768,771,777,806,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("576x384"),32497, 576,589,657,736,0, 384,385,388,403,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1400x1050 @ 60Hz (VESA GTF) hsync: 65.5kHz */
	{MODEPREFIX("1400x1050"),122000, 1400,1488,1640,1880,0, 1050,1052,1064,1082,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("700x525"),61000, 700,744,820,940,0, 525,526,532,541,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1400x1050 @ 75Hz (VESA GTF) hsync: 82.2kHz */
	{MODEPREFIX("1400x1050"),155800, 1400,1464,1784,1912,0, 1050,1052,1064,1090,0, V_PHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("700x525"),77900, 700,732,892,956,0, 525,526,532,545,0, V_PHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1600x1024 @ 60Hz (SGI 1600SW) hsync: 64.0kHz */
	{MODEPREFIX("1600x1024"),106910, 1600,1620,1640,1670,0, 1024,1027,1030,1067,0, V_NHSYNC | V_NVSYNC, MODESUFFIX},
	{MODEPREFIX("800x512"),53455, 800,810,820,835,0, 512,513,515,533,0, V_NHSYNC | V_NVSYNC | V_DBLSCAN, MODESUFFIX},
/* 1920x1440 @ 85Hz (VESA GTF) hsync: 128.5kHz */
	{MODEPREFIX("1920x1440"),341350, 1920,2072,2288,2656,0, 1440,1441,1444,1512,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("960x720"),170675, 960,1036,1144,1328,0, 720,720,722,756,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 2048x1536 @ 60Hz (VESA GTF) hsync: 95.3kHz */
	{MODEPREFIX("2048x1536"),266950, 2048,2200,2424,2800,0, 1536,1537,1540,1589,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("1024x768"),133475, 1024,1100,1212,1400,0, 768,768,770,794,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 2048x1536 @ 75Hz (VESA GTF) hsync: 120.2kHz */
	{MODEPREFIX("2048x1536"),340480, 2048,2216,2440,2832,0, 1536,1537,1540,1603,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("1024x768"),170240, 1024,1108,1220,1416,0, 768,768,770,801,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
/* 2048x1536 @ 85Hz (VESA GTF) hsync: 137.0kHz */
	{MODEPREFIX("2048x1536"),388040, 2048,2216,2440,2832,0, 1536,1537,1540,1612,0, V_NHSYNC | V_PVSYNC, MODESUFFIX},
	{MODEPREFIX("1024x768"),194020, 1024,1108,1220,1416,0, 768,768,770,806,0, V_NHSYNC | V_PVSYNC | V_DBLSCAN, MODESUFFIX},
	{MODEPREFIX(NULL),0,0,0,0,0,0,0,0,0,0,0,0,MODESUFFIX}
};
