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

#include <Xm/MessageB.h>

#define NONE    0
#define YES     1
#define NO      2

static int info_response;
static int question_response;
static Widget w_question;
static Widget w_info;

extern XtAppContext app;
extern Widget w_toplev;

void cb_question_yes();
void cb_question_no();
void cb_info_ok();
XmString text_to_cstring();


/*
 * question_dialog - Pops up dialog to ask user question.  Returns True
 *                   or False, depending on user's reponse.
 */
question_dialog(question)
char *question;
{
	XmString string;

    /* Need to build dialog? */
    create_question_dialog();

	/* Enter question */
	string = text_to_cstring(question);
	XtVaSetValues(w_question, XmNmessageString, string, NULL);
	XmStringFree(string);

    /* Pop up dialog and wait for user response */
	use_normal_cursor();
    question_response = NONE;
    XtManageChild(w_question);
	add_dialog_to_list(w_question);
    while (question_response == NONE)
        XtAppProcessEvent(app, (XtInputMask)XtIMAll);
	restore_prev_cursor();

	if (question_response == YES)
		return True;
	else
		return False;
}


/*
 * create_question_dialog - Create dialog window to ask user question.
 */
create_question_dialog()
{
	static int initialized = False;
    Arg args[1];
    int i;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create question dialog */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    w_question = XmCreateQuestionDialog(w_toplev, "question", args, i);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(w_question, cb_question_no, NULL);

    /* Won't need Help button */
    XtUnmanageChild(XmMessageBoxGetChild(w_question, XmDIALOG_HELP_BUTTON));

    /* Add callbacks */
    XtAddCallback(w_question, XmNokCallback, cb_question_yes,
		(XtPointer)NULL);
    XtAddCallback(w_question, XmNcancelCallback, cb_question_no,
		(XtPointer)NULL);
}


/*
 * cb_question_yes - Callback to handle "Yes" button in question dialog.
 */
void
cb_question_yes(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	question_response = YES;
}


/*
 * cb_question_no - Callback to handle "No" button in question dialog.
 */
void
cb_question_no(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	question_response = NO;
	XtUnmanageChild(w_question);
}


/*
 * info_dialog - Pop up a dialog that displays the string "information".
 */
info_dialog(information)
char *information;
{
	XmString string;

    create_info_dialog();

	/* Enter information */
	string = text_to_cstring(information);
	XtVaSetValues(w_info, XmNmessageString, string, NULL);
	XmStringFree(string);

	/* Pop up dialog */
	info_response = NONE;
	XtManageChild(w_info);
	add_dialog_to_list(w_info);
	
	/* Wait until user responds */
    while (info_response == NONE)
        XtAppProcessEvent(app, (XtInputMask)XtIMAll);
}


/*
 * create_info_dialog - Create the information dialog.
 */
create_info_dialog()
{
	static int initialized = False;
	Arg args[1];
    int i;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create warning dialog */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    w_info = XmCreateInformationDialog(w_toplev, "information", args, i);

    /* Won't need Help or Cancel buttons */
    XtUnmanageChild(XmMessageBoxGetChild(w_info, XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(w_info, XmDIALOG_CANCEL_BUTTON));

	/* Add callback */
    XtAddCallback(w_info, XmNokCallback, cb_info_ok, (XtPointer)NULL);
}


/*
 * cb_info_ok - Callback to handle "Ok" button in info dialog.
 */
void
cb_info_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	info_response = YES;
}

