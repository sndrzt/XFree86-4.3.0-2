/* $Xorg: XKBGeom.c,v 1.4 2000/08/17 19:45:01 cpqbld Exp $ */
/************************************************************
Copyright (c) 1993 by Silicon Graphics Computer Systems, Inc.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Silicon Graphics not be 
used in advertising or publicity pertaining to distribution 
of the software without specific prior written permission.
Silicon Graphics makes no representation about the suitability 
of this software for any purpose. It is provided "as is"
without any express or implied warranty.

SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/
/* $XFree86: xc/lib/X11/XKBGeom.c,v 1.3 2001/01/17 19:41:48 dawes Exp $ */

#ifdef DEBUG
#include <stdio.h>
#endif

#define NEED_EVENTS
#define NEED_REPLIES
#include "Xlibint.h"
#include "XKBlibint.h"
#include <X11/extensions/XKBgeom.h>
#include <X11/extensions/XKBproto.h>

#ifndef MINSHORT
#define	MINSHORT	-32768
#endif
#ifndef MAXSHORT
#define	MAXSHORT	32767
#endif

/***====================================================================***/

static void
#if NeedFunctionPrototypes
_XkbCheckBounds(XkbBoundsPtr bounds,int	x,int y)
#else
_XkbCheckBounds(bounds,x,y)
    XkbBoundsPtr	bounds;
    int			x,y;
#endif
{
    if (x<bounds->x1)	bounds->x1= x;
    if (x>bounds->x2)	bounds->x2= x;
    if (y<bounds->y1)	bounds->y1= y;
    if (y>bounds->y2)	bounds->y2= y;
    return;
}

Bool
#if NeedFunctionPrototypes
XkbComputeShapeBounds(XkbShapePtr shape)
#else
XkbComputeShapeBounds(shape)
   XkbShapePtr	shape;
#endif
{
register int	o,p;
XkbOutlinePtr	outline;
XkbPointPtr	pt;

    if ((!shape)||(shape->num_outlines<1))
	return False;
    shape->bounds.x1= shape->bounds.y1= MAXSHORT;
    shape->bounds.x2= shape->bounds.y2= MINSHORT;
    for (outline=shape->outlines,o=0;o<shape->num_outlines;o++,outline++) {
	for (pt=outline->points,p=0;p<outline->num_points;p++,pt++) {
	    _XkbCheckBounds(&shape->bounds,pt->x,pt->y);
	}
    }
    return True;
}

Bool
#if NeedFunctionPrototypes
XkbComputeShapeTop(XkbShapePtr shape,XkbBoundsPtr bounds)
#else
XkbComputeShapeTop(shape,bounds)
   XkbShapePtr	shape;
   XkbBoundsPtr	bounds;
#endif
{
register int	p;
XkbOutlinePtr	outline;
XkbPointPtr	pt;

    if ((!shape)||(shape->num_outlines<1))
	return False;
    if (shape->approx)	outline= shape->approx;
    else		outline= &shape->outlines[shape->num_outlines-1];
    if (outline->num_points<2) {
	 bounds->x1= bounds->y1= 0;
	 bounds->x2= bounds->y2= 0;
    }
    else {
	bounds->x1= bounds->y1= MAXSHORT;
	bounds->x2= bounds->y2= MINSHORT;
    }
    for (pt=outline->points,p=0;p<outline->num_points;p++,pt++) {
	_XkbCheckBounds(bounds,pt->x,pt->y);
    }
    return True;
}

Bool
#if NeedFunctionPrototypes
XkbComputeRowBounds(XkbGeometryPtr geom,XkbSectionPtr section,XkbRowPtr row)
#else
XkbComputeRowBounds(geom,section,row)
    XkbGeometryPtr	geom;
    XkbSectionPtr	section;
    XkbRowPtr		row;
#endif
{
register int	k,pos;
XkbKeyPtr	key;
XkbBoundsPtr	bounds,sbounds;

    if ((!geom)||(!section)||(!row))
	return False;
    pos= 0;
    bounds= &row->bounds;
    bzero(bounds,sizeof(XkbBoundsRec));
    for (key=row->keys,pos=k=0;k<row->num_keys;k++,key++) {
	sbounds= &XkbKeyShape(geom,key)->bounds;
	_XkbCheckBounds(bounds,pos,0);
	if (!row->vertical) {
	    if (key->gap!=0) {
		pos+= key->gap;
		_XkbCheckBounds(bounds,pos,0);
	    }
	    _XkbCheckBounds(bounds,pos+sbounds->x1,sbounds->y1);
	    _XkbCheckBounds(bounds,pos+sbounds->x2,sbounds->y2);
	    pos+= sbounds->x2;
	}
	else {
	    if (key->gap!=0) {
		pos+= key->gap;
		_XkbCheckBounds(bounds,0,pos);
	    }
	    _XkbCheckBounds(bounds,pos+sbounds->x1,sbounds->y1);
	    _XkbCheckBounds(bounds,pos+sbounds->x2,sbounds->y2);
	    pos+= sbounds->y2;
	}
    }
    return True;
}

