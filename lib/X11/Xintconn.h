/* $XFree86$ */

#ifndef _XINTCONN_H_
#define _XINTCONN_H_ 1

#include <X11/Xfuncproto.h>

_XFUNCPROTOBEGIN

/* ConnDis.c */

extern int _XDisconnectDisplay(XtransConnInfo trans_conn);
extern Bool _XSendClientPrefix(Display *dpy, xConnClientPrefix *client,
				char *auth_proto, char *auth_string,
				xConnSetupPrefix *prefix);
extern XtransConnInfo _X11TransConnectDisplay(char *display_name,
				char **fullnamep, int *dpynump,
				int *screenp, char **auth_namep,
				int *auth_namelenp, char **auth_datap,
				int *auth_datalenp);

/* OpenDis.c */
extern void _XFreeDisplayStructure(Display *dpy);

_XFUNCPROTOEND

#endif /* _XINTCONN_H_ */
