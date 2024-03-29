/* $XFree86: xc/lib/GL/mesa/src/drv/radeon/radeon_vtxtmp_x86.S,v 1.1 2002/10/30 12:51:58 alanh Exp $ */
/**************************************************************************

Copyright 2002 Tungsten Graphics Inc., Cedar Park, Texas.

All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
on the rights to use, copy, modify, merge, publish, distribute, sub
license, and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
ATI, TUNGSTEN GRAPHICS AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

#define GLOBL( x )	\
.globl x;		\
x##:

.data
.align 4
GLOBL( _x86_Normal3fv)
	movl 4(%esp), %eax      /* load 'v' off stack */
	movl (%eax), %ecx       /* load v[0] */
	movl 4(%eax), %edx      /* load v[1] */
	movl 8(%eax), %eax      /* load v[2] */
	movl %ecx, 0      	/* store v[0] to current vertex */
	movl %edx, 4      	/* store v[1] to current vertex */
	movl %eax, 8      	/* store v[2] to current vertex */
	ret
GLOBL ( _x86_Normal3fv_end )

/*
	vertex 3f vertex size 4
*/
	
GLOBL ( _x86_Vertex3f_4 )
	movl	(0), %ecx
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	movl	12(%esp), %eax
	movl	(0), %edx
	movl	%eax, 8(%ecx)
	movl	%edx, 12(%ecx)
	movl	(0), %eax
	addl	$16, %ecx
	dec 	%eax
	movl	%ecx, (0)
	movl	%eax, (0)
	je	.1 
	ret
.1:	jmp	*0
	
GLOBL ( _x86_Vertex3f_4_end )

/*
	vertex 3f vertex size 6
*/
GLOBL ( _x86_Vertex3f_6 )
	push	%edi
	movl	(0), %edi
	movl	8(%esp), %eax
	movl	12(%esp), %edx
	movl	16(%esp), %ecx
	movl	%eax, (%edi)
	movl	%edx, 4(%edi)
	movl	%ecx, 8(%edi)
	movl	(0), %eax
	movl	(0), %edx
	movl	(0), %ecx
	movl	%eax, 12(%edi)
	movl	%edx, 16(%edi)
	movl	%ecx, 20(%edi)
	addl	$24, %edi
	movl	(0), %eax
	movl	%edi, (0)
	dec 	%eax
	pop 	%edi
	movl	%eax, (0)
	je	.2
	ret
.2:	jmp	*0
GLOBL ( _x86_Vertex3f_6_end )
/*
	vertex 3f generic size
*/
GLOBL ( _x86_Vertex3f )
	push	%edi
	push	%esi
	movl	$0, %esi
	movl	(0), %edi
	movl	12(%esp), %eax
	movl	16(%esp), %edx
	movl	20(%esp), %ecx
	movl	%eax, (%edi)
	movl	%edx, 4(%edi)
	movl	%ecx, 8(%edi)
	addl	$12, %edi
	movl	$0, %ecx
	repz
	movsl %ds:(%esi), %es:(%edi)
	movl	(0), %eax
	movl	%edi, (0)
	dec 	%eax
	movl	%eax, (0)
	pop 	%esi
	pop 	%edi
	je  	.3
	ret
.3:	jmp	*0

GLOBL ( _x86_Vertex3f_end )

/*
	Vertex 3fv vertex size 6
*/
GLOBL ( _x86_Vertex3fv_6 )
	movl	(0), %eax
	movl	4(%esp), %ecx
	movl	(%ecx), %edx
	movl	%edx, (%eax)
	movl	4(%ecx), %edx
	movl	8(%ecx), %ecx
	movl	%edx, 4(%eax)
	movl	%ecx, 8(%eax)
	movl	(28), %edx
	movl	(32), %ecx
	movl	%edx, 12(%eax)
	movl	%ecx, 16(%eax)
	movl	(36), %edx
	movl	%edx, 20(%eax)
	addl	$24, %eax
	movl	%eax, 0
	movl	4, %eax
	dec 	%eax
	movl	%eax, 4
	je	.4
	ret
