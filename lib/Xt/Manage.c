/* $Xorg: Manage.c,v 1.4 2001/02/09 02:03:55 xorgcvs Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
Copyright 1993 by Sun Microsystems, Inc. Mountain View, CA.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or Sun not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/
/* $XFree86: xc/lib/Xt/Manage.c,v 3.7 2001/08/22 22:52:19 dawes Exp $ */

/*

Copyright 1987, 1988, 1994, 1998  The Open Group

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

#include "IntrinsicI.h"

static String XtNinvalidChild = "invalidChild";
static String XtNxtUnmanageChildren = "xtUnmanageChildren";
static String XtNxtManageChildren = "xtManageChildren";
static String XtNxtChangeManagedSet = "xtChangeManagedSet";

static void UnmanageChildren(
    WidgetList children,
    Cardinal num_children,
    Widget parent,
    Cardinal* num_unique_children,
    Boolean call_change_managed,
    String caller_func)
{
    Widget		child;
    Cardinal		i;
    XtWidgetProc	change_managed = NULL;
    Bool		parent_realized = False;

    *num_unique_children = 0;

    if (XtIsComposite((Widget) parent)) {
	LOCK_PROCESS;
        change_managed = ((CompositeWidgetClass) parent->core.widget_class)
		    ->composite_class.change_managed;
	UNLOCK_PROCESS;
	parent_realized = XtIsRealized((Widget)parent);
    } else {
        XtAppErrorMsg(XtWidgetToApplicationContext((Widget)parent),
		      "invalidParent",caller_func, XtCXtToolkitError,
		   "Attempt to unmanage a child when parent is not Composite",
		      (String *) NULL, (Cardinal *) NULL);
    }

    for (i = 0; i < num_children; i++) {
	child = children[i];
	if (child == NULL) {
	    XtAppWarningMsg(XtWidgetToApplicationContext(parent),
		  XtNinvalidChild,caller_func,XtCXtToolkitError,
                  "Null child passed to XtUnmanageChildren",
		  (String *)NULL, (Cardinal *)NULL);
	    return;
	}
        if (child->core.parent != parent) {
	   XtAppWarningMsg(XtWidgetToApplicationContext(parent),
		   "ambiguousParent",caller_func,XtCXtToolkitError,
           "Not all children have same parent in UnmanageChildren",
             (String *)NULL, (Cardinal *)NULL);
	} else
        if (child->core.managed) {
            (*num_unique_children)++;
	    CALLGEOTAT(_XtGeoTrace(child,"Child \"%s\" is marked unmanaged\n", 
			   XtName(child)));
	    child->core.managed = FALSE;
            if (XtIsWidget(child)
		&& XtIsRealized(child)
		&& child->core.mapped_when_managed)
                    XtUnmapWidget(child);
            else
	    { /* RectObj child */
		Widget pw = child->core.parent;
		RectObj r = (RectObj) child;
		while ((pw!=NULL) && (!XtIsWidget(pw))) pw = pw->core.parent;
		if ((pw!=NULL) && XtIsRealized (pw))
		    XClearArea (XtDisplay (pw), XtWindow (pw),
			r->rectangle.x, r->rectangle.y,
			r->rectangle.width + (r->rectangle.border_width << 1),
			r->rectangle.height + (r->rectangle.border_width << 1),
			TRUE);
	    }

        }
    }
    if (call_change_managed && *num_unique_children != 0 &&
	change_managed != NULL && parent_realized) {
	CALLGEOTAT(_XtGeoTrace((Widget)parent,
		       "Call parent: \"%s\"[%d,%d]'s changemanaged proc\n", 
		       XtName((Widget)parent),
		       parent->core.width,parent->core.height));
	(*change_managed) (parent);
    }
} /* UnmanageChildren */

