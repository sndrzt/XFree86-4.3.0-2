/* $Xorg: access.c,v 1.5 2001/02/09 02:05:23 xorgcvs Exp $ */
/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: xc/programs/Xserver/os/access.c,v 3.41 2002/05/31 18:46:05 dawes Exp $ */

#ifdef WIN32
#include <X11/Xwinsock.h>
#endif

#include <stdio.h>
#include <X11/Xtrans.h>
#include <X11/Xauth.h>
#include <X.h>
#include <Xproto.h>
#include "misc.h"
#include "site.h"
#include <errno.h>
#include <sys/types.h>
#ifndef WIN32
#ifndef Lynx
#include <sys/socket.h>
#else
#include <socket.h>
#endif
#include <sys/ioctl.h>
#include <ctype.h>

#if defined(TCPCONN) || defined(STREAMSCONN) || defined(ISC) || defined(SCO)
#include <netinet/in.h>
#endif /* TCPCONN || STREAMSCONN || ISC || SCO */
#ifdef DNETCONN
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif


#if defined(DGUX)
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/param.h>
#include <sys/sockio.h>
#endif


#if defined(hpux) || defined(QNX4)
# include <sys/utsname.h>
# ifdef HAS_IFREQ
#  include <net/if.h>
# endif
#else
#if defined(SVR4) ||  (defined(SYSV) && defined(i386)) || defined(__GNU__)
# include <sys/utsname.h>
#endif
#if defined(SYSV) &&  defined(i386)
# include <sys/stream.h>
# ifdef ISC
#  include <sys/stropts.h>
#  include <sys/sioctl.h>
# endif /* ISC */
#endif
#ifdef __GNU__
#undef SIOCGIFCONF
#include <netdb.h>
#else /*!__GNU__*/
# include <net/if.h>
#endif /*__GNU__ */
#endif /* hpux */

#ifdef SVR4
#ifndef SCO
#include <sys/sockio.h>
#endif
#include <sys/stropts.h>
#endif

#include <netdb.h>

#ifdef CSRG_BASED
#include <sys/param.h>
#if (BSD >= 199103)
#define VARIABLE_IFREQ
#endif
#endif

#ifdef BSD44SOCKETS
#ifndef VARIABLE_IFREQ
#define VARIABLE_IFREQ
#endif
#endif

#ifdef HAS_GETIFADDRS
#include <ifaddrs.h>
#endif

#endif /* WIN32 */

#ifndef PATH_MAX
#ifndef Lynx
#include <sys/param.h>
#else
#include <param.h>
#endif 
#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX 1024
#endif
#endif
#endif 

#define X_INCLUDE_NETDB_H
#include <X11/Xos_r.h>

#include "dixstruct.h"
#include "osdep.h"

#ifdef XCSECURITY
#define _SECURITY_SERVER
#include "extensions/security.h"
#endif

Bool defeatAccessControl = FALSE;

#define acmp(a1, a2, len) memcmp((char *)(a1), (char *)(a2), len)
#define acopy(a1, a2, len) memmove((char *)(a2), (char *)(a1), len)
#define addrEqual(fam, address, length, host) \
			 ((fam) == (host)->family &&\
			  (length) == (host)->len &&\
			  !acmp (address, (host)->addr, length))

static int ConvertAddr(struct sockaddr */*saddr*/,
		       int */*len*/,
		       pointer */*addr*/);

static int CheckAddr(int /*family*/,
		     pointer /*pAddr*/,
		     unsigned /*length*/);

static Bool NewHost(int /*family*/,
		    pointer /*addr*/,
		    int /*len*/);

typedef struct _host {
	short		family;
	short		len;
	unsigned char	*addr;
	struct _host *next;
} HOST;

#define MakeHost(h,l)	(h)=(HOST *) xalloc(sizeof *(h)+(l));\
                        if((h))\
			(h)->addr=(unsigned char *) ((h) + 1);
#define FreeHost(h)	xfree(h)
static HOST *selfhosts = NULL;
static HOST *validhosts = NULL;
static int AccessEnabled = DEFAULT_ACCESS_CONTROL;
static int LocalHostEnabled = FALSE;
static int UsingXdmcp = FALSE;


/*
 * called when authorization is not enabled to add the
 * local host to the access list
 */

void
EnableLocalHost (void)
{
    if (!UsingXdmcp)
    {
	LocalHostEnabled = TRUE;
	AddLocalHosts ();
    }
}

/*
 * called when authorization is enabled to keep us secure
 */
void
DisableLocalHost (void)
{
    HOST *self;

    LocalHostEnabled = FALSE;
    for (self = selfhosts; self; self = self->next)
	(void) RemoveHost ((ClientPtr)NULL, self->family, self->len, (pointer)self->addr);
}

/*
 * called at init time when XDMCP will be used; xdmcp always
 * adds local hosts manually when needed
 */

void
AccessUsingXdmcp (void)
{
    UsingXdmcp = TRUE;
    LocalHostEnabled = FALSE;
}


#if ((defined(SVR4) && !defined(DGUX) && !defined(SCO325) && !defined(sun) && !defined(NCR)) || defined(ISC)) && defined(SIOCGIFCONF)

