/* $XFree86: xc/lib/GL/glx/pixel.c,v 1.6 2002/02/22 21:32:54 dawes Exp $ */
/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
*/

#include "packrender.h"

static GLubyte MsbToLsbTable[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

static GLubyte LowBitsMask[9] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
};

static GLubyte HighBitsMask[9] = {
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff,
};

/*
** Return the number of elements per group of a specified format
*/
static GLint ElementsPerGroup(GLenum format, GLenum type) 
{
    /*
    ** To make row length computation valid for image extraction,
    ** packed pixel types assume elements per group equals one.
    */
    switch(type) {
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
      return 1;
    default:
      break;
    }

    switch(format) {
      case GL_RGB:
      case GL_BGR:
	return 3;
      case GL_LUMINANCE_ALPHA:
	return 2;
      case GL_RGBA:
      case GL_BGRA:
      case GL_ABGR_EXT:
	return 4;
      case GL_COLOR_INDEX:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_INTENSITY:
	return 1;
      default:
	return 0;
    }
}

/*
** Return the number of bytes per element, based on the element type (other
** than GL_BITMAP).
*/
static GLint BytesPerElement(GLenum type) 
{
    switch(type) {
      case GL_UNSIGNED_SHORT:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
	return 2;
      case GL_UNSIGNED_BYTE:
      case GL_BYTE:
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
	return 1;
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
	return 4;
      default:
	return 0;
    }
}

/*
** Compute memory required for internal packed array of data of given type
** and format.
*/
GLint __glImageSize(GLsizei width, GLsizei height, GLsizei depth,
		    GLenum format, GLenum type) 
{
    int bytes_per_row;
    int components;

    if (width < 0 || height < 0 || depth < 0) {
	return 0;
    }
    /*
    ** Zero is returned if either format or type are invalid.
    */
    components = ElementsPerGroup(format,type);
    if (type == GL_BITMAP) {
	if (format == GL_COLOR_INDEX || format == GL_STENCIL_INDEX) {
	    bytes_per_row = (width + 7) >> 3;
	} else {
	    return 0;
	}
    } else {
	bytes_per_row = BytesPerElement(type) * width;
    }
    return bytes_per_row * height * depth * components;
}

/*
** Copy bitmap data from clients packed memory applying unpacking modes as the
** data is transfered into the destImage buffer.  Return in modes the
** set of pixel modes that are to be done by the server.
*/
static void FillBitmap(__GLXcontext *gc, GLint width, GLint height,
		       GLenum format, const GLvoid *userdata,
		       GLubyte *destImage)
{
    GLint rowLength = gc->state.storeUnpack.rowLength;
    GLint alignment = gc->state.storeUnpack.alignment;
    GLint skipPixels = gc->state.storeUnpack.skipPixels;
    GLint skipRows = gc->state.storeUnpack.skipRows;
    GLint lsbFirst = gc->state.storeUnpack.lsbFirst;
    GLint elementsLeft, bitOffset, currentByte, nextByte, highBitMask;
    GLint lowBitMask, i;
    GLint components, groupsPerRow, rowSize, padding, elementsPerRow;
    const GLubyte *start, *iter;

    if (rowLength > 0) {
	groupsPerRow = rowLength;
    } else {
	groupsPerRow = width;
    }
    components = ElementsPerGroup(format,GL_BITMAP);
    rowSize = (groupsPerRow * components + 7) >> 3;
    padding = (rowSize % alignment);
    if (padding) {
	rowSize += alignment - padding;
    }
    start = ((const GLubyte*) userdata) + skipRows * rowSize + 
	((skipPixels * components) >> 3);
    bitOffset = (skipPixels * components) & 7;
    highBitMask = LowBitsMask[8-bitOffset];
    lowBitMask = HighBitsMask[bitOffset];
    elementsPerRow = width * components;
    for (i = 0; i < height; i++) {
	elementsLeft = elementsPerRow;
	iter = start;
	while (elementsLeft) {
	    /* First retrieve low bits from current byte */
	    if (lsbFirst) {
		currentByte = MsbToLsbTable[iter[0]];
	    } else {
		currentByte = iter[0];
	    }
	    if (bitOffset) {
		/* Need to read next byte to finish current byte */
		if (elementsLeft > (8 - bitOffset)) {
		    if (lsbFirst) {
			nextByte = MsbToLsbTable[iter[1]];
		    } else {
			nextByte = iter[1];
		    }
		    currentByte = 
			((currentByte & highBitMask) << bitOffset) |
			    ((nextByte & lowBitMask) >> (8 - bitOffset));
		} else {
		    currentByte = 
			((currentByte & highBitMask) << bitOffset);
		}
	    } 
	    if (elementsLeft >= 8) {
		*destImage = currentByte;
		elementsLeft -= 8;
	    } else {
		*destImage = currentByte & HighBitsMask[elementsLeft];
		elementsLeft = 0;
	    }
	    destImage++;
	    iter++;
	}
	start += rowSize;
    }
}

