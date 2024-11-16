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
#include <stdlib.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include "list.h"
#include "str.h"

#define VIEWFILE       ".xftpview"
#define MAXVPLINE      200
#define EV_IDENTIFIER  "Extension_Viewer:"
#define MAXQUESTION    1000

struct sl_struct *viewer_mappings;
int need_to_save_viewprefs = False;

static struct {
	Widget w_dialog;
	Widget w_form;
	Widget w_listLabel;
	Widget w_list;
	Widget w_extensionLabel;
	Widget w_extension;
	Widget w_commandLineLabel;
	Widget w_commandLine;
	Widget w_editFrame;
	Widget w_editForm;
	Widget w_addButton;
	Widget w_replaceButton;
	Widget w_deleteButton;
	Widget w_messageFrame;
	Widget w_messageRowcolumn;
	Widget w_message1;
	Widget w_message2;
} viewprefs;

static char *viewprefs_file;

static char *viewprefs_help[] = {
	"This dialog enables you to control which file will be displayed",
	"with which viewer, based on the file extension.\n",
	"\n",
	"A file to be viewed is first transferred to a temporary",
	"directory and then, if necessary, is decompressed.  The",
	"file extension (e.g., \".gif\") is then examined to",
	"determine which viewer the file is to be displayed in.  There",
	"are two types of viewers: the viewer built into LLNL XFTP",
	"(which is suitable for displaying text files) and external",
	"viewers (e.g., emacs for text and xv for graphics).\n",
	"\n",
	"Single-click on an entry in the extensions list to place",
	"that extension and its associated command line into the",
	"\"Extension\" and \"Viewer Command Line\" text fields,",
	"respectively.\n",
	"\n",
	"An extension is a file suffix that begins with a period",
	"(\".\").  A special entry in the extensions list, \"OTHERS\",",
	"represents all files that do not have their extension listed",
	"(including files with no extensions).\n",
	"\n",
	"The pattern \"<>\" in a command line is a placeholder for",
	"the name of the file to be viewed.  A blank command line",
	"indicates that the built-in viewer is to be used.\n",
	"\n",
	"To add a new extension to the list, fill in the \"Extension\"",
	"and \"Viewer Command Line\" text fields and press the ADD",
	"button.  If the text in \"Extension\" does not begin with",
	"a period, one will be added.  If the extension is already",
	"in the list, it will be replaced.\n",
	"\n",
	"To replace the command line associated with an extension,",
	"(1) single-click on the list item to select it, (2) edit",
	"the command line, and (3) press the REPLACE button.\n",
	"\n",
	"To delete an extension from the list, single-click on the",
	"list item to select it and then press the DELETE button.  The",
	"\"OTHERS\" entry cannot be deleted (but it's command line",
	"can be replaced).\n",
	"\n",
	"Click the APPLY button to cause the new changes to take",
	"effect.  The OK button has the same effect as APPLY, but",
	"the dialog is also closed.  The CANCEL button undoes the",
	"changes made since OK or APPLY were last pressed.\n",
	"\n",
	"The viewer preferences can be preserved across LLNL XFTP",
	"sessions in a text file named \".xftpview\" in the user's",
	"home directory (select SAVE PREFERENCES in the main window's",
	"OPTIONS menu).",
	NULL
};

static char *default_mappings[] = {
	".gif xv <>",
	".jpg xv <>",
	".ras xv <>",
	"OTHERS"
};
static ndefault_mappings = XtNumber(default_mappings);

static struct sl_struct *temp_viewer_mappings;

extern Widget w_toplev;

void cb_viewprefs_cancel();
void cb_viewprefs_help();
void cb_viewprefs_add();
void cb_viewprefs_replace();
void cb_viewprefs_delete();
void cb_viewprefs_ok();
void cb_viewprefs_apply();
void cb_viewprefs_single_selection();
void cb_extension_modified();
struct sl_struct *create_array_list();
int compare_first_token();
struct sl_struct *create_null_array_list();
struct sl_struct *duplicate_array_list();
char *get_extension();
char *get_command_line();
char *cstring_to_text();


/*
 * cb_viewer_preferences - Callback to pop up Viewer Preferences dialog.
 */
