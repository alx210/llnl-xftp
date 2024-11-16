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
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

static struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_listLabel;
	Widget w_list;
	Widget w_textLabel;
	Widget w_text;
	Widget w_radioBoxFrame;
	Widget w_radioBox;
	Widget w_replaceToggle;
	Widget w_addToggle;
} wildcard;

static char *wildcard_help[] = {
	"This dialog enables you to apply a wildcard expression to",
	"select entries in the associated host's directory list.\n",
	"\n",
	"Wildcard expressions that contain the characters '*', '?',",
	"'[', and ']' are interpreted in the standard way (i.e., in the",
	"same way as the UNIX C Shell does).\n",
	"\n",
	"Previously referenced wildcard expressions are cached and",
	"displayed in the scrollable list labeled \"Recently",
	"Referenced Wildcards\".  Double-click on an expression in the",
	"list to apply that wildcard expression.  Single-click on an",
	"expression in the list to place that expression into the text",
	"field labeled \"Wildcard to Apply\", where it can be edited.\n",
	"\n",
    "The user preference SORT CACHES specifies whether the cached",
    "wildcard expressions are displayed in alphabetical order or",
    "with the most-recently-referenced expressions at the top of",
    "the list.  The user preference WILDCARD CACHE SIZE",
    "specifies the maximum number of wildcard expressions to cache",
    "per host.  The wildcard cache is preserved across LLNL XFTP",
    "sessions.\n",
    "\n",
    "Choose the toggle item \"Replace Filename Selection\",",
    "to cause the currently selected entries to be cleared",
    "before applying the wildcard expression.  Choose the",
    "toggle item \"Add to Filename Selection\" to cause the",
    "entries selected by the wildcard to be added to the",
    "current selection.  The user preference INITIAL WILDCARD",
    "MODE determines which of these two modes is the default.\n",
    "\n",
    "Click on the OK button to perform the operation and",
    "remove the dialog.  Click on the CANCEL button to remove",
    "the dialog without performing the operation.",
    NULL
};

extern Widget w_dirList[];
extern Widget w_toplev;
extern int initial_wildcard_mode;
extern int add_to_cache_ok;
extern struct st_host_info hinfo[];
extern int sort_caches;

void cb_wildcard_ok();
void cb_wildcard_help();
void cb_wildcard_single_selection();
char *cstring_to_text();


/*
 * cb_select_all_entries - Select all directory entries.
 */
void
cb_select_all_entries(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    int host = (int)((long)client_data);
    int nselected_items;
    int i;

	/* Clear error flag */
	raise_okflag();

    /* First clear all selected directory entries */
	clear_selected_entries(LEFT);
	clear_selected_entries(RIGHT);

    XtVaGetValues(
        w_dirList[host],
        XmNitemCount,  &nselected_items,
        NULL
    );

    for (i=1; i<=nselected_items; i++)
        XmListSelectPos(w_dirList[host], i, False);

	/* Now do the tabular directory display */
	if (dirtable_in_use(host))
		for (i=0; i<nselected_items; i++)
			select_table_entry(host, i);

    /* Update the display */
    update_xfer_controls();
    for (i=0; i<NUMHOSTS; i++)
        update_host_controls(i);
}


/*
 * cb_deselect_all_entries - Callback to deselected all entries.
 */
void
cb_deselect_all_entries(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

    clear_selected_entries(host);


    /* Update the display */
    update_xfer_controls();
    update_host_controls(host);
}


/*
 * cb_use_wildcard - Pop up dialog that prompts for wildcard.
 */
void
cb_use_wildcard(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

    /* Clear error flag */
	raise_okflag();

    create_wildcard_dialog();

	update_wildcard_dialog(host);

	XtManageChild(wildcard.w_dialog);
	add_dialog_to_list(wildcard.w_dialog);
	XmProcessTraversal(wildcard.w_text, XmTRAVERSE_CURRENT);
	XmProcessTraversal(wildcard.w_text, XmTRAVERSE_CURRENT);
}


/*
 * create_wildcard_dialog - Create dialog that prompts for wildcard.
 */