/* Deal with different SIOCGIFCONF ioctl semantics on these OSs */

static int
ifioctl (int fd, int cmd, char *arg)
{
    struct strioctl ioc;
    int ret;

    bzero((char *) &ioc, sizeof(ioc));
    ioc.ic_cmd = cmd;
    ioc.ic_timout = 0;
    if (cmd == SIOCGIFCONF)
    {
	ioc.ic_len = ((struct ifconf *) arg)->ifc_len;
	ioc.ic_dp = ((struct ifconf *) arg)->ifc_buf;
#ifdef ISC
	/* SIOCGIFCONF is somewhat brain damaged on ISC. The argument
	 * buffer must contain the ifconf structure as header. Ifc_req
	 * is also not a pointer but a one element array of ifreq
	 * structures. On return this array is extended by enough
	 * ifreq fields to hold all interfaces. The return buffer length
	 * is placed in the buffer header.
	 */
        ((struct ifconf *) ioc.ic_dp)->ifc_len =
                                         ioc.ic_len - sizeof(struct ifconf);
#endif
    }
    else
    {
	ioc.ic_len = sizeof(struct ifreq);
	ioc.ic_dp = arg;
    }
    ret = ioctl(fd, I_STR, (char *) &ioc);
    if (ret >= 0 && cmd == SIOCGIFCONF)
#ifdef SVR4
	((struct ifconf *) arg)->ifc_len = ioc.ic_len;
#endif
#ifdef ISC
    {
	((struct ifconf *) arg)->ifc_len =
				 ((struct ifconf *)ioc.ic_dp)->ifc_len;
	((struct ifconf *) arg)->ifc_buf = 
			(caddr_t)((struct ifconf *)ioc.ic_dp)->ifc_req;
    }
#endif
    return(ret);
}
#else /* Case DGUX, sun, SCO325 NCR and others  */
#define ifioctl ioctl
#endif /* ((SVR4 && !DGUX !sun !SCO325 !NCR) || ISC) && SIOCGIFCONF */

/*
 * DefineSelf (fd):
 *
 * Define this host for access control.  Find all the hosts the OS knows about 
 * for this fd and add them to the selfhosts list.
 */

#ifdef WINTCP /* NCR Wollongong based TCP */

#include <sys/un.h>
#include <stropts.h>
#include <tiuser.h>

#include <sys/stream.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/in.h>
#include <netinet/in_var.h>

void
DefineSelf (int fd)
{
    /*
     * The Wolongong drivers used by NCR SVR4/MP-RAS don't understand the
     * socket IO calls that most other drivers seem to like. Because of
     * this, this routine must be special cased for NCR. Eventually,
     * this will be cleared up.
     */

    struct ipb ifnet;
    struct in_ifaddr ifaddr;
    struct strioctl str;
    unsigned char *addr;
    register HOST *host;
    int	family, len;

    if ((fd = open ("/dev/ip", O_RDWR, 0 )) < 0)
        Error ("Getting interface configuration (1)");

    /* Indicate that we want to start at the begining */
    ifnet.ib_next = (struct ipb *) 1;

    while (ifnet.ib_next)
    {
	str.ic_cmd = IPIOC_GETIPB;
	str.ic_timout = 0;
	str.ic_len = sizeof (struct ipb);
	str.ic_dp = (char *) &ifnet;

	if (ioctl (fd, (int) I_STR, (char *) &str) < 0)
	{
	    close (fd);
	    Error ("Getting interface configuration (2)");
	}

	ifaddr.ia_next = (struct in_ifaddr *) ifnet.if_addrlist;
	str.ic_cmd = IPIOC_GETINADDR;
	str.ic_timout = 0;
	str.ic_len = sizeof (struct in_ifaddr);
	str.ic_dp = (char *) &ifaddr;

	if (ioctl (fd, (int) I_STR, (char *) &str) < 0)
	{
	    close (fd);
	    Error ("Getting interface configuration (3)");
	}

	len = sizeof(struct sockaddr_in);
	family = ConvertAddr (IA_SIN(&ifaddr), &len, (pointer *)&addr);
        if (family == -1 || family == FamilyLocal)
	    continue;
        for (host = selfhosts;
 	     host && !addrEqual (family, addr, len, host);
	     host = host->next)
	    ;
        if (host)
	    continue;
	MakeHost(host,len)
	if (host)
	{
	    host->family = family;
	    host->len = len;
	    acopy(addr, host->addr, len);
	    host->next = selfhosts;
	    selfhosts = host;
	}
#ifdef XDMCP
        {
	    struct sockaddr broad_addr;

	    /*
	     * If this isn't an Internet Address, don't register it.
	     */
	    if (family != FamilyInternet)
		continue;

	    /*
 	     * ignore 'localhost' entries as they're not useful
	     * on the other end of the wire
	     */
	    if (len == 4 &&
		addr[0] == 127 && addr[1] == 0 &&
		addr[2] == 0 && addr[3] == 1)
		continue;

	    XdmcpRegisterConnection (family, (char *)addr, len);


#define IA_BROADADDR(ia) ((struct sockaddr_in *)(&((struct in_ifaddr *)ia)->ia_broadaddr))

	    XdmcpRegisterBroadcastAddress (
		(struct sockaddr_in *) IA_BROADADDR(&ifaddr));

#undef IA_BROADADDR
	}
#endif /* XDMCP */
    }

    close(fd);

    /*
     * add something of FamilyLocalHost
     */
    for (host = selfhosts;
	 host && !addrEqual(FamilyLocalHost, "", 0, host);
	 host = host->next);
    if (!host)
    {
	MakeHost(host, 0);
	if (host)
	{
	    host->family = FamilyLocalHost;
	    host->len = 0;
	    acopy("", host->addr, 0);
	    host->next = selfhosts;
	    selfhosts = host;
	}
    }
}

