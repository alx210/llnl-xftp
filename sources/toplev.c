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

#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/ToggleBG.h>
#include <Xm/Text.h>
#include <Xm/CascadeBG.h>
#include "pulldown.h"
#include "pixmaps.h"
#include "xftp.h"
#include "xfer.h"

Widget	w_main;
Widget	w_mainMenuBar;
Widget	w_pane;
Widget	w_topForm;
Widget	w_bottomForm;
Widget	w_centerControls;
Widget	w_xferOpsLabel;
Widget	w_xferOpsFrame;
Widget	w_xferOps;
Widget	w_copyButton;
Widget	w_rCopyButton;
Widget  w_viewButton;
Widget	w_xferModeLabel;
Widget	w_xferMode;
Widget	w_ascii;
Widget	w_binary;
Widget	w_xferModeFrame;
Widget  w_logo;
Widget	w_host[NUMHOSTS];
Widget	w_hostName[NUMHOSTS];
Widget	w_hostMenuBar[NUMHOSTS];
Widget	w_dirNameForm[NUMHOSTS];
Widget	w_dirList[NUMHOSTS];
Widget	w_resultFlagFrame;
Widget	w_resultFlag;
Widget	w_log;
Widget	w_connectToLocalItem[NUMHOSTS];
Widget	w_connectToRemoteItem[NUMHOSTS];
Widget	w_connectToAnonItem[NUMHOSTS];
Widget	w_disconnectItem[NUMHOSTS];
Widget	w_displayDirPathItem[NUMHOSTS];
Widget	w_displayDirLongItem[NUMHOSTS];
Widget	w_displayDirColumnsItem[NUMHOSTS];
Widget	w_selectAllEntriesItem[NUMHOSTS];
Widget	w_deselectAllEntriesItem[NUMHOSTS];
Widget	w_useWildCardItem[NUMHOSTS];
Widget  w_gotoPrevDirMenu[NUMHOSTS];
Widget	w_changeDirItem[NUMHOSTS];
Widget	w_makeDirItem[NUMHOSTS];
Widget	w_deleteSelectedEntriesItem[NUMHOSTS];
Widget  w_rDeleteSelectedEntriesItem[NUMHOSTS];
Widget	w_moveSelectedEntriesItem[NUMHOSTS];
Widget	w_renameSelectedEntriesItem[NUMHOSTS];
Widget  w_sendQuotedCommandItem[NUMHOSTS];
Widget	w_connectMenu[NUMHOSTS];
Widget  w_dirMenu[NUMHOSTS];
Widget  w_selectMenu[NUMHOSTS];
Widget  w_opsMenu[NUMHOSTS];
Widget	w_dirNameMenuItem[NUMHOSTS][MAXLINKS];
Widget	w_dirNameMenu[NUMHOSTS];
Widget  w_dirName[NUMHOSTS];

static Pixmap logo_pixmap;

Widget BuildPulldownMenu(Widget, char*, KeySym, MenuItem*, XtPointer);

extern Widget w_toplev;
extern int initial_xfer_mode;