void
cb_viewer_preferences(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    /* Clear error flag */
    raise_okflag();

	/* Create the Viewer Preferences Dialog */
    create_viewprefs_dialog();

	/* Grey out "Viewers Preferences menu button */
	XtSetSensitive(widget, False);
	XtVaSetValues(viewprefs.w_dialog, XmNuserData, (XtPointer)widget, NULL);

	/* Make temporary copy of viewer/command line mappings */
	temp_viewer_mappings = duplicate_array_list(viewer_mappings);

	/* Update the Viewer Prefs dialog */
	display_viewprefs(viewer_mappings);
	XmTextFieldSetString(viewprefs.w_extension, "");
	XmTextFieldSetString(viewprefs.w_commandLine, "");

	/* Select first item in extensions list */
	XmListSelectPos(viewprefs.w_list, 1, True);

	XtManageChild(viewprefs.w_dialog);
	add_dialog_to_list(viewprefs.w_dialog);
	XmProcessTraversal(viewprefs.w_extension, XmTRAVERSE_CURRENT);
	XmProcessTraversal(viewprefs.w_extension, XmTRAVERSE_CURRENT);
}


/*
 * create_viewprefs_dialog - Create dialog for specifying which viewer 
 *                           should be used with each file type.
 */
create_viewprefs_dialog()
{
	static int initialized = False;
	Widget widget;
	Arg args[2];
	int i;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create prompt dialog */
    viewprefs.w_dialog = XmCreatePromptDialog(w_toplev, "viewPrefs", NULL, 0);
	XtAddCallback(viewprefs.w_dialog, XmNokCallback, cb_viewprefs_ok,
		(XtPointer)NULL);
	XtAddCallback(viewprefs.w_dialog, XmNapplyCallback, cb_viewprefs_apply,
		(XtPointer)NULL);
	XtAddCallback(viewprefs.w_dialog, XmNcancelCallback, cb_viewprefs_cancel,
		(XtPointer)NULL);
	XtAddCallback(viewprefs.w_dialog, XmNhelpCallback, cb_viewprefs_help,
		(XtPointer)NULL);

    /* Don't show prompt dialog's selection label and text */
    widget = XmSelectionBoxGetChild(viewprefs.w_dialog,
		 XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(widget);
    widget = XmSelectionBoxGetChild(viewprefs.w_dialog, XmDIALOG_TEXT);
    XtUnmanageChild(widget);

	/* Want to see "Apply" button */
	widget = XmSelectionBoxGetChild(viewprefs.w_dialog, XmDIALOG_APPLY_BUTTON);
	XtManageChild(widget);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(viewprefs.w_dialog, cb_viewprefs_cancel, NULL);

    /* Create form for control area */
    viewprefs.w_form = XtVaCreateWidget(
        "form",
        xmFormWidgetClass,
        viewprefs.w_dialog,
        NULL
    );

    /* Create label for extension */
    viewprefs.w_extensionLabel = XtVaCreateManagedWidget(
        "extensionLabel",
        xmLabelWidgetClass,
        viewprefs.w_form,
        XmNalignment,       XmALIGNMENT_BEGINNING,
        XmNtopAttachment,   XmATTACH_FORM,
        XmNleftAttachment,  XmATTACH_POSITION,
		XmNleftPosition,	45,
		XmNrightAttachment, XmATTACH_FORM,
        NULL
    );

    /* Create textfield for extension */
    viewprefs.w_extension = XtVaCreateManagedWidget(
        "extension",
        xmTextFieldWidgetClass,
        viewprefs.w_form,
        XmNtopAttachment,   XmATTACH_WIDGET,
        XmNtopWidget,       viewprefs.w_extensionLabel,
        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget,		viewprefs.w_extensionLabel,
        XmNrightAttachment, XmATTACH_FORM,
        NULL
    );
    XtAddCallback(viewprefs.w_extension, XmNvalueChangedCallback,
        cb_extension_modified, (XtPointer)NULL);

    /* Create label for command line */
    viewprefs.w_commandLineLabel = XtVaCreateManagedWidget(
        "commandLineLabel",
        xmLabelWidgetClass,
        viewprefs.w_form,
        XmNalignment,       XmALIGNMENT_BEGINNING,
        XmNtopAttachment,   XmATTACH_WIDGET,
		XmNtopWidget,		viewprefs.w_extension,
		XmNtopOffset,		10,
        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget,		viewprefs.w_extension,
		XmNrightAttachment, XmATTACH_FORM,
        NULL
    );

    /* Create textfield for command line */
    viewprefs.w_commandLine = XtVaCreateManagedWidget(
        "commandLine",
        xmTextFieldWidgetClass,
        viewprefs.w_form,
        XmNtopAttachment,   XmATTACH_WIDGET,
        XmNtopWidget,       viewprefs.w_commandLineLabel,
        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget,		viewprefs.w_commandLineLabel,
        XmNrightAttachment, XmATTACH_FORM,
        NULL
    );

    /* Create frame for edit buttons */
    viewprefs.w_editFrame = XtVaCreateManagedWidget(
        "editFrame",
        xmFrameWidgetClass,
        viewprefs.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			viewprefs.w_commandLine,
		XmNtopOffset,			10,
        XmNleftAttachment,      XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget,			viewprefs.w_commandLine,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );

    /* Create form for edit buttons */
    viewprefs.w_editForm = XtVaCreateManagedWidget(
        "editForm",
        xmFormWidgetClass,
        viewprefs.w_editFrame,
		XmNmarginHeight,		2,
		XmNmarginWidth,			2,
        NULL
    );

    /* Create "Add" pushbutton */
    viewprefs.w_addButton = XtVaCreateManagedWidget(
        "addButton",
        xmPushButtonWidgetClass,
        viewprefs.w_editForm,
		XmNtopAttachment,		XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       50,
        NULL
    );
    XtAddCallback(viewprefs.w_addButton, XmNactivateCallback, cb_viewprefs_add,
		(XtPointer)NULL);

    /* Create "Replace" pushbutton */
    viewprefs.w_replaceButton = XtVaCreateManagedWidget(
        "replaceButton",
        xmPushButtonWidgetClass,
        viewprefs.w_editForm,
		XmNtopAttachment,		XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_POSITION,
		XmNleftPosition,		50,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );
    XtAddCallback(viewprefs.w_replaceButton, XmNactivateCallback,
        cb_viewprefs_replace, (XtPointer)NULL);

    /* Create "Delete" pushbutton */
    viewprefs.w_deleteButton = XtVaCreateManagedWidget(
        "deleteButton",
        xmPushButtonWidgetClass,
        viewprefs.w_editForm,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			viewprefs.w_addButton,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_POSITION,
		XmNleftPosition,		25,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       75,
        NULL
    );
    XtAddCallback(viewprefs.w_deleteButton, XmNactivateCallback,
        cb_viewprefs_delete, (XtPointer)NULL);

    /* Create label for scrolled list of extensions */
    viewprefs.w_listLabel = XtVaCreateManagedWidget(
        "listLabel",
        xmLabelWidgetClass,
        viewprefs.w_form,
        XmNalignment,       XmALIGNMENT_BEGINNING,
        XmNtopAttachment,   XmATTACH_FORM,
        XmNleftAttachment,  XmATTACH_FORM,
        NULL
    );

    /* Create scrolled list of quoted expressions */
    i = 0;
    XtSetArg(args[i], XmNselectionPolicy, XmSINGLE_SELECT); i++;
    XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
    viewprefs.w_list = XmCreateScrolledList(viewprefs.w_form, "list", args, i);
    XtVaSetValues(
        XtParent(viewprefs.w_list),
        XmNtopAttachment,   	XmATTACH_WIDGET,
        XmNtopWidget,       	viewprefs.w_listLabel,
        XmNleftAttachment,  	XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget,			viewprefs.w_listLabel,
        XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition,		40,
		XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget,		viewprefs.w_editFrame,
        NULL
    );
    XtManageChild(viewprefs.w_list);
    fix_list_color(viewprefs.w_list);

    /* Add single-selection callback */
    XtAddCallback(viewprefs.w_list, XmNsingleSelectionCallback,
        cb_viewprefs_single_selection, (XtPointer)NULL);

    /* Create frame for message */
    viewprefs.w_messageFrame = XtVaCreateManagedWidget(
        "messageFrame",
        xmFrameWidgetClass,
        viewprefs.w_form,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			viewprefs.w_editFrame,
		XmNtopOffset,			15,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );

    /* Create rowcolumn to hold warning message */
    viewprefs.w_messageRowcolumn = XtVaCreateManagedWidget(
        "messageRowcolumn",
        xmRowColumnWidgetClass,
        viewprefs.w_messageFrame,
        NULL
    );

    /* Create label for first line of message */
    viewprefs.w_message1 = XtVaCreateManagedWidget(
        "message1",
        xmLabelWidgetClass,
        viewprefs.w_messageRowcolumn,
        NULL
    );

    /* Create label for second line of message */
    viewprefs.w_message2 = XtVaCreateManagedWidget(
        "message2",
        xmLabelWidgetClass,
        viewprefs.w_messageRowcolumn,
        NULL
    );

    /* Make background color of textfield widget same as scrollbar trough */
    set_bg_to_sb_trough_color(viewprefs.w_extension,
        vertical_sb(XtParent(viewprefs.w_list)));

    /* Make background color of textfield widget same as scrollbar trough */
    set_bg_to_sb_trough_color(viewprefs.w_commandLine,
        vertical_sb(XtParent(viewprefs.w_list)));

	XtManageChild(viewprefs.w_form);
}


/*
 * cb_viewprefs_ok - Callback that applies current viewer preferences and
 *                   then closes View Preferences dialog.
 */
void
cb_viewprefs_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	Widget w;

	did_user_forget();
	apply_viewer_prefs();
	release_array_list(temp_viewer_mappings);
	XtUnmanageChild(viewprefs.w_dialog);

	/* Re-sensitize "Viewer Preferences" menu item */
	XtVaGetValues(viewprefs.w_dialog, XmNuserData, &w, NULL);
	XtSetSensitive(w, True);
}