#else /* WINTCP */

#if !defined(SIOCGIFCONF) || (defined (hpux) && ! defined (HAS_IFREQ)) || defined(QNX4)
void
DefineSelf (int fd)
{
#if !defined(TCPCONN) && !defined(STREAMSCONN) && !defined(UNIXCONN) && !defined(MNX_TCPCONN)
    return;
#else
    register int n;
    int	len;
    caddr_t	addr;
    int		family;
    register HOST	*host;

    struct utsname name;
    register struct hostent  *hp;

    union {
	struct  sockaddr   sa;
	struct  sockaddr_in  in;
    } saddr;
	
    struct	sockaddr_in	*inetaddr;
    struct sockaddr_in broad_addr;
#ifdef XTHREADS_NEEDS_BYNAMEPARAMS
    _Xgethostbynameparams hparams;
#endif

    /* Why not use gethostname()?  Well, at least on my system, I've had to
     * make an ugly kernel patch to get a name longer than 8 characters, and
     * uname() lets me access to the whole string (it smashes release, you
     * see), whereas gethostname() kindly truncates it for me.
     */
#ifndef QNX4
    uname(&name);
#else
    /* QNX4's uname returns node number in name.nodename, not the hostname
       have to overwrite it */
    char hname[1024];
    gethostname(hname, 1024);
    name.nodename = hname;
#endif

    hp = _XGethostbyname(name.nodename, hparams);
    if (hp != NULL)
    {
	saddr.sa.sa_family = hp->h_addrtype;
	inetaddr = (struct sockaddr_in *) (&(saddr.sa));
	acopy ( hp->h_addr, &(inetaddr->sin_addr), hp->h_length);
	len = sizeof(saddr.sa);
	family = ConvertAddr ( &(saddr.sa), &len, (pointer *)&addr);
	if ( family != -1 && family != FamilyLocal )
	{
	    for (host = selfhosts;
		 host && !addrEqual (family, addr, len, host);
		 host = host->next) ;
	    if (!host)
	    {
		/* add this host to the host list.	*/
		MakeHost(host,len)
		if (host)
		{
		    host->family = family;
		    host->len = len;
		    acopy ( addr, host->addr, len);
		    host->next = selfhosts;
		    selfhosts = host;
		}
#ifdef XDMCP
		/*
		 *  If this is an Internet Address, but not the localhost
		 *  address (127.0.0.1), register it.
		 */
		if (family == FamilyInternet &&
		    !(len == 4 && addr[0] == 127 && addr[1] == 0 &&
		      addr[2] == 0 && addr[3] == 1)
		   )
		{
		    XdmcpRegisterConnection (family, (char *)addr, len);
		    broad_addr = *inetaddr;
		    ((struct sockaddr_in *) &broad_addr)->sin_addr.s_addr =
			htonl (INADDR_BROADCAST);
		    XdmcpRegisterBroadcastAddress ((struct sockaddr_in *)
						   &broad_addr);
		}
#endif /* XDMCP */
	    }
	}
    }
    /*
     * now add a host of family FamilyLocalHost...
     */
    for (host = selfhosts;
	 host && !addrEqual(FamilyLocalHost, "", 0, host);
	 host = host->next);
    if (!host)
    {
	MakeHost(host, 0);
	if (host)
	{
	    host->family = FamilyLocalHost;
	    host->len = 0;
	    acopy("", host->addr, 0);
	    host->next = selfhosts;
	    selfhosts = host;
	}
    }
#endif /* !TCPCONN && !STREAMSCONN && !UNIXCONN && !MNX_TCPCONN */
}

#else

#ifdef VARIABLE_IFREQ
#define ifr_size(p) (sizeof (struct ifreq) + \
		     (p->ifr_addr.sa_len > sizeof (p->ifr_addr) ? \
		      p->ifr_addr.sa_len - sizeof (p->ifr_addr) : 0))
#define ifraddr_size(a) (a.sa_len)
#else
#ifdef QNX4
#define ifr_size(p) (p->ifr_addr.sa_len + IFNAMSIZ)
#define ifraddr_size(a) (a.sa_len)
#else
#define ifr_size(p) (sizeof (struct ifreq))
#define ifraddr_size(a) (sizeof (a))
#endif
#endif

#ifdef DEF_SELF_DEBUG
#include <arpa/inet.h>
#endif