/*
** Extract array from user's data applying all pixel store modes.
** The internal packed array format used has LSB_FIRST = FALSE and 
** ALIGNMENT = 1.
*/
void __glFillImage(__GLXcontext *gc, GLint dim, GLint width, GLint height,
		   GLint depth, GLenum format, GLenum type,
		   const GLvoid *userdata, GLubyte *newimage, GLubyte *modes)
{
    GLint rowLength = gc->state.storeUnpack.rowLength;
    GLint imageHeight = gc->state.storeUnpack.imageHeight;
    GLint alignment = gc->state.storeUnpack.alignment;
    GLint skipPixels = gc->state.storeUnpack.skipPixels;
    GLint skipRows = gc->state.storeUnpack.skipRows;
    GLint skipImages = gc->state.storeUnpack.skipImages;
    GLint swapBytes = gc->state.storeUnpack.swapEndian;
    GLint components, elementSize, rowSize, padding, groupsPerRow, groupSize;
    GLint elementsPerRow, imageSize, rowsPerImage, h, i, j, k;
    const GLubyte *start, *iter, *itera, *iterb, *iterc;
    GLubyte *iter2;

    if (type == GL_BITMAP) {
	FillBitmap(gc, width, height, format, userdata, newimage);
    } else {
	components = ElementsPerGroup(format,type);
	if (rowLength > 0) {
	    groupsPerRow = rowLength;
	} else {
	    groupsPerRow = width;
	}
	if (imageHeight > 0) {
	    rowsPerImage = imageHeight;
	} else {
	    rowsPerImage = height;
	}

	elementSize = BytesPerElement(type);
	groupSize = elementSize * components;
	if (elementSize == 1) swapBytes = 0;

	rowSize = groupsPerRow * groupSize;
	padding = (rowSize % alignment);
	if (padding) {
	    rowSize += alignment - padding;
	}
	imageSize = rowSize * rowsPerImage;
	start = ((const GLubyte*) userdata) + skipImages * imageSize +
		skipRows * rowSize + skipPixels * groupSize;
	iter2 = newimage;
	elementsPerRow = width * components;

	if (swapBytes) {
	    itera = start;
	    for (h = 0; h < depth; h++) {
		iterb = itera;
		for (i = 0; i < height; i++) {
		    iterc = iterb;
		    for (j = 0; j < elementsPerRow; j++) {
			for (k = 1; k <= elementSize; k++) {
			    iter2[k-1] = iterc[elementSize - k];
			}
			iter2 += elementSize;
			iterc += elementSize;
		    }
		    iterb += rowSize;
		}
		itera += imageSize;
	    }
	} else {
	    itera = start;
	    for (h = 0; h < depth; h++) {
		if (rowSize == elementsPerRow * elementSize) {
		    /* Ha!  This is mondo easy! */
		    __GLX_MEM_COPY(iter2, itera,
				   elementsPerRow * elementSize * height);
		    iter2 += elementsPerRow * elementSize * height;
		} else {
		    iter = itera;
		    for (i = 0; i < height; i++) {
			__GLX_MEM_COPY(iter2, iter, elementsPerRow*elementSize);
			iter2 += elementsPerRow * elementSize;
			iter += rowSize;
		    }
		}
		itera += imageSize;
	    }    
	}
    }

    /* Setup store modes that describe what we just did */
    if (modes) {
	if (dim == 3) {
	    GLubyte *pc = modes;
	    __GLX_PUT_CHAR(0,GL_FALSE);
	    __GLX_PUT_CHAR(1,GL_FALSE);
	    __GLX_PUT_CHAR(2,0);
	    __GLX_PUT_CHAR(3,0);
	    __GLX_PUT_LONG(4,0);
	    __GLX_PUT_LONG(8,0);
	    __GLX_PUT_LONG(12,0);
	    __GLX_PUT_LONG(16,0);
	    __GLX_PUT_LONG(20,0);
	    __GLX_PUT_LONG(24,0);
	    __GLX_PUT_LONG(28,0);
	    __GLX_PUT_LONG(32,1);
	} else {
	    GLubyte *pc = modes;
	    __GLX_PUT_CHAR(0,GL_FALSE);
	    __GLX_PUT_CHAR(1,GL_FALSE);
	    __GLX_PUT_CHAR(2,0);
	    __GLX_PUT_CHAR(3,0);
	    __GLX_PUT_LONG(4,0);
	    __GLX_PUT_LONG(8,0);
	    __GLX_PUT_LONG(12,0);
	    __GLX_PUT_LONG(16,1);
	}
    }
}