void XtUnmanageChildren (children, num_children)
    WidgetList children;
    Cardinal num_children;
{
    Widget parent, hookobj;
    Cardinal ii;
    XtAppContext app;

    if (num_children == 0) return;
    if (children[0] == NULL) {
	XtWarningMsg(XtNinvalidChild,XtNxtUnmanageChildren,XtCXtToolkitError,
		     "Null child found in argument list to unmanage",
		     (String *)NULL, (Cardinal *)NULL);
	return;
    }
    app = XtWidgetToApplicationContext(children[0]);
    LOCK_APP(app);
    parent = children[0]->core.parent;
    if (parent->core.being_destroyed) {
	UNLOCK_APP(app);
	return;
    }
    UnmanageChildren(children, num_children, parent, &ii, 
		     (Boolean)True, XtNxtUnmanageChildren);
    hookobj = XtHooksOfDisplay(XtDisplayOfObject(children[0]));
    if (XtHasCallbacks(hookobj, XtNchangeHook) == XtCallbackHasSome) {
	XtChangeHookDataRec call_data;

	call_data.type = XtHunmanageChildren;
	call_data.widget = parent;
	call_data.event_data = (XtPointer) children;
	call_data.num_event_data = num_children;
	XtCallCallbackList(hookobj, 
		((HookObject)hookobj)->hooks.changehook_callbacks, 
		(XtPointer)&call_data);
    }
    UNLOCK_APP(app);
} /* XtUnmanageChildren */

void XtUnmanageChild(child)
    Widget child;
{
    XtUnmanageChildren(&child, (Cardinal)1);
} /* XtUnmanageChild */


static void ManageChildren(
    WidgetList  children,
    Cardinal    num_children,
    Widget	parent,
    Boolean	call_change_managed,
    String	caller_func)
{
#define MAXCHILDREN 100
    Widget		child;
    Cardinal		num_unique_children, i;
    XtWidgetProc	change_managed = NULL;
    WidgetList		unique_children;
    Widget		cache[MAXCHILDREN];
    Bool		parent_realized = False;

    if (XtIsComposite((Widget) parent)) {
	LOCK_PROCESS;
        change_managed = ((CompositeWidgetClass) parent->core.widget_class)
		    ->composite_class.change_managed;
	UNLOCK_PROCESS;
	parent_realized = XtIsRealized((Widget)parent);
    } else {
	XtAppErrorMsg(XtWidgetToApplicationContext((Widget)parent),
		"invalidParent",caller_func, XtCXtToolkitError,
	    "Attempt to manage a child when parent is not Composite",
	    (String *) NULL, (Cardinal *) NULL);
    }

    /* Construct new list of children that really need to be operated upon. */
    if (num_children <= MAXCHILDREN) {
	unique_children = cache;
    } else {
	unique_children = (WidgetList) __XtMalloc(num_children * sizeof(Widget));
    }
    num_unique_children = 0;
    for (i = 0; i < num_children; i++) {
	child = children[i];
	if (child == NULL) {
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
		XtNinvalidChild,caller_func,XtCXtToolkitError,
		"null child passed to ManageChildren",
		(String *)NULL, (Cardinal *)NULL);
	    if (unique_children != cache) XtFree((char *) unique_children);
	    return;
	}
#ifdef DEBUG
	if (!XtIsRectObj(child)) {
	    String params[2];
	    Cardinal num_params = 2;
	    params[0] = XtName(child);
	    params[1] = child->core.widget_class->core_class.class_name;
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
			    "notRectObj",caller_func,XtCXtToolkitError,
			    "child \"%s\", class %s is not a RectObj",
			    params, &num_params);
	    continue;
	}