void
DefineSelf (int fd)
{
#ifndef HAS_GETIFADDRS
    char		buf[2048], *cp, *cplim;
    struct ifconf	ifc;
    register struct ifreq *ifr;
#else 
    struct ifaddrs *	ifap, *ifr;
#endif
    int 		len;
    unsigned char *	addr;
    int 		family;
    register HOST 	*host;
    
#ifdef DNETCONN
    struct dn_naddr *dnaddr = getnodeadd();
    /*
     * AF_DECnet may not be listed in the interface list.  Instead use
     * the supported library call to find out the local address (if any).
     */
    if (dnaddr)
    {    
	addr = (unsigned char *) dnaddr;
	len = dnaddr->a_len + sizeof(dnaddr->a_len);
	family = FamilyDECnet;
	for (host = selfhosts;
	     host && !addrEqual (family, addr, len, host);
	     host = host->next)
	    ;
        if (!host)
	{
	    MakeHost(host,len)
	    if (host)
	    {
		host->family = family;
		host->len = len;
		acopy(addr, host->addr, len);
		host->next = selfhosts;
		selfhosts = host;
	    }
	}
    }
#endif /* DNETCONN */
#ifndef HAS_GETIFADDRS
    ifc.ifc_len = sizeof (buf);
    ifc.ifc_buf = buf;
    if (ifioctl (fd, SIOCGIFCONF, (pointer) &ifc) < 0)
        Error ("Getting interface configuration (4)");

#ifdef ISC
#define IFC_IFC_REQ (struct ifreq *) ifc.ifc_buf
#else
#define IFC_IFC_REQ ifc.ifc_req
#endif /* ISC */

    cplim = (char *) IFC_IFC_REQ + ifc.ifc_len;
    
    for (cp = (char *) IFC_IFC_REQ; cp < cplim; cp += ifr_size (ifr))
    {
	ifr = (struct ifreq *) cp;
	len = ifraddr_size (ifr->ifr_addr);
#ifdef DNETCONN
	/*
	 * DECnet was handled up above.
	 */
	if (ifr->ifr_addr.sa_family == AF_DECnet)
	    continue;
#endif /* DNETCONN */
	family = ConvertAddr (&ifr->ifr_addr, &len, (pointer *)&addr);
        if (family == -1 || family == FamilyLocal)
	    continue;
#ifdef DEF_SELF_DEBUG
	if (family == FamilyInternet) 
	    ErrorF("Xserver: DefineSelf(): ifname = %s, addr = %d.%d.%d.%d\n",
		   ifr->ifr_name, addr[0], addr[1], addr[2], addr[3]);
#endif /* DEF_SELF_DEBUG */
        for (host = selfhosts;
 	     host && !addrEqual (family, addr, len, host);
	     host = host->next)
	    ;
        if (host)
	    continue;
	MakeHost(host,len)
	if (host)
	{
	    host->family = family;
	    host->len = len;
	    acopy(addr, host->addr, len);
	    host->next = selfhosts;
	    selfhosts = host;
	}
#ifdef XDMCP
	{
	    struct sockaddr broad_addr;

	    /*
	     * If this isn't an Internet Address, don't register it.
	     */
	    if (family != FamilyInternet)
		continue;

	    /*
 	     * ignore 'localhost' entries as they're not useful
	     * on the other end of the wire
	     */
	    if (len == 4 &&
		addr[0] == 127 && addr[1] == 0 &&
		addr[2] == 0 && addr[3] == 1)
		continue;

	    XdmcpRegisterConnection (family, (char *)addr, len);
	    broad_addr = ifr->ifr_addr;
	    ((struct sockaddr_in *) &broad_addr)->sin_addr.s_addr =
		htonl (INADDR_BROADCAST);
#ifdef SIOCGIFBRDADDR
	    {
	    	struct ifreq    broad_req;
    
	    	broad_req = *ifr;
		if (ifioctl (fd, SIOCGIFFLAGS, (char *) &broad_req) != -1 &&
		    (broad_req.ifr_flags & IFF_BROADCAST) &&
		    (broad_req.ifr_flags & IFF_UP)
		    )
		{
		    broad_req = *ifr;
		    if (ifioctl (fd, SIOCGIFBRDADDR, &broad_req) != -1)
			broad_addr = broad_req.ifr_addr;
		    else
			continue;
		}
		else
		    continue;
	    }
#endif /* SIOCGIFBRDADDR */
#ifdef DEF_SELF_DEBUG
	    ErrorF("Xserver: DefineSelf(): ifname = %s, baddr = %s\n",
		   ifr->ifr_name,
	           inet_ntoa(((struct sockaddr_in *) &broad_addr)->sin_addr));
#endif /* DEF_SELF_DEBUG */
	    XdmcpRegisterBroadcastAddress ((struct sockaddr_in *) &broad_addr);
	}
#endif /* XDMCP */
    }
#else /* HAS_GETIFADDRS */
    if (getifaddrs(&ifap) < 0) {
	ErrorF("Warning: getifaddrs returns %s\n", strerror(errno));
	return;
    }
    for (ifr = ifap; ifr != NULL; ifr = ifr->ifa_next) {
#ifdef DNETCONN
	if (ifr->ifa_addr.sa_family == AF_DECnet) 
	    continue;
#endif /* DNETCONN */
	family = ConvertAddr(ifr->ifa_addr, &len, (pointer *)&addr);
	if (family == -1 || family == FamilyLocal) 
	    continue;
#ifdef DEF_SELF_DEBUG
	if (family == FamilyInternet) 
	    ErrorF("Xserver: DefineSelf(): ifname = %s, addr = %d.%d.%d.%d\n",
		   ifr->ifa_name, addr[0], addr[1], addr[2], addr[3]);
#endif /* DEF_SELF_DEBUG */
	for (host = selfhosts; 
	     host != NULL && !addrEqual(family, addr, len, host);
	     host = host->next) 
	    ;
	if (host != NULL) 
	    continue;
	MakeHost(host, len);
	if (host != NULL) {
	    host->family = family;
	    host->len = len;
	    acopy(addr, host->addr, len);
	    host->next = selfhosts;
	    selfhosts = host;
	}
#ifdef XDMCP
	{
	    struct sockaddr broad_addr;
	    /*
	     * If this isn't an Internet Address, don't register it.
	     */
	    if (family != FamilyInternet) 
		continue;
	    /* 
	     * ignore 'localhost' entries as they're not usefule
	     * on the other end of the wire
	     */
	    if (len == 4 && 
		addr[0] == 127 && addr[1] == 0 &&
		addr[2] == 0 && addr[2] == 1) 
		continue;
	    XdmcpRegisterConnection(family, (char *)addr, len);
	    if ((ifr->ifa_flags & IFF_BROADCAST) &&
		(ifr->ifa_flags & IFF_UP))
		broad_addr = *ifr->ifa_broadaddr;
	    else
		continue;
#ifdef DEF_SELF_DEBUG
	    ErrorF("Xserver: DefineSelf(): ifname = %s, baddr = %s\n",
		   ifr->ifa_name,
	           inet_ntoa(((struct sockaddr_in *) &broad_addr)->sin_addr));
#endif /* DEF_SELF_DEBUG */
	    XdmcpRegisterBroadcastAddress((struct sockaddr_in *)
					  &broad_addr);
	}
#endif /* XDMCP */
		
    } /* for */
    freeifaddrs(ifap);
#endif /* HAS_GETIFADDRS */

    /*
     * add something of FamilyLocalHost
     */
    for (host = selfhosts;
	 host && !addrEqual(FamilyLocalHost, "", 0, host);
	 host = host->next);
    if (!host)
    {
	MakeHost(host, 0);
	if (host)
	{
	    host->family = FamilyLocalHost;
	    host->len = 0;
	    acopy("", host->addr, 0);
	    host->next = selfhosts;
	    selfhosts = host;
	}
    }
}
#endif /* hpux && !HAS_IFREQ */
#endif /* WINTCP */

