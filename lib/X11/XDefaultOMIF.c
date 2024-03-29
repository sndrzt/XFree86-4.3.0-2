/*
Copyright 1985, 1986, 1987, 1991, 1998  The Open Group

Portions Copyright 2000 Sun Microsystems, Inc. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions: The above copyright notice and this
permission notice shall be included in all copies or substantial
portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP OR SUN MICROSYSTEMS, INC. BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE EVEN IF
ADVISED IN ADVANCE OF THE POSSIBILITY OF SUCH DAMAGES.


Except as contained in this notice, the names of The Open Group and/or
Sun Microsystems, Inc. shall not be used in advertising or otherwise to
promote the sale, use or other dealings in this Software without prior
written authorization from The Open Group and/or Sun Microsystems,
Inc., as applicable.


X Window System is a trademark of The Open Group

OSF/1, OSF/Motif and Motif are registered trademarks, and OSF, the OSF
logo, LBX, X Window System, and Xinerama are trademarks of the Open
Group. All other trademarks and registered trademarks mentioned herein
are the property of their respective owners. No right, title or
interest in or to any trademark, service mark, logo or trade name of
Sun Microsystems, Inc. or its licensors is granted.

*/
/* $XFree86: xc/lib/X11/XDefaultOMIF.c,v 1.2tsi Exp $ */

#include "Xlibint.h"
#include "Xlcint.h"
#include "XlcPublic.h"
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>

#define MAXFONTS		100

#define XOM_GENERIC(om)		(&((XOMGeneric) om)->gen)
#define XOC_GENERIC(font_set)	(&((XOCGeneric) font_set)->gen)

#define DefineLocalBuf		char local_buf[BUFSIZ]
#define AllocLocalBuf(length)	(length > BUFSIZ ? (char *)Xmalloc(length) : local_buf)
#define FreeLocalBuf(ptr)	if (ptr != local_buf) Xfree(ptr)

typedef struct _FontDataRec {
    char *name;
} FontDataRec, *FontData;

typedef struct _OMDataRec {
    int font_data_count;
    FontData font_data;
} OMDataRec, *OMData;

typedef struct _XOMGenericPart {
    OMData data;
} XOMGenericPart;

typedef struct _XOMGenericRec {
    XOMMethods methods;
    XOMCoreRec core;
    XOMGenericPart gen;
} XOMGenericRec, *XOMGeneric;

typedef struct _FontSetRec {
    int id;
    int font_data_count;
    FontData font_data;
    char *font_name;
    XFontStruct *info;
    XFontStruct *font;
} FontSetRec, *FontSet;

typedef struct _XOCGenericPart {
    XlcConv wcs_to_cs;
    FontSet font_set;
} XOCGenericPart;

typedef struct _XOCGenericRec {
    XOCMethods methods;
    XOCCoreRec core;	
    XOCGenericPart gen;
} XOCGenericRec, *XOCGeneric;

static Bool
init_fontset(oc)
    XOC oc;
{
    XOCGenericPart *gen;
    FontSet font_set;
    OMData data;

    data = XOM_GENERIC(oc->core.om)->data;

    font_set = (FontSet) Xmalloc(sizeof(FontSetRec));
    if (font_set == NULL)
	return False;
    bzero((char *) font_set, sizeof(FontSetRec));

    gen = XOC_GENERIC(oc);
    gen->font_set = font_set;

    font_set->font_data_count = data->font_data_count;
    font_set->font_data = data->font_data;

    return True;
}

static char *
get_prop_name(dpy, fs)
    Display *dpy;
    XFontStruct	*fs;
{
    unsigned long fp;

    if (XGetFontProperty(fs, XA_FONT, &fp))
	return XGetAtomName(dpy, fp); 

    return (char *) NULL;
}

static FontData
check_charset(font_set, font_name)
    FontSet font_set;
    char *font_name;
{
    FontData font_data;
    char *last;
    int count;
    ssize_t length, name_len;

    name_len = strlen(font_name);
    last = font_name + name_len;

    count = font_set->font_data_count;
    font_data = font_set->font_data;

    for ( ; count-- > 0; font_data++) {
	length = strlen(font_data->name);

	if (length > name_len)
	    return(NULL);
	
	if (_XlcCompareISOLatin1(last - length, font_data->name) == 0)
	    return font_data;
    }
    return (FontData) NULL;
}

