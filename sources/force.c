/*
 * This source module is derived from an example program from Chapter 21 of
 * Volume Six A of the O'Reilly & Associates, Inc. book, "Motif Programming
 * Manual", by Dan Heller & Paula M. Ferguson.  Here is the required copy-
 * right notice:
 *
 *      Written by Dan Heller and Paula Ferguson.  
 *      Copyright 1994, O'Reilly & Associates, Inc.
 *      Permission to use, copy, and modify this program without
 *      restriction is hereby granted, as long as this copyright
 *      notice appears in each copy of the program source code.
 *      This program is freely distributable without licensing fees and
 *      is provided without guarantee or warrantee expressed or implied.
 *      This program is -not- in the public domain.
 */


#include <Xm/Xm.h>

/*
 * This function is a superset of XmUpdateDisplay() that ensures
 * that a window's contents are visible before returning.
 * The monitoring of window states is necessary because an attempt to
 * map a window is subject to the whim of the window manager, which can
 * introduce a significant delay before the window is actually mapped
 * and exposed.  This function is intended to be called after XtPopup(),
 * XtManageChild() or XMapRaised().  Don't use it in other situations
 * as it may sit and process other unrelated events until the widget
 * becomes visible.
 */
force_update(w)
Widget w;  /* This widget must be visible before the function returns */
{
	Widget diashell, topshell;
	Window diawindow, topwindow;
	XtAppContext cxt = XtWidgetToApplicationContext(w);
	Display *dpy;
	XWindowAttributes xwa;
	XEvent event;

	/* Locate the shell we are interested in */
	for (diashell=w; !XtIsShell(diashell); diashell=XtParent(diashell))
		;

	/* Locate its primary window's shell (which may be the same) */
	for (topshell = diashell; !XtIsTopLevelShell(topshell);
    		topshell = XtParent(topshell))
    	;

	/* If the dialog shell (or its primary shell window) is not realized,
	 * don't bother ... nothing can possibly happen
	 */
	if (XtIsRealized(diashell) && XtIsRealized(topshell)) {
		dpy = XtDisplay(topshell);
		diawindow = XtWindow(diashell);
		topwindow = XtWindow(topshell);

		/* Wait for the dialog to be mapped.  It's guaranteed to become so */
		while (XGetWindowAttributes(dpy, diawindow, &xwa),
			xwa.map_state != IsViewable) {

			/* ...if the primary is (or becomes) unviewable or unmapped,
			 * it's probably iconic, and nothing will happen.
			 */
			if (XGetWindowAttributes(dpy, topwindow, &xwa) &&
				xwa.map_state != IsViewable)
				break;

			/* we are guaranteed there will be an event of some kind */
			XtAppNextEvent(cxt, &event);
			XtDispatchEvent(&event);
		}
	}

	/* The next XSync() will get an expose event */
	XmUpdateDisplay(topshell);
}

