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
#include <Xm/Text.h>
#include "xftp.h"
#include "list.h"

static Widget w_changeDirDialog;

static char *help_change_dir[] = {
    "This dialog enables you to change the associated host's",
    "current directory to the path specified in the \"Directory\"",
    "field.\n",
    "\n",
    "Previously referenced directories are cached and displayed",
    "in the scrollable list labeled \"Recently Referenced",
    "Directories\".  Double-click on a path in the list to",
    "change to that directory.  Single-click on a path in the",
    "list to place that entry into the text field labeled",
    "\"Directory\", where it can be edited.\n",
    "\n",
    "The host's directory cache is modified every time the",
    "host's current directory changes, regardless of the",
    "mechanism used to change directories.  The user",
    "preference SORT CACHES specifies whether the cached",
    "directory paths are displayed in alphabetical order or",
    "with the most-recently-referenced paths at the top of",
    "the list.  The user preference DIRECTORY CACHE SIZE",
    "specifies the maximum number of directory paths to cache",
    "per host.  The directory cache is preserved across LLNL XFTP",
    "sessions.\n",
    "\n",
    "Click on the OK button to perform the operation and",
    "remove the dialog.  Click on the CANCEL button to remove",
    "the dialog without performing the operation.",
    NULL
};

extern Widget w_toplev;
extern struct st_host_info hinfo[];
extern int sort_caches;

void cb_changedir_ok();
void cb_changedir_help();
Widget vertical_sb();


/*
 * cb_change_dir - Display the "Change Directory" dialog.
 */
void
cb_change_dir(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	Widget w_dir;

    /* Clear error flag */
    raise_okflag();

    create_changedir_dialog();

    update_changedir_dialog(host);

    XtManageChild(w_changeDirDialog);
	add_dialog_to_list(w_changeDirDialog);
	w_dir = XmSelectionBoxGetChild(w_changeDirDialog, XmDIALOG_TEXT);
	XmProcessTraversal(w_dir, XmTRAVERSE_CURRENT);
	XmProcessTraversal(w_dir, XmTRAVERSE_CURRENT);
}


/*
 * create_changedir_dialog - Create "Change Directory" dialog.
 */
create_changedir_dialog()
{
	static int initialized = False;
	Arg args[1];
	int i;
	Widget w_list;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

	i = 0;
	XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	w_changeDirDialog = XmCreateSelectionDialog( w_toplev, "changeDir", args,i);
	XtAddCallback(w_changeDirDialog, XmNokCallback, cb_changedir_ok,
		(XtPointer)NULL);
	XtAddCallback(w_changeDirDialog, XmNhelpCallback, cb_changedir_help,
		(XtPointer)NULL);

	/* Enhance appearance of scrolled list widget */
	w_list = XmSelectionBoxGetChild(w_changeDirDialog, XmDIALOG_LIST);
	fix_list_color(w_list);

	/* Enhance appearance of text widget */
	set_bg_to_sb_trough_color(
		XmSelectionBoxGetChild(w_changeDirDialog, XmDIALOG_TEXT),
		vertical_sb(XtParent(w_list)));

	/* Don't need "Apply" button */
	XtUnmanageChild(XmSelectionBoxGetChild(w_changeDirDialog,
		XmDIALOG_APPLY_BUTTON));
}


/*
 * update_changedir_dialog - Display most-recently-used directory paths
 *                           in the "Change Directory" display for "host".
 */
