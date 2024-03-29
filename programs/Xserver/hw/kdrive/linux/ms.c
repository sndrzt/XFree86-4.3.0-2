/*
Copyright (c) 2001 by Juliusz Chroboczek
Copyright (c) 1999 by Keith Packard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/* $XFree86: xc/programs/Xserver/hw/kdrive/linux/ms.c,v 1.1 2001/08/09 20:45:15 dawes Exp $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"
#include "kdrive.h"
#include "Xpoll.h"
#include <errno.h>
#include <termios.h>

int
MsReadBytes (int fd, char *buf, int len, int min)
{
    int		    n, tot;
    fd_set	    set;
    struct timeval  tv;

    tot = 0;
    while (len)
    {
	n = read (fd, buf, len);
	if (n > 0)
	{
	    tot += n;
	    buf += n;
	    len -= n;
	}
	if (tot % min == 0)
	    break;
	FD_ZERO (&set);
	FD_SET (fd, &set);
	tv.tv_sec = 0;
	tv.tv_usec = 100 * 1000;
	n = select (fd + 1, &set, 0, 0, &tv);
	if (n <= 0)
	    break;
    }
    return tot;
}

void
MsRead (int port, void *closure)
{
    unsigned char   buf[3 * 200];
    unsigned char   *b;
    int		    n;
    int		    dx, dy;
    unsigned long   flags;

    while ((n = MsReadBytes (port, buf, sizeof (buf), 3)) > 0)
    {
	b = buf;
	while (n >= 3)
	{
	    flags = KD_MOUSE_DELTA;

	    if (b[0] & 0x20)
		flags |= KD_BUTTON_1;
	    if (b[0] & 0x10)
		flags |= KD_BUTTON_3;
	    
	    dx = (char)(((b[0] & 0x03) << 6) | (b[1] & 0x3F));
	    dy = (char)(((b[0] & 0x0C) << 4) | (b[2] & 0x3F));
            n -= 3;
            b += 3;
	    KdEnqueueMouseEvent (kdMouseInfo, flags, dx, dy);
	}
    }
}

int MsInputType;

int
MsInit (void)
{
    int port;
    char *device = "/dev/mouse";
    struct termios t;
    int ret;

    if (!MsInputType)
	MsInputType = KdAllocInputType ();
    port = open (device, O_RDWR | O_NONBLOCK);
    if(port < 0) {
        ErrorF("Couldn't open %s (%d)\n", device, (int)errno);
        return 0;
    } else if (port == 0) {
        ErrorF("Opening %s returned 0!  Please complain to Keith.\n",
               device);
	goto bail;
    }

    if(!isatty(port)) {
        ErrorF("%s is not a tty\n", device);
        goto bail;
    }

    ret = tcgetattr(port, &t);
    if(ret < 0) {
        ErrorF("Couldn't tcgetattr(%s): %d\n", device, errno);
        goto bail;
    }
    t.c_iflag &= ~ (IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR |
                   IGNCR | ICRNL | IXON | IXOFF);
    t.c_oflag &= ~ OPOST;
    t.c_lflag &= ~ (ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    t.c_cflag &= ~ (CSIZE | PARENB);
    t.c_cflag |= CS8 | CLOCAL | CSTOPB;

    cfsetispeed (&t, B1200);
    cfsetospeed (&t, B1200);
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;
    ret = tcsetattr(port, TCSANOW, &t);
    if(ret < 0) {
        ErrorF("Couldn't tcsetattr(%s): %d\n", device, errno);
        goto bail;
    }
    if (KdRegisterFd (MsInputType, port, MsRead, (void *) 0))
	return 1;

 bail:
    close(port);
    return 0;
}

void
MsFini (void)
{
    KdUnregisterFds (MsInputType, TRUE);
}

KdMouseFuncs MsMouseFuncs = {
    MsInit,
    MsFini
};
