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

#include <ctype.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>

struct help_st {
	Widget w_dialog;
	Widget w_pane;
	Widget w_controlArea;
	Widget w_topic;
	Widget w_text;
	Widget w_actionArea;
	Widget w_closeButton;
};

static struct help_st *current_help = NULL;
static struct help_st *old_help = NULL;

extern Widget w_toplev;
extern Display *display;

void cb_help_close();
void cb_help_destroy();
Widget vertical_sb();
struct help_st *create_help_dialog();


/*
 * cb_contextual_help - Callback to put main window into "contextual help"
 *                      mode in which the cursor changes to a hand until
 *                      the user clicks on a graphical element in the
 *                      main window he/she wishes to know more about.
 */
void
cb_contextual_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	Cursor cursor;
	Widget help_widget;

    /* Clear error flag */
    raise_okflag();

	/* Change cursor and wait for user to select widget */
	cursor = XCreateFontCursor(display, XC_hand2);
	if ((help_widget = XmTrackingLocate(w_toplev, cursor, True)))
		XtCallCallbacks(help_widget, XmNhelpCallback, (XtPointer)NULL);
	XFreeCursor(display, cursor);
}


/*
 * help_dialog - Pops up a dialog that display the help message specified by
 *               "text" in a scrolled text window.  "topic" identifies the
 *               help topic to the user.  "w_parent" is the widget to center
 *               the dialog over.  If "modal" is True, add the help dialog
 *               to the modal cascade.
 */
help_dialog(w_parent, modal, topic, text)
Widget w_parent;
int modal;
char *topic;
char **text;
{
	int indx = 0;
	char *ptr;
	char *buf;
	int len = 0;
	int i;
	XmString string;

	/* If help dialog already in use, destroy it */
	if (current_help) {
		old_help = current_help;
		XtDestroyWidget(XtParent(old_help->w_dialog));
	}

	/* Pop up help window */
	current_help = create_help_dialog(w_parent);
	add_dialog_to_list(current_help->w_dialog);
	XmProcessTraversal(current_help->w_closeButton, XmTRAVERSE_CURRENT);
	XmProcessTraversal(current_help->w_closeButton, XmTRAVERSE_CURRENT);

	/* Add dialog to modal cascade? */
	if (modal)
		XtAddGrab(XtParent(current_help->w_dialog), False, False);

	/* Insert help topic */
	string = XmStringCreateSimple(topic);
	XtVaSetValues(current_help->w_topic, XmNlabelString, string, NULL);
	XmStringFree(string);

	/* Insert help message */
	for (i=0; text[i]; i++)
		len += strlen(text[i])+1;
	len++;
	buf = XtMalloc(len+1);
	for (i=0; text[i]; i++) {
		ptr = text[i];
		while (*ptr)
			buf[indx++] = *ptr++;
		if (!isspace(buf[indx-1]))
			buf[indx++] = ' ';
	}
	buf[indx] = '\0';
	XtVaSetValues(current_help->w_text, XmNvalue, buf, NULL);
	XtFree(buf);
}


/*
 * create_help_dialog - Create dialog go display a scrollable help message.
 */
struct help_st *
create_help_dialog(w_parent)
Widget w_parent;
{
	Arg args[5];
	XmString string;
	int i;
	Dimension height;
	struct help_st *help;

	/* Allocate help structure */
	help = XtNew(struct help_st);

	/* Create form dialog*/
	help->w_dialog = XmCreateFormDialog(w_parent, "help", NULL, 0);
	XtAddCallback(help->w_dialog, XmNdestroyCallback, cb_help_destroy,
		(XtPointer)NULL);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(help->w_dialog, cb_help_close, NULL);

	/* Create paned window */
	help->w_pane = XtVaCreateWidget(
		"pane",
		xmPanedWindowWidgetClass,
		help->w_dialog,
		XmNsashWidth,			1,
		XmNsashHeight,			1,
		XmNtopAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);

	/* Create form widget for control area */
	help->w_controlArea = XtVaCreateWidget(
		"controlArea",
		xmFormWidgetClass,
		help->w_pane,
		XmNmarginWidth,		6,
		XmNmarginHeight,	5,
		NULL
	);

    /* Create label to display topic */
    string = XmStringCreateSimple(" ");
    help->w_topic= XtVaCreateManagedWidget(
        "topic",
        xmLabelWidgetClass,
        help->w_controlArea,
        XmNlabelString,         string,
        XmNtopAttachment,       XmATTACH_FORM,
        XmNtopOffset,           10,
        XmNleftAttachment,      XmATTACH_FORM,
		XmNleftOffset,			13,
        NULL
    );
    XmStringFree(string);

	/* Create scrolled text widget to hold message */
	i = 0;
	XtSetArg(args[i], XmNeditable, False); i++;
	XtSetArg(args[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(args[i], XmNwordWrap, True); i++;
	XtSetArg(args[i], XmNscrollHorizontal, False); i++;
	XtSetArg(args[i], XmNcursorPositionVisible, False); i++;
	help->w_text = XmCreateScrolledText(help->w_controlArea, "text", args, i);
	XtVaSetValues(
		XtParent(help->w_text),
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			help->w_topic,
		XmNtopOffset,			10,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);
	XtManageChild(help->w_text);
	fix_vertical_text_sb_color(help->w_text);
	set_bg_to_sb_trough_color(help->w_text,
		vertical_sb(XtParent(help->w_text)));

	XtManageChild(help->w_controlArea);

	/* Create form widget for action area */
	help->w_actionArea = XtVaCreateWidget(
		"actionArea",
		xmFormWidgetClass,
		help->w_pane,
		NULL
	);

	/* Create Close button */
	string = XmStringCreateSimple("Close");
	help->w_closeButton = XtVaCreateManagedWidget(
		"closeButton",
		xmPushButtonWidgetClass,
		help->w_actionArea,
		XmNlabelString,						string,
		XmNshowAsDefault,					True,
		XmNdefaultButtonShadowThickness,	1,
		XmNleftAttachment,					XmATTACH_POSITION,
		XmNleftPosition,					40,
		XmNrightAttachment,					XmATTACH_POSITION,
		XmNrightPosition,					60,
		NULL
	);
	XmStringFree(string);
	XtAddCallback(help->w_closeButton, XmNactivateCallback, cb_help_close,
		(XtPointer)NULL);

	XtManageChild(help->w_actionArea);

	/* Freeze height of action area */
	XtVaGetValues(help->w_closeButton, XmNheight, &height, NULL);
	XtVaSetValues(
		help->w_actionArea,
		XmNpaneMaximum,	height,
		XmNpaneMinimum,	height,
		NULL
	);

	XtManageChild(help->w_pane);
	XtManageChild(help->w_dialog);

	return help;
}


/*
 * cb_help_close - Callback to close "Help" dialog.
 */
void
cb_help_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	/* Get rid of help dialog */
	if (current_help)
		XtDestroyWidget(XtParent(current_help->w_dialog));
	else
		fatal_error("*** Bug in cb_help_close()");
}


/*
 * cb_help_destroy - Callback to destroy "Help" dialog.
 */
void
cb_help_destroy(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	/* Destroy help dialog */
	if (old_help) {
		remove_dialog_from_list(old_help->w_dialog);
		XtFree((char *)old_help);
		old_help = NULL;
	} else if (current_help) {
		remove_dialog_from_list(current_help->w_dialog);
		XtFree((char *)current_help);
		current_help = NULL;
	} else
		fatal_error("Bug in cb_help_destroy()");
}