#if 0 /* Unused */
static int
check_fontname(oc, name)
XOC oc;
char *name;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontData data;
    FontSet font_set;
    XFontStruct *fs_list;
    char **fn_list, *fname, *prop_fname = NULL;
    int list_num, i;
    int list2_num;
    char **fn2_list = NULL;
    int found_num = 0;

    fn_list = XListFonts(dpy, name, MAXFONTS, &list_num);
    if (fn_list == NULL)
	return found_num;

    for (i = 0; i < list_num; i++) {
	fname = fn_list[i];

	font_set = gen->font_set;

	if ((data = check_charset(font_set, fname)) == NULL) {
	    if ((fn2_list = XListFontsWithInfo(dpy, name, MAXFONTS,
					       &list2_num, &fs_list))
		&& (prop_fname = get_prop_name(dpy, fs_list))
		&& (data = check_charset(font_set, prop_fname)))
		fname = prop_fname;
	}
	if (data) {
	    font_set->font_name = (char *) Xmalloc(strlen(fname) + 1);
	    if (font_set->font_name) {
		strcpy(font_set->font_name, fname);
		found_num++;
	    }
	}
	if (fn2_list) {
	    XFreeFontInfo(fn2_list, fs_list, list2_num);
	    fn2_list = NULL;
	    if (prop_fname) {
		Xfree(prop_fname);
		prop_fname = NULL;
	    }
	}
	if (found_num == 1)
	    break;
    }
    XFreeFontNames(fn_list);
    return found_num;
}
#endif

static Bool
load_font(oc)
    XOC oc;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;

    if (font_set->font_name == NULL)
	return False;

    if (font_set->font == NULL) {
	font_set->font = XLoadQueryFont(dpy, font_set->font_name);
	if (font_set->font == NULL)
	    return False;
    }
    return True;
}

static Bool
load_font_info(oc)
    XOC oc;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set = gen->font_set;
    char **fn_list;
    int fn_num;

    if (font_set->font_name == NULL)
	return False;

    if (font_set->info == NULL) {
	fn_list = XListFontsWithInfo(dpy, font_set->font_name, 1, &fn_num,
				     &font_set->info);
	if (font_set->info == NULL)
	    return False;
	if (fn_num > 0)
	    font_set->info->fid = XLoadFont(dpy, font_set->font_name);
	    
	if (fn_list) XFreeFontNames(fn_list);
    }
    return True;
}

static void
set_fontset_extents(oc)
    XOC oc;
{
    XRectangle *ink = &oc->core.font_set_extents.max_ink_extent;
    XRectangle *logical = &oc->core.font_set_extents.max_logical_extent;
    XFontStruct **font_list, *font;
    XCharStruct overall;
    int logical_ascent, logical_descent;

    font_list = oc->core.font_info.font_struct_list;
    font = *font_list++;
    overall = font->max_bounds;
    overall.lbearing = font->min_bounds.lbearing;
    logical_ascent = font->ascent;
    logical_descent = font->descent;

    ink->x = overall.lbearing;
    ink->y = -(overall.ascent);
    ink->width = overall.rbearing - overall.lbearing;
    ink->height = overall.ascent + overall.descent;

    logical->x = 0;
    logical->y = -(logical_ascent);
    logical->width = overall.width;
    logical->height = logical_ascent + logical_descent;
}

static Bool
init_core_part(oc)
    XOC oc;
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    XFontStruct **font_struct_list;
    char **font_name_list, *font_name_buf;
    int	count, length;

    font_set = gen->font_set;
    count = length = 0;

    if (font_set->font_name != NULL) {
	length += strlen(font_set->font_name) + 1;
	count++;
    }
    if (count == 0)
        return False;

    font_struct_list = (XFontStruct **) Xmalloc(sizeof(XFontStruct *));
    if (font_struct_list == NULL)
	return False;

    font_name_list = (char **) Xmalloc(sizeof(char *));
    if (font_name_list == NULL)
	goto err;

    font_name_buf = (char *) Xmalloc(length);
    if (font_name_buf == NULL)
	goto err;

    oc->core.font_info.num_font = 1;
    oc->core.font_info.font_name_list = font_name_list;
    oc->core.font_info.font_struct_list = font_struct_list;

    font_set = gen->font_set;

    if (font_set->font_name != NULL) {
	font_set->id = 1;
	if (font_set->font)
	    *font_struct_list++ = font_set->font;
	else
	    *font_struct_list++ = font_set->info;
	strcpy(font_name_buf, font_set->font_name);
	Xfree(font_set->font_name);
	*font_name_list++ = font_set->font_name = font_name_buf;
	font_name_buf += strlen(font_name_buf) + 1;
    }

    set_fontset_extents(oc);

    return True;

err:
    if (font_name_list)
	Xfree(font_name_list);
    Xfree(font_struct_list);

    return False;
}