void cb_help_hostname(Widget, XtPointer, XtPointer);
void cb_help_file_menu(Widget, XtPointer, XtPointer);
void cb_help_log_menu(Widget, XtPointer, XtPointer);
void cb_help_options_menu(Widget, XtPointer, XtPointer);
void cb_help_connect_menu(Widget, XtPointer, XtPointer);
void cb_help_dir_menu(Widget, XtPointer, XtPointer);
void cb_help_select_menu(Widget, XtPointer, XtPointer);
void cb_help_ops_menu(Widget, XtPointer, XtPointer);
void cb_help_dirname(Widget, XtPointer, XtPointer);
void cb_help_dirlist(Widget, XtPointer, XtPointer);
void cb_help_copy(Widget, XtPointer, XtPointer);
void cb_help_rcopy(Widget, XtPointer, XtPointer);
void cb_help_view(Widget, XtPointer, XtPointer);
void cb_help_xfer_mode(Widget, XtPointer, XtPointer);
void cb_help_result_flag(Widget, XtPointer, XtPointer);
void cb_help_log(Widget, XtPointer, XtPointer);
void cb_save_log(Widget, XtPointer, XtPointer);
void cb_save_prefs(Widget, XtPointer, XtPointer);
void cb_quit(Widget, XtPointer, XtPointer);
void cb_clear_log(Widget, XtPointer, XtPointer);
void cb_preferences(Widget, XtPointer, XtPointer);
void cb_viewer_preferences(Widget, XtPointer, XtPointer);
void cb_xftp_overview(Widget, XtPointer, XtPointer);
void cb_getting_started(Widget, XtPointer, XtPointer);
void cb_about_xftp(Widget, XtPointer, XtPointer);
void cb_using_help(Widget, XtPointer, XtPointer);
void cb_contextual_help(Widget, XtPointer, XtPointer);
void cb_connect_to_local(Widget, XtPointer, XtPointer);
void cb_connect_to_remote(Widget, XtPointer, XtPointer);
void cb_connect_to_anonymous(Widget, XtPointer, XtPointer);
void cb_disconnect(Widget, XtPointer, XtPointer);
void cb_goto_prev_dir(Widget, XtPointer, XtPointer);
void cb_change_dir(Widget, XtPointer, XtPointer);
void cb_display_dirpath(Widget, XtPointer, XtPointer);
void cb_display_dir_long(Widget, XtPointer, XtPointer);
void cb_display_dirtable(Widget, XtPointer, XtPointer);
void cb_select_all_entries(Widget, XtPointer, XtPointer);
void cb_deselect_all_entries(Widget, XtPointer, XtPointer);
void cb_use_wildcard(Widget, XtPointer, XtPointer);
void cb_make_dir(Widget, XtPointer, XtPointer);
void cb_delete(Widget, XtPointer, XtPointer);
void cb_rdelete(Widget, XtPointer, XtPointer);
void cb_move_selected_entries(Widget, XtPointer, XtPointer);
void cb_rename_selected_entries(Widget, XtPointer, XtPointer);
void cb_send_quoted_command(Widget, XtPointer, XtPointer);
void cb_multiple_selection(Widget, XtPointer, XtPointer);
void cb_double_click(Widget, XtPointer, XtPointer);
void cb_copy(Widget, XtPointer, XtPointer);
void cb_view_selected_file(Widget, XtPointer, XtPointer);
void cb_set_dir(Widget, XtPointer, XtPointer);
void cb_dir_menu(Widget, XtPointer, XtPointer);
void cb_availability(Widget, XtPointer, XtPointer);
Widget vertical_sb();
Pixmap create_mainlogo_pixmap();


MenuItem file_menu[] = {
	{ "About LLNL XFTP", &xmPushButtonGadgetClass,
	  'A', NULL, NULL, cb_about_xftp, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Quit", &xmPushButtonGadgetClass,
	  'Q', NULL, NULL, cb_quit, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ NULL }
};

MenuItem log_menu[] = {
	{ "Clear Log", &xmPushButtonGadgetClass,
	  'C', NULL, NULL, cb_clear_log, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Save Log...", &xmPushButtonGadgetClass,
	  'S', NULL, NULL, cb_save_log, (MenuItem *)NULL,
	  (Widget *)NULL},
	{ NULL }
};

MenuItem options_menu[] = {
	{ "Preferences...", &xmPushButtonGadgetClass,
	  'P', NULL, NULL, cb_preferences, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Viewer Preferences...", &xmPushButtonGadgetClass,
	  'V', NULL, NULL, cb_viewer_preferences, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Save Preferences", &xmPushButtonGadgetClass,
	  'S', NULL, NULL, cb_save_prefs, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ NULL }
};

MenuItem help_menu[] = {
	{ "Using Help", &xmPushButtonGadgetClass,
	  'U', NULL, NULL, cb_using_help, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "LLNL XFTP Overview", &xmPushButtonGadgetClass,
	  'O', NULL, NULL, cb_xftp_overview, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Getting Started", &xmPushButtonGadgetClass,
	  'G', NULL, NULL, cb_getting_started, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Availability", &xmPushButtonGadgetClass,
	  'A', NULL, NULL, cb_availability, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Contextual Help", &xmPushButtonGadgetClass,
	  'C', NULL, NULL, cb_contextual_help, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ NULL }
};

