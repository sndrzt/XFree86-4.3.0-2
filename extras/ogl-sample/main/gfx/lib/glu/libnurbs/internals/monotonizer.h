/**************************************************************************
 *									  *
 * 		 Copyright (C) 1999, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/*
 * monotonizer.h
 *
 * $Header: /srv/cvs/cvsup/xc/extras/ogl-sample/main/gfx/lib/glu/libnurbs/internals/monotonizer.h,v 1.1.1.1 2001/01/15 21:48:33 dawes Exp $
 */

#ifndef __glumonotonizer_h_
#define __glumonotonizer_h_

#include "mysetjmp.h"
#include "types.h"

class Arc;
class ArcTessellator;
class Pool;
class Bin;
class PwlArcPool;
class Mapdesc;

class Monotonizer {
    ArcTessellator&	arctessellator;
    Pool&		arcpool;
    Pool&		pwlarcpool;
    jmp_buf&		nurbsJmpBuf;

    enum dir 		{ down, same, up, none };
    void		tessellate( Arc *, REAL );
    void		monotonize( Arc *, Bin & );
    int			isMonotone( Arc * );
public:
    			Monotonizer( ArcTessellator& at, Pool& ap, Pool& p, jmp_buf& j ) 
				: arctessellator(at), arcpool(ap), pwlarcpool(p), nurbsJmpBuf(j) {}
    int			decompose( Bin &, REAL );
};
#endif /* __glumonotonizer_h_ */