.4:	jmp    *8
	
GLOBL ( _x86_Vertex3fv_6_end )

/*
	Vertex 3fv vertex size 8
*/
GLOBL ( _x86_Vertex3fv_8 )
	movl	(0), %eax
	movl	4(%esp), %ecx
	movl	(%ecx), %edx
	movl	%edx ,(%eax)
	movl	4(%ecx) ,%edx
	movl	8(%ecx) ,%ecx
	movl	%edx, 4(%eax)
	movl	%ecx, 8(%eax)
	movl	(28), %edx
	movl	(32), %ecx
	movl	%edx, 12(%eax)
	movl	%ecx, 16(%eax)
	movl	(28), %edx
	movl	(32), %ecx
	movl	%edx, 20(%eax)
	movl	%ecx, 24(%eax)
	movl	(36), %edx
	movl	%edx, 28(%eax)
	addl	$32, %eax
	movl	%eax, (0)
	movl	4, %eax
	dec	%eax
	movl    %eax, (4)
	je	.5
	ret
.5:	jmp    *8
	
GLOBL ( _x86_Vertex3fv_8_end )

/*
	Vertex 3fv generic vertex size
*/
GLOBL ( _x86_Vertex3fv )
	movl	4(%esp), %edx
	push	%edi
	push	%esi
	movl	(0x1010101), %edi
	movl	(%edx), %eax
	movl	4(%edx), %ecx
	movl	8(%edx), %esi
	movl	%eax, (%edi)
	movl	%ecx, 4(%edi)
	movl	%esi, 8(%edi)
	addl	$12, %edi
	movl	$6, %ecx
	movl	$0x58, %esi
	repz
	movsl %ds:(%esi), %es:(%edi)
	movl	%edi, (0x1010101)
	movl	(0x2020202), %eax
	pop	%esi
	pop	%edi
	dec	%eax
	movl	%eax, (0x2020202)
	je	.6
	ret
.6:	jmp    *0
GLOBL ( _x86_Vertex3fv_end )

/*
	Normal 3f
*/
GLOBL ( _x86_Normal3f )
	movl	$0x12345678, %edx
	movl	4(%esp), %eax
	movl	%eax, (%edx)
	movl	8(%esp), %eax
	movl	%eax, 4(%edx)
	movl	12(%esp), %eax
	movl	%eax, 8(%edx)
	ret
GLOBL ( _x86_Normal3f_end )

/*
	Color 4ubv_ub
*/
GLOBL ( _x86_Color4ubv_ub )
	movl 4(%esp), %eax
	movl $0x12345678, %edx
	movl (%eax), %eax
	movl %eax, (%edx)
	ret
GLOBL ( _x86_Color4ubv_ub_end )

/*
	Color 4ubv 4f
*/
GLOBL ( _x86_Color4ubv_4f )
	push	%ebx
	movl	$0, %edx
	xor	%eax, %eax
	xor	%ecx, %ecx
	movl	8(%esp), %ebx
	movl	(%ebx), %ebx
	mov	%bl, %al
	mov	%bh, %cl
	movl	(%edx,%eax,4),%eax
	movl	(%edx,%ecx,4),%ecx
	movl	%eax, (0xdeadbeaf)
	movl	%ecx, (0xdeadbeaf)
	xor	%eax, %eax
	xor	%ecx, %ecx
	shr	$16, %ebx
	mov	%bl, %al
	mov	%bh, %cl
	movl	(%edx,%eax,4), %eax
	movl	(%edx,%ecx,4), %ecx
	movl	%eax, (0xdeadbeaf)
	movl	%ecx, (0xdeadbeaf)
	pop	%ebx
	ret
GLOBL ( _x86_Color4ubv_4f_end )

