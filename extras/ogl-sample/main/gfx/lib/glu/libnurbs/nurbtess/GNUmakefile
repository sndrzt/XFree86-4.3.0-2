#!gmake
#
# License Applicability. Except to the extent portions of this file are
# made subject to an alternative license as permitted in the SGI Free
# Software License B, Version 1.1 (the "License"), the contents of this
# file are subject only to the provisions of the License. You may not use
# this file except in compliance with the License. You may obtain a copy
# of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
# Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
#
# http://oss.sgi.com/projects/FreeB
#
# Note that, as provided in the License, the Software is distributed on an
# "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
# DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
# CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# Original Code. The Original Code is: OpenGL Sample Implementation,
# Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
# Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
# Copyright in any portions created by third parties is as indicated
# elsewhere herein. All Rights Reserved.
#
# Additional Notice Provisions: The application programming interfaces
# established by SGI in conjunction with the Original Code are The
# OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
# April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
# 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
# Window System(R) (Version 1.3), released October 19, 1998. This software
# was created using the OpenGL(R) version 1.2.1 Sample Implementation
# published by SGI, but has not been independently verified as being
# compliant with the OpenGL(R) version 1.2.1 Specification.
#
# $Date: 2001/01/15 21:48:46 $ $Revision: 1.1.1.1 $
# $Header: /srv/cvs/cvsup/xc/extras/ogl-sample/main/gfx/lib/glu/libnurbs/nurbtess/GNUmakefile,v 1.1.1.1 2001/01/15 21:48:46 dawes Exp $

include $(ROOT)/usr/include/make/commondefs

TARGET = libnurbtess.a

TARGETS = $(TARGET)

LCXXINCS = -I../../include

HFILES = \
	definitions.h \
	directedLine.h \
	gridWrap.h \
	monoTriangulation.h \
	partitionY.h \
	polyDBG.h \
	polyUtil.h \
	primitiveStream.h \
	quicksort.h \
	rectBlock.h \
	sampleComp.h \
	sampleCompBot.h \
	sampleCompRight.h \
	sampleCompTop.h \
	sampleMonoPoly.h \
	sampledLine.h \
	searchTree.h \
	zlassert.h \
	partitionX.h \
	monoChain.h \
	monoPolyPart.h \
	$(NULL)

CCFILES = \
	directedLine.cc \
	gridWrap.cc \
	monoTriangulation.cc \
	partitionY.cc \
	polyDBG.cc \
	polyUtil.cc \
	primitiveStream.cc \
	quicksort.cc \
	rectBlock.cc \
	sampleComp.cc \
	sampleCompBot.cc \
	sampleCompRight.cc \
	sampleCompTop.cc \
	sampleMonoPoly.cc \
	sampledLine.cc \
	searchTree.cc \
	partitionX.cc \
	monoChain.cc \
	monoPolyPart.cc \
	$(NULL)

default libs libs_install install: $(TARGET)

headers headers_install apps:

$(TARGET): $(OBJECTS)
	$(AR) crl $@ $(OBJECTS);

include $(COMMONRULES)