/*
** Empty a bitmap in LSB_FIRST=GL_FALSE and ALIGNMENT=4 format packing it
** into the clients memory using the pixel store PACK modes.
*/
static void EmptyBitmap(__GLXcontext *gc, GLint width, GLint height,
			GLenum format, const GLubyte *sourceImage,
			GLvoid *userdata)
{
    GLint rowLength = gc->state.storePack.rowLength;
    GLint alignment = gc->state.storePack.alignment;
    GLint skipPixels = gc->state.storePack.skipPixels;
    GLint skipRows = gc->state.storePack.skipRows;
    GLint lsbFirst = gc->state.storePack.lsbFirst;
    GLint components, groupsPerRow, rowSize, padding, elementsPerRow;
    GLint sourceRowSize, sourcePadding, sourceSkip;
    GLubyte *start, *iter;
    GLint elementsLeft, bitOffset, currentByte, highBitMask, lowBitMask;
    GLint writeMask, i;
    GLubyte writeByte;

    components = ElementsPerGroup(format,GL_BITMAP);
    if (rowLength > 0) {
	groupsPerRow = rowLength;
    } else {
	groupsPerRow = width;
    }

    rowSize = (groupsPerRow * components + 7) >> 3;
    padding = (rowSize % alignment);
    if (padding) {
	rowSize += alignment - padding;
    }
    sourceRowSize = (width * components + 7) >> 3;
    sourcePadding = (sourceRowSize % 4);
    if (sourcePadding) {
	sourceSkip = 4 - sourcePadding;
    } else {
	sourceSkip = 0;
    }
    start = ((GLubyte*) userdata) + skipRows * rowSize + 
	((skipPixels * components) >> 3);
    bitOffset = (skipPixels * components) & 7;
    highBitMask = LowBitsMask[8-bitOffset];
    lowBitMask = HighBitsMask[bitOffset];
    elementsPerRow = width * components;
    for (i = 0; i < height; i++) {
	elementsLeft = elementsPerRow;
	iter = start;
	writeMask = highBitMask;
	writeByte = 0;
	while (elementsLeft) {
	    /* Set up writeMask (to write to current byte) */
	    if (elementsLeft + bitOffset < 8) {
		/* Need to trim writeMask */
		writeMask &= HighBitsMask[bitOffset+elementsLeft];
	    }

	    if (lsbFirst) {
		currentByte = MsbToLsbTable[iter[0]];
	    } else {
		currentByte = iter[0];
	    }

	    if (bitOffset) {
		writeByte |= (sourceImage[0] >> bitOffset);
		currentByte = (currentByte & ~writeMask) | 
		    (writeByte & writeMask);
		writeByte = (sourceImage[0] << (8 - bitOffset));
	    } else {
		currentByte = (currentByte & ~writeMask) |
		    (sourceImage[0] & writeMask);
	    }

	    if (lsbFirst) {
		iter[0] = MsbToLsbTable[currentByte];
	    } else {
		iter[0] = currentByte;
	    }

	    if (elementsLeft >= 8) {
		elementsLeft -= 8;
	    } else {
		elementsLeft = 0;
	    }
	    sourceImage++;
	    iter++;
	    writeMask = 0xff;
	}
	if (writeByte) {
	    /* Some data left over that still needs writing */
	    writeMask &= lowBitMask;
	    if (lsbFirst) {
		currentByte = MsbToLsbTable[iter[0]];
	    } else {
		currentByte = iter[0];
	    }
	    currentByte = (currentByte & ~writeMask) | (writeByte & writeMask);
	    if (lsbFirst) {
		iter[0] = MsbToLsbTable[currentByte];
	    } else {
		iter[0] = currentByte;
	    }
	}
	start += rowSize;
	sourceImage += sourceSkip;
    }
}

