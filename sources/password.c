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
#include "xftp.h"
#include "str.h"

#define MAXPASSWD 100

#define NONE    0
#define YES     1
#define NO      2

static char passwd[MAXPASSWD+1];
static Widget w_passwordDialog;
static int passwd_response;
static char key1[MAXPASSWD];
static char *key2;

extern XtAppContext app;
extern Widget w_toplev;

void cb_password_ok();
void cb_password_cancel();
void cb_password();
void cb_map_password_dialog();


/*
 * encrypt_password - Returns a string containing an encrypted version of
 *                    the supplied password.  The caller is responsible
 *                    for releasing the memory occupied by the returned
 *                    string by calling XtFree().  NULL is returned if
 *                    password contains more than MAXPASSWD characters.
 *                    Call decrypt_password() to decrypt the password.
 */
char *
encrypt_password(password)
char *password;
{
	static int initialized = False;
	char padded_password[MAXPASSWD];
	int len = strlen(password);
	int i;
	char *encrypted_password;

	/* Make sure that password is not too long */
	if (len > MAXPASSWD)
		return NULL;

	/* Generate keys the first time thru */
	if (!initialized) {
		key2 = XtMalloc(MAXPASSWD);
		for (i=0; i<MAXPASSWD; i++) {
			key1[i] = rand()&0xff;
			key2[i] = rand()&0xff;
		}
		initialized = True;
	}

	/* Pad password with NULL characters */
	for (i=0; i<len; i++)
		padded_password[i] = password[i];
	for (; i<MAXPASSWD; i++)
		padded_password[i] = '\0';

	/* Encrypt password by XORing with key1 and key2 */
	encrypted_password = XtMalloc(MAXPASSWD+1);
	for (i=0; i<MAXPASSWD; i++)
		encrypted_password[i] = (padded_password[i]^key1[i])^key2[i];
	encrypted_password[MAXPASSWD] = '\0';
	bzero(padded_password, MAXPASSWD);

	return encrypted_password;
}


/*
 * decrypt_password - Returns a string containing the original version of
 *                    the supplied encrypted password.  The caller is
 *                    responsible for releasing the memory occupied by the
 *                    returned string by calling XtFree().  The supplied
 *                    encrypted password must have been returned by
 *                    encrypt_password().
 */
char *
decrypt_password(password)
char *password;
{
	char *decrypted_password;
	int i;

	/* Decrypt password by XORing with key2 and key1 */
	decrypted_password = XtMalloc(MAXPASSWD+1);
	for (i=0; i<MAXPASSWD; i++)
		decrypted_password[i] = (password[i]^key2[i])^key1[i];
	decrypted_password[MAXPASSWD] = '\0';

	return decrypted_password;
}

/*
 * create_password_dialog - Create dialog that prompts user for password.
 */
create_password_dialog()
{
    static int initialized = False;
    int i;
    Arg args[2];
    Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog to get password */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
    w_passwordDialog = XmCreatePromptDialog(w_toplev, "password", args, i);
    XtAddCallback(w_passwordDialog, XmNokCallback, cb_password_ok,
		(XtPointer)NULL);
    XtAddCallback(w_passwordDialog, XmNcancelCallback, cb_password_cancel,
		(XtPointer)NULL);
	XtAddCallback(w_passwordDialog, XmNmapCallback, cb_map_password_dialog,
		(XtPointer)NULL);

    /* Intercept characters written to dialog's textfield widget */
    widget = XmSelectionBoxGetChild(w_passwordDialog, XmDIALOG_TEXT);
    XtAddCallback(widget, XmNmodifyVerifyCallback, cb_password,
		(XtPointer)NULL);

    /* Don't display dialog's "Help" button */
    widget = XmSelectionBoxGetChild(w_passwordDialog, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(widget);

    /* Add callback for the WM_DELETE_WINDOW protocol */
    add_wm_delete_window_cb(w_passwordDialog, cb_password_cancel, NULL);
}


/*
 * cb_password_ok - Callback that notes that the user has finished typing
 *                  in his/her password.
 */
void
cb_password_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    passwd_response = YES;
}


/*
 * cb_password_cancel - Callback that notes that the user wishes to abort
 *                      connection process (while in password dialog).
 */
void
cb_password_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    passwd_response = NO;
    XtUnmanageChild(w_passwordDialog);
}


/*
 * cb_password - Callback that individually processes each password
 *               character as it is typed in.
 */
void
cb_password(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_data;
    int i;
    int passwd_len = strlen(passwd);

    /* Allow deletions and replacements only to end of password */
    if (cbs->endPos < passwd_len)
        cbs->endPos = passwd_len;
    passwd_len = cbs->startPos;

    /* Add character(s) to end of password */
    if (cbs->text->ptr)
        for (i=0; i<cbs->text->length; i++)
            if (passwd_len < MAXPASSWD) {
                passwd[passwd_len++] = cbs->text->ptr[i];
                passwd[passwd_len] = '\0';
                cbs->text->ptr[i] = '%';
            } else
                cbs->doit = False;
}




/*
 * get_password - Pops up a dialog to prompt user for a password.  If
 *                successful, a pointer to a string containing the 
 *                password is returned.  If user decides not to provide
 *                a password, a NULL is returned.  Call XtFree() to
 *                release returned memory.  "w_pseudo_parent" is the dialog
 *                to center the password dialog on.
 */
char *
get_password(w_pseudo_parent)
Widget w_pseudo_parent;
{
	Widget w_passwd;
	char *temp;

	/* Create the password dialog */
    create_password_dialog();

	/* Center password dialog over pseudo-parent dialog */
	XtVaSetValues(w_passwordDialog, XmNuserData, w_pseudo_parent, NULL);

	/* Initialize password to null string */
    w_passwd = XmSelectionBoxGetChild(w_passwordDialog, XmDIALOG_TEXT);
    XmTextSetString(w_passwd, "");
	passwd[0] = '\0';

	/* Get password from user */
    passwd_response = NONE;
    XtManageChild(w_passwordDialog);
    use_normal_cursor();
    add_dialog_to_list(w_passwordDialog);
    XmProcessTraversal(w_passwd, XmTRAVERSE_CURRENT);
    XmProcessTraversal(w_passwd, XmTRAVERSE_CURRENT);
    while (passwd_response == NONE)
        XtAppProcessEvent(app, (XtInputMask)XtIMAll);
    restore_prev_cursor();

	/* Did the user bail out? */
    if (passwd_response == NO)
        return NULL;

	/* Return password */
	temp = XtNewString(passwd);
	bzero(passwd, MAXPASSWD);
	return temp;
}


/*
 * cb_map_password_dialog - Callback to center password dialog over its
 *                          pseudo parent.
 */
void
cb_map_password_dialog(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	Widget w_pseudo_parent;

	XtVaGetValues(widget, XmNuserData, &w_pseudo_parent, NULL);
	center_dialog_over_dialog(widget, w_pseudo_parent);
}

