/* $Xorg: policy.c,v 1.4 2001/02/09 02:05:40 xorgcvs Exp $ */
/*

Copyright 1988, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * policy.c.  Implement site-dependent policy for XDMCP connections
 */
/* $XFree86: xc/programs/xdm/policy.c,v 3.7 2001/12/27 20:01:56 dawes Exp $ */

# include "dm.h"
# include "dm_auth.h"

#include <errno.h>

#ifdef XDMCP

# include <X11/X.h>

# include "dm_socket.h"

static ARRAY8 noAuthentication = { (CARD16) 0, (CARD8Ptr) 0 };

typedef struct _XdmAuth {
    ARRAY8  authentication;
    ARRAY8  authorization;
} XdmAuthRec, *XdmAuthPtr;

static XdmAuthRec auth[] = {
#ifdef HASXDMAUTH
{ {(CARD16) 20, (CARD8 *) "XDM-AUTHENTICATION-1"},
  {(CARD16) 19, (CARD8 *) "XDM-AUTHORIZATION-1"},
},
#endif
{ {(CARD16) 0, (CARD8 *) 0},
  {(CARD16) 0, (CARD8 *) 0},
}
};

#define NumAuth	(sizeof auth / sizeof auth[0])

ARRAY8Ptr
ChooseAuthentication (ARRAYofARRAY8Ptr authenticationNames)
{
    int	i, j;

    for (i = 0; i < (int)authenticationNames->length; i++)
	for (j = 0; j < NumAuth; j++)
	    if (XdmcpARRAY8Equal (&authenticationNames->data[i],
				  &auth[j].authentication))
		return &authenticationNames->data[i];
    return &noAuthentication;
}

int
CheckAuthentication (
    struct protoDisplay	*pdpy,
    ARRAY8Ptr		displayID,
    ARRAY8Ptr		name,
    ARRAY8Ptr		data)
{
#ifdef HASXDMAUTH
    if (name->length && !strncmp ((char *)name->data, "XDM-AUTHENTICATION-1", 20))
	return XdmCheckAuthentication (pdpy, displayID, name, data);
#endif
    return TRUE;
}

int
SelectAuthorizationTypeIndex (
    ARRAY8Ptr		authenticationName,
    ARRAYofARRAY8Ptr	authorizationNames)
{
    int	i, j;

    for (j = 0; j < NumAuth; j++)
	if (XdmcpARRAY8Equal (authenticationName,
			      &auth[j].authentication))
	    break;
    if (j < NumAuth)
    {
    	for (i = 0; i < (int)authorizationNames->length; i++)
	    if (XdmcpARRAY8Equal (&authorizationNames->data[i],
				  &auth[j].authorization))
	    	return i;
    }
    for (i = 0; i < (int)authorizationNames->length; i++)
	if (ValidAuthorization (authorizationNames->data[i].length,
				(char *) authorizationNames->data[i].data))
	    return i;
    return -1;
}

/*ARGSUSED*/
int
Willing (
    ARRAY8Ptr	    addr,
    CARD16	    connectionType,
    ARRAY8Ptr	    authenticationName,
    ARRAY8Ptr	    status,
    xdmOpCode	    type)
{
    char	statusBuf[256];
    int		ret;
    
    ret = AcceptableDisplayAddress (addr, connectionType, type);
    if (!ret)
	sprintf (statusBuf, "Display not authorized to connect");
    else
    {
        if (*willing)
	{   FILE *fd = NULL;
	    if ((fd = popen(willing, "r")))
	    {
		char *s = NULL;
		while(!(s = fgets(statusBuf, 256, fd)) && errno == EINTR)
			;
		if (s && strlen(statusBuf) > 0)
			statusBuf[strlen(statusBuf)-1] = 0; /* chop newline */
		else
			snprintf (statusBuf, sizeof(statusBuf), "Willing, but %s failed",willing);
	    }
	    else
	        snprintf (statusBuf, sizeof(statusBuf), "Willing, but %s failed",willing);
	    if (fd) pclose(fd);
	}
	else
	    sprintf (statusBuf, "Willing to manage");
    }
    status->length = strlen (statusBuf);
    status->data = (CARD8Ptr) malloc (status->length);
    if (!status->data)
	status->length = 0;
    else
	memmove( status->data, statusBuf, status->length);
    return ret;
}

/*ARGSUSED*/
ARRAY8Ptr
Accept (
    struct sockaddr *from,
    int		    fromlen,
    CARD16	    displayNumber)
{
    return 0;
}

/*ARGSUSED*/
int
SelectConnectionTypeIndex (
    ARRAY16Ptr	     connectionTypes,
    ARRAYofARRAY8Ptr connectionAddresses)
{
    return 0;
}

#endif /* XDMCP */