#ifdef XDMCP
void
AugmentSelf(pointer from, int len)
{
    int family;
    pointer addr;
    register HOST *host;

    family = ConvertAddr(from, &len, (pointer *)&addr);
    if (family == -1 || family == FamilyLocal)
	return;
    for (host = selfhosts; host; host = host->next)
    {
	if (addrEqual(family, addr, len, host))
	    return;
    }
    MakeHost(host,len)
    if (!host)
	return;
    host->family = family;
    host->len = len;
    acopy(addr, host->addr, len);
    host->next = selfhosts;
    selfhosts = host;
}
#endif

void
AddLocalHosts (void)
{
    HOST    *self;

    for (self = selfhosts; self; self = self->next)
	(void) NewHost (self->family, self->addr, self->len);
}

/* Reset access control list to initial hosts */
void
ResetHosts (char *display)
{
    register HOST	*host;
    char                lhostname[120], ohostname[120];
    char 		*hostname = ohostname;
    char		fname[PATH_MAX + 1];
    int			fnamelen;
    FILE		*fd;
    char		*ptr;
    int                 i, hostlen;
    union {
        struct sockaddr	sa;
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(MNX_TCPCONN)
        struct sockaddr_in in;
#endif /* TCPCONN || STREAMSCONN */
#ifdef DNETCONN
        struct sockaddr_dn dn;
#endif
    } 			saddr;
#ifdef DNETCONN
    struct nodeent 	*np;
    struct dn_naddr 	dnaddr, *dnaddrp, *dnet_addr();
#endif
#ifdef K5AUTH
    krb5_principal      princ;
    krb5_data		kbuf;
#endif
    int			family = 0;
    pointer		addr;
    int 		len;
    register struct hostent *hp;

    AccessEnabled = defeatAccessControl ? FALSE : DEFAULT_ACCESS_CONTROL;
    LocalHostEnabled = FALSE;
    while ((host = validhosts) != 0)
    {
        validhosts = host->next;
        FreeHost (host);
    }
#define ETC_HOST_PREFIX "/etc/X"
#define ETC_HOST_SUFFIX ".hosts"
    fnamelen = strlen(ETC_HOST_PREFIX) + strlen(ETC_HOST_SUFFIX) +
		strlen(display) + 1;
    if (fnamelen > sizeof(fname))
	FatalError("Display name `%s' is too long\n", display);
    sprintf(fname, ETC_HOST_PREFIX "%s" ETC_HOST_SUFFIX, display);
#ifdef __UNIXOS2__
    strcpy(fname, (char*)__XOS2RedirRoot(fname));
#endif /* __UNIXOS2__ */

    if ((fd = fopen (fname, "r")) != 0)
    {
        while (fgets (ohostname, sizeof (ohostname), fd))
	{
	if (*ohostname == '#')
	    continue;
    	if ((ptr = strchr(ohostname, '\n')) != 0)
    	    *ptr = 0;
#ifdef __UNIXOS2__
    	if ((ptr = strchr(ohostname, '\r')) != 0)
    	    *ptr = 0;
#endif
        hostlen = strlen(ohostname) + 1;
        for (i = 0; i < hostlen; i++)
	    lhostname[i] = tolower(ohostname[i]);
	hostname = ohostname;
	if (!strncmp("local:", lhostname, 6))
	{
	    family = FamilyLocalHost;
	    NewHost(family, "", 0);
	}
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(MNX_TCPCONN)
	else if (!strncmp("inet:", lhostname, 5))
	{
	    family = FamilyInternet;
	    hostname = ohostname + 5;
	}
#endif
#ifdef DNETCONN
	else if (!strncmp("dnet:", lhostname, 5))
	{
	    family = FamilyDECnet;
	    hostname = ohostname + 5;
	}
#endif
#ifdef SECURE_RPC
	else if (!strncmp("nis:", lhostname, 4))
	{
	    family = FamilyNetname;
	    hostname = ohostname + 4;
	}
#endif
#ifdef K5AUTH
	else if (!strncmp("krb:", lhostname, 4))
	{
	    family = FamilyKrb5Principal;
	    hostname = ohostname + 4;
	}
#endif
#ifdef DNETCONN
    	if ((family == FamilyDECnet) ||
	    (ptr = strchr(hostname, ':')) && (*(ptr + 1) == ':') &&
	    !(*ptr = '\0'))	/* bash trailing colons if necessary */
	{
    	    /* node name (DECnet names end in "::") */
	    dnaddrp = dnet_addr(hostname);
    	    if (!dnaddrp && (np = getnodebyname (hostname)))
	    {
		/* node was specified by name */
		saddr.sa.sa_family = np->n_addrtype;
		len = sizeof(saddr.sa);
		if (ConvertAddr (&saddr.sa, &len, (pointer *)&addr) == FamilyDECnet)
		{
		    bzero ((char *) &dnaddr, sizeof (dnaddr));
		    dnaddr.a_len = np->n_length;
		    acopy (np->n_addr, dnaddr.a_addr, np->n_length);
		    dnaddrp = &dnaddr;
		}
    	    }
	    if (dnaddrp)
		(void) NewHost(FamilyDECnet, (pointer)dnaddrp,
			(int)(dnaddrp->a_len + sizeof(dnaddrp->a_len)));
    	}
	else
#endif /* DNETCONN */
#ifdef K5AUTH
	if (family == FamilyKrb5Principal)
	{
            krb5_parse_name(hostname, &princ);
	    XauKrb5Encode(princ, &kbuf);
	    (void) NewHost(FamilyKrb5Principal, kbuf.data, kbuf.length);
	    krb5_free_principal(princ);
        }
	else
#endif
#ifdef SECURE_RPC
	if ((family == FamilyNetname) || (strchr(hostname, '@')))
	{
	    SecureRPCInit ();
	    (void) NewHost (FamilyNetname, hostname, strlen (hostname));
	}
	else
#endif /* SECURE_RPC */
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(MNX_TCPCONN)
	{
#ifdef XTHREADS_NEEDS_BYNAMEPARAMS
	    _Xgethostbynameparams hparams;
#endif

    	    /* host name */
    	    if ((family == FamilyInternet &&
		 ((hp = _XGethostbyname(hostname, hparams)) != 0)) ||
		((hp = _XGethostbyname(hostname, hparams)) != 0))
	    {
    		saddr.sa.sa_family = hp->h_addrtype;
		len = sizeof(saddr.sa);
    		if ((family = ConvertAddr (&saddr.sa, &len, (pointer *)&addr)) != -1)
		{
#ifdef h_addr				/* new 4.3bsd version of gethostent */
		    char **list;

		    /* iterate over the addresses */
		    for (list = hp->h_addr_list; *list; list++)
			(void) NewHost (family, (pointer)*list, len);
#else
    		    (void) NewHost (family, (pointer)hp->h_addr, len);
#endif
		}
    	    }
        }
#endif /* TCPCONN || STREAMSCONN */
	family = FamilyWild;
        }
        fclose (fd);
    }
}

