/* $XFree86: xc/lib/GL/mesa/src/drv/ffb/ffb_tris.h,v 1.1 2000/06/20 05:08:40 dawes Exp $ */

#ifndef _FFB_TRIS_H
#define _FFB_TRIS_H

extern void ffbDDInitRenderFuncs( GLcontext *ctx );


#define _FFB_NEW_RENDER (_DD_NEW_TRI_LIGHT_TWOSIDE |	\
			 _DD_NEW_TRI_OFFSET |		\
			 _DD_NEW_TRI_UNFILLED)

extern void ffbChooseRenderState(GLcontext *ctx);


#define _FFB_NEW_TRIANGLE (_DD_NEW_TRI_SMOOTH |	\
			   _DD_NEW_FLATSHADE |	\
			   _NEW_POLYGON | 	\
			   _NEW_COLOR)

extern void ffbChooseTriangleState(GLcontext *ctx);

extern void ffbFallback( GLcontext *ctx, GLuint bit, GLboolean mode );
#define FALLBACK( ctx, bit, mode ) ffbFallback( ctx, bit, mode )

extern void ffb_fallback_line(GLcontext *, ffb_vertex *, ffb_vertex *);
extern void ffb_fallback_point(GLcontext *, ffb_vertex *);

#endif /* !(_FFB_TRIS_H) */
