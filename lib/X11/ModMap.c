/* $Xorg: ModMap.c,v 1.4 2001/02/09 02:03:34 xorgcvs Exp $ */
/*

Copyright 1986, 1998  The Open Group

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

*/
/* $XFree86: xc/lib/X11/ModMap.c,v 1.3 2001/01/17 19:41:40 dawes Exp $ */

#define NEED_REPLIES
#include "Xlibint.h"

XModifierKeymap *
XGetModifierMapping(dpy)
     register Display *dpy;
{       
    xGetModifierMappingReply rep;
    register xReq *req;
    unsigned long nbytes;
    XModifierKeymap *res;

    LockDisplay(dpy);
    GetEmptyReq(GetModifierMapping, req);
    (void) _XReply (dpy, (xReply *)&rep, 0, xFalse);

    nbytes = (unsigned long)rep.length << 2;
    res = (XModifierKeymap *) Xmalloc(sizeof (XModifierKeymap));
    if (res) res->modifiermap = (KeyCode *) Xmalloc ((unsigned) nbytes);
    if ((! res) || (! res->modifiermap)) {
	if (res) Xfree((char *) res);
	res = (XModifierKeymap *) NULL;
	_XEatData(dpy, nbytes);
    } else {
	_XReadPad(dpy, (char *) res->modifiermap, (long) nbytes);
	res->max_keypermod = rep.numKeyPerModifier;
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return (res);
}

/*
 *	Returns:
 *	0	Success
 *	1	Busy - one or more old or new modifiers are down
 *	2	Failed - one or more new modifiers unacceptable
 */
int
XSetModifierMapping(dpy, modifier_map)
    register Display *dpy;
    register XModifierKeymap *modifier_map;
{
    register xSetModifierMappingReq *req;
    xSetModifierMappingReply rep;
    int         mapSize = modifier_map->max_keypermod << 3;	/* 8 modifiers */

    LockDisplay(dpy);
    GetReqExtra(SetModifierMapping, mapSize, req);

    req->numKeyPerModifier = modifier_map->max_keypermod;

    memcpy((char *) NEXTPTR(req,xSetModifierMappingReq),
	   (char *) modifier_map->modifiermap,
	   mapSize);

    (void) _XReply(dpy, (xReply *) & rep,
	(SIZEOF(xSetModifierMappingReply) - SIZEOF(xReply)) >> 2, xTrue);
    UnlockDisplay(dpy);
    SyncHandle();
    return (rep.success);
}

XModifierKeymap *
XNewModifiermap(keyspermodifier)
    int keyspermodifier;
{
    XModifierKeymap *res = (XModifierKeymap *) Xmalloc((sizeof (XModifierKeymap)));
    if (res) {
	res->max_keypermod = keyspermodifier;
	res->modifiermap = (keyspermodifier > 0 ?
			    (KeyCode *) Xmalloc((unsigned) (8 * keyspermodifier))
			    : (KeyCode *) NULL);
	if (keyspermodifier && (res->modifiermap == NULL)) {
	    Xfree((char *) res);
	    return (XModifierKeymap *) NULL;
	}
    }
    return (res);
}


int
XFreeModifiermap(map)
    XModifierKeymap *map;
{
    if (map) {
	if (map->modifiermap)
	    Xfree((char *) map->modifiermap);
	Xfree((char *) map);
    }
    return 1;
}

#if NeedFunctionPrototypes
XModifierKeymap *
XInsertModifiermapEntry(XModifierKeymap *map,
#if NeedWidePrototypes
			unsigned int keycode,
#else
			KeyCode keycode,
#endif
			int modifier)
#else
XModifierKeymap *
XInsertModifiermapEntry(map, keycode, modifier)
    XModifierKeymap *map;
    KeyCode keycode;
    int modifier;
#endif
{
    XModifierKeymap *newmap;
    int i,
	row = modifier * map->max_keypermod,
	newrow,
	lastrow;

    for (i=0; i<map->max_keypermod; i++) {
        if (map->modifiermap[ row+i ] == keycode)
	    return(map); /* already in the map */
        if (map->modifiermap[ row+i ] == 0) {
            map->modifiermap[ row+i ] = keycode;
	    return(map); /* we added it without stretching the map */
	}
    }   

    /* stretch the map */
    if ((newmap = XNewModifiermap(map->max_keypermod+1)) == NULL)
	return (XModifierKeymap *) NULL;
    newrow = row = 0;
    lastrow = newmap->max_keypermod * 8;
    while (newrow < lastrow) {
	for (i=0; i<map->max_keypermod; i++)
	    newmap->modifiermap[ newrow+i ] = map->modifiermap[ row+i ];
	newmap->modifiermap[ newrow+i ] = 0;
	row += map->max_keypermod;
	newrow += newmap->max_keypermod;
    }
    (void) XFreeModifiermap(map);
    newrow = newmap->max_keypermod * modifier + newmap->max_keypermod - 1;
    newmap->modifiermap[ newrow ] = keycode;
    return(newmap);
}

#if NeedFunctionPrototypes
XModifierKeymap *
XDeleteModifiermapEntry(XModifierKeymap *map,
#if NeedWidePrototypes
			unsigned int keycode,
#else
			KeyCode keycode,
#endif
			int modifier)
#else
XModifierKeymap *
XDeleteModifiermapEntry(map, keycode, modifier)
    XModifierKeymap *map;
    KeyCode keycode;
    int modifier;
#endif
{
    int i,
	row = modifier * map->max_keypermod;

    for (i=0; i<map->max_keypermod; i++) {
        if (map->modifiermap[ row+i ] == keycode)
            map->modifiermap[ row+i ] = 0;
    }
    /* should we shrink the map?? */
    return (map);
}