/* Is client on the local host */
Bool LocalClient(ClientPtr client)
{
    int    		alen, family, notused;
    Xtransaddr		*from = NULL;
    pointer		addr;
    register HOST	*host;

#ifdef XCSECURITY
    /* untrusted clients can't change host access */
    if (client->trustLevel != XSecurityClientTrusted)
    {
	SecurityAudit("client %d attempted to change host access\n",
		      client->index);
	return FALSE;
    }
#endif
#ifdef LBX
    if (!((OsCommPtr)client->osPrivate)->trans_conn)
	return FALSE;
#endif
    if (!_XSERVTransGetPeerAddr (((OsCommPtr)client->osPrivate)->trans_conn,
	&notused, &alen, &from))
    {
	family = ConvertAddr ((struct sockaddr *) from,
	    &alen, (pointer *)&addr);
	if (family == -1)
	{
	    xfree ((char *) from);
	    return FALSE;
	}
	if (family == FamilyLocal)
	{
	    xfree ((char *) from);
	    return TRUE;
	}
	for (host = selfhosts; host; host = host->next)
	{
	    if (addrEqual (family, addr, alen, host))
		return TRUE;
	}
	xfree ((char *) from);
    }
    return FALSE;
}

/*
 * Return the uid and gid of a connected local client
 * or the uid/gid for nobody those ids cannot be determinded
 * 
 * Used by XShm to test access rights to shared memory segments
 */