#endif /*DEBUG*/
        if (child->core.parent != parent) {
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)parent),
		    "ambiguousParent",caller_func,XtCXtToolkitError,
		"Not all children have same parent in XtManageChildren",
		(String *)NULL, (Cardinal *)NULL);
	} else if (! child->core.managed && !child->core.being_destroyed) {
	    unique_children[num_unique_children++] = child;
	    CALLGEOTAT(_XtGeoTrace(child,
			   "Child \"%s\"[%d,%d] is marked managed\n", 
			   XtName(child),
			   child->core.width,child->core.height));
	    child->core.managed = TRUE;
	}
    }

    if ((call_change_managed || num_unique_children != 0) && parent_realized) {
	/* Compute geometry of new managed set of children. */
	if (change_managed != NULL) {
	    CALLGEOTAT(_XtGeoTrace((Widget)parent,
			   "Call parent: \"%s\"[%d,%d]'s changemanaged\n", 
			   XtName((Widget)parent), 
			   parent->core.width,parent->core.height));
	    (*change_managed) ((Widget)parent);
	}

	/* Realize each child if necessary, then map if necessary */
	for (i = 0; i < num_unique_children; i++) {
	    child = unique_children[i];
	    if (XtIsWidget(child)) {
		if (! XtIsRealized(child)) XtRealizeWidget(child);
		if (child->core.mapped_when_managed) XtMapWidget(child);
	    } else { /* RectObj child */
		Widget pw = child->core.parent;
		RectObj r = (RectObj) child;
		while ((pw!=NULL) && (!XtIsWidget(pw)))
		    pw = pw->core.parent;
		if (pw != NULL)
		    XClearArea (XtDisplay (pw), XtWindow (pw),
		    r->rectangle.x, r->rectangle.y,
		    r->rectangle.width + (r->rectangle.border_width << 1),
		    r->rectangle.height + (r->rectangle.border_width << 1),
		    TRUE);
            }
        }
    }

    if (unique_children != cache) XtFree((char *) unique_children);
} /* ManageChildren */

void XtManageChildren(children, num_children)
    WidgetList children;
    Cardinal num_children;
{
    Widget parent, hookobj;
    XtAppContext app;

    if (num_children == 0) return;
    if (children[0] == NULL) {
	XtWarningMsg(XtNinvalidChild, XtNxtManageChildren, XtCXtToolkitError,
		     "null child passed to XtManageChildren",
		     (String*)NULL, (Cardinal*)NULL);
	return;
    }
    app = XtWidgetToApplicationContext(children[0]);
    LOCK_APP(app);
    parent = children[0]->core.parent;
    if (parent->core.being_destroyed) {
	UNLOCK_APP(app);
	return;
    }
    ManageChildren(children, num_children, parent, (Boolean)False, 
		   XtNxtManageChildren);
    hookobj = XtHooksOfDisplay(XtDisplayOfObject(children[0]));
    if (XtHasCallbacks(hookobj, XtNchangeHook) == XtCallbackHasSome) {
	XtChangeHookDataRec call_data;

	call_data.type = XtHmanageChildren;
	call_data.widget = parent;
	call_data.event_data = (XtPointer) children;
	call_data.num_event_data = num_children;
	XtCallCallbackList(hookobj, 
		((HookObject)hookobj)->hooks.changehook_callbacks, 
		(XtPointer)&call_data);
    }
    UNLOCK_APP(app);
} /* XtManageChildren */

void XtManageChild(child)
    Widget child;
{
    XtManageChildren(&child, (Cardinal) 1);
} /* XtManageChild */


#if NeedFunctionPrototypes
void XtSetMappedWhenManaged(
    Widget widget,
    _XtBoolean mapped_when_managed
    )
#else
void XtSetMappedWhenManaged(widget, mapped_when_managed)
    Widget widget;
    Boolean mapped_when_managed;
#endif
{
    Widget hookobj;
    WIDGET_TO_APPCON(widget);

    LOCK_APP(app);
    if (widget->core.mapped_when_managed == mapped_when_managed) {
	UNLOCK_APP(app);
	return;
    }
    widget->core.mapped_when_managed = mapped_when_managed;

    hookobj = XtHooksOfDisplay(XtDisplay(widget));
    if (XtHasCallbacks(hookobj, XtNchangeHook) == XtCallbackHasSome) {
	XtChangeHookDataRec call_data;

	call_data.type = XtHsetMappedWhenManaged;
	call_data.widget = widget;
	call_data.event_data = (XtPointer) (unsigned long) mapped_when_managed;
	XtCallCallbackList(hookobj, 
		((HookObject)hookobj)->hooks.changehook_callbacks, 
		(XtPointer)&call_data);
    }

    if (! XtIsManaged(widget)) {
	UNLOCK_APP(app);
	return;
    }

    if (mapped_when_managed) {
	/* Didn't used to be mapped when managed.		*/
	if (XtIsRealized(widget)) XtMapWidget(widget);
    } else {
	/* Used to be mapped when managed.			*/
	if (XtIsRealized(widget)) XtUnmapWidget(widget);
    }
    UNLOCK_APP(app);
} /* XtSetMappedWhenManaged */


