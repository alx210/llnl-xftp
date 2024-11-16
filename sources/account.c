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
#include <Xm/MessageB.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>

#define NONE    0
#define YES     1
#define NO      2

static Widget w_accountDialog;
static int account_response;

extern XtAppContext app;
extern Widget w_toplev;

void cb_account_ok();
void cb_account_cancel();
void cb_map_account_dialog();


/*
 * create_account_dialog - Creates the "Get Account" dialog.
 */
create_account_dialog()
{
    static int initialized = False;
    int i;
    Arg args[2];
    Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog to get account */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
    w_accountDialog = XmCreatePromptDialog(w_toplev, "account", args, i);
    XtAddCallback(w_accountDialog, XmNokCallback, cb_account_ok,
		(XtPointer)NULL);
    XtAddCallback(w_accountDialog, XmNcancelCallback, cb_account_cancel,
		(XtPointer)NULL);
	XtAddCallback(w_accountDialog, XmNmapCallback, cb_map_account_dialog,
		(XtPointer)NULL);

    /* Don't display dialog's "Help" button */
    widget = XmSelectionBoxGetChild(w_accountDialog, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(widget);

    /* Add callback for the WM_DELETE_WINDOW protocol */
    add_wm_delete_window_cb(w_accountDialog, cb_account_cancel, NULL);
}


/*
 * cb_account_ok - Callback that notes that the user has finished typing
 *                 in his/her account.
 */
void
cb_account_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    account_response = YES;
}


/*
 * cb_account_cancel - Callback that notes that the user wishes to abort
 *                     connection process (while in account dialog).
 */
void
cb_account_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    account_response = NO;
    XtUnmanageChild(w_accountDialog);
}


/*
 * get_account - Pops up a dialog to prompt user for an account.  If
 *               successful, a pointer to a string containing the
 *               account is returned.  If user decides not to provide
 *               an account, a NULL is returned.  Call XtFree() to
 *               release returned memory.  "w_pseudo_parent" is the dialog
 *               to center the account dialog on.
 */
char *
get_account(w_pseudo_parent)
Widget w_pseudo_parent;
{
	Widget w_account;

	/* Create the account dialog */
    create_account_dialog();

    /* Center account dialog over pseudo-parent dialog */
    XtVaSetValues(w_accountDialog, XmNuserData, w_pseudo_parent, NULL);

	/* Initialize the account to null string */
    w_account = XmSelectionBoxGetChild(w_accountDialog, XmDIALOG_TEXT);
    XmTextSetString(w_account, "");

	/* Get account from user */
    account_response = NONE;
    XtManageChild(w_accountDialog);
    add_dialog_to_list(w_accountDialog);
    use_normal_cursor();
    XmProcessTraversal(w_account, XmTRAVERSE_CURRENT);
    XmProcessTraversal(w_account, XmTRAVERSE_CURRENT);
    while (account_response == NONE)
        XtAppProcessEvent(app, (XtInputMask)XtIMAll);
    restore_prev_cursor();

	/* Did the user bail out? */
    if (account_response == NO)
		return NULL;

	/* Return account */
    return XmTextGetString(w_account);
}


/*
 * cb_map_account_dialog - Callback to center account dialog over its
 *                         pseudo parent.
 */
void
cb_map_account_dialog(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    Widget w_pseudo_parent;

    XtVaGetValues(widget, XmNuserData, &w_pseudo_parent, NULL);
    center_dialog_over_dialog(widget, w_pseudo_parent);
}