/*
 * cb_viewprefs_apply - Callback that applies current viewer preferences.
 */
void
cb_viewprefs_apply(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	did_user_forget();
	apply_viewer_prefs();
}


/*
 * cb_viewprefs_cancel - Callback that restores old viewer preferences 
 *                       and then the closes Viewer Preferences dialog.
 */
void
cb_viewprefs_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	Widget w;

	release_array_list(temp_viewer_mappings);
	XtUnmanageChild(viewprefs.w_dialog);

	/* Re-sensitize "Viewer Preferences" menu item */
	XtVaGetValues(viewprefs.w_dialog, XmNuserData, &w, NULL);
	XtSetSensitive(w, True);
}


/*
 * cb_viewprefs_help - Callback that displays help info for the Viewer
 *                     Preferences dialog.
 */
void
cb_viewprefs_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(viewprefs.w_dialog, False, "Viewer Preferences",
		viewprefs_help);
}


/*
 * add_to_list - Add extension/viewer pair to list.  If an entry in the
 *               list has the same extension, it is deleted.
 */
add_to_list()
{
	char *command_line;
	char *extension;
	char *ext_command;
	int i;
	XmString string;

	/* Get extension */
	extension = get_extension();
	if (strlen(extension) == 0)
		fatal_error("Trouble in cb_viewprefs_add()");

	/* Get command line */
	command_line = get_command_line();

	/* Form viewer/command line pair */
	ext_command = XtMalloc(strlen(extension)+strlen(command_line)+2);
    strcpy(ext_command, extension);
    if (strlen(command_line)) {
        strcat(ext_command, " ");
        strcat(ext_command, command_line);
    }

	/* Add to temporary mappings (but first check for existing extension) */
    for (i=0; i<temp_viewer_mappings->nentries; i++)
        if (!compare_first_token(ext_command,
				temp_viewer_mappings->entries[i]))  {
            delete_from_array_list(&temp_viewer_mappings, i);
            break;
        }
    add_to_array_list(&temp_viewer_mappings, ext_command);
    XtFree(ext_command);

	/* Sort and redisplay entries */
	quicksort(temp_viewer_mappings->entries, temp_viewer_mappings->nentries,
		compare_first_token);
	display_viewprefs(temp_viewer_mappings);

	/* Select newly added item */
	string = XmStringCreateSimple(extension);
	XmListSelectItem(viewprefs.w_list, string, True);
	XmStringFree(string);

    XtFree(extension);
    XtFree(command_line);
}


