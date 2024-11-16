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
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

struct {
    int host;
    struct entry_link *head;
} quote_ctrl;

static struct {
    Widget w_dialog;
    Widget w_form;
    Widget w_listLabel;
    Widget w_list;
    Widget w_textLabel;
    Widget w_text;
    Widget w_warningLabel;
    Widget w_warningFrame;
    Widget w_warning;
	Widget w_warningMsg1;
	Widget w_warningMsg2;
	Widget w_warningMsg3;
} quote;

static char *quote_help[] = {
	"This dialog enables you to send a \"quoted\" (that is,",
	"low_level) command directly to the FTP server of the associated",
	"remote host.  (This is similar the the capability offered by",
	"FTP's QUOTE command.)\n",
	"\n",
	"WARNING: Exercise extreme caution when using this command",
	"because LLNL XFTP depends on the FTP server being in a known state.  ",
	"In particular, be sure that you only issue commands that",
	"generate one response from the FTP server.  Do not issue",
	"commands that change the state of the FTP server (e.g.,",
	"connection/login sequences, file transfers, directory",
	"listings, etc.).\n",
	"\n",
    "Previously referenced commands are cached and",
    "displayed in the scrollable list labeled \"Recently",
    "Referenced Commands\".  Double-click on a command in the",
    "list to issue it.  Single-click on a command",
    "in the list to place that command into the text",
    "field labeled \"Low-Level FTP Command\", where it can be edited.\n",
    "\n",
    "The user preference SORT CACHES specifies whether the cached",
    "quoted commands are displayed in alphabetical order or",
    "with the most-recently-referenced commands at the top of",
    "the list.  The user preference QUOTE CACHE SIZE",
    "specifies the maximum number of quoted commands to cache",
    "per host.  The quoted command cache is preserved across LLNL XFTP",
    "sessions.\n",
    "\n",
	"To enable a command to act on a set of items selected in",
	"the associated host's directory list, use the quote",
	"placeholder (which by defaults is \"<>\" to represent a",
	"selected item.  For example, type \"stage 1 <>\" to send",
	"a STAGE command to the remote host for each selected item.  ",
	"The quote placeholder can be set via the user preference",
	"QUOTE PLACEHOLDER.\n",
	"\n",
	"Click on the SEND button to send the command to the FTP",
	"server on the remote host.  A dialog will pop up to enable",
	"the user to monitor the progress of the sending of commands.  ",
	"The dialog contains a button for aborting the operation.\n",
	"\n",
	"Click on the CANCEL button to remove",
    "the dialog without performing the operation.",
    NULL
};

extern Widget w_toplev;
extern struct st_host_info hinfo[];
extern Widget w_dirList[];
extern XtAppContext app;
extern int beep_when_ops_done;
extern char *quote_placeholder;
extern int inquire_on_quote;
extern int sort_caches;

void cb_quote_ok();
void cb_quote_single_selection();
void cb_quote_help();
int cb_quote_send_commands();
char *cstring_to_text();


/*
 * cb_send_quoted_command - Pop up dialog that prompts for low-level command
 *                          to be sent to remote FTP server.
 */
void
cb_send_quoted_command(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	int retval;

	/* Sanity check */
	if (hinfo[host].type == LOCAL)
		fatal_error("Bug in cb_send_quoted_command()");

    /* Clear error flag */
    raise_okflag();

    /* Create quote dialog */
    create_quote_dialog();

	/* cd into working directory */
	use_busy_cursor();
	retval = remote_cd(host, hinfo[host].wd, False, True);
	restore_prev_cursor();
    switch (retval) {
    case -1:
        warning_error("Problems setting up Quote mode -- Try again.");
        return;
    case -3:
        lost_connection(host);
        return;
    }

    update_quote_dialog(host);

    XtManageChild(quote.w_dialog);
    add_dialog_to_list(quote.w_dialog);
	XmProcessTraversal(quote.w_text, XmTRAVERSE_CURRENT);
	XmProcessTraversal(quote.w_text, XmTRAVERSE_CURRENT);
}


/*
 * create_quote_dialog - Create dialog that prompts for a quoted command.
 */