int
LocalClientCred(ClientPtr client, int *pUid, int *pGid)
{
    int fd;
    XtransConnInfo ci;
#ifdef HAS_GETPEEREID
    uid_t uid;
    gid_t gid;
#elif defined(SO_PEERCRED)
    struct ucred peercred;
    socklen_t so_len = sizeof(peercred);
#endif

    if (client == NULL)
	return -1;
    ci = ((OsCommPtr)client->osPrivate)->trans_conn;
    /* We can only determine peer credentials for Unix domain sockets */
    if (!_XSERVTransIsLocal(ci)) {
	return -1;
    }
    fd = _XSERVTransGetConnectionNumber(ci);
#ifdef HAS_GETPEEREID
    if (getpeereid(fd, &uid, &gid) == -1) 
	    return -1;
    if (pUid != NULL)
	    *pUid = uid;
    if (pGid != NULL)
	    *pGid = gid;
    return 0;
#elif defined(SO_PEERCRED)
    if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &peercred, &so_len) == -1) 
	    return -1;
    if (pUid != NULL)
	    *pUid = peercred.uid;
    if (pGid != NULL)
	    *pGid = peercred.gid;
    return 0;
#else
    /* No system call available to get the credentials of the peer */
    return -1;
#endif
}

static Bool
AuthorizedClient(ClientPtr client)
{
    if (!client || defeatAccessControl)
	return TRUE;
    return LocalClient(client);
}

/* Add a host to the access control list.  This is the external interface
 * called from the dispatcher */

int
AddHost (ClientPtr	client,
	 int            family,
	 unsigned       length,        /* of bytes in pAddr */
	 pointer        pAddr)
{
    int			len;

    if (!AuthorizedClient(client))
	return(BadAccess);
    switch (family) {
    case FamilyLocalHost:
	len = length;
	LocalHostEnabled = TRUE;
	break;
#ifdef K5AUTH
    case FamilyKrb5Principal:
        len = length;
        break;
#endif
#ifdef SECURE_RPC
    case FamilyNetname:
	len = length;
	SecureRPCInit ();
	break;
#endif
    case FamilyInternet:
    case FamilyDECnet:
    case FamilyChaos:
	if ((len = CheckAddr (family, pAddr, length)) < 0)
	{
	    client->errorValue = length;
	    return (BadValue);
	}
	break;
    case FamilyLocal:
    default:
	client->errorValue = family;
	return (BadValue);
    }
    if (NewHost (family, pAddr, len))
	return Success;
    return BadAlloc;
}

Bool
ForEachHostInFamily (int	    family,
		     Bool    (*func)(
			 unsigned char * /* addr */,
			 short           /* len */,
			 pointer         /* closure */),
		     pointer closure)
{
    HOST    *host;

    for (host = validhosts; host; host = host->next)
	if (family == host->family && func (host->addr, host->len, closure))
	    return TRUE;
    return FALSE;
}

/* Add a host to the access control list. This is the internal interface 
 * called when starting or resetting the server */
static Bool
NewHost (int		family,
	 pointer	addr,
	 int		len)
{
    register HOST *host;

    for (host = validhosts; host; host = host->next)
    {
        if (addrEqual (family, addr, len, host))
	    return TRUE;
    }
    MakeHost(host,len)
    if (!host)
	return FALSE;
    host->family = family;
    host->len = len;
    acopy(addr, host->addr, len);
    host->next = validhosts;
    validhosts = host;
    return TRUE;
}

/* Remove a host from the access control list */

int
RemoveHost (
    ClientPtr		client,
    int                 family,
    unsigned            length,        /* of bytes in pAddr */
    pointer             pAddr)
{
    int			len;
    register HOST	*host, **prev;

    if (!AuthorizedClient(client))
	return(BadAccess);
    switch (family) {
    case FamilyLocalHost:
	len = length;
	LocalHostEnabled = FALSE;
	break;
#ifdef K5AUTH
    case FamilyKrb5Principal:
        len = length;
	break;
#endif
#ifdef SECURE_RPC
    case FamilyNetname:
	len = length;
	break;
#endif
    case FamilyInternet:
    case FamilyDECnet:
    case FamilyChaos:
    	if ((len = CheckAddr (family, pAddr, length)) < 0)
    	{
	    client->errorValue = length;
            return(BadValue);
    	}
	break;
    case FamilyLocal:
    default:
	client->errorValue = family;
        return(BadValue);
    }
    for (prev = &validhosts;
         (host = *prev) && (!addrEqual (family, pAddr, len, host));
         prev = &host->next)
        ;
    if (host)
    {
        *prev = host->next;
        FreeHost (host);
    }
    return (Success);
}