/*
 * cb_viewprefs_add - Callback that adds new extension/viewer pair to list.
 *                    If an entry in the list has the same extension, it
 *                    is deleted.
 */
void
cb_viewprefs_add(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	add_to_list();
}


/*
 * cb_viewprefs_replace - Callback that adds new extension/viewer pair to list.
 */
void
cb_viewprefs_replace(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	add_to_list();
}


/*
 * cb_viewprefs_delete - Callback that deletes new extension/viewer pair
 *                       from list.
 */
void
cb_viewprefs_delete(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int *position_list;
	int position_count;
	int retval;
	XmStringTable selected_items;
	char *extension;

	/* Get position of selected item */
	retval = XmListGetSelectedPos(viewprefs.w_list, &position_list,
		&position_count);

	/* Do not let "OTHERS" be deleted */
	XtVaGetValues(viewprefs.w_list, XmNselectedItems, &selected_items, NULL);
	extension = cstring_to_text(selected_items[0]);
	if (!strcmp(extension, "OTHERS")) {
		warning_error("Can't delete \"OTHERS\" - Use \"Add\" to replace.");
		XtFree(extension);
		return;
	}

	/* Sanity check */
	if (!retval || (position_count != 1))
		fatal_error("Trouble in cb_viewprefs_delete()");

	/* Delete corresponding item from temporary viewer/command line list */
	delete_from_array_list(&temp_viewer_mappings, position_list[0]-1);

	/* Update the extensions list */
	display_viewprefs(temp_viewer_mappings);

	/* Select first item */
	XmListSelectPos(viewprefs.w_list, 1, True);
}