create_quote_dialog()
{
	static int initialized = False;
    int i;
    Arg args[2];
    Widget widget;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog to get quoted command */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    quote.w_dialog = XmCreatePromptDialog(w_toplev, "quote", args, i);
    XtAddCallback(quote.w_dialog, XmNokCallback, cb_quote_ok,
		(XtPointer)NULL);
	XtAddCallback(quote.w_dialog, XmNhelpCallback, cb_quote_help,
		(XtPointer)NULL);

    /* Don't show prompt dialog's selection label and text */
    widget = XmSelectionBoxGetChild(quote.w_dialog, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(widget);
    widget = XmSelectionBoxGetChild(quote.w_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(widget);

	/* Create form for control area */
	quote.w_form = XtVaCreateWidget(
		"form",
		xmFormWidgetClass,
		quote.w_dialog,
		NULL
	);

	/* Create label for scrolled list of quoted expressions */
	quote.w_listLabel = XtVaCreateManagedWidget(
		"listLabel",
		xmLabelWidgetClass,
		quote.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create scrolled list of quoted expressions */
	i = 0;
	XtSetArg(args[i], XmNselectionPolicy, XmSINGLE_SELECT); i++;
	XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
	quote.w_list = XmCreateScrolledList(quote.w_form, "list", args, i);
	XtVaSetValues(
		XtParent(quote.w_list),
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		quote.w_listLabel,
		NULL
	);
	XtManageChild(quote.w_list);
	fix_list_color(quote.w_list);

    /* Add single-selection callback */
    XtAddCallback(
        quote.w_list,
        XmNsingleSelectionCallback,
        cb_quote_single_selection,
        (XtPointer)NULL
    );

    /* Add double-click callback to directory list*/
    XtAddCallback(
        quote.w_list,
        XmNdefaultActionCallback,
        cb_quote_ok,
        (XtPointer)NULL
    );

	/* Create label for quoted command */
	quote.w_textLabel = XtVaCreateManagedWidget(
		"textLabel",
		xmLabelWidgetClass,
		quote.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		quote.w_list,
		XmNtopOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create textfield for quoted command */
	quote.w_text = XtVaCreateManagedWidget(
		"text",
		xmTextFieldWidgetClass,
		quote.w_form,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		quote.w_textLabel,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Make background color of textfield widget same as scrollbar trough */
	set_bg_to_sb_trough_color(quote.w_text,
		vertical_sb(XtParent(quote.w_list)));

	/* Create label for warning message */
	quote.w_warningLabel = XtVaCreateManagedWidget(
		"warningLabel",
		xmLabelWidgetClass,
		quote.w_form,
		XmNalignment,		XmALIGNMENT_CENTER,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		quote.w_text,
		XmNtopOffset,		15,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create frame for warning message */
	quote.w_warningFrame = XtVaCreateManagedWidget(
		"warningFrame",
		xmFrameWidgetClass,
		quote.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			quote.w_warningLabel,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,		10,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNleftOffset,			10,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNrightOffset,			10,
		NULL
	);

	/* Create rowcolumn to hold warning message */
    quote.w_warning = XtVaCreateManagedWidget(
        "warning",
        xmRowColumnWidgetClass,
        quote.w_warningFrame,
        NULL
    );

	/* Create label for first line of warning message */
	quote.w_warningMsg1 = XtVaCreateManagedWidget(
		"warningMsg1",
		xmLabelWidgetClass,
		quote.w_warning,
		NULL
	);

	/* Create label for second line of warning message */
	quote.w_warningMsg2 = XtVaCreateManagedWidget(
		"warningMsg2",
		xmLabelWidgetClass,
		quote.w_warning,
		NULL
	);

	/* Create label for third line of warning message */
	quote.w_warningMsg3 = XtVaCreateManagedWidget(
		"warningMsg3",
		xmLabelWidgetClass,
		quote.w_warning,
		NULL
	);

	XtManageChild(quote.w_form);
}


/*
 * cb_quote_ok - This callback actually sends the quoted command.
 */
void
cb_quote_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    int host;
	char *command;
	struct entry_link *ptr;
	struct entry_link *head;
	char *prefix;
	char *suffix;
	char *pat;
	char *temp;
    XmStringTable selected_items;
    int nselected_items;
    int i;
	int len;
	char *entry;
	int process_selected_items = False;

    /* Get host */
    XtVaGetValues(quote.w_dialog, XmNuserData, &host, NULL);

	/* Get command */
	command = XmTextFieldGetString(quote.w_text);

	/* Add command to cache */
	add_to_cache(QUOTE, hinfo[host].hostname, command);

	/* Should the selection (if any) be processed? */
	if (strlen(quote_placeholder) && strstr(command, quote_placeholder)) {
	    XtVaGetValues(w_dirList[host],
   	    	XmNselectedItemCount, &nselected_items,
   	    	NULL
    	);
		if (nselected_items == 0) {
			warning_error("A placeholder is used, but no items are selected.");
			XtUnmanageChild(quote.w_dialog);
			return;
		}
		XtUnmanageChild(quote.w_dialog);
		if (inquire_on_quote && !verify_selection(host,
				"Do you really want to operate on these items?"))
			return;
		process_selected_items = True;
	}

	/* Set up linked list of quoted commands */
	if (process_selected_items) {   /* Replace pattern with selected entries */
		temp = XtNewString(command);
		prefix = temp;
		pat = strstr(prefix, quote_placeholder);
		*pat = '\0';
		suffix = &pat[strlen(quote_placeholder)];
		len = strlen(prefix)+strlen(suffix)+1;
	    XtVaGetValues(
	        w_dirList[host],
	        XmNselectedItemCount, &nselected_items,
	        XmNselectedItems,     &selected_items,
	        NULL
	    );
	    head = NULL;
	    for (i=nselected_items-1; i>=0; i--) {
	        ptr = XtNew(struct entry_link);
			entry = cstring_to_text(selected_items[i]);
	        strip_off_symbol(hinfo[host].system, hinfo[host].server, entry);
			ptr->entry = XtMalloc(len+strlen(entry));
			strcpy(ptr->entry, prefix);
			strcat(ptr->entry, entry);
			strcat(ptr->entry, suffix);
			XtFree(entry);
	        ptr->next = head;
	        head = ptr;
	    }
		XtFree(temp);
	} else {   /* Form single cmd */
		head = XtNew(struct entry_link);
		head->entry = XtNewString(command);
		head->next = NULL;
	}

	/* Set up control block */
	quote_ctrl.host = host;
	quote_ctrl.head = head;

    /* This might take some time */
    use_busy_cursor();

    /* Pop up multipurpose monitor */
    show_mp_monitor("Now Issuing:");
    clear_mp_monitor();

    /* Disable controls */
    enable_controls(False);

	/* Preform quoted command */
	XtAppAddWorkProc(app, (XtWorkProc)cb_quote_send_commands, NULL);

	/* Hide quote dialog */
	XtUnmanageChild(quote.w_dialog);
}


/*
 * update_quote_dialog - Display last issued "quoted" commands in scrolled
 *                       list in "Quoted Command" dialog.
 */
update_quote_dialog(host)
int host;
{
	struct sl_struct *list;
	XmString string;
	int i;

    /* Enter quoted expressions into scrolled list */
	retrieve_cache(QUOTE, hinfo[host].hostname, &list, sort_caches);
	reset_list(quote.w_list);
    for (i=0; i<list->nentries; i++) {
        string = XmStringCreateSimple(list->entries[i]);
		XmListAddItem(quote.w_list, string, 0);
		XmStringFree(string);
	}
    release_array_list(list);

	/* Clear text and selection */
	XmTextFieldSetString(quote.w_text, "");

    /* Remember host */
    XtVaSetValues(quote.w_dialog, XmNuserData, host, NULL);
}


/*
 * cb_quote_single_selection - This callback is invoked when a cached
 *                             quoted command is selected.
 */
void
cb_quote_single_selection(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    XmStringTable selected_items;
    int nselected_items;
    char *text;

	/* Don't let user deselect item */
    XtVaGetValues(
        quote.w_list,
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );
    if (nselected_items == 0) {
        XmListSelectPos(quote.w_list, cbs->item_position, False);
        XtVaGetValues(
            quote.w_list,
            XmNselectedItems,     &selected_items,
            NULL
        );
    }

    /* Use selected item to text field */
	text = cstring_to_text(selected_items[0]);
    XmTextFieldSetString(quote.w_text, text);
    XtFree(text);
}


/*
 * cb_quote_send_commands - Work proc that sends commands specified in Quoted
 *                          Command dialog to remote FTP server.
 */
cb_quote_send_commands()
{
    struct entry_link *ptr;
    char msg[MAXPATHLEN+40];

    /* Are we done? */
    if (quote_ctrl.head == NULL)
        goto done;

    /* Did user push abort button? */
    if (mp_abort_requested()) {
        hide_abort_dialog();
        warning_error("Quoted command(s) aborted");
        goto done;
    }

    /* Let user know where we are */
    update_mp_monitor(quote_ctrl.head->entry);

    /* Issue quoted command */
	switch (quote_cmd(quote_ctrl.host, quote_ctrl.head->entry)) {
    case -3:
        goto lost;
    case -1:
        sprintf(msg, "Unable to issue quoted command: '%s'",
			quote_ctrl.head->entry);
        warning_error(msg);
    }
    ptr = quote_ctrl.head;
    quote_ctrl.head = quote_ctrl.head->next;
    XtFree(ptr->entry);
    XtFree((char *)ptr);
    return False;

done:

    release_linked_list(&quote_ctrl.head);
    hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
    enable_controls(True);
	clear_selected_entries(quote_ctrl.host);
    update_xfer_controls();
    update_host_controls(quote_ctrl.host);
    if (beep_when_ops_done)
        beep();
    restore_prev_cursor();
    return True;

lost:

    release_linked_list(&quote_ctrl.head);
    hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
    enable_controls(True);
    clear_selected_entries(quote_ctrl.host);
    restore_prev_cursor();
    lost_connection(quote_ctrl.host);
    if (beep_when_ops_done)
        beep();
    return True;
}


/*
 * cb_quote_help - Callback that display help info for the Quoted Command
 *                 dialog.
 */
void
cb_quote_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    help_dialog(quote.w_dialog, True, "Send Quote Command", quote_help);
}


