/* $Xorg: Wrap.h,v 1.3 2000/08/17 19:45:50 cpqbld Exp $ */
/*
 * header file for compatibility with something useful
 */

/* $XFree86: xc/lib/Xdmcp/Wrap.h,v 1.2 1998/10/10 15:25:18 dawes Exp $ */

typedef unsigned char auth_cblock[8];	/* block size */

typedef struct auth_ks_struct { auth_cblock _; } auth_wrapper_schedule[16];

extern void _XdmcpWrapperToOddParity (unsigned char *in, unsigned char *out);