MenuItem connect_menu[NUMHOSTS][6] = {
{
    { "Connect to Local", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_connect_to_local, (MenuItem *)NULL,
	  &w_connectToLocalItem[LEFT] },
    { "Connect to Remote...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_connect_to_remote, (MenuItem *)NULL,
	  &w_connectToRemoteItem[LEFT] },
    { "Connect to Anonymous...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_connect_to_anonymous, (MenuItem *)NULL,
	  &w_connectToAnonItem[LEFT] },
    { "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
    { "Disconnect", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_disconnect, (MenuItem *)NULL,
	  &w_disconnectItem[LEFT] },
    { NULL }
},
{
    { "Connect to Local", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_connect_to_local, (MenuItem *)NULL,
	  &w_connectToLocalItem[RIGHT] },
    { "Connect to Remote...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_connect_to_remote, (MenuItem *)NULL,
	  &w_connectToRemoteItem[RIGHT]},
    { "Connect to Anonymous...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_connect_to_anonymous, (MenuItem *)NULL,
	  &w_connectToAnonItem[RIGHT]},
    { "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
    { "Disconnect", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_disconnect, (MenuItem *)NULL,
	  &w_disconnectItem[RIGHT]},
    { NULL }
}
};

MenuItem goto_menu[1] = {
	{ NULL }
};

MenuItem dir_menu[NUMHOSTS][8] = {
{
	{ "Go to Previous Directory", &xmCascadeButtonGadgetClass,
	  '\0', NULL, NULL, NULL, goto_menu,
	  &w_gotoPrevDirMenu[LEFT] },
	{ "Change Directory...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_change_dir, (MenuItem *)NULL,
	  &w_changeDirItem[LEFT] },
	{ "Make Directory...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_make_dir, (MenuItem *)NULL,
	  &w_makeDirItem[LEFT] },
	{ "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Display Directory Path...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_display_dirpath, (MenuItem *)NULL,
	  &w_displayDirPathItem[LEFT] },
	{ "Display Directory (Long)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL,cb_display_dir_long, (MenuItem *)NULL,
	  &w_displayDirLongItem[LEFT] },
	{ "Display Directory (Table)...",&xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_display_dirtable, (MenuItem *)NULL,
	  &w_displayDirColumnsItem[LEFT]},
	{ NULL }
},
{
	{ "Go to Previous Directory", &xmCascadeButtonGadgetClass,
	  '\0', NULL, NULL, NULL, goto_menu,
	  &w_gotoPrevDirMenu[RIGHT] },
	{ "Change Directory...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_change_dir, (MenuItem *)NULL,
	  &w_changeDirItem[RIGHT] },
	{ "Make Directory...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_make_dir, (MenuItem *)NULL,
	  &w_makeDirItem[RIGHT] },
	{ "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Display Directory Path...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL,cb_display_dirpath, (MenuItem *)NULL,
	  &w_displayDirPathItem[RIGHT] },
	{ "Display Directory (Long)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_display_dir_long, (MenuItem *)NULL,
	  &w_displayDirLongItem[RIGHT] },
	{ "Display Directory (Table)...",&xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_display_dirtable, (MenuItem *)NULL,
	  &w_displayDirColumnsItem[RIGHT]},
	{ NULL }
}
};

MenuItem select_menu[NUMHOSTS][4] = {
{
	{ "Select All Entries", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_select_all_entries, (MenuItem *)NULL,
	  &w_selectAllEntriesItem[LEFT] },
	{ "Deselect All Entries", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_deselect_all_entries, (MenuItem *)NULL,
	  &w_deselectAllEntriesItem[LEFT]},
	{ "Use Wildcard...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_use_wildcard, (MenuItem *)NULL,
	  &w_useWildCardItem[LEFT] },
	{ NULL }
},
{
	{ "Select All Entries", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_select_all_entries, (MenuItem *)NULL,
	  &w_selectAllEntriesItem[RIGHT] },
	{ "Deselect All Entries", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_deselect_all_entries, (MenuItem *)NULL,
	  &w_deselectAllEntriesItem[RIGHT]},
	{ "Use Wildcard...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_use_wildcard, (MenuItem *)NULL,
	  &w_useWildCardItem[RIGHT] },
	{ NULL }
}
};

MenuItem ops_menu[NUMHOSTS][7] = {
{
	{ "Delete Selected Entry(s)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_delete, (MenuItem *)NULL,
	  &w_deleteSelectedEntriesItem[LEFT] },
	{ "Move Selected Entry(s)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_move_selected_entries, (MenuItem *)NULL,
	  &w_moveSelectedEntriesItem[LEFT] },
	{ "Rename Selected Entry(s)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_rename_selected_entries,(MenuItem *)NULL,
	  &w_renameSelectedEntriesItem[LEFT]},
	{ "Send Quoted Command...", &xmPushButtonGadgetClass,
	  '\0' , NULL, NULL, cb_send_quoted_command,(MenuItem *)NULL,
	  &w_sendQuotedCommandItem[LEFT]},
	{ "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Recursively Delete Selected Entry(s)...",&xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_rdelete, (MenuItem *)NULL,
	  &w_rDeleteSelectedEntriesItem[LEFT] },
	{ NULL }
},
{
	{ "Delete Selected Entry(s)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_delete, (MenuItem *)NULL,
	  &w_deleteSelectedEntriesItem[RIGHT] },
	{ "Move Selected Entry(s)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_move_selected_entries, (MenuItem *)NULL,
	  &w_moveSelectedEntriesItem[RIGHT] },
	{ "Rename Selected Entry(s)...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_rename_selected_entries,(MenuItem *)NULL,
	  &w_renameSelectedEntriesItem[RIGHT]},
	{ "Send Quoted Command...", &xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_send_quoted_command,(MenuItem *)NULL,
	  &w_sendQuotedCommandItem[RIGHT]},
	{ "", &xmSeparatorGadgetClass,
	  '\0', NULL, NULL, NULL, (MenuItem *)NULL,
	  (Widget *)NULL },
	{ "Recursively Delete Selected Entry(s)...",&xmPushButtonGadgetClass,
	  '\0', NULL, NULL, cb_rdelete, (MenuItem *)NULL,
	  &w_rDeleteSelectedEntriesItem[RIGHT] },
	{ NULL }
}
};


/*
 * create_main_window - Create the main window.
 */
create_main_window()
{
	Widget widget;
	Arg	args[7];
	int	i;
	unsigned long foreground;
	unsigned long background;

	/* Create main window for the application */
	w_main = XtVaCreateWidget("main", xmMainWindowWidgetClass, w_toplev, NULL);

	/* Create main menu */
	w_mainMenuBar = XmCreateMenuBar(w_main, "mainMenuBar", NULL, 0);
	widget = BuildPulldownMenu(w_mainMenuBar, "File", 'F', file_menu, NULL);
	XtAddCallback(widget, XmNhelpCallback, cb_help_file_menu,
		(XtPointer)NULL);
	widget = BuildPulldownMenu(w_mainMenuBar, "Log", 'L', log_menu, NULL);
	XtAddCallback(widget, XmNhelpCallback, cb_help_log_menu, (XtPointer)NULL);
	widget = BuildPulldownMenu(w_mainMenuBar, "Options", 'O',options_menu,NULL);
	XtAddCallback(widget, XmNhelpCallback, cb_help_options_menu,
		(XtPointer)NULL);
	widget = BuildPulldownMenu(w_mainMenuBar, "Help", 'H', help_menu, NULL);
	XtVaSetValues(w_mainMenuBar, XmNmenuHelpWidget, widget, NULL);
	XtManageChild(w_mainMenuBar);

	/* Create pane to divide working area into top and bottom */
	w_pane = XtVaCreateWidget("pane", xmPanedWindowWidgetClass, w_main, NULL);

	/* Create form for top pane */
	w_topForm = XtVaCreateWidget("topForm", xmFormWidgetClass, w_pane,
		XmNheight,		420,
		XmNmarginWidth,	5,
		NULL
	);

	/* Create left host controls */
	CreateHostGUI(LEFT, w_topForm);
	XtVaSetValues(w_host[LEFT],
 		XmNtopAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
 		XmNrightAttachment,		XmATTACH_POSITION,
		XmNrightPosition,		38,
		NULL
	);

	/* Create right host controls */
	CreateHostGUI(RIGHT, w_topForm);
	XtVaSetValues(w_host[RIGHT],
		XmNtopAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_POSITION,
		XmNleftPosition,		62,
		NULL
	);

	/* Create center controls form */
	w_centerControls = XtVaCreateWidget("centerControls", xmFormWidgetClass,
		w_topForm,
		XmNmarginWidth,			10,
		XmNtopAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_WIDGET,
		XmNleftWidget,          w_host[LEFT],
		XmNrightAttachment,		XmATTACH_WIDGET,
		XmNrightWidget,			w_host[RIGHT],
		NULL
	);

	/* Create transfer mode frame */
	w_xferModeFrame = XtVaCreateManagedWidget(
		"xferModeFrame",
		xmFrameWidgetClass,
		w_centerControls,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,		19,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);

	/* Create transfer mode radio box */
	i = 0;
	XtSetArg(args[i], XmNmarginHeight, 3); i++;
	XtSetArg(args[i], XmNspacing, 0); i++;
	w_xferMode = XmCreateRadioBox(w_xferModeFrame, "xferMode", args, i);
	XtAddCallback(w_xferMode, XmNhelpCallback, cb_help_xfer_mode,
		(XtPointer)NULL);

	/* Create "ASCII" toggle */
	w_ascii = XtVaCreateManagedWidget(
		"ascii",
		xmToggleButtonGadgetClass,
		w_xferMode,
		XmNspacing,			4,
		XmNmarginHeight,	0,
		NULL
	);
	XtAddCallback(w_ascii, XmNhelpCallback, cb_help_xfer_mode,
		(XtPointer)NULL);

	/* Create "Binary" toggle */
	w_binary = XtVaCreateManagedWidget(
		"binary",
		xmToggleButtonGadgetClass,
		w_xferMode,
		XmNspacing,			4,
		XmNmarginHeight,	0,
		NULL
	);
	XtAddCallback(w_binary, XmNhelpCallback, cb_help_xfer_mode,
		(XtPointer)NULL);

	XtManageChild(w_xferMode);

    /* Initialize file transfer mode */
    if (initial_xfer_mode == ASCII)
        XmToggleButtonSetState(w_ascii, True, True);
    else
        XmToggleButtonSetState(w_binary, True, True);

	/* Create transfer mode label */
    w_xferModeLabel = XtVaCreateManagedWidget(
		"xferModeLabel",
		xmLabelWidgetClass,
        w_centerControls,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,		w_xferModeFrame,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
        NULL
    );

	/* Create transfer ops frame */
	w_xferOpsFrame = XtVaCreateManagedWidget(
		"xferOpsFrame",
		xmFrameWidgetClass,
		w_centerControls,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,		w_xferModeLabel,
		XmNbottomOffset,		9,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);

	/* Create transfer ops rowcolumn */
	w_xferOps = XtVaCreateManagedWidget(
		"xferOps",
		xmRowColumnWidgetClass,
		w_xferOpsFrame,
		XmNspacing,             0,
		NULL
	);

	/* Create "Copy" button */
	w_copyButton = XtVaCreateManagedWidget(
		"copyButton",
		xmPushButtonWidgetClass,
		w_xferOps,
		NULL
	);
	XtAddCallback(w_copyButton, XmNactivateCallback, cb_copy, (XtPointer)COPY);
	XtAddCallback(w_copyButton, XmNhelpCallback, cb_help_copy, (XtPointer)NULL);

	/* Create "Rcopy" button */
	w_rCopyButton = XtVaCreateManagedWidget(
		"rCopyButton",
		xmPushButtonWidgetClass,
		w_xferOps,
		NULL
	);
	XtAddCallback(w_rCopyButton, XmNactivateCallback, cb_copy,(XtPointer)RCOPY);
	XtAddCallback(w_rCopyButton, XmNhelpCallback, cb_help_rcopy,
		(XtPointer)NULL);

	/* Create "View" button */
	w_viewButton = XtVaCreateManagedWidget(
		"viewButton",
		xmPushButtonWidgetClass,
		w_xferOps,
		NULL
	);
	XtAddCallback(w_viewButton, XmNactivateCallback, cb_view_selected_file,
		(XtPointer)VIEW);
	XtAddCallback(w_viewButton, XmNhelpCallback, cb_help_view, (XtPointer)NULL);

	/* Create transfer ops label */
    w_xferOpsLabel = XtVaCreateManagedWidget(
		"xferOpsLabel",
		xmLabelWidgetClass,
        w_centerControls,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,		w_xferOpsFrame,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
        NULL
    );

    /* Create logo */
    w_logo = XtVaCreateWidget(
        "logo",
        xmLabelWidgetClass,
        w_centerControls,
        XmNlabelType,      		XmPIXMAP,
        XmNbottomAttachment,    XmATTACH_WIDGET,
        XmNbottomWidget,        w_xferOpsLabel,
        XmNbottomOffset,        4,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );
    XtVaGetValues(w_logo,
        XmNforeground,  &foreground,
        XmNbackground,  &background,
        NULL
    );
	logo_pixmap = create_mainlogo_pixmap(foreground, background);
	XtVaSetValues(w_logo, XmNlabelPixmap, logo_pixmap, NULL);
	XtManageChild(w_logo);

	XtManageChild(w_centerControls);

	XtManageChild(w_topForm);

	/* Create form for bottom pane */
	w_bottomForm = XtVaCreateWidget("bottomForm", xmFormWidgetClass, w_pane,
		NULL);

	/* Create frame for result flag */
	w_resultFlagFrame = XtVaCreateManagedWidget(
		"resultFlagFrame",
		xmFrameWidgetClass,
		w_bottomForm,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		2,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		5,
		NULL
	);

	/* Create result flag */
	w_resultFlag = XtVaCreateManagedWidget(
		"resultFlag",
		xmLabelWidgetClass,
		w_resultFlagFrame,
		XmNlabelType,		XmPIXMAP,
		NULL
	);
	XtAddCallback(w_resultFlag, XmNhelpCallback, cb_help_result_flag,
		(XtPointer)NULL);

	/* Initialize result flag to "OK" */
	init_result_flag();

	/* Create diagnostics log */
	i = 0;
	XtSetArg(args[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	w_log = XmCreateScrolledText(w_bottomForm, "log", args, i);
	XtVaSetValues(XtParent(w_log),
		XmNtopAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_WIDGET,
		XmNleftWidget,			w_resultFlagFrame,
		XmNleftOffset,			7,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNrightOffset,			5,
		NULL
	);
	XtManageChild(w_log);
	XtAddCallback(w_log, XmNhelpCallback, cb_help_log, (XtPointer)NULL);
	fix_vertical_text_sb_color(w_log);
	fix_horizontal_text_sb_color(w_log);
	widget = vertical_sb(XtParent(w_log));
	set_bg_to_sb_trough_color(w_log, widget);
	XtAddCallback(widget, XmNhelpCallback, cb_help_log, (XtPointer)NULL);
	XtVaGetValues(XtParent(w_log), XmNhorizontalScrollBar, &widget, NULL);
	XtAddCallback(widget, XmNhelpCallback, cb_help_log, (XtPointer)NULL);

	XtManageChild(w_bottomForm);

	XtManageChild(w_pane);
	XtManageChild(w_main);
}


/*
 * CreateHostGUI - Create either the left or right (depending on "host")
 *                 part fo the main window.
 */
CreateHostGUI(host, parent)
int host;
Widget parent;
{
	static char *form_name[] = { "leftHost", "rightHost" };
	XmString label;
	Arg	args[4];
	int	i;
	Widget widget;
	Widget w_sb;
	Widget w_pulldown;
	
	/* Create form to hold host controls */
	w_host[host] = XtVaCreateWidget(form_name[host], xmFormWidgetClass, parent,
		NULL);

	/* Create host name label */
	label = XmStringCreateSimple("Not Connected");
	w_hostName[host] = XtVaCreateManagedWidget("hostName", xmLabelWidgetClass,
		w_host[host],
		XmNlabelString,		label,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNtopOffset,		14,
		NULL
	);
	XmStringFree(label);
	XtAddCallback(w_hostName[host], XmNhelpCallback, cb_help_hostname,
		(XtPointer)((long)host));

	/* Create host menus */
	i = 0;
	XtSetArg(args[i], XmNtopAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNtopOffset, 43); i++;
	w_hostMenuBar[host] = XmCreateMenuBar(w_host[host], "hostMenuBar", args, i);
	w_connectMenu[host] =  BuildPulldownMenu(w_hostMenuBar[host], "Connect",
		0, connect_menu[host], (XtPointer)((long)host));
	XtAddCallback(w_connectMenu[host], XmNhelpCallback, cb_help_connect_menu,
		(XtPointer)((long)host));
	w_dirMenu[host] =  BuildPulldownMenu(w_hostMenuBar[host], "Dir", 0,
		dir_menu[host], (XtPointer)((long)host));
	XtAddCallback(w_dirMenu[host], XmNhelpCallback, cb_help_dir_menu,
		(XtPointer)((long)host));
	XtVaGetValues(w_dirMenu[host], XmNsubMenuId, &w_pulldown, NULL);
	XtAddCallback(XtParent(w_pulldown), XmNpopupCallback, cb_dir_menu,
		(XtPointer)((long)host));
	initialize_goto_menu(host);
	w_selectMenu[host] =  BuildPulldownMenu(w_hostMenuBar[host], "Select",
		0, select_menu[host], (XtPointer)((long)host));
	XtAddCallback(w_selectMenu[host], XmNhelpCallback, cb_help_select_menu,
		(XtPointer)((long)host));
	w_opsMenu[host] =  BuildPulldownMenu(w_hostMenuBar[host], "Ops", 0,
		ops_menu[host], (XtPointer)((long)host));
	XtAddCallback(w_opsMenu[host], XmNhelpCallback, cb_help_ops_menu,
		(XtPointer)((long)host));
	XtManageChild(w_hostMenuBar[host]);

	/* Create form for directory name option menu (this kluge forces resize) */
    w_dirNameForm[host] = XtVaCreateManagedWidget(
        "dirNameForm",
        xmFormWidgetClass,
        w_host[host],
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		w_hostMenuBar[host],
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNtopOffset,		12,
		NULL
	);

	/* Create the directory name pulldown menu */
	w_dirNameMenu[host] = XmCreatePulldownMenu(w_dirNameForm[host],
		"dirNameMenu", NULL, 0);
	label = XmStringCreateSimple("Dummy");
	for (i=0; i<MAXLINKS; i++) {
		w_dirNameMenuItem[host][i] = XtVaCreateWidget(
			"",
			xmPushButtonGadgetClass,
			w_dirNameMenu[host],
			XmNlabelString, label,
			NULL
		);
		XtAddCallback(w_dirNameMenuItem[host][i], XmNactivateCallback,
			cb_set_dir, (XtPointer)((long)host));
	}
	XmStringFree(label);

    /* Create directory name option menu */
    i = 0;
    XtSetArg(args[i], XmNsubMenuId, w_dirNameMenu[host]); i++;
    w_dirName[host] = XmCreateOptionMenu(w_dirNameForm[host], "dirName",
        args, i);
    XtManageChild(w_dirName[host]);
	widget = XmOptionButtonGadget(w_dirName[host]);
	XtAddCallback(widget, XmNhelpCallback, cb_help_dirname, (XtPointer)((long)host));

	update_dirname_menu(host);

	/* Create directory list  */
	i = 0;
	XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;
	XtSetArg(args[i], XmNselectionPolicy, XmMULTIPLE_SELECT); i++;
	w_dirList[host] = XmCreateScrolledList(w_host[host], "dirList", args, i);
	XtVaSetValues(XtParent(w_dirList[host]),
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftOffset,			0,
		XmNrightOffset,			0,
		XmNtopAttachment,		XmATTACH_WIDGET,
		XmNtopWidget,			w_dirNameForm[host],
		NULL
	);
	XtManageChild(w_dirList[host]);
	fix_list_color(w_dirList[host]);

	/* Add help callbacks */
	XtAddCallback(w_dirList[host], XmNhelpCallback, cb_help_dirlist,
		(XtPointer)((long)host));
	XtVaGetValues(XtParent(w_dirList[host]), XmNverticalScrollBar, &w_sb, NULL);
	XtAddCallback(w_sb, XmNhelpCallback, cb_help_dirlist, (XtPointer)((long)host));
	XtVaGetValues(XtParent(w_dirList[host]), XmNhorizontalScrollBar,&w_sb,NULL);
	XtAddCallback(w_sb, XmNhelpCallback, cb_help_dirlist, (XtPointer)((long)host));

    /* Add multiple-selection callback to directory list*/
    XtAddCallback(w_dirList[host], XmNmultipleSelectionCallback,
        cb_multiple_selection, (XtPointer)((long)host));

    /* Add double-click callback to directory list */
    XtAddCallback(w_dirList[host], XmNdefaultActionCallback, cb_double_click,
        (XtPointer)((long)host));

	XtManageChild(w_host[host]);
}

