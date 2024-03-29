/* $XFree86$ */
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

#ifndef __R200_IOCTL_H__
#define __R200_IOCTL_H__

#ifdef GLX_DIRECT_RENDERING

#include "simple_list.h"
#include "radeon_dri.h"
#include "r200_lock.h"

#include "xf86drm.h"
#include "radeon_common.h"

extern void r200EmitState( r200ContextPtr rmesa );
extern void r200EmitVertexAOS( r200ContextPtr rmesa,
				 GLuint vertex_size,
				 GLuint offset );

extern void r200EmitVbufPrim( r200ContextPtr rmesa,
				GLuint primitive,
				GLuint vertex_nr );

extern void r200FlushElts( r200ContextPtr rmesa );

extern GLushort *r200AllocEltsOpenEnded( r200ContextPtr rmesa,
					   GLuint primitive,
					   GLuint min_nr );

extern void r200EmitAOS( r200ContextPtr rmesa,
			   struct r200_dma_region **regions,
			   GLuint n,
			   GLuint offset );

extern void r200EmitBlit( r200ContextPtr rmesa,
			  GLuint color_fmt,
			  GLuint src_pitch,
			  GLuint src_offset,
			  GLuint dst_pitch,
			  GLuint dst_offset,
			  GLint srcx, GLint srcy,
			  GLint dstx, GLint dsty,
			  GLuint w, GLuint h );

extern void r200EmitWait( r200ContextPtr rmesa, GLuint flags );

extern void r200FlushCmdBuf( r200ContextPtr rmesa, const char * );
extern int r200FlushCmdBufLocked( r200ContextPtr rmesa, const char * caller );

extern void r200RefillCurrentDmaRegion( r200ContextPtr rmesa );

extern void r200AllocDmaRegion( r200ContextPtr rmesa,
				  struct r200_dma_region *region,
				  int bytes, 
				  int alignment );

extern void r200AllocDmaRegionVerts( r200ContextPtr rmesa,
				       struct r200_dma_region *region,
				       int numverts,
				       int vertsize, 
				       int alignment );

extern void r200ReleaseDmaRegion( r200ContextPtr rmesa,
				    struct r200_dma_region *region,
				    const char *caller );

extern void r200CopyBuffer( const __DRIdrawablePrivate *drawable );
extern void r200PageFlip( const __DRIdrawablePrivate *drawable );
extern void r200Flush( GLcontext *ctx );
extern void r200Finish( GLcontext *ctx );
extern void r200WaitForIdleLocked( r200ContextPtr rmesa );
extern void r200WaitForVBlank( r200ContextPtr rmesa );
extern void r200InitIoctlFuncs( GLcontext *ctx );

extern void *r200AllocateMemoryNV( GLsizei size, GLfloat readfreq,
				   GLfloat writefreq, GLfloat priority );
extern void r200FreeMemoryNV( GLvoid *pointer );
extern GLuint r200GetAGPOffset( const GLvoid *pointer );
extern GLboolean r200IsAgpMemory( r200ContextPtr rmesa, const GLvoid *pointer,
				  GLint size );

extern GLuint r200AgpOffsetFromVirtual( r200ContextPtr rmesa, 
					const GLvoid *pointer );

/* ================================================================
 * Helper macros:
 */

/* Close off the last primitive, if it exists.
 */
#define R200_NEWPRIM( rmesa )			\
do {						\
   if ( rmesa->dma.flush )			\
      rmesa->dma.flush( rmesa );	\
} while (0)

/* Can accomodate several state changes and primitive changes without
 * actually firing the buffer.
 */
#define R200_STATECHANGE( rmesa, ATOM )			\
do {								\
   R200_NEWPRIM( rmesa );					\
   move_to_head( &(rmesa->hw.dirty), &(rmesa->hw.ATOM));	\
} while (0)

#define R200_DB_STATE( ATOM )			        \
   memcpy( rmesa->hw.ATOM.lastcmd, rmesa->hw.ATOM.cmd,	\
	   rmesa->hw.ATOM.cmd_size * 4)

static __inline int R200_DB_STATECHANGE( 
   r200ContextPtr rmesa,
   struct r200_state_atom *atom )
{
   if (memcmp(atom->cmd, atom->lastcmd, atom->cmd_size*4)) {
      int *tmp;
      R200_NEWPRIM( rmesa );
      move_to_head( &(rmesa->hw.dirty), atom );
      tmp = atom->cmd; 
      atom->cmd = atom->lastcmd;
      atom->lastcmd = tmp;
      return 1;
   }
   else
      return 0;
}


/* Fire the buffered vertices no matter what.
 */
#define R200_FIREVERTICES( rmesa )			\
do {							\
   if ( rmesa->store.cmd_used || rmesa->dma.flush ) {	\
      r200Flush( rmesa->glCtx );			\
   }							\
} while (0)

/* Alloc space in the command buffer
 */
static __inline char *r200AllocCmdBuf( r200ContextPtr rmesa,
					 int bytes, const char *where )
{
   if (rmesa->store.cmd_used + bytes > R200_CMD_BUF_SZ)
      r200FlushCmdBuf( rmesa, __FUNCTION__ );
   
   {
      char *head = rmesa->store.cmd_buf + rmesa->store.cmd_used;
      rmesa->store.cmd_used += bytes;
      return head;
   }
}




#endif
#endif /* __R200_IOCTL_H__ */