/*

	Color4ub_ub
*/
GLOBL( _x86_Color4ub_ub )
	push	%ebx
	movl	8(%esp), %eax
	movl	12(%esp), %edx
	movl	16(%esp), %ecx
	movl	20(%esp), %ebx
	mov	%al, (0)
	mov	%dl, (0)
	mov	%cl, (0)
	mov	%bl, (0)
	pop	%ebx
	ret
GLOBL( _x86_Color4ub_ub_end )

/*
	Color3fv_3f
*/
GLOBL( _x86_Color3fv_3f )
	movl	4(%esp), %eax
	movl	$0, %edx
	movl	(%eax), %ecx
	movl	%ecx, (%edx)
	movl	4(%eax), %ecx
	movl	%ecx, 4(%edx)
	movl	8(%eax), %ecx
	movl	%ecx, 8(%edx)
	ret
GLOBL( _x86_Color3fv_3f_end )

/*
	Color3f_3f
*/
GLOBL( _x86_Color3f_3f )
	movl	$0x12345678, %edx
	movl	4(%esp), %eax
	movl	%eax, (%edx)
	movl	8(%esp,1), %eax
	movl	%eax, 4(%edx)
	movl	12(%esp), %eax
	movl	%eax, 8(%edx)
	ret
GLOBL( _x86_Color3f_3f_end )

/*
	TexCoord2fv
*/

GLOBL( _x86_TexCoord2fv )
	movl	4(%esp), %eax
	movl	$0x12345678, %edx
	movl	(%eax), %ecx
	movl	4(%eax), %eax
	movl	%ecx, (%edx)
	movl	%eax, 4(%edx)
	ret

GLOBL( _x86_TexCoord2fv_end )
/*
	TexCoord2f
*/
GLOBL( _x86_TexCoord2f )
	movl	$0x12345678, %edx
	movl	4(%esp), %eax
	movl	8(%esp), %ecx
	movl	%eax, (%edx)
	movl	%ecx, 4(%edx)
	ret
GLOBL( _x86_TexCoord2f_end )

/*
	MultiTexCoord2fvARB st0/st1
*/
GLOBL( _x86_MultiTexCoord2fvARB )

	movl	4(%esp), %eax
	movl	8(%esp), %ecx
	sub	$0x84c0, %eax
	and	$1, %eax
	movl	(%ecx), %edx
	shl	$3, %eax
	movl	4(%ecx), %ecx
	movl	%edx, 0xdeadbeef(%eax)
	movl	%ecx, 0xdeadbeef(%eax)
	ret
GLOBL( _x86_MultiTexCoord2fvARB_end )
/*
	MultiTexCoord2fvARB
*/

GLOBL( _x86_MultiTexCoord2fvARB_2 )
	movl	4(%esp,1), %eax
	movl	8(%esp,1), %ecx
	sub	$0x84c0, %eax
	and	$0x1, %eax
	movl	0(,%eax,4), %edx
	movl	(%ecx), %eax
	movl	%eax, (%edx)
	movl	4(%ecx), %eax
	movl	%eax, 4(%edx)
	ret

GLOBL( _x86_MultiTexCoord2fvARB_2_end )

/*
	MultiTexCoord2fARB st0/st1
*/
GLOBL( _x86_MultiTexCoord2fARB )
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	sub	$0x84c0, %eax
	movl	12(%esp), %ecx
	and	$1, %eax
	shl	$3, %eax
	movl	%edx, 0xdeadbeef(%eax)
	movl	%ecx, 0xdeadbeef(%eax)
	ret
GLOBL( _x86_MultiTexCoord2fARB_end )

/*
	MultiTexCoord2fARB
*/
GLOBL( _x86_MultiTexCoord2fARB_2 )
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	sub	$0x84c0, %eax
	movl	12(%esp,1), %ecx
	and	$1,%eax
	movl	0(,%eax,4), %eax
	movl	%edx, (%eax)
	movl	%ecx, 4(%eax)
	ret
GLOBL( _x86_MultiTexCoord2fARB_2_end )