static char *
get_font_name(oc, pattern)
    XOC oc;
    char *pattern;
{
    char **list, *name, *prop_name;
    int count;
    XFontStruct *fs;
    Display *dpy = oc->core.om->core.display;

    list = XListFonts(dpy, pattern, 1, &count);
    if (list != NULL) {
	name = (char *) Xmalloc(strlen(*list) + 1);
	if (name)
	    strcpy(name, *list);
    
	XFreeFontNames(list);
    } else {
	fs = XLoadQueryFont(dpy, pattern);
	if (fs == NULL) return NULL;

	prop_name = get_prop_name(dpy, fs);
	if (prop_name == NULL) return NULL;

	name = (char*) Xmalloc(strlen(prop_name) + 1);
	if (name)
	    strcpy(name, prop_name);

	XFreeFont(dpy, fs);
    }
    return name;
}

static int
parse_fontname(oc)
    XOC oc;
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    FontData font_data;
    char *pattern, *last, buf[BUFSIZ];
    int font_data_count, found_num = 0;
    ssize_t length;
    int count, num_fields;
    char *base_name, *font_name, **name_list, **cur_name_list;
    char *charset_p = NULL;
    Bool append_charset;
    /*
       append_charset flag should be set to True when the XLFD fontname
       doesn't contain a chaset part.
     */

    name_list = _XParseBaseFontNameList(oc->core.base_name_list, &count);
    if (name_list == NULL)
	return -1;
    cur_name_list = name_list;

    while (count-- > 0) {
        pattern = *cur_name_list++;
	if (pattern == NULL || *pattern == '\0')
	    continue;

	append_charset = False;

	if (strchr(pattern, '*') == NULL &&
	    (font_name = get_font_name(oc, pattern))) {

	    font_set = gen->font_set;

	    font_data = check_charset(font_set, font_name);
	    if (font_data == NULL) {
		Display *dpy = oc->core.om->core.display;
		char **fn_list = NULL, *prop_fname = NULL;
		int list_num;
		XFontStruct *fs_list;
		if ((fn_list = XListFontsWithInfo(dpy, font_name,
						  MAXFONTS,
						  &list_num, &fs_list))
		    && (prop_fname = get_prop_name(dpy, fs_list))
		    && (font_data = check_charset(font_set, prop_fname))) {
		    if (fn_list) {
			XFreeFontInfo(fn_list, fs_list, list_num);
			fn_list = NULL;
		    }
		    font_name = prop_fname;
		}
	    }
	    if (font_data == NULL)
		continue;

	    font_set->font_name = (char *) Xmalloc(strlen(font_name) + 1);
	    if (font_set->font_name == NULL) {
		Xfree(font_name);
		goto err;
	    }
	    strcpy(font_set->font_name, font_name);
	    Xfree(font_name);
	    found_num++;
	    goto found;
	}
/*
1266793
Limit the length of the string copy to prevent stack corruption.
	strcpy(buf, pattern);
*/
	strncpy(buf, pattern, BUFSIZ);
	buf[BUFSIZ-1] = '\0';
	length = strlen(buf);
	last = buf + length - 1;

	for (num_fields = 0, base_name = buf; *base_name != '\0'; base_name++)
	    if (*base_name == '-') num_fields++;
	if (strchr(pattern, '*') == NULL) {
	    if (num_fields == 12) {
		append_charset = True;
		*++last = '-';
		last++;
	    } else
		continue;
	} else {
	    if (num_fields == 13 || num_fields == 14) {
	    /* 
	     * There are 14 fields in an XLFD name -- make certain the
	     * charset (& encoding) is placed in the correct field.
	     */
		append_charset = True;
		last = strrchr (buf, '-');
		if (num_fields == 14) {
		    *last = '\0';
		    last = strrchr (buf, '-');
		}
		last++;
	    } else if (*last == '*') {
		append_charset = True;
		if (length > 3 && *(last-3) == '-' && *(last-2) == '*'
		    && *(last-1) == '-') {
		    last -= 2;
		}
		*++last = '-';
		last++;
	    } else {
		last = strrchr (buf, '-');
		charset_p = last;
		charset_p = strrchr (buf, '-');
		while (*(--charset_p) != '-');
		charset_p++;
	    }
	}

	font_set = gen->font_set;

	font_data = font_set->font_data;
	font_data_count = font_set->font_data_count;
	for ( ; font_data_count-- > 0; font_data++) {
	    if (append_charset)
		{
/*
1266793
Limit the length of the string copy to prevent stack corruption.
		strcpy(last, font_data->name);
*/
		strncpy(last, font_data->name, BUFSIZ - length);
		buf[BUFSIZ-1] = '\0';
		}
	    else {
		if (_XlcCompareISOLatin1(charset_p,
					 font_data->name)) {
		    continue;
		}
	    }
	    if ((font_set->font_name = get_font_name(oc, buf)))
		break;
	}
	if (font_set->font_name != NULL) {
	    found_num++;
	    goto found;
	}
    }
  found:
    base_name = (char *) Xmalloc(strlen(oc->core.base_name_list) + 1);
    if (base_name == NULL)
	goto err;

    strcpy(base_name, oc->core.base_name_list);
    oc->core.base_name_list = base_name;

    XFreeStringList(name_list);		

    return found_num;