Bool
#if NeedFunctionPrototypes
XkbComputeSectionBounds(XkbGeometryPtr geom,XkbSectionPtr section)
#else
XkbComputeSectionBounds(geom,section)
    XkbGeometryPtr	geom;
    XkbSectionPtr	section;
#endif
{
register int	i;
XkbShapePtr	shape;
XkbRowPtr	row;
XkbDoodadPtr	doodad;
XkbBoundsPtr	bounds,rbounds=NULL;

    if ((!geom)||(!section))
	return False;
    bounds= &section->bounds;
    bzero(bounds,sizeof(XkbBoundsRec));
    for (i=0,row=section->rows;i<section->num_rows;i++,row++) {
	if (!XkbComputeRowBounds(geom,section,row))
	    return False;
	rbounds= &row->bounds;
	_XkbCheckBounds(bounds,row->left+rbounds->x1,row->top+rbounds->y1);
	_XkbCheckBounds(bounds,row->left+rbounds->x2,row->top+rbounds->y2);
    }
    for (i=0,doodad=section->doodads;i<section->num_doodads;i++,doodad++) {
	static XkbBoundsRec	tbounds;
	switch (doodad->any.type) {
	    case XkbOutlineDoodad:
	    case XkbSolidDoodad:
		shape= XkbShapeDoodadShape(geom,&doodad->shape);
		rbounds= &shape->bounds;
		break;
	    case XkbTextDoodad:
		tbounds.x1= doodad->text.left;
		tbounds.y1= doodad->text.top;
		tbounds.x2= tbounds.x1+doodad->text.width;
		tbounds.y2= tbounds.y1+doodad->text.height;
		rbounds= &tbounds;
		break;
	    case XkbIndicatorDoodad:
		shape= XkbIndicatorDoodadShape(geom,&doodad->indicator);
		rbounds= &shape->bounds;
		break;
	    case XkbLogoDoodad:
		shape= XkbLogoDoodadShape(geom,&doodad->logo);
		rbounds= &shape->bounds;
		break;
	    default:
		tbounds.x1= tbounds.x2= doodad->any.left;
		tbounds.y1= tbounds.y2= doodad->any.top;
		break;
	}
	_XkbCheckBounds(bounds,rbounds->x1,rbounds->y1);
	_XkbCheckBounds(bounds,rbounds->x2,rbounds->y2);
    }
    return True;
}

/***====================================================================***/

char *
#if NeedFunctionPrototypes
XkbFindOverlayForKey(XkbGeometryPtr geom,XkbSectionPtr wanted,char *under)
#else
XkbFindOverlayForKey(geom,wanted,under)
    XkbGeometryPtr	geom;
    XkbSectionPtr	wanted;
    char *		under;
#endif
{
int		s;
XkbSectionPtr	section;

    if ((geom==NULL)||(under==NULL)||(geom->num_sections<1))
	return NULL;

    if (wanted)
	 section= wanted;
    else section= geom->sections;

    for (s=0;s<geom->num_sections;s++,section++) {
	XkbOverlayPtr	ol;
	int		o;

	if (section->num_overlays<1)
	    continue;
	for (o=0,ol=section->overlays;o<section->num_overlays;o++,ol++) {
	    XkbOverlayRowPtr	row;
	    int			r;

	    for (r=0,row=ol->rows;r<ol->num_rows;r++,row++) {
		XkbOverlayKeyPtr	key;
		int			k;
		for (k=0,key=row->keys;k<row->num_keys;k++,key++) {
		    if (strncmp(under,key->under.name,XkbKeyNameLength)==0)
			return key->over.name;
		}
	    }
	}
	if (wanted!=NULL)
	    break;
    }
    return NULL;
}

/***====================================================================***/

static Status
#if NeedFunctionPrototypes
_XkbReadGeomProperties(	XkbReadBufferPtr	buf,
			XkbGeometryPtr 		geom,
			xkbGetGeometryReply *	rep)
