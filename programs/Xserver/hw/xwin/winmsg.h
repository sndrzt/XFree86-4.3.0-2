/*
 *Copyright (C) 1994-2000 The XFree86 Project, Inc. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE XFREE86 PROJECT BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of the XFree86 Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors: Alexander Gottwald	
 */
/* $XFree86$ */

#ifndef __WIN_MSG_H__
#define __WIN_MSG_H__


#define __msg_name(name,string) name
#define __msg(name,string) __msg_name(name,string)
#define _msg(name,string) __msg(name,string),

#define MESSAGE_STRINGS \
    _msg(X_PROBED,"(--)"            /* Value was probed */)\
    _msg(X_CONFIG,"(**)"            /* Value was given in the config file */)\
    _msg(X_DEFAULT,"(==)"           /* Value is a default */)\
    _msg(X_CMDLINE,"(++)"           /* Value was given on the command line */)\
    _msg(X_NOTICE,"(!!)"            /* Notice */) \
    _msg(X_ERROR,"(EE)"             /* Error message */) \
    _msg(X_WARNING,"(WW)"           /* Warning message */) \
    _msg(X_INFO,"(II)"              /* Informational message */) \
    _msg(X_UNKNOWN,"(?""?)"         /* Unknown, trigraph fix */) \
    _msg(X_NONE,NULL                /* No prefix */) \
    __msg(X_NOT_IMPLEMENTED,"(NI)"  /* Not implemented */)

typedef enum
{
  MESSAGE_STRINGS
}
MessageType;


/*
 * Function prototypes
 */

void winDrvMsgVerb (int scrnIndex,
		    MessageType type, int verb, const char *format, ...);
void winDrvMsg (int scrnIndex, MessageType type, const char *format, ...);
void winMsgVerb (MessageType type, int verb, const char *format, ...);
void winMsg (MessageType type, const char *format, ...);

void winErrorFVerb (int verb, const char *format, ...);

#endif