err:
    XFreeStringList(name_list);		

    return -1;
}

static Bool
set_missing_list(oc)
    XOC oc;
{
    XOCGenericPart *gen = XOC_GENERIC(oc);
    FontSet font_set;
    char **charset_list, *charset_buf;
    int	count, length;

    font_set = gen->font_set;
    count = length = 0;

    if (!font_set->info && !font_set->font) {
	length += strlen(font_set->font_data->name) + 1;
	count++;
    }

    if (count == 0)
	return True;

    charset_list = (char **) Xmalloc(sizeof(char *));
    if (charset_list == NULL)
	return False;

    charset_buf = (char *) Xmalloc(length);
    if (charset_buf == NULL) {
	Xfree(charset_list);
	return False;
    }

    oc->core.missing_list.charset_list = charset_list;

    font_set = gen->font_set;

    if (!font_set->info && !font_set->font) {
	strcpy(charset_buf, font_set->font_data->name);
	*charset_list++ = charset_buf;
	charset_buf += strlen(charset_buf) + 1;
    }
    return True;
}

static Bool
create_fontset(oc)
    XOC oc;
{
    int found_num;

    if (init_fontset(oc) == False)
        return False;

    found_num = parse_fontname(oc);
    if (found_num <= 0) {
	if (found_num == 0)
	    set_missing_list(oc);
	return False;
    }

    if (load_font(oc) == False)
	return False;

    if (init_core_part(oc) == False)
	return False;

    if (set_missing_list(oc) == False)
	return False;

    return True;
}

static void
destroy_oc(oc)
    XOC oc;
{
    Display *dpy = oc->core.om->core.display;
    XOCGenericPart *gen = XOC_GENERIC(oc);
    XFontStruct **font_list, *font;

    if (gen->font_set)
	Xfree(gen->font_set);

    if (oc->core.base_name_list)
	Xfree(oc->core.base_name_list);

    if (oc->core.font_info.font_name_list)
	XFreeStringList(oc->core.font_info.font_name_list);

    if ((font_list = oc->core.font_info.font_struct_list)) {
	if ((font = *font_list)) {
	    if (font->fid)
		XFreeFont(dpy, font);
	    else
		XFreeFontInfo(NULL, font, 1);
	}
	Xfree(oc->core.font_info.font_struct_list);
    }

    if (oc->core.missing_list.charset_list)
	XFreeStringList(oc->core.missing_list.charset_list);

#ifdef notdef
    if (oc->core.res_name)
	Xfree(oc->core.res_name);
    if (oc->core.res_class)
	Xfree(oc->core.res_class);
#endif
    
    Xfree(oc);
}

static char *
set_oc_values(oc, args, num_args)
    XOC oc;
    XlcArgList args;
    int num_args;
{
    if (oc->core.resources == NULL)
	return NULL;

    return _XlcSetValues((XPointer) oc, oc->core.resources,
			 oc->core.num_resources, args, num_args, XlcSetMask);
}

static char *
get_oc_values(oc, args, num_args)
    XOC oc;
    XlcArgList args;
    int num_args;
{
    if (oc->core.resources == NULL)
	return NULL;

    return _XlcGetValues((XPointer) oc, oc->core.resources,
			 oc->core.num_resources, args, num_args, XlcGetMask);
}

static Bool
wcs_to_mbs(oc, to, from, length)
    XOC oc;
    char *to;
    _Xconst wchar_t *from;
    int length;
{
    XlcConv conv = XOC_GENERIC(oc)->wcs_to_cs;
    XLCd lcd;
    int ret, to_left = length;

    if (conv == NULL) {
	lcd = oc->core.om->core.lcd;
	conv = _XlcOpenConverter(lcd, XlcNWideChar, lcd, XlcNMultiByte);
	if (conv == NULL)
	    return False;
	XOC_GENERIC(oc)->wcs_to_cs = conv;
    } else
	_XlcResetConverter(conv);

    ret = _XlcConvert(conv, (XPointer *) &from, &length, (XPointer *) &to,
		      &to_left, NULL, 0);
    if (ret != 0 || length > 0)
	return False;
    
    return True;
}

static int
#if NeedFunctionPrototypes
_XmbDefaultTextEscapement(XOC oc, _Xconst char *text, int length)
#else
_XmbDefaultTextEscapement(oc, text, length)
    XOC oc;
    char *text;
    int length;