/* Get all hosts in the access control list */
int
GetHosts (
    pointer		*data,
    int			*pnHosts,
    int			*pLen,
    BOOL		*pEnabled)
{
    int			len;
    register int 	n = 0;
    register unsigned char *ptr;
    register HOST	*host;
    int			nHosts = 0;

    *pEnabled = AccessEnabled ? EnableAccess : DisableAccess;
    for (host = validhosts; host; host = host->next)
    {
	nHosts++;
	n += (((host->len + 3) >> 2) << 2) + sizeof(xHostEntry);
    }
    if (n)
    {
        *data = ptr = (pointer) xalloc (n);
	if (!ptr)
	{
	    return(BadAlloc);
	}
        for (host = validhosts; host; host = host->next)
	{
	    len = host->len;
	    ((xHostEntry *)ptr)->family = host->family;
	    ((xHostEntry *)ptr)->length = len;
	    ptr += sizeof(xHostEntry);
	    acopy (host->addr, ptr, len);
	    ptr += ((len + 3) >> 2) << 2;
        }
    } else {
	*data = NULL;
    }
    *pnHosts = nHosts;
    *pLen = n;
    return(Success);
}

/* Check for valid address family and length, and return address length. */

/*ARGSUSED*/
static int
CheckAddr (
    int			family,
    pointer		pAddr,
    unsigned		length)
{
    int	len;

    switch (family)
    {
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(AMTCPCONN) || defined(MNX_TCPCONN)
      case FamilyInternet:
	if (length == sizeof (struct in_addr))
	    len = length;
	else
	    len = -1;
        break;
#endif 
#ifdef DNETCONN
      case FamilyDECnet:
        {
	    struct dn_naddr *dnaddr = (struct dn_naddr *) pAddr;

	    if ((length < sizeof(dnaddr->a_len)) ||
		(length < dnaddr->a_len + sizeof(dnaddr->a_len)))
		len = -1;
	    else
		len = dnaddr->a_len + sizeof(dnaddr->a_len);
	    if (len > sizeof(struct dn_naddr))
		len = -1;
	}
        break;
#endif
      default:
        len = -1;
    }
    return (len);
}

/* Check if a host is not in the access control list. 
 * Returns 1 if host is invalid, 0 if we've found it. */

int
InvalidHost (
    register struct sockaddr	*saddr,
    int				len)
{
    int 			family;
    pointer			addr;
    register HOST 		*selfhost, *host;

    if (!AccessEnabled)   /* just let them in */
        return(0);    
    family = ConvertAddr (saddr, &len, (pointer *)&addr);
    if (family == -1)
        return 1;
    if (family == FamilyLocal)
    {
	if (!LocalHostEnabled)
 	{
	    /*
	     * check to see if any local address is enabled.  This 
	     * implicitly enables local connections.
	     */
	    for (selfhost = selfhosts; selfhost; selfhost=selfhost->next)
 	    {
		for (host = validhosts; host; host=host->next)
		{
		    if (addrEqual (selfhost->family, selfhost->addr,
				   selfhost->len, host))
			return 0;
		}
	    }
	    return 1;
	} else
	    return 0;
    }
    for (host = validhosts; host; host = host->next)
    {
        if (addrEqual (family, addr, len, host))
    	    return (0);
    }
    return (1);
}

static int
ConvertAddr (
    register struct sockaddr	*saddr,
    int				*len,
    pointer			*addr)
{
    if (*len == 0)
        return (FamilyLocal);
    switch (saddr->sa_family)
    {
    case AF_UNSPEC:
#if defined(UNIXCONN) || defined(LOCALCONN) || defined(OS2PIPECONN)
    case AF_UNIX:
#endif
        return FamilyLocal;
#if defined(TCPCONN) || defined(STREAMSCONN) || defined(MNX_TCPCONN)
    case AF_INET:
        *len = sizeof (struct in_addr);
        *addr = (pointer) &(((struct sockaddr_in *) saddr)->sin_addr);
        return FamilyInternet;
#endif
#ifdef DNETCONN
    case AF_DECnet:
	{
	    struct sockaddr_dn *sdn = (struct sockaddr_dn *) saddr;
	    *len = sdn->sdn_nodeaddrl + sizeof(sdn->sdn_nodeaddrl);
	    *addr = (pointer) &(sdn->sdn_add);
	}
        return FamilyDECnet;
#endif
#ifdef CHAOSCONN
    case AF_CHAOS:
	{
	    not implemented
	}
	return FamilyChaos;
#endif
    default:
        return -1;
    }
}

int
ChangeAccessControl(
    ClientPtr client,
    int fEnabled)
{
    if (!AuthorizedClient(client))
	return BadAccess;
    AccessEnabled = fEnabled;
    return Success;
}

/* returns FALSE if xhost + in effect, else TRUE */
int
GetAccessControl(void)
{
    return AccessEnabled;
}
