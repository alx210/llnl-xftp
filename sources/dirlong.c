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
#include <Xm/Form.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

static int dirlong_dialog_created[] = { False, False };

static struct {
	Widget w_dialog;
	Widget w_separator;
	Widget w_closeButton;
	Widget w_helpButton;
	Widget w_list;
} dirlong[NUMHOSTS];


static char *dirlong_dialog_name[] = {
    "leftDirLong",
    "rightDirLong"
};

static char *dirlong_help[] = {
	"The \"long\" directory list allows you to view the contents",
	"of the associated host's current directory in a scrollable",
	"list.  If supported by the host (and this is almost always",
	"the case), additional information about each directory entry",
	"is displayed, such as file length and access restrictions.  ",
	"This list is automatically updated as the current directory",
	"changes.  The long directory list does not support",
	"selection of entries or display the current selection.\n",
	"\n",
	"Use the user preference SORT LONG LISTS BY DATE to sort",
	"long directory lists by date instead of by name.  Be sure",
	"to read the online help for this preference (by clicking",
	"on SORT LONG LISTS BY DATE in the user preferences dialog)",
	"for an explanation and warning.\n",
	"\n",
	"WARNING: Using this feature can increase network traffic,",
	"put an additional load on the associated host, and slow",
	"down interactivity.",
	NULL
};

extern Widget w_dirList[];
extern struct st_host_info hinfo[];

void cb_dirlong_close();
void cb_dirlong_help();
struct sl_struct *create_null_array_list();
Widget vertical_sb();


/*
 * cb_display_dir_long - Callback to pop up "long" directory list dialog.
 */
void
cb_display_dir_long(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

    create_dirlong_dialog(host);

    XtManageChild(dirlong[host].w_dialog);
    add_dialog_to_list(dirlong[host].w_dialog);

    /* Update the display */
    if (update_dir_displays(host, True) < 0)
        lost_connection(host);
}


/*
 * create_dirlong_dialog - Create a dialog for the specified host that
 *                         displays a long list of the the entries of the
 *                         current working directory.
 */
create_dirlong_dialog(host)
int host;
{
	int i;
	Arg args[6];

    /* Create dialog only once */
    if (dirlong_dialog_created[host])
        return;
    dirlong_dialog_created[host] = True;

    /* Create form dialog */
    i = 0;
	XtSetArg(args[i], XmNautoUnmanage, False); i++;
    dirlong[host].w_dialog = XmCreateFormDialog(
        w_dirList[host],
        dirlong_dialog_name[host],
        args,
        i
    );

    /* Create "Close" pushbutton */
    dirlong[host].w_closeButton = XtVaCreateManagedWidget(
        "closeButton",
        xmPushButtonWidgetClass,
        dirlong[host].w_dialog,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNbottomOffset,        10,
        XmNleftAttachment,      XmATTACH_POSITION,
        XmNleftPosition,        20,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       32,
        NULL
    );
    XtAddCallback(dirlong[host].w_closeButton, XmNactivateCallback,
        cb_dirlong_close, (XtPointer)((long)host));

    /* Create "Help" pushbutton */
    dirlong[host].w_helpButton = XtVaCreateManagedWidget(
        "helpButton",
        xmPushButtonWidgetClass,
        dirlong[host].w_dialog,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNbottomOffset,        10,
        XmNleftAttachment,      XmATTACH_POSITION,
        XmNleftPosition,        68,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       80,
        NULL
    );
    XtAddCallback(dirlong[host].w_helpButton, XmNactivateCallback,
        cb_dirlong_help, (XtPointer)((long)host));

	/* Create separator */
    dirlong[host].w_separator = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        dirlong[host].w_dialog,
        XmNbottomAttachment,    XmATTACH_WIDGET,
        XmNbottomWidget,        dirlong[host].w_closeButton,
		XmNbottomOffset,		8,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );

	/* Create scrolled text window to hold list */
	i = 0;
	XtSetArg(args[i], XmNeditable, False); i++;
	XtSetArg(args[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(args[i], XmNcursorPositionVisible, False); i++;
	dirlong[host].w_list = XmCreateScrolledText(dirlong[host].w_dialog,
		"dirList", args, i);
	XtVaSetValues(XtParent(dirlong[host].w_list),
		XmNtopAttachment,		XmATTACH_FORM,
		XmNtopOffset,			8,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,		dirlong[host].w_separator,
		XmNbottomOffset,		8,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNleftOffset,			8,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNrightOffset,			8,
		NULL
	);
	XtManageChild(dirlong[host].w_list);
    fix_vertical_text_sb_color(dirlong[host].w_list);
    fix_horizontal_text_sb_color(dirlong[host].w_list);
	set_bg_to_sb_trough_color(dirlong[host].w_list,
		vertical_sb(XtParent(dirlong[host].w_list)));
}


/*
 * cb_dirlong_close - Callback for "Long Directory List" dialog's "Close"
 *                    button.
 */
void
cb_dirlong_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	XtUnmanageChild(dirlong[host].w_dialog);
}


/*
 * update_dirlong - Update the long directory display for host.  Returns 0 
 *                  for success, -3 for broken connection, and -1 for other
 *                  errors.
 */
update_dirlong(host)
int host;
{
	struct sl_struct *dlist;
	int i;
	int retval;
	char *buf;
	int len;
	int indx;

	/* Is long directory display in use? */
	if (!dirlong_dialog_created[host] || !XtIsManaged(dirlong[host].w_dialog))
		return 0;

    /* Get long directory list */
    switch (hinfo[host].type) {
    case NEITHER:
        dlist = create_null_array_list();
        break;
    case LOCAL:
        if (local_dir(hinfo[host].wd, &dlist) != 0)
            fatal_error("Unable to get local directory list");
        break;
    case REMOTE:
        if ((retval = remote_dir(host, hinfo[host].wd, &dlist)))
            return retval;
    }

	/* Place directory entries into long list */
	len = 0;
	for (i=0; i<dlist->nentries; i++)
		len += strlen(dlist->entries[i])+1;
	buf = XtMalloc(len+1);
	indx = 0;
	for (i=0; i<dlist->nentries; i++) {
		len = strlen(dlist->entries[i]);
		bcopy(dlist->entries[i], &(buf[indx]), len);
		indx += len;
		buf[indx++] = '\n';
	}
	buf[indx] = '\0';
	XmTextSetString(dirlong[host].w_list, buf);
	XtFree(buf);

	/* Free directory list */
	release_array_list(dlist);

	return 0;
}


/*
 * cb_dirlong_help - Callback to display "Long Directory List" dialog's
 *                   help package.
 */
void
cb_dirlong_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	char *text;

	XtVaGetValues(XtParent(dirlong[host].w_dialog), XmNtitle, &text, NULL);
    help_dialog(dirlong[host].w_dialog, False, text, dirlong_help);
}