#endif
{
    return XTextWidth(*oc->core.font_info.font_struct_list, text, length);
}

static int
#if NeedFunctionPrototypes
_XwcDefaultTextEscapement(XOC oc, _Xconst wchar_t *text, int length)
#else
_XwcDefaultTextEscapement(oc, text, length)
    XOC oc;
    wchar_t *text;
    int length;
#endif
{
    DefineLocalBuf;
    char *buf = AllocLocalBuf(length);
    int ret = 0;

    if (buf == NULL)
	return 0;

    if (wcs_to_mbs(oc, buf, text, length) == False)
	goto err;

    ret = _XmbDefaultTextEscapement(oc, buf, length);

err:
    FreeLocalBuf(buf);

    return ret;
}

static int
#if NeedFunctionPrototypes
_XmbDefaultTextExtents(XOC oc, _Xconst char *text, int length,
		       XRectangle *overall_ink, XRectangle *overall_logical)
#else
_XmbDefaultTextExtents(oc, text, length, overall_ink, overall_logical)
    XOC oc;
    char *text;
    int length;
    XRectangle *overall_ink;
    XRectangle *overall_logical;
#endif
{
    int direction, logical_ascent, logical_descent;
    XCharStruct overall;

    XTextExtents(*oc->core.font_info.font_struct_list, text, length, &direction,
		 &logical_ascent, &logical_descent, &overall);

    if (overall_ink) {
	overall_ink->x = overall.lbearing;
	overall_ink->y = -(overall.ascent);
	overall_ink->width = overall.rbearing - overall.lbearing;
	overall_ink->height = overall.ascent + overall.descent;
    }

    if (overall_logical) {
	overall_logical->x = 0;
        overall_logical->y = -(logical_ascent);
	overall_logical->width = overall.width;
        overall_logical->height = logical_ascent + logical_descent;
    }

    return overall.width;
}

static int
#if NeedFunctionPrototypes
_XwcDefaultTextExtents(XOC oc, _Xconst wchar_t *text, int length,
		       XRectangle *overall_ink, XRectangle *overall_logical)
#else
_XwcDefaultTextExtents(oc, text, length, overall_ink, overall_logical)
    XOC oc;
    wchar_t *text;
    int length;
    XRectangle *overall_ink;
    XRectangle *overall_logical;
#endif
{
    DefineLocalBuf;
    char *buf = AllocLocalBuf(length);
    int ret = 0;

    if (buf == NULL)
	return 0;

    if (wcs_to_mbs(oc, buf, text, length) == False)
	goto err;

    ret = _XmbDefaultTextExtents(oc, buf, length, overall_ink, overall_logical);

err:
    FreeLocalBuf(buf);

    return ret;
}

static Status
#if NeedFunctionPrototypes
_XmbDefaultTextPerCharExtents(XOC oc, _Xconst char *text, int length,
			      XRectangle *ink_buf, XRectangle *logical_buf,
			      int buf_size, int *num_chars,
			      XRectangle *overall_ink,
			      XRectangle *overall_logical)
#else
_XmbDefaultTextPerCharExtents(oc, text, length, ink_buf, logical_buf, buf_size,
			      num_chars, overall_ink, overall_logical)
    XOC oc;
    char *text;	
    int length;
    XRectangle *ink_buf;
    XRectangle *logical_buf;
    int buf_size;
    int *num_chars;
    XRectangle *overall_ink;
    XRectangle *overall_logical;
#endif
{
    XFontStruct *font = *oc->core.font_info.font_struct_list;
    XCharStruct *def, *cs, overall;
    Bool first = True;

    if (buf_size < length)
	return 0;

    bzero((char *) &overall, sizeof(XCharStruct));
    *num_chars = 0;

    CI_GET_DEFAULT_INFO_1D(font, def)

    while (length-- > 0) {
	CI_GET_CHAR_INFO_1D(font, *text, def, cs)
	text++;
	if (cs == NULL)
	    continue;

	ink_buf->x = overall.width + cs->lbearing;
	ink_buf->y = -(cs->ascent);
	ink_buf->width = cs->rbearing - cs->lbearing;
	ink_buf->height = cs->ascent + cs->descent;
	ink_buf++;

	logical_buf->x = overall.width;
	logical_buf->y = -(font->ascent);
	logical_buf->width = cs->width;
	logical_buf->height = font->ascent + font->descent;
	logical_buf++;

	if (first) {
	    overall = *cs;
	    first = False;
	} else {
	    overall.ascent = max(overall.ascent, cs->ascent);
	    overall.descent = max(overall.descent, cs->descent);
	    overall.lbearing = min(overall.lbearing, overall.width +
				   cs->lbearing);
	    overall.rbearing = max(overall.rbearing, overall.width +
				   cs->rbearing);
	    overall.width += cs->width;
	}
   	(*num_chars)++; 
    }

    if (overall_ink) {
	overall_ink->x = overall.lbearing;
	overall_ink->y = -(overall.ascent);
	overall_ink->width = overall.rbearing - overall.lbearing;
	overall_ink->height = overall.ascent + overall.descent;
    }

    if (overall_logical) {
	overall_logical->x = 0;
	overall_logical->y = -(font->ascent);
	overall_logical->width = overall.width;
	overall_logical->height = font->ascent + font->descent;
    }

    return 1;
}