#if NeedFunctionPrototypes
void XtChangeManagedSet(
    WidgetList unmanage_children,
    Cardinal num_unmanage,
    XtDoChangeProc do_change_proc,
    XtPointer client_data,
    WidgetList manage_children,
    Cardinal num_manage
)
#else
void XtChangeManagedSet(unmanage_children, num_unmanage, do_change_proc,
			client_data, manage_children, num_manage)
    WidgetList unmanage_children;
    Cardinal num_unmanage;
    XtDoChangeProc do_change_proc;
    XtPointer client_data;
    WidgetList manage_children;
    Cardinal num_manage;
#endif
{
    WidgetList childp;
    Widget parent;
    int i;
    Cardinal some_unmanaged;
    Boolean call_out;
    CompositeClassExtension ext;
    XtAppContext app;
    Widget hookobj;
    XtChangeHookDataRec call_data;

    if (num_unmanage == 0 && num_manage == 0)
	return;

    /* specification doesn't state that library will check for NULL in list */

    childp = num_unmanage ? unmanage_children : manage_children;
    app = XtWidgetToApplicationContext(*childp);
    LOCK_APP(app);

    parent = XtParent(*childp);
    childp = unmanage_children;
    for (i = num_unmanage; --i >= 0 && XtParent(*childp) == parent; childp++);
    call_out = (i >= 0);
    childp = manage_children;
    for (i = num_manage;   --i >= 0 && XtParent(*childp) == parent; childp++);
    if (call_out || i >= 0) {
	XtAppWarningMsg(app, "ambiguousParent", XtNxtChangeManagedSet,
			XtCXtToolkitError, "Not all children have same parent",
			(String *)NULL, (Cardinal *)NULL);
    }
    if (! XtIsComposite(parent)) {
	UNLOCK_APP(app);
	XtAppErrorMsg(app, "invalidParent", XtNxtChangeManagedSet,
		      XtCXtToolkitError,
		      "Attempt to manage a child when parent is not Composite",
		      (String *) NULL, (Cardinal *) NULL);
    }
    if (parent->core.being_destroyed) {
	UNLOCK_APP(app);
	return;
    }

    call_out = False;
    if (do_change_proc) {
	ext = (CompositeClassExtension)
	    XtGetClassExtension(parent->core.widget_class,
				XtOffsetOf(CompositeClassRec,
					   composite_class.extension),
				NULLQUARK, XtCompositeExtensionVersion,
				sizeof(CompositeClassExtensionRec));
	if (!ext || !ext->allows_change_managed_set)
	    call_out = True;
    }

    UnmanageChildren(unmanage_children, num_unmanage, parent, 
		     &some_unmanaged, call_out, XtNxtChangeManagedSet);

    hookobj = XtHooksOfDisplay(XtDisplay(parent));
    if (XtHasCallbacks(hookobj, XtNchangeHook) == XtCallbackHasSome) {
	call_data.type = XtHunmanageSet;
	call_data.widget = parent;
	call_data.event_data = (XtPointer) unmanage_children;
	call_data.num_event_data = num_unmanage;
	XtCallCallbackList(hookobj, 
		((HookObject)hookobj)->hooks.changehook_callbacks, 
		(XtPointer) &call_data);
    }

    if (do_change_proc) 
	(*do_change_proc)(parent, unmanage_children, &num_unmanage,
			  manage_children, &num_manage, client_data);

    call_out = (some_unmanaged && !call_out);
    ManageChildren(manage_children, num_manage, parent, call_out,
		   XtNxtChangeManagedSet);

    if (XtHasCallbacks(hookobj, XtNchangeHook) == XtCallbackHasSome) {
	call_data.type = XtHmanageSet;
	call_data.event_data = (XtPointer) manage_children;
	call_data.num_event_data = num_manage;
	XtCallCallbackList(hookobj, 
		((HookObject)hookobj)->hooks.changehook_callbacks, 
		(XtPointer) &call_data);
    }
    UNLOCK_APP(app);
} /* XtChangeManagedSet */
