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

#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/List.h>
#include "xftp.h"
#include "list.h"

#define NONE    0
#define OK      1
#define CANCEL  2

static struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_instruction;
	Widget w_dirNameForm;
	Widget w_dirNameMenu;
	Widget w_dirNameMenuItem[MAXLINKS];
	Widget w_dirName;
	Widget w_dirList;
	char *wd;
	int host;
	int type;
	int system;
	int server;
	int response;
	Widget pseudo_parent;
} destdir;


extern Widget w_toplev;
extern struct st_host_info hinfo[];
extern int symbol_mode;
extern XtAppContext app;

char **path_to_links();
char *links_to_path();
void cb_destdir_ok();
void cb_destdir_cancel();
void cb_destdir_set_dir();
void cb_destdir_double_click();
void cb_map_destdir_dialog();
char *merge_paths();
char *cstring_to_text();


/*
 * create_destdir_dialog - Create dialog that prompts for destination
 *                         directory.
 */
create_destdir_dialog()
{
	static int initialized = False;
    XmString label;
    int i;
    Arg args[2];
	Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog to get destination directory */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(args[i], XmNdefaultPosition, False); i++;
    destdir.w_dialog = XmCreatePromptDialog(w_toplev, "destdir", args, i);
    XtAddCallback(destdir.w_dialog, XmNokCallback, cb_destdir_ok,
		(XtPointer)NULL);
	XtAddCallback(destdir.w_dialog, XmNcancelCallback, cb_destdir_cancel,
		(XtPointer)NULL);
	XtAddCallback(destdir.w_dialog, XmNmapCallback, cb_map_destdir_dialog,
		(XtPointer)NULL);

    /* Don't show prompt dialog's selection label, text and Help button */
    widget = XmSelectionBoxGetChild(destdir.w_dialog, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(widget);
    widget = XmSelectionBoxGetChild(destdir.w_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(widget);
	widget = XmSelectionBoxGetChild(destdir.w_dialog, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(widget);

    /* Create form for control area */
    destdir.w_form = XtVaCreateWidget(
        "form",
        xmFormWidgetClass,
        destdir.w_dialog,
        NULL
    );

	/* Create label that gives instruction */
	destdir.w_instruction = XtVaCreateManagedWidget(
		"instruction",
		xmLabelWidgetClass,
		destdir.w_form,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNtopOffset,		10,
		NULL
	);

    /* Create form for directory name option menu (this kluge forces resize) */
    destdir.w_dirNameForm = XtVaCreateManagedWidget(
        "dirNameForm",
        xmFormWidgetClass,
        destdir.w_form,
        XmNtopAttachment,   XmATTACH_WIDGET,
        XmNtopWidget,       destdir.w_instruction,
        XmNleftAttachment,  XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNtopOffset,       14,
        NULL
    );

    /* Create the directory name pulldown menu */
    destdir.w_dirNameMenu = XmCreatePulldownMenu(destdir.w_dirNameForm,
        "dirNameMenu", NULL, 0);
    label = XmStringCreateSimple("Dummy");
    for (i=0; i<MAXLINKS; i++) {
        destdir.w_dirNameMenuItem[i] = XtVaCreateWidget(
            "",
            xmPushButtonGadgetClass,
            destdir.w_dirNameMenu,
            XmNlabelString, label,
            NULL
        );
        XtAddCallback(destdir.w_dirNameMenuItem[i], XmNactivateCallback,
            cb_destdir_set_dir, (XtPointer)NULL);
    }
    XmStringFree(label);

    /* Create directory name option menu */
    i = 0;
    XtSetArg(args[i], XmNsubMenuId, destdir.w_dirNameMenu); i++;
    destdir.w_dirName = XmCreateOptionMenu(destdir.w_dirNameForm, "dirName", args, i);
    XtManageChild(destdir.w_dirName);

    /* Create directory list  */
    i = 0;
    XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
    destdir.w_dirList = XmCreateScrolledList(destdir.w_form, "dirList", args,
		i);
    XtVaSetValues(XtParent(destdir.w_dirList),
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNleftOffset,          0,
        XmNrightOffset,         0,
        XmNtopAttachment,       XmATTACH_WIDGET,
        XmNtopWidget,           destdir.w_dirNameForm,
        NULL
    );
    XtManageChild(destdir.w_dirList);
    fix_list_color(destdir.w_dirList);

    /* Add double-click callback to directory list */
    XtAddCallback(
        destdir.w_dirList,
        XmNdefaultActionCallback,
        cb_destdir_double_click,
        (XtPointer)NULL
    );

    XtManageChild(destdir.w_dirList);
	XtManageChild(destdir.w_form);
}


/*
 * cb_destdir_set_dir - Callback to change the destination directory in
 *                      response to the user selecting a directory link.
 */
void
cb_destdir_set_dir(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    long indx;
    char **wd_links;
    char *new_wd;
	int retval;

    /* Clear error flag */
    raise_okflag();

    /* Get position of selected link in path */
    XtVaGetValues(widget, XmNuserData, &indx, NULL);

    /* Parse working directory path */
    wd_links = path_to_links(destdir.system, destdir.wd);

	/* Build path name for new working directory */
	new_wd = links_to_path(destdir.system, wd_links, indx);

    /* Free up memory returned by path_to_links() */
    release_path_links(wd_links);

    /* Try to cd to specified directory */
    use_busy_cursor();
	retval = change_destdir_directory(new_wd);
	restore_prev_cursor();
	XtFree(new_wd);
	switch (retval) {
	case -1:
        warning_error("Unable to change directory");
		break;
	case -3:
		XtUnmanageChild(destdir.w_dialog);
        lost_connection(destdir.host);
	}
}


/*
 * cb_destdir_double_click - Callback to change the destination directory
 *                           in response to the user double- clicking on a
 *                           directory entry.
 */
void
cb_destdir_double_click(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    char *entry;
    char *new_wd;
	int retval;

    /* Clear error flag */
    raise_okflag();

    /* Get entry that was double-clicked */
	entry = cstring_to_text(cbs->item);
	strip_off_symbol(destdir.system, destdir.server, entry);

    /* Form full path name */
	new_wd = merge_paths(destdir.system, destdir.wd, entry);
	XtFree(entry);

    /* Try to cd to specified directory */
    use_busy_cursor();
	retval = change_destdir_directory(new_wd);
	restore_prev_cursor();
	XtFree(new_wd);
	switch (retval) {
	case -1:
        warning_error("Unable to change directory");
		break;
	case -3:
		XtUnmanageChild(destdir.w_dialog);
        lost_connection(destdir.host);
	}
}


/*
 * update_destdir_dirname_menu - Update the destination directory name menu.
 */
update_destdir_dirname_menu()
{
    XmString label;
    char **wd_links;
    int nlinks = 0;
    int i;
	int len;

    /* Parse working directory path. */
    wd_links = path_to_links(destdir.system, destdir.wd);

    /* Count number of links in path name */
    while (wd_links[nlinks])
        nlinks++;

    /* Truncate ".dir" from VMS links */
    if (destdir.system == SYS_VMS) {
        for (i=0; i<nlinks; i++) {
            len = strlen(wd_links[i]);
            if ((len > 4) && !strcmp(&wd_links[i][len-4], ".dir"))
                wd_links[i][len-4] = '\0';
        }
    }

    /* Add the path links to menu in reverse order */
    for (i=0; i<nlinks; i++) {
        label = XmStringCreateSimple(wd_links[nlinks-1-i]);
        XtVaSetValues(destdir.w_dirNameMenuItem[i],
            XmNlabelString, label,
            XmNuserData,    (long)nlinks-i,
            NULL
        );
        XmStringFree(label);
        XtManageChild(destdir.w_dirNameMenuItem[i]);
    }

    /* Unmanage unused menu items */
    for (i=nlinks; i<MAXLINKS; i++)
        XtUnmanageChild(destdir.w_dirNameMenuItem[i]);

    /* Free up memory returned by path_to_links() */
    release_path_links(wd_links);

    /* Make first menu item current */
    XtVaSetValues(
        destdir.w_dirName,
        XmNmenuHistory, destdir.w_dirNameMenuItem[0],
        NULL
    );
}


/*
 * update_destdir_dirlist - Update the list widget displaying the contents
 *                          of the destination directory.  Returns 0 for
 *                          success, -3 for broken connection and -1 for
 *                          other errors.
 */
update_destdir_dirlist()
{
    struct sl_struct *dlist;
    int retval;
    int i;
    XmString string;

    /* This might take some time */
    use_busy_cursor();

    /* Get short directory list */
    switch (destdir.type) {
    case NEITHER:
		fatal_error("Bug in update_destdir_dirlist()");
    case LOCAL:
        if (local_ls(destdir.wd, &dlist, symbol_mode) != 0)
            fatal_error("Unable to get local directory list");
        break;
    case REMOTE:
        if ((retval = remote_ls(destdir.host, destdir.wd, &dlist,
				symbol_mode))) {
            restore_prev_cursor();
            return retval;
        }
    }

    /* Delete scrollinglist items */
    reset_list(destdir.w_dirList);

    /* Place directory entries into short list */
    for (i=0; i<dlist->nentries; i++) {
        string = XmStringCreateSimple(dlist->entries[i]);
        XmListAddItem(destdir.w_dirList, string, 0);
        XmStringFree(string);
    }

    /* Free directory list */
    release_array_list(dlist);

    restore_prev_cursor();
    return 0;
}


/*
 * change_destdir_directory - Change the destination directory to "new_wd".
 *                            Returns 0 if successful, -3 for connection,
 *                            and -1 for other errors.
 */
change_destdir_directory(new_wd)
char *new_wd;
{
    char *wd;
	int retval;

    /* cd into new directory */
    if (destdir.type == LOCAL) {
        if (local_cd(destdir.host, new_wd, True) < 0)
			return -1;
		if (destdir.wd)
        	XtFree(destdir.wd);
        if (local_pwd(&destdir.wd) != 0)
            fatal_error("Unable to get local working directory");
    } else {
		if ((retval = remote_cd(destdir.host, new_wd, False, True)) < 0)
			return retval;
        if (remote_pwd(destdir.host, &wd) < 0)
            return -3;
		if (destdir.wd)
        	XtFree(destdir.wd);
        destdir.wd = wd;
    }

    /* Update the display */
    if (update_destdir_dirlist() < 0)
        return -3;
	update_destdir_dirname_menu();

	return 0;
}


/*
 * get_destination_dir - Pops up dialog asking user to select a destination
 *                       directory.  "type" is LOCAL or REMOTE.  Set "host"
 *                       to LEFT, RIGHT, or LOCAL_HOST.  If possible,
 *                       "desired_start_dir" will be the first directory
 *                       presented to the user; otherwise "fallback_start_dir"
 *                       will be used.  If user press OK button, the
 *                       destination directory is returned.  Use XtFree()
 *                       to free returned string.  If user presses CANCEL
 *                       button or if connection is lost, NULL is returned.
 *                       "pseudo_parent" is the widget to center the
 *                       "Destination Directory" dialog over.
 */
char *
get_destination_dir(host, type, desired_start_dir, fallback_start_dir,
					pseudo_parent)
int host;
int type;
char *desired_start_dir;
char *fallback_start_dir;
Widget pseudo_parent;
{
	int initialized_destdir = False;
	int retval;
	char *temp;

	/* Differentiate between LOCAL and REMOTE hosts */
	destdir.type = type;
	destdir.host = host;
	destdir.pseudo_parent = pseudo_parent;
	destdir.wd = NULL;
	if (destdir.type == LOCAL) {
		destdir.system = SYS_UNIX;
		destdir.server = SERVER_UNKNOWN;
	} else {
		destdir.system = hinfo[host].system;
		destdir.server = hinfo[host].server;
	}

	/* Create "Get Destination Directory" dialog */
	create_destdir_dialog();

    /* Try to initialize destination directory to desired one */
    if (desired_start_dir) {
        use_busy_cursor();
        retval = change_destdir_directory(desired_start_dir);
        restore_prev_cursor();
        switch (retval) {
        case 0:
            XtManageChild(destdir.w_dialog);
            add_dialog_to_list(destdir.w_dialog);
			initialized_destdir = True;
			break;
        case -3:
            lost_connection(destdir.host);
			if (destdir.wd)
				XtFree(destdir.wd);
            return NULL;
		}
    }

    /* Try to initialize destination directory to fallback value */
	if (!initialized_destdir && fallback_start_dir) {
	    use_busy_cursor();
	   	retval = change_destdir_directory(fallback_start_dir);
	    restore_prev_cursor();
	    if (retval < 0) {
	        lost_connection(destdir.host);
			if (destdir.wd)
				XtFree(destdir.wd);
	        return NULL;
	    }
	    XtManageChild(destdir.w_dialog);
	    add_dialog_to_list(destdir.w_dialog);
		initialized_destdir = True;
	}

	/* Sanity check */
	if (!initialized_destdir)
		fatal_error("Trouble in get_destination_dir()");

	/* Pop up dialog and wait for user response */
	destdir.response = NONE;
    XtManageChild(destdir.w_dialog);
    add_dialog_to_list(destdir.w_dialog);
	while (destdir.response == NONE)
		XtAppProcessEvent(app, (XtInputMask)XtIMAll);

	if (destdir.response == OK)
		temp = XtNewString(destdir.wd);
	else
		temp = NULL;
	XtFree(destdir.wd);
	return temp;
}


/*
 * cb_destdir_ok - Callback to handle "Ok" button in destdir dialog.
 */
void
cb_destdir_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    destdir.response = OK;
}


/*
 * cb_destdir_cancel - Callback to handle "Cancel" button in destdir dialog.
 */
void
cb_destdir_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    destdir.response = CANCEL;
}


/*
 * cb_map_destdir_dialog - Callback to center destination directory dialog
 *                         over its pseudo parent.
 */
void
cb_map_destdir_dialog(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    center_dialog_over_dialog(widget, destdir.pseudo_parent);
}