static Status
#if NeedFunctionPrototypes
_XwcDefaultTextPerCharExtents(XOC oc, _Xconst wchar_t *text, int length,
			      XRectangle *ink_buf, XRectangle *logical_buf,
			      int buf_size, int *num_chars,
			      XRectangle *overall_ink,
			      XRectangle *overall_logical)
#else
_XwcDefaultTextPerCharExtents(oc, text, length, ink_buf, logical_buf, buf_size,
			      num_chars, overall_ink, overall_logical)
    XOC oc;
    wchar_t *text;
    int length;
    XRectangle *ink_buf;
    XRectangle *logical_buf;
    int buf_size;
    int *num_chars;
    XRectangle *overall_ink;
    XRectangle *overall_logical;
#endif
{
    DefineLocalBuf;
    char *buf = AllocLocalBuf(length);
    Status ret = 0;

    if (buf == NULL)
	return 0;

    if (wcs_to_mbs(oc, buf, text, length) == False)
	goto err;

    ret = _XmbDefaultTextPerCharExtents(oc, buf, length, ink_buf, logical_buf,
					buf_size, num_chars, overall_ink,
					overall_logical);

err:
    FreeLocalBuf(buf);

    return ret;
}

static int
#if NeedFunctionPrototypes
_XmbDefaultDrawString(Display *dpy, Drawable d, XOC oc, GC gc, int x, int y,
		      _Xconst char *text, int length)
#else
_XmbDefaultDrawString(dpy, d, oc, gc, x, y, text, length)
    Display *dpy;
    Drawable d;
    XOC oc;
    GC gc;
    int x, y;
    char *text;
    int length;
#endif
{
    XFontStruct *font = *oc->core.font_info.font_struct_list;

    XSetFont(dpy, gc, font->fid);
    XDrawString(dpy, d, gc, x, y, text, length);

    return XTextWidth(font, text, length);
}

static int
#if NeedFunctionPrototypes
_XwcDefaultDrawString(Display *dpy, Drawable d, XOC oc, GC gc, int x, int y,
		      _Xconst wchar_t *text, int length)
#else
_XwcDefaultDrawString(dpy, d, oc, gc, x, y, text, length)
    Display *dpy;
    Drawable d;
    XOC oc;
    GC gc;
    int x, y;
    wchar_t *text;
    int length;
#endif
{
    DefineLocalBuf;
    char *buf = AllocLocalBuf(length);
    int ret = 0;

    if (buf == NULL)
	return 0;

    if (wcs_to_mbs(oc, buf, text, length) == False)
	goto err;

    ret = _XmbDefaultDrawString(dpy, d, oc, gc, x, y, buf, length);

err:
    FreeLocalBuf(buf);

    return ret;
}

static void
#if NeedFunctionPrototypes
_XmbDefaultDrawImageString(Display *dpy, Drawable d, XOC oc, GC gc, int x,
			   int y, _Xconst char *text, int length)
#else
_XmbDefaultDrawImageString(dpy, d, oc, gc, x, y, text, length)
    Display *dpy;
    Drawable d;
    XOC oc;
    GC gc;
    int x, y;
    char *text;
    int length;
#endif
{
    XSetFont(dpy, gc, (*oc->core.font_info.font_struct_list)->fid);
    XDrawImageString(dpy, d, gc, x, y, text, length);
}

static void
#if NeedFunctionPrototypes
_XwcDefaultDrawImageString(Display *dpy, Drawable d, XOC oc, GC gc, int x,
			   int y, _Xconst wchar_t *text, int length)
#else
_XwcDefaultDrawImageString(dpy, d, oc, gc, x, y, text, length)
    Display *dpy;
    Drawable d;
    XOC oc;
    GC gc;
    int x, y;
    wchar_t *text;
    int length;
#endif
{
    DefineLocalBuf;
    char *buf = AllocLocalBuf(length);

    if (buf == NULL)
	return;

    if (wcs_to_mbs(oc, buf, text, length) == False)
	goto err;

    _XmbDefaultDrawImageString(dpy, d, oc, gc, x, y, buf, length);

err:
    FreeLocalBuf(buf);
}

