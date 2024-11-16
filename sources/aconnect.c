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
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

static struct {
    Widget w_dialog;
    Widget w_form;
    Widget w_hostListLabel;
    Widget w_hostList;
    Widget w_hostName;
    Widget w_hostNameLabel;
    Widget w_userName;
    Widget w_userNameLabel;
	Widget w_password;
	Widget w_passwordLabel;
} aconnect;

static char *aconnect_help[] = {
	"This dialog enables you to easily connect to an anonymous",
	"FTP site.  The \"Connect to Remote Host\" dialog (accessible",
	"via each host's \"Connect\" menu) can also",
	"be used for anonymous FTPing, but it is more awkward to use",
	"for this purpose.\n",
	"\n",
	"When this dialog pops up, the \"User Name\" field is",
	"initialized to \"anonymous\".  If the user name is changed,",
	"it should be set to \"ftp\" (though any name is legal).\n",
	"\n",
	"Also, when this dialog pops up, the \"Password\" field is",
	"initialized to the value of the user preference",
	"ANONYMOUS FTP PASSWORD (which you will probably want to set to your",
	"email address).  If a password is not given,",
	"you will be prompted for one (if the remote host",
	"requires one).\n",
	"\n",
	"An optional port number may follow the host name (using",
	"a space as the delimiter).\n",
	"\n",
    "Previously referenced host/user pairs are cached, and",
	"those with user names \"anonymous\" or \"ftp\" are",
    "displayed in the scrollable list labeled \"Recently",
    "Referenced Hosts\".  Double-click on an expression in the",
    "list to initiate a connection using that host name and user",
	"name.  Single-click on an entry in the list to place that",
    "information into the text fields labeled \"Host Name\" and",
    "\"User Name\", where it can be edited.\n",
    "\n",
    "The user preference SORT CACHES specifies whether the cached",
    "host/user pairs are displayed in alphabetical order or",
    "with the most-recently-referenced pairs at the top of",
    "the list.  The user preference USER CACHE SIZE",
    "specifies the maximum number of user names to cache per host.  ",
	"The user preference HOST CACHE SIZE specifies the maximum",
	"number of hosts to cache information for (including user names).  ",
    "The host/user cache is preserved across LLNL XFTP sessions.\n",
    "\n",
    "Click on the CONNECT button to initiate a connection and",
    "remove the dialog.  Click on the CANCEL button to remove",
    "the dialog without attemping to connect.",
    NULL
};

extern Widget w_toplev;
extern struct st_host_info hinfo[];
extern int sort_caches;
extern int add_to_cache_ok;
extern char *anonymous_ftp_password;

void cb_aconnect_single_selection();
void cb_aconnect_connect();
void cb_aconnect_help();
void cb_aconnect_def_action();
Widget vertical_sb();
char *encrypt_password();
char *cstring_to_text();


/*
 * cb_connect_to_anonymous - Callback that pops up the Anonymous Remote
 *                           Connection dialog.
 */
void
cb_connect_to_anonymous(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

	/* First disconnect if already logged in */
	if (hinfo[host].type != NEITHER)
		disconnect(host);

	create_aconnect_dialog();

	update_aconnect_dialog(host);

    /* Pop up dialog */
    XtManageChild(aconnect.w_dialog);
	add_dialog_to_list(aconnect.w_dialog);
	XmProcessTraversal(aconnect.w_hostName, XmTRAVERSE_CURRENT);
	XmProcessTraversal(aconnect.w_hostName, XmTRAVERSE_CURRENT);
}


/*
 * create_aconnect_dialog - Creates the Remote Connection dialog.
 */
