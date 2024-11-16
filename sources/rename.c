;/***************************************************************************/
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
#include <Xm/TextF.h>
#include <stdio.h>
#include "xftp.h"

#define NONE    0
#define RENAME	1
#define SKIP	2
#define CANCEL	3

static struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_oldNameLabel;
	Widget w_oldName;
	Widget w_newNameLabel;
	Widget w_newName;
} renam;

static int response;

static char *rename_help[] = {
	"This dialog enables you to individually rename the selected",
	"entries in the directory list of the associated host.\n",
	"\n",
	"Type the desired name in the \"New Name\" field.  Click on",
	"the RENAME button to change the entry's name.  Click on the",
	"SKIP button to skip to the next selected entry without renaming",
	"the current entry.  Click on the CANCEL button to remove the",
	"dialog (this will not undo previously renamed entries).",
	NULL
};

extern Widget w_toplev;
extern Widget w_dirList[];
extern XtAppContext app;
extern struct st_host_info hinfo[];
extern int diagnostics;

void cb_rename_ok();
void cb_rename_skip();
void cb_rename_cancel();
void cb_rename_help();
char *merge_paths();
char *equivalent_file_path();
char *cstring_to_text();
void create_rename_dialog();


/*
 * cb_rename_selected_entries - Callback that pops up "Rename Selected
 *                              Entries" dialog.
 */
void
cb_rename_selected_entries(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	XmStringTable selected_items;
	int nselected_items;
	int i;
	char *old_name;
	char *new_name;
	int success;
	char *src_path;
	char *snk_path;
	int retval;
	char msg[MAXPATHLEN+40];
	XmString string;
	char *temp_path;

    /* Clear error flag */
    raise_okflag();

    /* Create rename dialog */
    create_rename_dialog();

	/* Pop up dialog */
	XtManageChild(renam.w_dialog);
	add_dialog_to_list(renam.w_dialog);

	/* Get selected items */
    XtVaGetValues(
        w_dirList[host],
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );

	/* Rename selected items */
	for (i=0; i<nselected_items; i++) {
		old_name = cstring_to_text(selected_items[i]);
		strip_off_symbol(hinfo[host].system, hinfo[host].server, old_name);
		string = XmStringCreateSimple(old_name);
		XtVaSetValues(renam.w_oldName, XmNlabelString, string, NULL);
		XmStringFree(string);
		success = False;
		while (!success) {
			XmTextFieldSetString(renam.w_newName, "");
			XmProcessTraversal(renam.w_newName, XmTRAVERSE_CURRENT);
			XmProcessTraversal(renam.w_newName, XmTRAVERSE_CURRENT);
			XtManageChild(renam.w_dialog);
			response = NONE;
			while (response == NONE)
				XtAppProcessEvent(app, (XtInputMask)XtIMAll);
			switch (response) {
			case RENAME:
				new_name = XmTextFieldGetString(renam.w_newName);
				if (strlen(new_name) == 0) {
			        sprintf(msg, "Unable to rename %s", old_name);
			        warning_error(msg);
					XtFree(new_name);
					continue;
				}
				temp_path = merge_paths(hinfo[host].system, hinfo[host].wd,
					old_name);
				src_path = equivalent_file_path(hinfo[host].system, temp_path);
				XtFree(temp_path);
				temp_path = merge_paths(hinfo[host].system, hinfo[host].wd,
					new_name);
				snk_path = equivalent_file_path(hinfo[host].system, temp_path);
				XtFree(temp_path);
				XtFree(new_name);
				use_busy_cursor();
			    if (hinfo[host].type == LOCAL)
			        retval = local_rename(host, src_path, snk_path);
			    else
			        retval = remote_rename(host, src_path, snk_path);
				restore_prev_cursor();
			    XtFree(src_path);
			    XtFree(snk_path);
			    switch (retval) {
			    case -3:
					XtFree(old_name);
			        goto lost;
			    case -1:
			        sprintf(msg, "Unable to rename %s", old_name);
			        warning_error(msg);
					continue;
			    case 0:
			        if (diagnostics >= NORMAL) {
			            sprintf(msg, "*** Successfully renamed: %s\n",old_name);
			            write_log(msg);
			        }
					raise_okflag();
			    }
				success = True;
				break;
			case SKIP:
				raise_okflag();
				success = True;
				break;
			case CANCEL:
				raise_okflag();
				XtFree(old_name);
				goto done;
			}
		}
		XtFree(old_name);
	}

done:

	if (update_dir_displays(host, False) < 0)
		goto lost;
    update_xfer_controls();
    update_host_controls(host);
	XtUnmanageChild(renam.w_dialog);
	return;

lost:

	lost_connection(host);
    update_xfer_controls();
    update_host_controls(host);
	XtUnmanageChild(renam.w_dialog);
}