create_wildcard_dialog()
{
	static int initialized = False;
    int i;
    Arg args[5];
    Widget widget;

	/* Create dialog only once */
	if (initialized)
		return;
	initialized = True;

    /* Create prompt dialog to get wildcard */
    i = 0;
    XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    wildcard.w_dialog = XmCreatePromptDialog(w_toplev, "wildcard", args, i);
    XtAddCallback(wildcard.w_dialog, XmNokCallback, cb_wildcard_ok,
		(XtPointer)NULL);
	XtAddCallback(wildcard.w_dialog, XmNhelpCallback, cb_wildcard_help,
		(XtPointer)NULL);

    /* Don't show prompt dialog's selection label and text */
    widget = XmSelectionBoxGetChild(wildcard.w_dialog,XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(widget);
    widget = XmSelectionBoxGetChild(wildcard.w_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(widget);

	/* Create form for control area */
	wildcard.w_form = XtVaCreateWidget(
		"form",
		xmFormWidgetClass,
		wildcard.w_dialog,
		NULL
	);

	/* Create label for scrolled list of wildcard expressions */
	wildcard.w_listLabel = XtVaCreateManagedWidget(
		"listLabel",
		xmLabelWidgetClass,
		wildcard.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create scrolled list of wildcard expressions */
	i = 0;
	XtSetArg(args[i], XmNselectionPolicy, XmSINGLE_SELECT); i++;
	XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
	wildcard.w_list = XmCreateScrolledList(wildcard.w_form, "list", args, i);
	XtVaSetValues(
		XtParent(wildcard.w_list),
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		wildcard.w_listLabel,
		NULL
	);
	XtManageChild(wildcard.w_list);
	fix_list_color(wildcard.w_list);

    /* Add single-selection callback */
    XtAddCallback(
        wildcard.w_list,
        XmNsingleSelectionCallback,
        cb_wildcard_single_selection,
        (XtPointer)NULL
    );

    /* Add double-click callback to directory list*/
    XtAddCallback(
        wildcard.w_list,
        XmNdefaultActionCallback,
        cb_wildcard_ok,
        (XtPointer)NULL
    );

	/* Create label for wildcard expression */
	wildcard.w_textLabel = XtVaCreateManagedWidget(
		"textLabel",
		xmLabelWidgetClass,
		wildcard.w_form,
		XmNalignment,		XmALIGNMENT_BEGINNING,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		wildcard.w_list,
		XmNtopOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Create textfield for wildcard expression */
	wildcard.w_text = XtVaCreateManagedWidget(
		"text",
		xmTextFieldWidgetClass,
		wildcard.w_form,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		wildcard.w_textLabel,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL
	);

	/* Make background color of textfield widget same as scrollbar trough */
	set_bg_to_sb_trough_color(wildcard.w_text,
		vertical_sb(XtParent(wildcard.w_list)));

	/* Create frame for radio box */
	wildcard.w_radioBoxFrame = XtVaCreateManagedWidget(
		"radioBoxFrame",
		xmFrameWidgetClass,
		wildcard.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			wildcard.w_text,
		XmNtopOffset,			10,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);

	/* Create radio box */
	wildcard.w_radioBox = XmCreateRadioBox(wildcard.w_radioBoxFrame, "radioBox",
		NULL, 0);

	/* Create "Replace Selection" toggle */
	wildcard.w_replaceToggle = XtVaCreateManagedWidget(
		"replaceToggle",
		xmToggleButtonGadgetClass,
		wildcard.w_radioBox,
		XmNmarginHeight,	0,
		NULL
	);

	/* Create "Add to Selection" toggle */
	wildcard.w_addToggle = XtVaCreateManagedWidget(
		"addToggle",
		xmToggleButtonGadgetClass,
		wildcard.w_radioBox,
		XmNmarginHeight,	0,
		NULL
	);

	XtManageChild(wildcard.w_radioBox);

	/* Initialize wildcard mode */
	if (initial_wildcard_mode == REPLACE)
		XmToggleButtonSetState(wildcard.w_replaceToggle, True, True);
	else
		XmToggleButtonSetState(wildcard.w_addToggle, True, True);
	XtManageChild(wildcard.w_form);
}


/*
 * cb_wildcard_ok - This callback actually performs the wildcard processing.
 */
void
cb_wildcard_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    int host;
    char *expression;
	char *raw_expression;
	XmStringTable items;
	int nitems;
	char *citem;
	int i;
	int count;
	int wildcard_mode;
	char *ptr;

    /* Get host */
    XtVaGetValues(wildcard.w_dialog, XmNuserData, &host, NULL);

    /* First clear other host's directory entry selections */
	if (host == LEFT)
		clear_selected_entries(RIGHT);
	else
    	clear_selected_entries(LEFT);

	/* Grab wildcard and use first blank-delimited token */
	raw_expression = XmTextFieldGetString(wildcard.w_text);
	expression = strtok(raw_expression, " ");
	if (expression == NULL)
		expression = raw_expression;
	else {
		if (add_to_cache_ok) {
			ptr = XmTextFieldGetString(wildcard.w_text);
			add_to_cache(WILDCARD, hinfo[host].hostname, ptr);
			XtFree(ptr);
		}
	}

	/* Grab list of items */
	XtVaGetValues(w_dirList[host],
		XmNitems,		&items,
		XmNitemCount,	&nitems,
		NULL
	);

	/* Determine mode */
	if (XmToggleButtonGetState(wildcard.w_replaceToggle))
		wildcard_mode = REPLACE;
	else
		wildcard_mode = ADD;

	/* If replacement mode, first clear host's directory entry selections */
	if (wildcard_mode == REPLACE)
		clear_selected_entries(host);

	/* Process wildcard */
	count = 0;
	for (i=0; i<nitems; i++) {
		citem = cstring_to_text(items[i]);
		strip_off_symbol(hinfo[host].system, hinfo[host].server, citem);
		if (match(citem, expression)) {
			if (wildcard_mode == ADD)
				XmListDeselectPos(w_dirList[host], i+1);
			XmListSelectPos(w_dirList[host], i+1, False);
			if (dirtable_in_use(host))
				select_table_entry(host, i);
			count++;
		}
		XtFree(citem);
	}
	XtFree(raw_expression);

	/* Give warning if no match */
	if (count == 0)
		warning_error("No match.");

    /* Update the display */
    update_xfer_controls();
    for (i=0; i<NUMHOSTS; i++)
        update_host_controls(i);

	/* Hide wildcard dialog */
	XtUnmanageChild(wildcard.w_dialog);
}


/*
 * update_wildcard_dialog - Displays recently used wildcard expressions in
 *                          a scrolled list in the Wildcard dialog.
 */
update_wildcard_dialog(host)
int host;
{
	struct sl_struct *list;
	XmString string;
	int i;

    /* Enter wildcard expressions into scrolled list */
	retrieve_cache(WILDCARD, hinfo[host].hostname, &list, sort_caches);
	reset_list(wildcard.w_list);
    for (i=0; i<list->nentries; i++) {
        string = XmStringCreateSimple(list->entries[i]);
		XmListAddItem(wildcard.w_list, string, 0);
		XmStringFree(string);
	}
    release_array_list(list);

	/* Clear text and selection */
	XmTextFieldSetString(wildcard.w_text, "");

    /* Remember host */
    XtVaSetValues(wildcard.w_dialog, XmNuserData, host, NULL);
}


/*
 * cb_wildcard_single_selection - This callback is invoked when a cached
 *                                wildcard expression is selected.
 */
void
cb_wildcard_single_selection(widget, client_data, call_data)
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
        wildcard.w_list,
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );
    if (nselected_items == 0) {
        XmListSelectPos(wildcard.w_list, cbs->item_position, False);
        XtVaGetValues(
            wildcard.w_list,
            XmNselectedItems,     &selected_items,
            NULL
        );
    }

    /* Use selected item to text field */
	text = cstring_to_text(selected_items[0]);
    XmTextFieldSetString(wildcard.w_text, text);
    XtFree(text);
}


/*
 * cb_wildcard_help - Callback to display help info for Wildcard dialog.
 */
void
cb_wildcard_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(wildcard.w_dialog, True, "Wildcard", wildcard_help);
}