create_aconnect_dialog()
{
	static int initialized = False;
	Arg	args[2];
	int i;
	Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

	/* Create prompt dialog for connection to remote host */
	i = 0;
	XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	aconnect.w_dialog = XmCreatePromptDialog(w_toplev, "aConnect", args, i);
    XtAddCallback(aconnect.w_dialog, XmNokCallback, cb_aconnect_connect,
		(XtPointer)NULL);
	XtAddCallback(aconnect.w_dialog, XmNhelpCallback, cb_aconnect_help,
		(XtPointer)NULL);

	/* Don't show prompt dialog's selection label and text */
	widget = XmSelectionBoxGetChild(aconnect.w_dialog,XmDIALOG_SELECTION_LABEL);
	XtUnmanageChild(widget);
	widget = XmSelectionBoxGetChild(aconnect.w_dialog, XmDIALOG_TEXT);
	XtUnmanageChild(widget);

	/* Create form for control area */
	aconnect.w_form = XtVaCreateManagedWidget(
		"form",
		xmFormWidgetClass,
		aconnect.w_dialog,
		XmNwidth,	250,
		NULL
	);

	/* Create label for host list */
	aconnect.w_hostListLabel = XtVaCreateManagedWidget(
		"hostListLabel",
		xmLabelWidgetClass,
		aconnect.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create scrolled list to contain host names */
	i = 0;
	XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(args[i], XmNselectionPolicy, XmSINGLE_SELECT); i++;
	aconnect.w_hostList = XmCreateScrolledList(aconnect.w_form,
		"hostList", args, i);
	XtVaSetValues(XtParent(aconnect.w_hostList),
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			aconnect.w_hostListLabel,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		NULL
	);
	XtManageChild(aconnect.w_hostList);
	fix_list_color(aconnect.w_hostList);

    /* Add single-selection callback */
    XtAddCallback(
        aconnect.w_hostList,
        XmNsingleSelectionCallback,
        cb_aconnect_single_selection,
        (XtPointer)NULL
    );

    /* Add double-click callback to directory list*/
    XtAddCallback(
        aconnect.w_hostList,
        XmNdefaultActionCallback,
        cb_aconnect_def_action,
        (XtPointer)NULL
    );

	/* Create label for host name */
	aconnect.w_hostNameLabel = XtVaCreateManagedWidget(
		"hostNameLabel",
		xmLabelWidgetClass,
		aconnect.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		aconnect.w_hostList,
		XmNtopOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create text field for host name */
	aconnect.w_hostName = XtVaCreateManagedWidget(
		"hostName",
		xmTextFieldWidgetClass,
		aconnect.w_form,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		aconnect.w_hostNameLabel,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create label for user name */
	aconnect.w_userNameLabel = XtVaCreateManagedWidget(
		"userNameLabel",
		xmLabelWidgetClass,
		aconnect.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		aconnect.w_hostName,
		XmNtopOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create text field for user name */
	aconnect.w_userName = XtVaCreateManagedWidget(
		"userName",
		xmTextFieldWidgetClass,
		aconnect.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			aconnect.w_userNameLabel,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		NULL
	);

	/* Create label for password */
	aconnect.w_passwordLabel = XtVaCreateManagedWidget(
		"passwordLabel",
		xmLabelWidgetClass,
		aconnect.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		aconnect.w_userName,
		XmNtopOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create text field for password */
	aconnect.w_password = XtVaCreateManagedWidget(
		"password",
		xmTextFieldWidgetClass,
		aconnect.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			aconnect.w_passwordLabel,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Make background color of textfield widgets same as scrollbar trough */
	set_bg_to_sb_trough_color(aconnect.w_hostName,
		vertical_sb(XtParent(aconnect.w_hostList)));
	set_bg_to_sb_trough_color(aconnect.w_userName,
		vertical_sb(XtParent(aconnect.w_hostList)));
	set_bg_to_sb_trough_color(aconnect.w_password,
		vertical_sb(XtParent(aconnect.w_hostList)));
}


/*
 * update_aconnect_dialog - Display the last referenced hostname/username
 *                          pairs in the scrolled list of the anonymous
 *                          connection dialog.
 */
update_aconnect_dialog(host)
int host;
{
    struct sl_struct *list;
    XmString string;
    int i;
	char *host_part;
	char *user_part;

    /* Get cached host/user names*/
    retrieve_hostuser_cache(&list, sort_caches);

    /* Enter host/user names into scrolled list */
    reset_list(aconnect.w_hostList);
    for (i=0; i<list->nentries; i++)
		if (parse_hostuser(list->entries[i], &host_part, &user_part) == 0) {
			if (!strcmp(user_part, "anonymous")
					|| !strncmp(user_part, "anonymous@", 10)
					|| !strcmp(user_part, "ftp")
					|| !strncmp(user_part, "ftp@", 4)) {
       	 		string = XmStringCreateSimple(list->entries[i]);
       	 		XmListAddItem(aconnect.w_hostList, string, 0);
       	 		XmStringFree(string);
			}
			XtFree(host_part);
			XtFree(user_part);
		}
    release_array_list(list);

    /* Clear hostname field */
    XmTextFieldSetString(aconnect.w_hostName, "");

	/* Initialize user field to "anonymous" */
	XmTextFieldSetString(aconnect.w_userName, "anonymous");

	/* Set password field from preferences */
	XmTextFieldSetString(aconnect.w_password, anonymous_ftp_password);

    /* Remember host */
    XtVaSetValues(aconnect.w_dialog, XmNuserData, host, NULL);
}


/*
 * cb_aconnect_single_selection - This callback is invoked when a
 *                                host/user pair is selected.
 */
void
cb_aconnect_single_selection(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    XmStringTable selected_items;
    int nselected_items;
    char *text;
	char *host_part;
	char *user_part;

	/* Don't let user deselect item */
    XtVaGetValues(
        aconnect.w_hostList,
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );
    if (nselected_items == 0) {
        XmListSelectPos(aconnect.w_hostList, cbs->item_position, False);
        XtVaGetValues(
            aconnect.w_hostList,
            XmNselectedItems,     &selected_items,
            NULL
        );
    }

    /* Use selected item to update host and user fields */
	text = cstring_to_text(selected_items[0]);
	if (parse_hostuser(text, &host_part, &user_part) < 0)
		fatal_error("Trouble in cb_aconnect_single_selection()");
    XmTextFieldSetString(aconnect.w_hostName, host_part);
	XmTextFieldSetString(aconnect.w_userName, user_part);
    XtFree(text);
	XtFree(host_part);
	XtFree(user_part);
}


/*
 * cb_aconnect_help - Callback to display help information for the remote
 *                    connection dialog.
 */
void
cb_aconnect_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    help_dialog(aconnect.w_dialog, True, "Connect to Anonymous", aconnect_help);
}


/*
 * cb_aconnect_def_action - Callback to filter out carriage returns pressed
 *                          in the remote connection dialog's scrolling
 *                          list widget.  This is necessary to avoid invoking
 *                          cb_aconnect_connect() twice (because of the way 
 *                          the Prompt Dialog widget works).
 */
void
cb_aconnect_def_action(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;

	/* Reality check */
	if (cbs->reason != XmCR_DEFAULT_ACTION)
		fatal_error("Programming bug in cb_aconnect_def_action()");

	/* Filter out carriage returns */
	if (cbs->event->type != KeyPress)
		cb_aconnect_connect(widget, client_data, call_data);
}


/*
 * cb_aconnect_connect - Callback that uses information in remote connection
 *                       dialog to attempt a control connection to a remote
 *                       FTP server.
 */
void
cb_aconnect_connect(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    char *username;
    char *hostname;
	char *password;
	char *passwrd;
    long host;
    int retval;
    int i;
    int len;
    char *host_part;
    int port_part;
	char *q;

    /* Hide "Remote Connect" dialog */
    XtUnmanageChild(aconnect.w_dialog);

    /* Get host identifier */
    XtVaGetValues(aconnect.w_dialog, XmNuserData, &host, NULL);

    /* Get user name and make sure it's non-null */
    username = XmTextFieldGetString(aconnect.w_userName);
    len = strlen(username);
    for (i=0; i<len; i++)
        if (username[i] != ' ')
            break;
    if (i == len) {
        XtFree(username);
        warning_error("Unable to log onto FTP server.  No user name.");
        return;
    }

    /* Get host name / port address and check format */
    hostname = XmTextFieldGetString(aconnect.w_hostName);
    switch (parse_hostname(hostname, &host_part, &port_part)) {
    case -1:
        XtFree(hostname);
        XtFree(username);
        warning_error("Unable to log onto FTP server.  No host name.");
        return;
    case -2:
        XtFree(hostname);
        XtFree(username);
        warning_error("Unable to log onto FTP server.  Bad port format.");
        return;
    case 0:
        XtFree(host_part);
    }

    /* Add user name to cache */
    if (add_to_cache_ok)
        add_to_cache(USER, hostname, username);

	/* Get password */
	password = XmTextFieldGetString(aconnect.w_password);
	if ((q = strtok(password, " ")) == NULL)
		retval = do_connect(host, hostname, username, NULL, NULL,
			aconnect.w_dialog);
	else {
		passwrd = encrypt_password(password);
		retval = do_connect(host, hostname, username, passwrd, NULL,
			aconnect.w_dialog);
		XtFree(passwrd);
	}

    /* Free up memory */
    XtFree(hostname);
    XtFree(username);
	XtFree(password);

    /* Well, are we connected? */
    if (retval < 0)
        return;

    /* This might take some time */
    use_busy_cursor();

    /* Successfully logged in.  Update the display */
    if (update_dir_displays(host, False) < 0) {
        restore_prev_cursor();
        lost_connection(host);
        return;
    }
    update_xfer_controls();
    update_dirname_menu(host);
    update_dirpath(host);
    update_dir_cache(host);
    update_hostname(host);
    update_host_controls(host);
    restore_prev_cursor();
}

