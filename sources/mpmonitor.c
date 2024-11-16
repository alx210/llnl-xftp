/***************************************************************************/
/***************************************************************************/
/*                                                                         */
/*   (c) 1993-1995.  The Regents of the University of California.  All     */
/*   rights reserved.                                                      */
/*                                                                         */
/*   This work was produced at the University of California, Lawrence      */
/*   Livermore National Laboratory (UC LLNL) under contract no.            */
/*   W-7405-ENG-48 (Contract 48) between the U.S. Department of Energy     */
/*   (DOE) and The Regents of the University of California (University)    */
/*   for the operation of UC LLNL.  Copyright is reserved to the           */
/*   University for purposes of controlled dissemination,                  */
/*   commercialization through formal licensing, or other disposition      */
/*   under terms of Contract 48; DOE policies, regulations and orders;     */
/*   and U.S. statutes.  The rights of the Federal Government are          */
/*   reserved under Contract 48 subject to the restrictions agreed upon    */
/*   by the DOE and University.                                            */
/*                                                                         */
/*                                                                         */
/*                              DISCLAIMER                                 */
/*                                                                         */
/*   This software was prepared as an account of work sponsored by an      */
/*   agency of the United States Government.  Neither the United States    */
/*   Government nor the University of California nor any of their          */
/*   employees, makes any warranty, express or implied, or assumes any     */
/*   liability or responsibility for the accuracy, completeness, or        */
/*   usefulness of any information, apparatus, product, or process         */
/*   disclosed, or represents that its specific commercial products,       */
/*   process, or service by trade name, trademark, manufacturer, or        */
/*   otherwise, does not necessarily constitute or imply its               */
/*   endorsement, recommendation, or favoring by the United States         */
/*   Government or the University of California. The views and opinions    */
/*   of the authors expressed herein do not necessarily state or reflect   */
/*   those of the United States Government or the University of            */
/*   California, and shall not be used for advertising or product          */
/*   endorsement purposes.                                                 */
/*                                                                         */
/*   Permission to use, copy, modify and distribute this software and its  */
/*   documentation for any non-commercial purpose, without fee, is         */
/*   hereby granted, provided that the above copyright notice and this     */
/*   permission notice appear in all copies of the software and            */
/*   supporting documentation, and that all UC LLNL identification in      */
/*   the user interface remain unchanged.  The title to copyright LLNL     */
/*   XFTP shall at all times remain with The Regents of the University     */
/*   of California and users agree to preserve same. Users seeking the     */
/*   right to make derivative works with LLNL XFTP for commercial          */
/*   purposes may obtain a license from the Lawrence Livermore National    */
/*   Laboratory's Technology Transfer Office, P.O. Box 808, L-795,         */
/*   Livermore, CA 94550.                                                  */
/*                                                                         */
/***************************************************************************/
/***************************************************************************/

#include <stdio.h>
#include <sys/param.h>
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>

static int abort_requested;

static struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_pathLabel;
	Widget w_path;
} mp_monitor;

extern Widget w_toplev;
extern Display *display;

void cb_mp_monitor_close();
void cb_mp_monitor_abort();


/*
 * show_mp_monitor - Pop up multipurpose monitor.
 */
show_mp_monitor(path_label)
char *path_label;
{
	XmString string;

	create_mp_monitor_dialog();

	/* Update the path label */
	string = XmStringCreateSimple(path_label);
	XtVaSetValues(mp_monitor.w_pathLabel, XmNlabelString, string, NULL);
	XmStringFree(string);

	/* Clear the abort flag */
	abort_requested = False;

	/* Pop up dialog */
	XtManageChild(mp_monitor.w_dialog);
	add_dialog_to_list(mp_monitor.w_dialog);
	force_update(mp_monitor.w_dialog);
}


/*
 * create_mp_monitor_dialog - Create dialog window that displays the
 *                            directory entry currently being operated on.
 */
create_mp_monitor_dialog()
{
	static int initialized = False;
	Arg args[1];
	XmString label;
	int i;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog to display current entry being operated on */
    i = 0;
	XtSetArg(args[i], XmNresizePolicy, XmRESIZE_NONE); i++;
    mp_monitor.w_dialog = XmCreatePromptDialog(
        w_toplev,
        "mpMonitor",
        args,
        i
    );
	XtAddCallback(mp_monitor.w_dialog,XmNokCallback,cb_mp_monitor_abort,
		(XtPointer)NULL);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(mp_monitor.w_dialog, cb_mp_monitor_close, NULL);

    /* Don't need all the prompt dialog's buttons */
    XtUnmanageChild(XmSelectionBoxGetChild(mp_monitor.w_dialog,
		XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(mp_monitor.w_dialog,XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(mp_monitor.w_dialog,
		XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(mp_monitor.w_dialog,
		XmDIALOG_HELP_BUTTON));

	/* Create form for control area */
	mp_monitor.w_form = XtVaCreateWidget(
		"form",
		xmFormWidgetClass,
		mp_monitor.w_dialog,
		NULL
	);

	/* Create path label */
	label = XmStringCreateSimple("");
    mp_monitor.w_pathLabel = XtVaCreateManagedWidget(
        "pathLabel",
        xmLabelWidgetClass,
        mp_monitor.w_form,
        XmNlabelString,     	label,
        XmNtopAttachment,   	XmATTACH_FORM,
        XmNtopOffset,       	10,
        XmNleftAttachment,  	XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,		10,
        NULL
    );
    XmStringFree(label);

    /* Create label widget to display path */
    mp_monitor.w_path= XtVaCreateManagedWidget(
        "path",
        xmTextFieldWidgetClass,
        mp_monitor.w_form,
		XmNshadowThickness,			0,
		XmNmarginHeight,			0,
		XmNmarginWidth,				0,
		XmNtraversalOn,				False,
		XmNeditable,				False,
		XmNcursorPositionVisible,	False,
		XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
        XmNtopWidget,				mp_monitor.w_pathLabel,
		XmNtopOffset,				-1,
        XmNleftAttachment,			XmATTACH_WIDGET,
		XmNleftWidget,				mp_monitor.w_pathLabel,
        XmNleftOffset,      		5,
		XmNrightAttachment,			XmATTACH_FORM,
        NULL
    );

	XtManageChild(mp_monitor.w_form);
}


/*
 * update_mp_monitor - Update the multipurpose monitor to display the
 *                     directory entry currently being operated on.
 */
update_mp_monitor(path)
char *path;
{
	XmTextFieldSetString(mp_monitor.w_path, path);
	XSync(display, 0);
	XmUpdateDisplay(w_toplev);
}


/*
 * clear_mp_monitor - Clear the multipurpose monitor.
 */
clear_mp_monitor()
{
	XmTextFieldSetString(mp_monitor.w_path, "");
	XSync(display, 0);
	XmUpdateDisplay(w_toplev);
}


/*
 * hide_mp_monitor - Pop down the multipurpose monitor.
 */
hide_mp_monitor()
{
	XtUnmanageChild(mp_monitor.w_dialog);
}


/*
 * cb_mp_monitor_abort - Callback for multipurpose monitor abort button.
 */
void
cb_mp_monitor_abort(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    abort_requested = True;
	show_abort_dialog();
}


/*
 * cb_mp_monitor_close - Callback to handle closes from window manager.
 *                       A close is not allowed.
 */
void
cb_mp_monitor_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	warning_error("Can't close monitor while operation is in progress.");
}


/*
 * mp_abort_requested - Called to indicate whether the abort button in the
 *                      multipurpose monitor dialog has been pushed.
 */
mp_abort_requested()
{
	return abort_requested;
}