/*
 * create_rename_dialog - Create dialog that prompts for new names.
 */
void create_rename_dialog()
{
	static int initialized = False;
	int i;
	Arg args[2];
	Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

	/* Create prompt dialog to get new names */
	i = 0;
	XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
	XtSetArg(args[i], XmNautoUnmanage, False); i++;
	renam.w_dialog = XmCreatePromptDialog(w_toplev, "rename", args, i);
	XtAddCallback(renam.w_dialog, XmNokCallback, cb_rename_ok,
		(XtPointer)NULL);
	XtAddCallback(renam.w_dialog, XmNapplyCallback, cb_rename_skip,
		(XtPointer)NULL);
	XtAddCallback(renam.w_dialog, XmNcancelCallback, cb_rename_cancel,
		(XtPointer)NULL);
	XtAddCallback(renam.w_dialog, XmNhelpCallback, cb_rename_help,
		(XtPointer)NULL);

	/* Adjust visibility of prompt dialog's widgets */
    widget = XmSelectionBoxGetChild(renam.w_dialog, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(widget);
    widget = XmSelectionBoxGetChild(renam.w_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(widget);
	widget = XmSelectionBoxGetChild(renam.w_dialog, XmDIALOG_APPLY_BUTTON);
	XtManageChild(widget);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(renam.w_dialog, cb_rename_cancel, NULL);

    /* Create form for control area */
    renam.w_form = XtVaCreateWidget(
        "form",
        xmFormWidgetClass,
        renam.w_dialog,
        NULL
    );

    /* Create label for old entry name */
    renam.w_oldNameLabel = XtVaCreateManagedWidget(
        "oldNameLabel",
        xmLabelWidgetClass,
        renam.w_form,
        XmNtopAttachment,   XmATTACH_FORM,
        XmNtopOffset,       10,
        XmNleftAttachment,  XmATTACH_FORM,
        NULL
    );

	/* Create label widget to display new name */
    renam.w_oldName = XtVaCreateManagedWidget(
        "oldName",
        xmLabelWidgetClass,
        renam.w_form,
        XmNtopAttachment,   XmATTACH_WIDGET,
		XmNtopWidget,		renam.w_oldNameLabel,
        XmNtopOffset,       6,
        XmNleftAttachment,  XmATTACH_FORM,
		XmNleftOffset,		35,
        NULL
    );

    /* Create label for new name */
    renam.w_newNameLabel = XtVaCreateManagedWidget(
        "newNameLabel",
        xmLabelWidgetClass,
        renam.w_form,
        XmNtopAttachment,   XmATTACH_WIDGET,
		XmNtopWidget,		renam.w_oldName,
        XmNtopOffset,       15,
        XmNleftAttachment,  XmATTACH_FORM,
        NULL
    );

	/* Create textfield for new name */
	renam.w_newName = XtVaCreateManagedWidget(
		"newName",
		xmTextFieldWidgetClass,
		renam.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			renam.w_newNameLabel,
		XmNtopOffset,			6,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNleftOffset,			28,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNrightOffset,			28,
		XmNbottomAttachment,	XmATTACH_FORM,
		NULL
	);

	XtManageChild(renam.w_form);

}


/*
 * cb_rename_ok - Callback that notes that user wishes to rename selected
 *                entry.
 */
void
cb_rename_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	response = RENAME;
}


/*
 * cb_rename_skip - Callback that notes that user does not wish to rename
 *                  selected entry.
 */
void
cb_rename_skip(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	response = SKIP;
}


/*
 * cb_rename_cancel - Callback that notes that user does not wish to continue
 *                    renaming selected entries.
 */
void
cb_rename_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	response = CANCEL;
}


/*
 * cb_rename_help - Callback that displays help information for "Rename"
 *                  dialog.
 */
void
cb_rename_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    help_dialog(renam.w_dialog, True, "Rename", rename_help);
}