update_changedir_dialog(host)
int host;
{
	struct sl_struct *list;
	XmStringTable items;
	int i;
	Widget widget;

	/* Enter directory paths into scrolled list */
	retrieve_cache(DIRECTORY, hinfo[host].hostname, &list, sort_caches);
	items = (XmStringTable)XtMalloc(sizeof(XmString)*list->nentries);
	for (i=0; i<list->nentries; i++)
		items[i] = XmStringCreateSimple(list->entries[i]);
	XtVaSetValues(
		w_changeDirDialog,
		XmNlistItems,		items,
		XmNlistItemCount,	list->nentries,
		NULL
	);

	/* Free stuff */
	for (i=0; i<list->nentries; i++)
		XmStringFree(items[i]);
	XtFree((char *)items);
	release_array_list(list);

    /* Clear text */
    widget = XmSelectionBoxGetChild(w_changeDirDialog, XmDIALOG_TEXT);
    if (hinfo[host].system == SYS_VMS) {
		XmTextSetString(widget, "[.]");
		XmTextSetInsertionPosition(widget, 2);
	} else 
        XmTextSetString(widget, "");

	/* Clear selection */
	widget = XmSelectionBoxGetChild(w_changeDirDialog, XmDIALOG_LIST);
	XtVaSetValues(widget, XmNselectedItemCount, 0, NULL);

	/* Remember host */
	XtVaSetValues(w_changeDirDialog, XmNuserData, host, NULL);
}


/*
 * cb_changedir_ok - Callback for Change Directory Dialog's OK button.
 *                   This routine actually performs the directory change.
 */
void
cb_changedir_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	long host;
	char *new_wd;

	XtVaGetValues(w_changeDirDialog, XmNuserData, &host, NULL);

    /* Clear all selected entries */
    clear_selected_entries(host);

	/* Get directory path*/
	new_wd = XmTextGetString(XmSelectionBoxGetChild(w_changeDirDialog,
		XmDIALOG_TEXT));

    /* Try to cd to specified directory */
	change_directory(host, new_wd);
	XtFree(new_wd);
}


/*
 * change_directory - Change host's working directory to "new_wd".  This
 *                    routine takes care of report errors, updating the
 *                    display, and updating hinfo[host].wd.
 */
change_directory(host, new_wd)
int host;
char *new_wd;
{
    char *wd;

    /* This might take some time */
    use_busy_cursor();

    /* If VMS or if partial path, first cd into working directory */
    if ((hinfo[host].system == SYS_VMS)||(strlen(new_wd) && new_wd[0] != '/')) {
        if (hinfo[host].type == LOCAL) {
            if (local_cd(host, hinfo[host].wd, True) < 0) {
                warning_error("Unable to change directory");
				goto update;
            }
        } else {
            switch (remote_cd(host, hinfo[host].wd, False, True)) {
            case -1:
                warning_error("Unable to change directory");
				goto update;
            case -3:
                restore_prev_cursor();
                lost_connection(host);
                return;
            }
        }
    }

    /* cd into new directory */
    if (hinfo[host].type == LOCAL) {
        if (local_cd(host, new_wd, True) < 0) {
            warning_error("Unable to change directory");
			goto update;
        }
        XtFree(hinfo[host].wd);
        if (local_pwd(&(hinfo[host].wd)) != 0)
            fatal_error("Unable to get local working directory");
    } else {
        switch (remote_cd(host, new_wd, False, True)) {
        case -1:
            warning_error("Unable to change directory");
			goto update;
        case -3:
            restore_prev_cursor();
            lost_connection(host);
            return;
        }
        if (remote_pwd(host, &wd) < 0) {
            restore_prev_cursor();
            lost_connection(host);
            return;
        }
        XtFree(hinfo[host].wd);
        hinfo[host].wd = wd;
    }

    /* Update the display */
    if (update_dir_displays(host, False) < 0) {
        restore_prev_cursor();
        lost_connection(host);
        return;
    }
update:
    update_xfer_controls();
    update_dirname_menu(host);
    update_dirpath(host);
    update_dir_cache(host);
    update_host_controls(host);
    restore_prev_cursor();
}


/*
 * cb_changedir_help - Callback that invokes the help package for the
 *                     "Change Directory" dialog.
 */
void
cb_changedir_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    help_dialog(w_toplev, True, "Change Directory", help_change_dir);
}