/*
** Insert array into user's data applying all pixel store modes.
** The packed array format from the server is LSB_FIRST = FALSE,
** SWAP_BYTES = the current pixel storage pack mode, and ALIGNMENT = 4.
** Named __glEmptyImage() because it is the opposite of __glFillImage().
*/
/* ARGSUSED */
void __glEmptyImage(__GLXcontext *gc, GLint dim, GLint width, GLint height, 
		    GLint depth, GLenum format, GLenum type,
		    const GLubyte *sourceImage, GLvoid *userdata)
{
    GLint rowLength = gc->state.storePack.rowLength;
    GLint imageHeight = gc->state.storePack.imageHeight;
    GLint alignment = gc->state.storePack.alignment;
    GLint skipPixels = gc->state.storePack.skipPixels;
    GLint skipRows = gc->state.storePack.skipRows;
    GLint skipImages = gc->state.storePack.skipImages;
    GLint components, elementSize, rowSize, padding, groupsPerRow, groupSize;
    GLint elementsPerRow, sourceRowSize, sourcePadding, h, i;
    GLint imageSize, rowsPerImage;
    GLubyte *start, *iter, *itera;

    if (type == GL_BITMAP) {
	EmptyBitmap(gc, width, height, format, sourceImage, userdata);
    } else {
	components = ElementsPerGroup(format,type);
	if (rowLength > 0) {
	    groupsPerRow = rowLength;
	} else {
	    groupsPerRow = width;
	}
	if (imageHeight > 0) {
	    rowsPerImage = imageHeight;
	} else {
	    rowsPerImage = height;
	}
	elementSize = BytesPerElement(type);
	groupSize = elementSize * components;
	rowSize = groupsPerRow * groupSize;
	padding = (rowSize % alignment);
	if (padding) {
	    rowSize += alignment - padding;
	}
	sourceRowSize = width * groupSize;
	sourcePadding = (sourceRowSize % 4);
	if (sourcePadding) {
	    sourceRowSize += 4 - sourcePadding;
	}
	imageSize = sourceRowSize * rowsPerImage;
	start = ((GLubyte*) userdata) + skipImages * imageSize +
		skipRows * rowSize + skipPixels * groupSize;
	elementsPerRow = width * components;

	itera = start;
	for (h = 0; h < depth; h++) {
	    if ((rowSize == sourceRowSize) && (sourcePadding == 0)) {
		/* Ha!  This is mondo easy! */
		__GLX_MEM_COPY(itera, sourceImage,
			       elementsPerRow * elementSize * height);
		sourceImage += elementsPerRow * elementSize * height;
	    } else {
		iter = itera;
		for (i = 0; i < height; i++) {
		    __GLX_MEM_COPY(iter, sourceImage,
				   elementsPerRow * elementSize);
		    sourceImage += sourceRowSize;
		    iter += rowSize;
		}
	    }
	    itera += imageSize;
	}
    }
}
