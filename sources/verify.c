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

#include <Xm/SelectioB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>

#define NONE    0
#define YES     1
#define NO      2

struct verify_struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_label;
	Widget w_list;
} verif;

static int response;

extern Widget w_toplev;
extern Widget w_dirList[];
extern XtAppContext app;

void cb_verify_ok();
void cb_verify_cancel();
void cb_verify_multiple_selection();


/*
 * verify_selection - Create a dialog the lists the specified host's
 *                    selection and asks the user the specified question
 *                    (usually whether an operation on the selected items
 *                    should proceed.  Returns True or False.
 */
verify_selection(host, question)
int host;
char *question;
{
	XmString label;
	XmStringTable selected_items;
	int nselected_items;

    create_verify_dialog();

	/* Insert the question */
	label = XmStringCreateSimple(question);
	XtVaSetValues(verif.w_label, XmNlabelString, label, NULL);
	XmStringFree(label);

	/* Insert the selected items */
    XtVaGetValues(
        w_dirList[host],
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );
    XtVaSetValues(
        verif.w_list,
        XmNitemCount, nselected_items,
        XmNitems,     selected_items,
        NULL
    );

    /* Pop up dialog and wait for user response */
	response = NONE;
    XtManageChild(verif.w_dialog);
	add_dialog_to_list(verif.w_dialog);
    while (response == NONE)
        XtAppProcessEvent(app, (XtInputMask)XtIMAll);

    if (response == YES)
        return True;
    else
        return False;
}


/*
 * create_verify_dialog - Create selection verification dialog.
 */
create_verify_dialog()
{
	static int initialized = False;
	Arg args[2];
	int i;
	Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

	/* Create prompt dialog */
	i = 0;
	XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	verif.w_dialog = XmCreatePromptDialog(w_toplev, "verify", args, i);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(verif.w_dialog, cb_verify_cancel, NULL);

    /* Don't show prompt dialog's selection label and text */
    widget = XmSelectionBoxGetChild(verif.w_dialog, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(widget);
    widget = XmSelectionBoxGetChild(verif.w_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(widget);

	/* Won't need Help button */
	XtUnmanageChild(XmSelectionBoxGetChild(verif.w_dialog,
		XmDIALOG_HELP_BUTTON));

    /* Create form for control area */
    verif.w_form = XtVaCreateWidget(
        "form",
        xmFormWidgetClass,
        verif.w_dialog,
        NULL
    );

	/* Create label for question */
	verif.w_label = XtVaCreateManagedWidget(
		"label",
		xmLabelWidgetClass,
		verif.w_form,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create scrolled list to contain selected items */
	i = 0;
	XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(args[i], XmNselectionPolicy, XmMULTIPLE_SELECT); i++;
	verif.w_list = XmCreateScrolledList(verif.w_form, "list", args, i);
	XtVaSetValues(XtParent(verif.w_list),
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			verif.w_label,
		XmNtopOffset,			10,
		NULL
	);
	XtManageChild(verif.w_list);
	fix_list_color(verif.w_list);

    /* Add multiple-selection callback to directory list*/
    XtAddCallback(
        verif.w_list,
        XmNmultipleSelectionCallback,
        cb_verify_multiple_selection,
        (XtPointer)NULL
    );

	/* Add callbacks for "OK" and "Cancel" buttons */
	XtAddCallback(verif.w_dialog, XmNokCallback, cb_verify_ok,
		(XtPointer)NULL);
	XtAddCallback(verif.w_dialog, XmNcancelCallback, cb_verify_cancel,
		(XtPointer)NULL);

	XtManageChild(verif.w_form);

}


/*
 * cb_verify_ok - Callback that notes that user likes the selection displayed
 *                in the selection verification dialog.
 */
void
cb_verify_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    response = YES;
}


/*
 * cb_verify_cancel - Callback that notes that user does not like the
 *                    selection displayed in the selection verification 
 *                    dialog.
 */
void
cb_verify_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    response = NO;
	XtUnmanageChild(verif.w_dialog);
}


/*
 * cb_verify_multiple_selection - Deselect the item just selected.
 */
void
cb_verify_multiple_selection(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;

	XmListDeselectPos(widget, cbs->item_position);
	beep();

}
