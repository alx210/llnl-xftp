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
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include "xftp.h"

static struct {
	Widget w_dialog;
	Widget w_controlArea;
	Widget w_statusLabel;
	Widget w_status;
	Widget w_nameLabel;
	Widget w_name;
	Widget w_lengthLabel;
	Widget w_length;
	Widget w_bytesMovedLabel;
	Widget w_bytesMoved;
	Widget w_scale;
} xfermon;

static int abort_requested;
static int ok_to_abort = True;

extern Widget w_toplev;
extern Display *display;

char *cstring_to_text();
void cb_xfermon_close();
void cb_xfermon_abort();


/*
 * show_xfermon - Pop up file transfer monitor.
 */
show_xfermon()
{
	/* Create file transfer monitor */
    create_xfermon_dialog();

	/* Initialize widgets */
	set_xfermon_status("");
	set_xfermon_name("");
	set_xfermon_progress((long)-1, (long)-1);

	/* Pop up dialog */
	XtManageChild(xfermon.w_dialog);
	add_dialog_to_list(xfermon.w_dialog);
	abort_requested = False;
	force_update(xfermon.w_dialog);
	XSync(display, 0);
}


/*
 * create_xfermon_dialog - Create file transfer monitor.
 */
create_xfermon_dialog()
{
	static int initialized = False;
	XmString label;
	Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog for connection to remote host */
    xfermon.w_dialog = XmCreatePromptDialog(w_toplev, "xfermon", NULL, 0);
    XtAddCallback(xfermon.w_dialog, XmNokCallback, cb_xfermon_abort,
		(XtPointer)NULL);

	/* Don't need all the prompt dialog's widgets */
	widget = XmSelectionBoxGetChild(xfermon.w_dialog,XmDIALOG_SELECTION_LABEL);
	XtUnmanageChild(widget);
	widget = XmSelectionBoxGetChild(xfermon.w_dialog, XmDIALOG_TEXT);
	XtUnmanageChild(widget);
	widget = XmSelectionBoxGetChild(xfermon.w_dialog, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(widget);
	widget = XmSelectionBoxGetChild(xfermon.w_dialog, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(widget);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(xfermon.w_dialog, cb_xfermon_close, NULL);

	/* Create form for control area */
    xfermon.w_controlArea = XtVaCreateWidget(
        "controlArea",
        xmFormWidgetClass,
        xfermon.w_dialog,
        NULL
    );

	/* Create status label */
	xfermon.w_statusLabel = XtVaCreateManagedWidget(
        "statusLabel",
        xmLabelWidgetClass,
        xfermon.w_controlArea,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset,       10,
		XmNleftAttachment, 	XmATTACH_FORM,
        NULL
    );

	/* Create status */
    xfermon.w_status = XtVaCreateManagedWidget(
        "status",
        xmTextFieldWidgetClass,
        xfermon.w_controlArea,
		XmNshadowThickness,			0,
		XmNmarginHeight,			0,
		XmNmarginWidth,				0,
		XmNtraversalOn,				False,
		XmNeditable,				False,
		XmNcursorPositionVisible,	False,
		XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,				xfermon.w_statusLabel,
		XmNtopOffset,				-1,
		XmNleftAttachment,			XmATTACH_FORM,
		XmNrightAttachment,			XmATTACH_FORM,
        NULL
    );

	/* Create file name label */
	xfermon.w_nameLabel = XtVaCreateManagedWidget(
        "nameLabel",
        xmLabelWidgetClass,
        xfermon.w_controlArea,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget,       xfermon.w_statusLabel,
		XmNleftAttachment, 	XmATTACH_FORM,
        NULL
    );

	/* Create file name */
    xfermon.w_name = XtVaCreateManagedWidget(
        "name",
        xmTextFieldWidgetClass,
        xfermon.w_controlArea,
		XmNshadowThickness,			0,
		XmNmarginHeight,			0,
		XmNmarginWidth,				0,
		XmNtraversalOn,				False,
		XmNeditable,				False,
		XmNcursorPositionVisible,	False,
		XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,				xfermon.w_nameLabel,
		XmNtopOffset,				-1,
		XmNleftAttachment,			XmATTACH_FORM,
		XmNrightAttachment,			XmATTACH_FORM,
        NULL
    );

	/* Create file length label */
	xfermon.w_lengthLabel = XtVaCreateManagedWidget(
        "lengthLabel",
        xmLabelWidgetClass,
        xfermon.w_controlArea,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget,		xfermon.w_nameLabel,
		XmNleftAttachment, 	XmATTACH_FORM,
        NULL
    );

	/* Create file length */
    xfermon.w_length = XtVaCreateManagedWidget(
        "length",
        xmTextFieldWidgetClass,
        xfermon.w_controlArea,
		XmNshadowThickness,			0,
		XmNmarginHeight,			0,
		XmNmarginWidth,				0,
		XmNtraversalOn,				False,
		XmNeditable,				False,
		XmNcursorPositionVisible,	False,
		XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,				xfermon.w_lengthLabel,
		XmNtopOffset,				-1,
		XmNleftAttachment,			XmATTACH_FORM,
		XmNrightAttachment,			XmATTACH_FORM,
        NULL
    );

	/* Create bytes moved label */
	xfermon.w_bytesMovedLabel = XtVaCreateManagedWidget(
        "bytesMovedLabel",
        xmLabelWidgetClass,
        xfermon.w_controlArea,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget,		xfermon.w_lengthLabel,
		XmNleftAttachment, 	XmATTACH_FORM,
        NULL
    );

	/* Create bytes moved */
    xfermon.w_bytesMoved = XtVaCreateManagedWidget(
        "bytesMoved",
        xmTextFieldWidgetClass,
        xfermon.w_controlArea,
		XmNshadowThickness,			0,
		XmNmarginHeight,			0,
		XmNmarginWidth,				0,
		XmNtraversalOn,				False,
		XmNeditable,				False,
		XmNcursorPositionVisible,	False,
		XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,				xfermon.w_bytesMovedLabel,
		XmNtopOffset,				-1,
		XmNleftAttachment,			XmATTACH_FORM,
		XmNrightAttachment,			XmATTACH_FORM,
        NULL
    );

	/* Create scale for displaying percentage of file moved */
	label = XmStringCreateSimple("Percent Transferred Unavailable");
	xfermon.w_scale = XtVaCreateManagedWidget(
		"scale",
		xmScaleWidgetClass,
		xfermon.w_controlArea,
		XmNtitleString,			label,
		XmNshowValue,			True,
		XmNorientation,			XmHORIZONTAL,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			xfermon.w_bytesMovedLabel,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNtopOffset,			15,
		NULL
	);
	XmStringFree(label);

	XtManageChild(xfermon.w_controlArea);
}


/*
 * hide_xfermon - Hide file transfer monitor dialog.
 */
hide_xfermon()
{
	XtUnmanageChild(xfermon.w_dialog);
}


/*
 * cb_xfermon_abort - Callback for file transfer abort button.
 */
void
cb_xfermon_abort(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	if (ok_to_abort) {
		abort_requested = True;
		show_abort_dialog();
	}
}


/*
 * cb_xfermon_close - Callback to handle closes from window manager.
 *                    A close is not allowed.
 */
void
cb_xfermon_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	warning_error("Can't close monitor while transfer is in progress.");
}


/*
 * xfer_abort_requested - Called to indicate whether the abort button
 *                        in the file transfer monitor has been pushed.
 */
xfer_abort_requested()
{
	return abort_requested;
}


/*
 * set_textfield - Set the textfield widget "widget" to the character
 *                 string "string".  If the textfield already has this
 *                 value, this routine is a no-op (to prevent the widget
 *                 from blinking).
 */
set_textfield(widget, string)
Widget widget;
char *string;
{
	char *old_string;

	old_string = XmTextFieldGetString(widget);
	if (strcmp(string, old_string))
		XmTextFieldSetString(widget, string);
	XtFree(old_string);
}


/*
 * set_xfermon_status - Set the status field of the file transfer monitor
 *                      to the character string "status".
 */
set_xfermon_status(status)
char *status;
{
	set_textfield(xfermon.w_status, status);
}


/*
 * set_xfermon_name - Set the name field of the file transfer monitor
 *                    to the character string "name".
 */
set_xfermon_name(name)
char *name;
{
	set_textfield(xfermon.w_name, name);
}


/*
 * set_xfermon_name_label - Set the label of the name field of the file
 *                          transfer monitor to the character string
 *                          label.
 */
set_xfermon_name_label(label)
char *label;
{
	XmString string;

	string = XmStringCreateSimple(label);
	XtVaSetValues(xfermon.w_nameLabel, XmNlabelString, string, NULL);
	XmStringFree(string);
}


/*
 * set_xfermon_progress - Update the file length, bytes moved, and percent
 *                         complete widgets in the file transfer monitor.
 *                         A value of -1 for "file_len" indicates that the
 *                         file length is unknown.  A value of -1 for
 *                         "file_index" indicates that the number of bytes
 *                         transferred is unknown.  Care is taken not to
 *                         update an unchanged widget value to prevent
 *                         unnecessary blinking.
 */
set_xfermon_progress(file_len, file_index)
long file_len;
long file_index;
{
	char buf[20];
	XmString prev_title;
	XmString title;
	int prev_percent;
	int percent;

	/* Display file length */
	if (file_len == -1)
		set_textfield(xfermon.w_length, "Unavailable");
	else {
		sprintf(buf, "%ld", file_len);
		set_textfield(xfermon.w_length, buf);
	}

	/* Display bytes transferred */
	if (file_index == -1)
		set_textfield(xfermon.w_bytesMoved, "Unavailable");
	else {
		sprintf(buf, "%ld", file_index);
		set_textfield(xfermon.w_bytesMoved, buf);
	}

	/* Display percent transferred */
	XtVaGetValues(xfermon.w_scale,
		XmNvalue,		&prev_percent,
		XmNtitleString,	&prev_title,
		NULL
	);
	if ((file_len == -1) || (file_index == -1)) {
		percent = 0;
		title = XmStringCreateSimple("Percent Transferred Unavailable");
	} else {
		if (file_len > 0) {
			percent = ((100.*(float)file_index)/(float)file_len);
			percent = MIN(100, percent);
		} else
			percent = 100;
		title = XmStringCreateSimple("Percent Transferred");
	}
	if (percent != prev_percent)
		XtVaSetValues(xfermon.w_scale, XmNvalue, percent, NULL);
	if (!XmStringCompare(prev_title, title)) {
		XtVaSetValues(xfermon.w_scale, XmNtitleString, title, NULL);
		XmUpdateDisplay(w_toplev);
	}
	XmStringFree(prev_title);  /* Yes, this is necessary */
	XmStringFree(title);
}


/*
 * enable_xfermon_abort - If "enable" is False, ignore pressing of "Abort"
 *                        button.  If "enable" is True, process presses of
 *                        "Abort" button.  This is a kludge, but is the
 *                        easiest way to prevent both the xfermon and
 *                        question dialogs to pop up simultaneously,
 *                        causing a deadlock.
 */
enable_xfermon_abort(enable)
int enable;
{
	ok_to_abort = enable;
}