/*
 * apply_viewer_prefs - Set current viewer preferences to displayed values.
 */
apply_viewer_prefs()
{
	need_to_save_viewprefs = True;

	/* Replace current mappings with temporary mappings */
	release_array_list(viewer_mappings);
	viewer_mappings = duplicate_array_list(temp_viewer_mappings);
}


/*
 * construct_viewprefs_filename - Form viewer preferences file name by
 *                                concatenating .xftpview to $HOME.
 */
construct_viewprefs_filename()
{
    char *homedir = getenv("HOME");

    viewprefs_file = XtMalloc(strlen(homedir)+strlen(VIEWFILE)+2);
    strcpy(viewprefs_file, homedir);
    strcat(viewprefs_file, "/");
    strcat(viewprefs_file, VIEWFILE);
}


/*
 * read_viewprefs_from_file - Initializes current viewer preferences with
 *                            values from .xftpview.
 */
read_viewprefs_from_file()
{
	static FILE *fp;
	char viewprefs_line[MAXVPLINE+1];
	char *q;
	char *extension;
	char *command_line;
	char *ext_command;
	int i;
	int j;
	int found_others = False;

	/* Create empty list of mappings */
	viewer_mappings = create_null_array_list();

    /* Try to open viewer preferences file.  If failure, use defaults */
    if ((fp = fopen(viewprefs_file, "r")) == NULL) {
		for (i=0; i<ndefault_mappings; i++) {
			for (j=0; j<viewer_mappings->nentries; j++)
				if (!compare_first_token(default_mappings[i],
						viewer_mappings->entries[j]))  {
					delete_from_array_list(&viewer_mappings, j);
					break;
				}
			add_to_array_list(&viewer_mappings, default_mappings[i]);
		}
		quicksort(viewer_mappings->entries, viewer_mappings->nentries,
			compare_first_token);
		return;
	}

	/* Process viewer preferences from file */
	while (fgets(viewprefs_line, MAXVPLINE+1, fp) != NULL) {
		if (viewprefs_line[strlen(viewprefs_line)-1] != '\n')
			continue;
		if ((q=strtok(viewprefs_line, " \t")) == NULL)
			continue;
		if (strcmp(q, EV_IDENTIFIER))
			continue;
		if ((q=strtok(NULL, " \t\n")) == NULL)
			continue;
		if (*q != '.' && strcmp(q, "OTHERS"))
			continue;
		extension = XtNewString(q);
		if ((q = strtok(NULL, "\n")) == NULL)
			command_line = XtNewString("");
		else {
			while (isspace(*q))
				q++;
			for (i=strlen(q)-1; i>=0; i--)
				if (isspace(q[i]))
					q[i] = '\0';
				else
					break;
			command_line = XtNewString(q);
		}
		if (!strcmp(extension, "OTHERS"))
			found_others = True;
		ext_command = XtMalloc(strlen(extension)+strlen(command_line)+2);
		strcpy(ext_command, extension);
		if (strlen(command_line)) {
			strcat(ext_command, " ");
			strcat(ext_command, command_line);
		}
		XtFree(extension);
		XtFree(command_line);
		for (j=0; j<viewer_mappings->nentries; j++)
			if (!compare_first_token(ext_command,
					viewer_mappings->entries[j]))  {
				delete_from_array_list(&viewer_mappings, j);
				break;
			}
		add_to_array_list(&viewer_mappings, ext_command);
		XtFree(ext_command);
	}

	/* Add viewer for unspecified extensions if not found in file */
	if (!found_others)
		add_to_array_list(&viewer_mappings, "OTHERS");

	/* Sort array of mappings */
	quicksort(viewer_mappings->entries, viewer_mappings->nentries,
		compare_first_token);
}