static _Xconst XOCMethodsRec oc_default_methods = {
    destroy_oc,
    set_oc_values,
    get_oc_values,
    _XmbDefaultTextEscapement,
    _XmbDefaultTextExtents,
    _XmbDefaultTextPerCharExtents,
    _XmbDefaultDrawString,
    _XmbDefaultDrawImageString,
    _XwcDefaultTextEscapement,
    _XwcDefaultTextExtents,
    _XwcDefaultTextPerCharExtents,
    _XwcDefaultDrawString,
    _XwcDefaultDrawImageString
};

static XlcResource oc_resources[] = {
    { XNBaseFontName, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.base_name_list), XlcCreateMask | XlcGetMask },
    { XNOMAutomatic, NULLQUARK, sizeof(Bool),
      XOffsetOf(XOCRec, core.om_automatic), XlcGetMask },
    { XNMissingCharSet, NULLQUARK, sizeof(XOMCharSetList),
      XOffsetOf(XOCRec, core.missing_list), XlcGetMask },
    { XNDefaultString, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.default_string), XlcGetMask },
    { XNOrientation, NULLQUARK, sizeof(XOrientation),
      XOffsetOf(XOCRec, core.orientation), XlcSetMask | XlcGetMask },
    { XNResourceName, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.res_name), XlcSetMask | XlcGetMask },
    { XNResourceClass, NULLQUARK, sizeof(char *),
      XOffsetOf(XOCRec, core.res_class), XlcSetMask | XlcGetMask },
    { XNFontInfo, NULLQUARK, sizeof(XOMFontInfo),
      XOffsetOf(XOCRec, core.font_info), XlcGetMask }
};

static XOC
create_oc(om, args, num_args)
    XOM om;
    XlcArgList args;
    int num_args;
{
    XOC oc;

    oc = (XOC) Xmalloc(sizeof(XOCGenericRec));
    if (oc == NULL)
	return (XOC) NULL;
    bzero((char *) oc, sizeof(XOCGenericRec));
    
    oc->core.om = om;

    if (oc_resources[0].xrm_name == NULLQUARK)
	_XlcCompileResourceList(oc_resources, XlcNumber(oc_resources));
    
    if (_XlcSetValues((XPointer) oc, oc_resources, XlcNumber(oc_resources),
		      args, num_args, XlcCreateMask | XlcDefaultMask))
	goto err;

    if (oc->core.base_name_list == NULL)
	goto err;

    oc->core.resources = oc_resources;
    oc->core.num_resources = XlcNumber(oc_resources);

    if (create_fontset(oc) == False)
	goto err;

    oc->methods = (XOCMethods)&oc_default_methods;

    return oc;

err:
    destroy_oc(oc);

    return (XOC) NULL;
}

static Status
close_om(om)
    XOM om;
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData data;
    FontData font_data;
    int count;

    if ((data = gen->data)) {
	if (data->font_data) {
	  for (font_data = data->font_data, count = data->font_data_count;
	       count-- > 0 ; font_data++) { 
	    if (font_data->name)
		Xfree(font_data->name);
	  }
	  Xfree(data->font_data);
	}
	Xfree(gen->data);
    }

    if (om->core.res_name)
	Xfree(om->core.res_name);
    if (om->core.res_class)
	Xfree(om->core.res_class);
    if (om->core.required_charset.charset_list)
	XFreeStringList(om->core.required_charset.charset_list);
    else
	Xfree((char*)om->core.required_charset.charset_list);
    if (om->core.orientation_list.orientation)
	Xfree(om->core.orientation_list.orientation);

    Xfree(om);

    return 1;
}

static char *
set_om_values(om, args, num_args)
    XOM om;
    XlcArgList args;
    int num_args;
{
    if (om->core.resources == NULL)
	return NULL;

    return _XlcSetValues((XPointer) om, om->core.resources,
			 om->core.num_resources, args, num_args, XlcSetMask);
}

static char *
get_om_values(om, args, num_args)
    XOM om;
    XlcArgList args;
    int num_args;
{
    if (om->core.resources == NULL)
	return NULL;

    return _XlcGetValues((XPointer) om, om->core.resources,
			 om->core.num_resources, args, num_args, XlcGetMask);
}

static _Xconst XOMMethodsRec methods = {
    close_om,
    set_om_values,
    get_om_values,
    create_oc
};

static XlcResource om_resources[] = {
    { XNRequiredCharSet, NULLQUARK, sizeof(XOMCharSetList),
      XOffsetOf(XOMRec, core.required_charset), XlcGetMask },
    { XNQueryOrientation, NULLQUARK, sizeof(XOMOrientation),
      XOffsetOf(XOMRec, core.orientation_list), XlcGetMask },
    { XNDirectionalDependentDrawing, NULLQUARK, sizeof(Bool),
      XOffsetOf(XOMRec, core.directional_dependent), XlcGetMask },
    { XNContextualDrawing, NULLQUARK, sizeof(Bool),
      XOffsetOf(XOMRec, core.contextual_drawing), XlcGetMask }
};

