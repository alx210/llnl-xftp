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

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include "xftp.h"

static struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_pathLabel;
	Widget w_path;
} dirpath[NUMHOSTS];

static char * dirpath_labels[] = {
	"\"Left\" Directory:",
	"\"Right\" Directory:"
};

static char *dirpath_dialog_name[] = {
	"leftDirPath",
	"rightDirPath"
};

static int dirpath_dialog_created[] = { False, False };

extern struct st_host_info hinfo[];
extern Widget w_dirList[];


/*
 * cb_display_dirpath - Display the full directory name in a dialog window.
 */
void
cb_display_dirpath(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

	create_dirpath_dialog(host);

	update_dirpath(host);

	XtManageChild(dirpath[host].w_dialog);
	add_dialog_to_list(dirpath[host].w_dialog);
}


/*
 * create_dirpath_dialog - Create dialog window that displays full
 *                         directory path name.
 */
create_dirpath_dialog(host)
int host;
{
	Arg args[1];
	XmString label;
	int i;

    /* Create dialog only once */
    if (dirpath_dialog_created[host])
        return;
    dirpath_dialog_created[host] = True;

	/* Create prompt dialog to display working directory */
	i = 0;
	XtSetArg(args[i], XmNresizePolicy, XmRESIZE_NONE); i++;
	dirpath[host].w_dialog = XmCreatePromptDialog(
		w_dirList[host],
		dirpath_dialog_name[host],
		args,
		i
	);

    /* Don't need all the prompt dialog's buttons */
    XtUnmanageChild(XmSelectionBoxGetChild(dirpath[host].w_dialog,
        XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(dirpath[host].w_dialog,
		XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(dirpath[host].w_dialog,
        XmDIALOG_OK_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(dirpath[host].w_dialog,
        XmDIALOG_HELP_BUTTON));

    /* Create form for control area */
    dirpath[host].w_form = XtVaCreateWidget(
        "form",
        xmFormWidgetClass,
        dirpath[host].w_dialog,
        NULL
    );

    /* Create identifying label */
    label = XmStringCreateSimple(dirpath_labels[host]);
    dirpath[host].w_pathLabel = XtVaCreateManagedWidget(
        "pathLabel",
        xmLabelGadgetClass,
        dirpath[host].w_form,
        XmNlabelString,         label,
        XmNtopAttachment,       XmATTACH_FORM,
        XmNtopOffset,           10,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNbottomOffset,        10,
        NULL
    );
    XmStringFree(label);

    /* Create label widget to display path */
    label = XmStringCreateSimple("");
    dirpath[host].w_path= XtVaCreateManagedWidget(
        "path",
        xmLabelGadgetClass,
        dirpath[host].w_form,
        XmNlabelString,     label,
        XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET,
        XmNtopWidget,       dirpath[host].w_pathLabel,
        XmNleftAttachment,  XmATTACH_WIDGET,
        XmNleftWidget,      dirpath[host].w_pathLabel,
        XmNleftOffset,      5,
        NULL
    );
    XmStringFree(label);

    XtManageChild(dirpath[host].w_form);
}


/*
 * update_dirpath - Update contents of directory path dialog.
 */
update_dirpath(host)
int host;
{
	XmString string;

	/* Make sure dialog exists */
	if (!dirpath_dialog_created[host])
		return;
		
	if (hinfo[host].type == NEITHER)
		string = XmStringCreateSimple("None");
	else
		string = XmStringCreateSimple(hinfo[host].wd);

	XtVaSetValues(dirpath[host].w_path, XmNlabelString, string, NULL);

	XmStringFree(string);
}