/*
 * compare_first_token - Compare two "string1" and "string2" by comparing
 *                       their first whitespace-delimited tokens.  Returns
 *                       -1 if "string1" is less than "string2", 0 if
 *                       "string1" equals "string2", and 1 if  "string1"
 *                       is greater than "string2".  Each string is required
 *                       to have at least one non-null token.
 */
compare_first_token(string1, string2)
char *string1;
char *string2;
{
	char *token1;
	char *token2;
	char *temp_string1 = XtNewString(string1);
	char *temp_string2 = XtNewString(string2);
	int retval;

	if ((token1 = strtok(temp_string1, " \t\n")) == NULL)
		fatal_error("Trouble in viewer_mapping_compare()");

	if ((token2 = strtok(temp_string2, " \t\n")) == NULL)
		fatal_error("Trouble in viewer_mapping_compare()");

	retval = strcmp(token1, token2);
	XtFree(temp_string1);
	XtFree(temp_string2);

	return retval;
}


/*
 * save_viewprefs - Write current viewer preferences to .xftpview.
 *                  Returns -1 if successful, else -1.
 */
save_viewprefs()
{
	FILE *fp;
	int i;

	if ((fp = fopen(viewprefs_file, "w")) == NULL)
		return -1;

	for (i=0; i<viewer_mappings->nentries; i++)
		fprintf(fp, "%s %s\n", EV_IDENTIFIER, viewer_mappings->entries[i]);

	fclose(fp);
	need_to_save_viewprefs = False;
	return 0;
}


/*
 * display_viewprefs - Update the Viewer Preferences dialog with the
 *                     specified extension-to-viewer mappings "mappings".
 */
display_viewprefs(mappings)
struct sl_struct *mappings;
{
	char *temp_buf;
	char *extension;
	XmString string;
	int i;

	/* Update extensions list */
	reset_list(viewprefs.w_list);
	for (i=0; i<mappings->nentries; i++) {
		temp_buf = XtNewString(mappings->entries[i]);
		extension = strtok(temp_buf, " ");
		string = XmStringCreateSimple(extension);
		XtFree(temp_buf);
		XmListAddItem(viewprefs.w_list, string, 0);
		XmStringFree(string);
	}
}


/*
 * cb_viewprefs_single_selection - This callback is invoked when an item
 *                                 in the extensions list is selected.
 */
void
cb_viewprefs_single_selection(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int nselected_items;
	char *ext_command;
	char *ptr;

    /* Don't let user deselect item */
    XtVaGetValues(viewprefs.w_list, XmNselectedItemCount, &nselected_items,
		NULL);
    if (nselected_items == 0)
        XmListSelectPos(viewprefs.w_list, cbs->item_position, False);

	/* Update text fields */
	ext_command = XtNewString(
		temp_viewer_mappings->entries[cbs->item_position-1]);
	if ((ptr = strchr(ext_command, ' '))) {
		*ptr++ = '\0';
		XmTextFieldSetString(viewprefs.w_commandLine, ptr);
	} else
		XmTextFieldSetString(viewprefs.w_commandLine, "");
	XmTextFieldSetString(viewprefs.w_extension, ext_command);
	XtFree(ext_command);
}