#else
_XkbReadGeomProperties(buf,geom,rep)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    xkbGetGeometryReply *	rep;
#endif
{
Status	rtrn;

    if (rep->nProperties<1)
	return Success;
    if ((rtrn=XkbAllocGeomProps(geom,rep->nProperties))==Success) {
	register int i;
	register Bool ok;
	char *name,*value;
	ok= True;
	for (i=0;(i<rep->nProperties)&&ok;i++) {
	    ok= _XkbGetReadBufferCountedString(buf,&name)&&ok;
	    ok= _XkbGetReadBufferCountedString(buf,&value)&&ok;
	    ok= ok&&(XkbAddGeomProperty(geom,name,value)!=NULL);
	}
	if (ok)	rtrn= Success;
	else	rtrn= BadLength;
    }
    return rtrn;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomKeyAliases(	XkbReadBufferPtr	buf,
			XkbGeometryPtr		geom,
			xkbGetGeometryReply *	rep)
#else
_XkbReadGeomKeyAliases(buf,geom,rep)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    xkbGetGeometryReply *	rep;
#endif
{
Status	rtrn;

    if (rep->nKeyAliases<1)
	return Success;
    if ((rtrn=XkbAllocGeomKeyAliases(geom,rep->nKeyAliases))==Success) {
	if (!_XkbCopyFromReadBuffer(buf,(char *)geom->key_aliases,
					(rep->nKeyAliases*XkbKeyNameLength*2)))
	    return BadLength;
	geom->num_key_aliases= rep->nKeyAliases;
	return Success;
    }
    else { /* alloc failed, just skip the aliases */
	_XkbSkipReadBufferData(buf,(rep->nKeyAliases*XkbKeyNameLength*2));
    }
    return rtrn;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomColors(	XkbReadBufferPtr	buf,
			XkbGeometryPtr		geom,
			xkbGetGeometryReply *	rep)
#else
_XkbReadGeomColors(buf,geom,rep)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    xkbGetGeometryReply *	rep;
#endif
{
Status	rtrn;

    if (rep->nColors<1)
	return Success;
    if ((rtrn=XkbAllocGeomColors(geom,rep->nColors))==Success) {
	register int i;
	char *spec;
	for (i=0;i<rep->nColors;i++) {
	    if (!_XkbGetReadBufferCountedString(buf,&spec))
		return BadLength;
	    if (XkbAddGeomColor(geom,spec,geom->num_colors)==NULL)
		return BadAlloc;
	}
	return Success;
    }
    return rtrn;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomShapes(	XkbReadBufferPtr	buf,
			XkbGeometryPtr		geom,
			xkbGetGeometryReply *	rep)
#else
_XkbReadGeomShapes(buf,geom,rep)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    xkbGetGeometryReply *	rep;
#endif
{
register int i;
Status	rtrn;

    if (rep->nShapes<1)
	return Success;
    if ((rtrn=XkbAllocGeomShapes(geom,rep->nShapes))!=Success)
	return rtrn;
    for (i=0;i<rep->nShapes;i++) {
	xkbShapeWireDesc *shapeWire;
	XkbShapePtr	 shape;
	register int	 o;
	shapeWire= (xkbShapeWireDesc *)
		   _XkbGetReadBufferPtr(buf,SIZEOF(xkbShapeWireDesc));
	if (!shapeWire)
	    return BadLength;
	shape= XkbAddGeomShape(geom,shapeWire->name,shapeWire->nOutlines);
	if (!shape)
	   return BadAlloc;
	for (o=0;o<shapeWire->nOutlines;o++) {
	    xkbOutlineWireDesc *olWire;
	    XkbOutlinePtr	ol;
	    register int	p;
	    XkbPointPtr		pt;
	    olWire=  (xkbOutlineWireDesc *)
		 _XkbGetReadBufferPtr(buf,SIZEOF(xkbOutlineWireDesc));
	    if (!olWire)
		return BadLength;
	    ol= XkbAddGeomOutline(shape,olWire->nPoints);
	    if (!ol)
		return BadAlloc;
	    ol->corner_radius=  olWire->cornerRadius;
	    for (p=0,pt=ol->points;p<olWire->nPoints;p++,pt++) {
		xkbPointWireDesc *	ptWire;
		ptWire= (xkbPointWireDesc *)
		    _XkbGetReadBufferPtr(buf,SIZEOF(xkbPointWireDesc));
		if (!ptWire)
		    return BadLength;
		pt->x= ptWire->x;
		pt->y= ptWire->y;
	    }
	    ol->num_points= olWire->nPoints;
	}
	if (shapeWire->primaryNdx!=XkbNoShape)
	     shape->primary= &shape->outlines[shapeWire->primaryNdx];
	else shape->primary= NULL;
	if (shapeWire->approxNdx!=XkbNoShape)
	     shape->approx= &shape->outlines[shapeWire->approxNdx];
	else shape->approx= NULL;
	XkbComputeShapeBounds(shape);
    }
    return Success;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomDoodad(	XkbReadBufferPtr 	buf,
			XkbGeometryPtr 		geom,
			XkbSectionPtr 		section)
#else
_XkbReadGeomDoodad(buf,geom,section)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    XkbSectionPtr		section;
#endif
{
XkbDoodadPtr		doodad;
xkbDoodadWireDesc *	doodadWire;

    doodadWire= (xkbDoodadWireDesc *)
		   _XkbGetReadBufferPtr(buf,SIZEOF(xkbDoodadWireDesc));
    if (!doodadWire)
	return 	BadLength;
    doodad= XkbAddGeomDoodad(geom,section,doodadWire->any.name);
    if (!doodad)
	return BadAlloc;
    doodad->any.type= doodadWire->any.type;
    doodad->any.priority= doodadWire->any.priority;
    doodad->any.top= doodadWire->any.top;
    doodad->any.left= doodadWire->any.left;
    doodad->any.angle= doodadWire->any.angle;
    switch (doodad->any.type) {
	case XkbOutlineDoodad:
	case XkbSolidDoodad:
	    doodad->shape.color_ndx= doodadWire->shape.colorNdx;
	    doodad->shape.shape_ndx= doodadWire->shape.shapeNdx;
	    break;
	case XkbTextDoodad:
	    doodad->text.width= doodadWire->text.width;
	    doodad->text.height= doodadWire->text.height;
	    doodad->text.color_ndx= doodadWire->text.colorNdx;
	    if (!_XkbGetReadBufferCountedString(buf,&doodad->text.text))
		return BadLength;
	    if (!_XkbGetReadBufferCountedString(buf,&doodad->text.font))
		return BadLength;
	    break;
	case XkbIndicatorDoodad:
	    doodad->indicator.shape_ndx= doodadWire->indicator.shapeNdx;
	    doodad->indicator.on_color_ndx= doodadWire->indicator.onColorNdx;
	    doodad->indicator.off_color_ndx= doodadWire->indicator.offColorNdx;
	    break;
	case XkbLogoDoodad:
	    doodad->logo.color_ndx= doodadWire->logo.colorNdx;
	    doodad->logo.shape_ndx= doodadWire->logo.shapeNdx;
	    if (!_XkbGetReadBufferCountedString(buf,&doodad->logo.logo_name))
		return BadLength;
	    break;
	default:
	    return BadValue;
    }
    return Success;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomOverlay(	XkbReadBufferPtr	buf,
			XkbGeometryPtr		geom,
			XkbSectionPtr		section)
#else
_XkbReadGeomOverlay(buf,geom,section)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    XkbSectionPtr		section;
#endif
{
XkbOverlayPtr		ol;
xkbOverlayWireDesc *	olWire;
register int		r;

    olWire= (xkbOverlayWireDesc *)
		   _XkbGetReadBufferPtr(buf,SIZEOF(xkbOverlayWireDesc));
    if (olWire==NULL)
	return BadLength;
    ol= XkbAddGeomOverlay(section,olWire->name,olWire->nRows);
    if (ol==NULL)
	return BadLength;
    for (r=0;r<olWire->nRows;r++) {
	register int 			k;
	XkbOverlayRowPtr		row;
	xkbOverlayRowWireDesc *		rowWire;
	xkbOverlayKeyWireDesc *		keyWire;
	rowWire= (xkbOverlayRowWireDesc *)
			_XkbGetReadBufferPtr(buf,SIZEOF(xkbOverlayRowWireDesc));
	if (rowWire==NULL)
	    return BadLength;
	row= XkbAddGeomOverlayRow(ol,rowWire->rowUnder,rowWire->nKeys);
	row->row_under= rowWire->rowUnder;
	if (!row)
	    return BadAlloc;
	if (rowWire->nKeys<1)
	    continue;
	keyWire= (xkbOverlayKeyWireDesc *)
     			_XkbGetReadBufferPtr(buf,
				SIZEOF(xkbOverlayKeyWireDesc)*rowWire->nKeys);
	if (keyWire==NULL)
	    return BadLength;
	for (k=0;k<rowWire->nKeys;k++,keyWire++,row->num_keys++) {
	    memcpy(row->keys[row->num_keys].over.name,keyWire->over,
	    						XkbKeyNameLength);
	    memcpy(row->keys[row->num_keys].under.name,keyWire->under,
	    						XkbKeyNameLength);
	}
    }
    return Success;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomSections(	XkbReadBufferPtr	buf,
			XkbGeometryPtr		geom,
			xkbGetGeometryReply *	rep)
#else
_XkbReadGeomSections(buf,geom,rep)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    xkbGetGeometryReply *	rep;
#endif
{
register int 		s;
XkbSectionPtr		section;
xkbSectionWireDesc *	sectionWire;
Status			rtrn;

    if (rep->nSections<1)
	return Success;
    if ((rtrn=XkbAllocGeomSections(geom,rep->nSections))!=Success)
	return rtrn;
    for (s=0;s<rep->nSections;s++) {
	sectionWire= (xkbSectionWireDesc *)
			_XkbGetReadBufferPtr(buf,SIZEOF(xkbSectionWireDesc));
	if (!sectionWire)
	    return BadLength;
	section= XkbAddGeomSection(geom,sectionWire->name,sectionWire->nRows,
						sectionWire->nDoodads,
						sectionWire->nOverlays);
	if (!section)
	    return BadAlloc;
	section->top= sectionWire->top;
	section->left= sectionWire->left;
	section->width= sectionWire->width;
	section->height= sectionWire->height;
	section->angle= sectionWire->angle;
	section->priority= sectionWire->priority;
	if (sectionWire->nRows>0) {
	    register int 	r;
	    XkbRowPtr		row;
	    xkbRowWireDesc *	rowWire;
	    for (r=0;r<sectionWire->nRows;r++) {
		rowWire= (xkbRowWireDesc *)
			 _XkbGetReadBufferPtr(buf,SIZEOF(xkbRowWireDesc));
		if (!rowWire)
		    return BadLength;
		row= XkbAddGeomRow(section,rowWire->nKeys);
		if (!row)
		    return BadAlloc;
		row->top= rowWire->top;
		row->left= rowWire->left;
		row->vertical= rowWire->vertical;
		if (rowWire->nKeys>0) {
		    register int	k;
		    XkbKeyPtr		key;
		    xkbKeyWireDesc *	keyWire;
		    for (k=0;k<rowWire->nKeys;k++) {
			keyWire= (xkbKeyWireDesc *)
			      _XkbGetReadBufferPtr(buf,SIZEOF(xkbKeyWireDesc));
			if (!keyWire)
			    return BadLength;
			key= XkbAddGeomKey(row);
			if (!key)
			    return BadAlloc;
			memcpy(key->name.name,keyWire->name,XkbKeyNameLength);
			key->gap= keyWire->gap;
			key->shape_ndx= keyWire->shapeNdx;
			key->color_ndx= keyWire->colorNdx;
		    }
		}
	    }
	}
	if (sectionWire->nDoodads>0) {
	    register int d;
	    for (d=0;d<sectionWire->nDoodads;d++) {
		if ((rtrn=_XkbReadGeomDoodad(buf,geom,section))!=Success)
		    return rtrn;
	    }
	}
	if (sectionWire->nOverlays>0) {
	    register int o;
	    for (o=0;o<sectionWire->nOverlays;o++) {
		if ((rtrn=_XkbReadGeomOverlay(buf,geom,section))!=Success)
		    return rtrn;
	    }
	}
    }
    return Success;
}

static Status
#if NeedFunctionPrototypes
_XkbReadGeomDoodads(	XkbReadBufferPtr	buf,
			XkbGeometryPtr		geom,
			xkbGetGeometryReply *	rep)
#else
_XkbReadGeomDoodads(buf,geom,rep)
    XkbReadBufferPtr		buf;
    XkbGeometryPtr		geom;
    xkbGetGeometryReply *	rep;
#endif
{
register int d;
Status	rtrn;

    if (rep->nDoodads<1)
	return Success;
    if ((rtrn=XkbAllocGeomDoodads(geom,rep->nDoodads))!=Success)
	return rtrn;
    for (d=0;d<rep->nDoodads;d++) {
	if ((rtrn=_XkbReadGeomDoodad(buf,geom,NULL))!=Success)
	    return rtrn;
    }
    return Success;
}

Status
#if NeedFunctionPrototypes
_XkbReadGetGeometryReply(	Display * dpy,
				xkbGetGeometryReply * rep,
				XkbDescPtr xkb,
				int * nread_rtrn)
#else
_XkbReadGetGeometryReply(dpy,rep,xkb,nread_rtrn)
    Display *			dpy;
    xkbGetGeometryReply *	rep;
    XkbDescPtr			xkb;
    int *			nread_rtrn;
#endif
{
XkbGeometryPtr	geom;

    geom= _XkbTypedCalloc(1,XkbGeometryRec);
    if (!geom)
	return BadAlloc;
    if (xkb->geom)
	XkbFreeGeometry(xkb->geom,XkbGeomAllMask,True);
    xkb->geom= geom;

    geom->name= rep->name;
    geom->width_mm= rep->widthMM;
    geom->height_mm= rep->heightMM;
    if (rep->length) {
	XkbReadBufferRec	buf;
	int			left;
	if (_XkbInitReadBuffer(dpy,&buf,(int)rep->length*4)) {
	    Status status= Success;
	    if (nread_rtrn)
		*nread_rtrn= (int)rep->length*4;
	    if (!_XkbGetReadBufferCountedString(&buf,&geom->label_font))
		status= BadLength;
	    if (status==Success)
		status= _XkbReadGeomProperties(&buf,geom,rep);
	    if (status==Success)
		status= _XkbReadGeomColors(&buf,geom,rep);
	    if (status==Success)
		status= _XkbReadGeomShapes(&buf,geom,rep);
	    if (status==Success)
		status= _XkbReadGeomSections(&buf,geom,rep);
	    if (status==Success)
		status= _XkbReadGeomDoodads(&buf,geom,rep);
	    if (status==Success)
		status= _XkbReadGeomKeyAliases(&buf,geom,rep);
	    left= _XkbFreeReadBuffer(&buf);
	    if ((status!=Success) || left || buf.error) {
		if (status==Success)	
		    status= BadLength;
		XkbFreeGeometry(geom,XkbGeomAllMask,True);
		xkb->geom= NULL;
		return status;
	    }
	    geom->base_color= &geom->colors[rep->baseColorNdx];
	    geom->label_color= &geom->colors[rep->labelColorNdx];
	}
	else {
	    XkbFreeGeometry(geom,XkbGeomAllMask,True);
	    xkb->geom= NULL;
	    return BadAlloc;
	}
    }
    return Success;
}

Status
#if NeedFunctionPrototypes
XkbGetGeometry(Display *dpy,XkbDescPtr xkb)
#else
XkbGetGeometry(dpy,xkb)
    Display *	dpy;
    XkbDescPtr	xkb;
#endif
{
xkbGetGeometryReq	*req;
xkbGetGeometryReply	 rep;

    if ( (!xkb) || (dpy->flags & XlibDisplayNoXkb) ||
	(!dpy->xkb_info && !XkbUseExtension(dpy,NULL,NULL)))
	return BadAccess;
    
    GetReq(kbGetGeometry, req);
    req->reqType = dpy->xkb_info->codes->major_opcode;
    req->xkbReqType = X_kbGetGeometry;
    req->deviceSpec = xkb->device_spec;
    req->name= None;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse))
	return BadImplementation;
    if (!rep.found)
	return BadName;
    return _XkbReadGetGeometryReply(dpy,&rep,xkb,NULL);
}

Status
#if NeedFunctionPrototypes
XkbGetNamedGeometry(Display *dpy,XkbDescPtr xkb,Atom name)
#else
XkbGetNamedGeometry(dpy,xkb,name)
    Display *	dpy;
    XkbDescPtr	xkb;
    Atom	name;
#endif
{
xkbGetGeometryReq	*req;
xkbGetGeometryReply	 rep;

    if ( (name==None) || (dpy->flags & XlibDisplayNoXkb) ||
	(!dpy->xkb_info && !XkbUseExtension(dpy,NULL,NULL)) )
	return BadAccess;
    
    GetReq(kbGetGeometry, req);
    req->reqType = dpy->xkb_info->codes->major_opcode;
    req->xkbReqType = X_kbGetGeometry;
    req->deviceSpec = xkb->device_spec;
    req->name= (CARD32)name;
    if ((!_XReply(dpy, (xReply *)&rep, 0, xFalse))||(!rep.found))
	return BadImplementation;
    if (!rep.found)
	return BadName;
    return _XkbReadGetGeometryReply(dpy,&rep,xkb,NULL);
}

