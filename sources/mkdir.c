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
#include <Xm/Text.h>
#include "xftp.h"
#include "str.h"

static Widget w_mkdirDialog;

static char *help_make_dir[] = {
	"This dialog enables you to create a directory with the",
	"name specified in the \"Directory Name\" field.  The new",
	"directory is created as a subdirectory of the current directory.\n",
    "\n",
	"The newly created directory is automatically entered if the",
	"user preference ENTER DIR UPON CREATION is set to YES.\n",
	"\n",
    "Click on the OK button to perform the operation and",
    "remove the dialog.  Click on the CANCEL button to remove",
    "the dialog without performing the operation.",
    NULL
};

extern struct st_host_info hinfo[];
extern Widget w_toplev;
extern int enter_dir_upon_creation;

void cb_mkdir_ok();
void cb_mkdir_help();
char *merge_paths();


/*
 * cb_make_dir - Callback for popping up the "Make Directory" dialog.
 */
void
cb_make_dir(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	long host = (long)client_data;
	Widget w_dir;

    /* Clear error flag */
    raise_okflag();

    /* Create "Make Directory" dialog */
    create_mkdir_dialog();

	/* Remember host */
	XtVaSetValues(w_mkdirDialog, XmNuserData, host, NULL);

	/* Pop up dialog */
	w_dir = XmSelectionBoxGetChild(w_mkdirDialog, XmDIALOG_TEXT);
    if (hinfo[host].system == SYS_VMS) {
        XmTextSetString(w_dir, "[.]");
        XmTextSetInsertionPosition(w_dir, 2);
	} else
        XmTextSetString(w_dir, "");
	XtManageChild(w_mkdirDialog);
	add_dialog_to_list(w_mkdirDialog);
	XmProcessTraversal(w_dir, XmTRAVERSE_CURRENT);
	XmProcessTraversal(w_dir, XmTRAVERSE_CURRENT);
}


/*
 * create_mkdir_dialog - Create the "Make Directory" dialog.
 */
create_mkdir_dialog()
{
	static int initialized = False;
    int i;
    Arg args[1];

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog to get directory path */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    w_mkdirDialog = XmCreatePromptDialog(w_toplev, "makeDir", args, i);
    XtAddCallback(w_mkdirDialog, XmNokCallback, cb_mkdir_ok,
		(XtPointer)NULL);
	XtAddCallback(w_mkdirDialog, XmNhelpCallback, cb_mkdir_help,
		(XtPointer)NULL);
}


/*
 * cb_mkdir_ok - This callback actually creates the directory.
 */
void
cb_mkdir_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	long host;
	char *dirname;
	char *new_dir;
	char *text;
	int len;

	/* This might take some time */
	use_busy_cursor();

	/* Get host */
	XtVaGetValues(w_mkdirDialog, XmNuserData, &host, NULL);

	/* Get directory name*/
	text = XmTextGetString(XmSelectionBoxGetChild(w_mkdirDialog,
		XmDIALOG_TEXT));
	if (hinfo[host].system == SYS_VMS) {
		len = strlen(text);
		if (strchr(text, '[') != NULL) {
			if ((len < 4) || (text[0] != '[') || (text[1] != '.')
			    	      || (text[len-1] != ']') || strchr(&text[2], '.')) {
				warning_error("Unable to make directory");
				restore_prev_cursor();
				XtFree(text);
				return;
			}
			dirname = XtNewString(text);
		} else if (strstr(text, ".dir"))
			dirname = XtNewString(text);
		else {
			dirname = XtMalloc(strlen(text)+5);
			strcpy(dirname, text);
			strcat(dirname, ".dir");
		}
	} else
		dirname = XtNewString(text);
	XtFree(text);

    /* Form full path name */
	new_dir = merge_paths(hinfo[host].system, hinfo[host].wd, dirname);
    XtFree(dirname);

	/* Make new directory */
    if (hinfo[host].type == LOCAL) {
        if (local_mkdir(host, new_dir, 0777) < 0) {
            warning_error("Unable to make directory");
			XtFree(new_dir);
			restore_prev_cursor();
            return;
        }
    } else {
        switch (remote_mkdir(host, new_dir)) {
        case -1:
            warning_error("Unable to make directory");
			XtFree(new_dir);
			restore_prev_cursor();
            return;
        case -3:
			restore_prev_cursor();
            lost_connection(host);
			XtFree(new_dir);
            return;
        }
    }

	/* Change to the new directory? */
	if (enter_dir_upon_creation) {
		change_directory(host, new_dir);
		XtFree(new_dir);
	} else {
		XtFree(new_dir);
		if (update_dir_displays(host, False) < 0) {
			restore_prev_cursor();
			lost_connection(host);
			return;
		}
		update_xfer_controls();
		update_dirname_menu(host);
		update_dirpath(host);
		update_dir_cache(host);
		update_host_controls(host);
	}
		
	/* Restore cursor*/
	restore_prev_cursor();
}


/*
 * cb_mkdir_help - Callback to display help information for "Make Directory"
 *                 dialog.
 */
void
cb_mkdir_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_mkdirDialog, True, "Make Directory", help_make_dir);
}