/*
 * get_extension - Returns the current value of the extension field with
 *                 white space removed.  If a leading period is absent,
 *                 one is prepended (except for the case of "OTHERS").
 *                 Caller should use XtFree() to release returned string.
 */
char *
get_extension()
{
	char *extension;
	char *text;
	char *temp;

	/* Process extension */
	text = XmTextFieldGetString(viewprefs.w_extension);
	if ((temp = strtok(text, " \t")) == NULL) {
		XtFree(text);
		return XtNewString("");
	}
	if (temp[0] == '.' || !strcmp(temp, "OTHERS"))
		extension = XtNewString(temp);
	else {
		extension = XtMalloc(strlen(temp)+2);
		strcpy(extension, ".");
		strcat(extension, temp);
	}
	XtFree(text);

	return extension;
}


/*
 * get_command_line - Returns the current value of the command line field 
 *                    with leading and trailing white space removed.  Caller
 *                    should use XtFree() to release returned string.
 */
char *
get_command_line()
{
	char *command_line;
	char *text;
	int i;
	int len;

	/* Process command line */
	text = XmTextFieldGetString(viewprefs.w_commandLine);
	for (i=strlen(text)-1; i>=0; i--)
		if (isspace(text[i]))
			text[i] = '\0';
		else
			break;
	len = strlen(text);
	for (i=0; i<len; i++)
		if (!isspace(text[i]))
			break;
	if (i == len)
		command_line = XtNewString("");
	else
		command_line = XtNewString(&text[i]);
	XtFree(text);

	return command_line;
}


/*
 * cb_extension_modified - Callback that is invoked when the value of the
 *                         extension field has changed.  The purpose of
 *                         this callback is to make the appropriate edit
 *                         buttons selectable.
 */
void
cb_extension_modified(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	char *extension;
	int i;

	extension = get_extension();

	/* Adjust sensitivity of edit buttons */
	if (strlen(extension)) {
	    for (i=0; i<temp_viewer_mappings->nentries; i++)
   	    	if (!compare_first_token(extension,
					temp_viewer_mappings->entries[i]))  {
   	         	break;
   	    	}
		if (i == temp_viewer_mappings->nentries) {
			XtSetSensitive(viewprefs.w_addButton, True);
			XtSetSensitive(viewprefs.w_replaceButton, False);
		} else {
			XtSetSensitive(viewprefs.w_addButton, False);
			XtSetSensitive(viewprefs.w_replaceButton, True);
		}
	} else {
		XtSetSensitive(viewprefs.w_addButton, False);
		XtSetSensitive(viewprefs.w_replaceButton, False);
	}
	XtFree(extension);
}


/*
 * did_user_forget - Make sure that user did not intend to press "Add"
 *                   or "Replace" button.
 */
did_user_forget()
{
	char *extension = get_extension();
	char question[MAXQUESTION];
	int i;
	char *ptr;
	char *command_line;
	char *cmd_line;

	/* No problem if extension is null */
	if (strlen(extension) == 0) {
		XtFree(extension);
		return;
	}

	/* If extension is already in list, did user intend to replace? */
    for (i=0; i<temp_viewer_mappings->nentries; i++)
        if (!compare_first_token(extension, temp_viewer_mappings->entries[i])) {
			command_line = get_command_line();
			if ((ptr = strchr(temp_viewer_mappings->entries[i], ' '))) {
				ptr++;
				cmd_line = XtNewString(ptr);
			} else
				cmd_line = XtNewString("");
			if (strcmp(command_line, cmd_line)) {
				strcpy(question, "You edited the command line associated ");
				strcat(question, "with an\nexisting extension, but did not ");
				strcat(question, "press the \"Replace\"\nbutton.\n\n");
				strcat(question, "Do you wish to replace the command line ");
				strcat(question, "associated\nwith \"");
				strcat(question, extension);
				strcat(question, "\"?");
				if (question_dialog(question))
					add_to_list();
			}
			XtFree(command_line);
			XtFree(cmd_line);
			XtFree(extension);
			return;
        }

	/* Extension is not in list, did user intend to add? */
	strcpy(question, "You specified a new extension, but did not add it ");
	strcat(question, "to the list.\n\nDo you wish to add \"");
	strcat(question, extension);
	strcat(question, "\" to the list?");
	if (question_dialog(question))
		add_to_list();
	XtFree(extension);
}