static OMData
add_data(om)
    XOM om;
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData new;

    new = (OMData) Xmalloc(sizeof(OMDataRec));

    if (new == NULL)
        return NULL;

    gen->data = new;

    bzero((char *) new, sizeof(OMDataRec));

    return new;
}

static _Xconst char *supported_charset_list[] = {
    "ISO8859-1",
/* fix for bug4332979 */
    "adobe-fontspecific",
/* fix for bug4237353: "JISX0201.1976-0" entry should be removed from
   supported_charset_list because it is not a supported_charset for C locale
    "JISX0201.1976-0", */
    "SUNOLCURSOR-1",
    "SUNOLGLYPH-1"
};
    
static Bool
init_om(om)
    XOM om;
{
    XOMGenericPart *gen = XOM_GENERIC(om);
    OMData data;
    FontData font_data;
    char **required_list;
    XOrientation *orientation;
    char **value, buf[BUFSIZ], *bufptr;
    int count, length = 0;

    value = (char**)supported_charset_list;
    count = XlcNumber(supported_charset_list);

    data = add_data(om);
    if (data == NULL)
	return False;

    font_data = (FontData) Xmalloc(sizeof(FontDataRec) * count);
    if (font_data == NULL)
	return False;
    bzero((char *) font_data, sizeof(FontDataRec) * count);
    data->font_data = font_data;
    data->font_data_count = count;

    for ( ; count-- > 0; font_data++) {
/*
1266793
This one is fine.  *value points to one of the local strings in
supported_charset_list[].
*/
	strcpy(buf, *value++);
	font_data->name = (char *) Xmalloc(strlen(buf) + 1);
	if (font_data->name == NULL)
	    return False;
	strcpy(font_data->name, buf);
    }

    length += strlen(data->font_data->name) + 1;

    /* required charset list */
    required_list = (char **) Xmalloc(sizeof(char *));
    if (required_list == NULL)
	return False;

    bufptr = (char *) Xmalloc(length);
    if (bufptr == NULL) {
	Xfree(required_list);
	return False;
    }

    om->core.required_charset.charset_list = required_list;
    om->core.required_charset.charset_count = 1; /* always 1 */

    data = gen->data;

    strcpy(bufptr, data->font_data->name);
    *required_list++ = bufptr;
    bufptr += strlen(bufptr) + 1;

    /* orientation list */
    orientation = (XOrientation *) Xmalloc(sizeof(XOrientation));
    if (orientation == NULL)
	return False;

    *orientation = XOMOrientation_LTR_TTB;
    om->core.orientation_list.orientation = orientation;
    om->core.orientation_list.num_orientation = 1;

    /* directional dependent drawing */
    om->core.directional_dependent = False;

    /* contexual drawing */
    om->core.contextual_drawing = False;

    /* context dependent */
    om->core.context_dependent = False;

    return True;
}

XOM
#if NeedFunctionPrototypes
_XDefaultOpenOM(XLCd lcd, Display *dpy, XrmDatabase rdb,
		_Xconst char *res_name, _Xconst char *res_class)
#else
_XDefaultOpenOM(lcd, dpy, rdb, res_name, res_class)
XLCd lcd;
Display *dpy;
XrmDatabase rdb;
char *res_name;
char *res_class;
#endif
{
    XOM om;

    om = (XOM) Xmalloc(sizeof(XOMGenericRec));
    if (om == NULL)
	return (XOM) NULL;
    bzero((char *) om, sizeof(XOMGenericRec));
    
    om->methods = (XOMMethods)&methods;
    om->core.lcd = lcd;
    om->core.display = dpy;
    om->core.rdb = rdb;
    if (res_name) {
	om->core.res_name = (char *)Xmalloc(strlen(res_name) + 1);
	if (om->core.res_name == NULL)
	    goto err;
	strcpy(om->core.res_name, res_name);
    }
    if (res_class) {
	om->core.res_class = (char *)Xmalloc(strlen(res_class) + 1);
	if (om->core.res_class == NULL)
	    goto err;
	strcpy(om->core.res_class, res_class);
    }

    if (om_resources[0].xrm_name == NULLQUARK)
	_XlcCompileResourceList(om_resources, XlcNumber(om_resources));
    
    om->core.resources = om_resources;
    om->core.num_resources = XlcNumber(om_resources);

    if (init_om(om) == False)
	goto err;

    return om;
err:
    close_om(om);

    return (XOM) NULL;
}
