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
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include "xftp.h"
#include "prefs.h"
#include "str.h"

#define PROPFILE ".xftprc"

#define MAXPLINE    100

#define CHOICE 0
#define TEXT   1
#define NUMBER 2

char *anonymous_ftp_password;
char *initial_logfile_name;
int sort_long_lists_by_date;
int initial_xfer_mode;
int initial_wildcard_mode;
int max_cache[NUMCACHES];
int max_cache_hosts;
int inquire_on_copy;
int inquire_on_delete;
int inquire_on_move;
int inquire_on_quote;
int auto_local_login[NUMHOSTS];
int need_to_save_prefs = False;
int symbol_mode;
int add_to_cache_ok;
int sort_caches;
int beep_when_ops_done;
int max_ftp_retries;
int rdelete_enabled;
int diagnostics;
char *quote_placeholder;
int store_unique;
int enhance_colors;
int use_last_dir;
int preserve_vms_ver_nums;
int print_password_in_debug;
int enter_dir_upon_creation;
int maxt;
int overwrite_dirs;

extern char *help_add_to_cache[];
extern char *help_anonymous_ftp_password[];
extern char *help_beep_when_ops_done[];
extern char *help_diagnostics[];
extern char *help_directory_cache_size[];
extern char *help_enhance_colors[];
extern char *help_enter_dir_upon_creation[];
extern char *help_host_cache_size[];
extern char *help_initial_log_file_name[];
extern char *help_initial_transfer_mode[];
extern char *help_initial_wildcard_mode[];
extern char *help_inquire_on_copy[];
extern char *help_inquire_on_delete[];
extern char *help_inquire_on_move[];
extern char *help_inquire_on_quote[];
extern char *help_left_auto_local_login[];
extern char *help_max_ftp_retries[];
extern char *help_max_time_for_ftp_reply[];
extern char *help_overwrite_dir_on_copy[];
extern char *help_preserve_vms_ver_nums[];
extern char *help_print_password_in_debug[];
extern char *help_quote_cache_size[];
extern char *help_quote_placeholder[];
extern char *help_recursive_delete_enabled[];
extern char *help_right_auto_local_login[];
extern char *help_sort_caches[];
extern char *help_sort_long_lists_by_date[];
extern char *help_store_unique[];
extern char *help_symbols_on_dir_entries[];
extern char *help_user_cache_size[];
extern char *help_wildcard_cache_size[];
extern char *help_use_last_dir[];

static String prefsTranslations =
	"<Btn1Down>:  Help()";

static struct {
	Widget w_dialog;
	Widget w_scrolledWindow;
	Widget w_rowColumn;
	Widget w_separator;
	Widget w_actionArea;
	Widget w_okButton;
	Widget w_applyButton;
	Widget w_cancelButton;
	Widget w_helpButton;
} pref;

static char *prefs_file;
static int *prefs_order;

static struct choice_st choice_prefs[] = {

	{
		"Add_To_Cache", &add_to_cache_ok, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Beep_When_Ops_Done", &beep_when_ops_done, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Diagnostics", &diagnostics, "Normal", 4,
		{ { "Quiet",  QUIET  }, { "Verbose", VERBOSE },
		  { "Normal", NORMAL }, { "Debug",   DEBUG   } }
	},

	{
		"Enhance_Colors", &enhance_colors, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Initial_Transfer_Mode", &initial_xfer_mode, "Binary", 2,
		{ { "ASCII",  ASCII }, { "Binary", BINARY } }
	},

	{
		"Initial_Wildcard_Mode", &initial_wildcard_mode, "Replace", 2,
		{ { "Replace",  REPLACE }, { "Add", ADD } }
	},

	{
		"Inquire_On_Copy", &inquire_on_copy, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Inquire_On_Delete", &inquire_on_delete, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Inquire_On_Move", &inquire_on_move, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Inquire_On_Quote", &inquire_on_quote, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Left_Auto_Local_Login", &auto_local_login[LEFT], "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},
	{
		"Overwrite_Dir_On_Copy", &overwrite_dirs, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Preserve_Vms_Ver_Nums", &preserve_vms_ver_nums, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Print_Password_In_Debug", &print_password_in_debug, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Recursive_Delete_Enabled", &rdelete_enabled, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Right_Auto_Local_Login", &auto_local_login[RIGHT], "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Sort_Caches", &sort_caches, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Sort_Long_Lists_By_Date", &sort_long_lists_by_date, "No(Safe)", 2,
		{ { "Yes(Unsafe)", True }, { "No(Safe)",  False } }
	},

	{
		"Store_Unique", &store_unique, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Symbols_On_Dir_Entries", &symbol_mode, "No(Safe)", 2,
		{ { "Yes(Unsafe)",  True }, { "No(Safe)", False } }
	},

	{
		"Use_Last_Dir_At_Login", &use_last_dir, "No", 2,
		{ { "Yes", True }, { "No",  False } }
	},

	{
		"Enter_Dir_Upon_Creation", &enter_dir_upon_creation, "Yes", 2,
		{ { "Yes", True }, { "No",  False } }
	}
};

static struct text_st text_prefs[] = {

	{ "Anonymous_Ftp_Password", &anonymous_ftp_password, "user@host" },

	{ "Initial_Logfile_Name", &initial_logfile_name, "xftp.log" },

	{ "Quote_Placeholder", &quote_placeholder, "<>" }
};

static struct number_st number_prefs[] = {
	
	{ "Directory_Cache_Size", &max_cache[DIRECTORY], 15,  1, MAXDIRCACHE },

	{ "Host_Cache_Size", &max_cache_hosts, 10, 1, 30 },

	{ "Max_Ftp_Retries", &max_ftp_retries, 3, 0, 10 },

	{ "Max_Time_For_Ftp_Reply", &maxt, 60, 30, 300 },

	{ "Quote_Cache_Size", &max_cache[QUOTE], 10,  1, 30 },

	{ "User_Cache_Size", &max_cache[USER], 10,  1, 30 },

	{ "Wildcard_Cache_Size", &max_cache[WILDCARD], 10,  1, 30 }
};

struct prefs_struct prefs[] = {

	{ "Add_To_Cache", CHOICE, help_add_to_cache },

	{ "Anonymous_Ftp_Password", TEXT, help_anonymous_ftp_password },

	{ "Beep_When_Ops_Done", CHOICE, help_beep_when_ops_done },

	{ "Diagnostics", CHOICE, help_diagnostics },

	{ "Directory_Cache_Size", NUMBER, help_directory_cache_size },

	{ "Enhance_Colors", CHOICE, help_enhance_colors },

	{ "Enter_Dir_Upon_Creation", CHOICE, help_enter_dir_upon_creation },

	{ "Host_Cache_Size", NUMBER, help_host_cache_size },

	{ "Initial_Logfile_Name", TEXT, help_initial_log_file_name },

	{ "Initial_Transfer_Mode", CHOICE, help_initial_transfer_mode },

	{ "Initial_Wildcard_Mode", CHOICE, help_initial_wildcard_mode},

	{ "Inquire_On_Copy", CHOICE, help_inquire_on_copy },

	{ "Inquire_On_Delete", CHOICE, help_inquire_on_delete },

	{ "Inquire_On_Move", CHOICE, help_inquire_on_move },

	{ "Inquire_On_Quote", CHOICE, help_inquire_on_quote },

	{ "Left_Auto_Local_Login", CHOICE, help_left_auto_local_login },

	{ "Max_Ftp_Retries", NUMBER, help_max_ftp_retries },

	{ "Max_Time_For_Ftp_Reply", NUMBER, help_max_time_for_ftp_reply },

	{ "Overwrite_Dir_On_Copy", CHOICE, help_overwrite_dir_on_copy },

	{ "Preserve_Vms_Ver_Nums", CHOICE, help_preserve_vms_ver_nums },

	{ "Print_Password_In_Debug", CHOICE, help_print_password_in_debug },

	{ "Quote_Cache_Size", NUMBER, help_quote_cache_size },

	{ "Quote_Placeholder", TEXT, help_quote_placeholder },

	{ "Recursive_Delete_Enabled", CHOICE, help_recursive_delete_enabled },

	{ "Right_Auto_Local_Login", CHOICE, help_right_auto_local_login},

	{ "Sort_Caches", CHOICE, help_sort_caches },

	{ "Sort_Long_Lists_By_Date", CHOICE, help_sort_long_lists_by_date },

	{ "Store_Unique", CHOICE, help_store_unique },

	{ "Symbols_On_Dir_Entries", CHOICE, help_symbols_on_dir_entries},

	{ "Use_Last_Dir_At_Login", CHOICE, help_use_last_dir },

	{ "User_Cache_Size", NUMBER, help_user_cache_size },

	{ "Wildcard_Cache_Size", NUMBER, help_wildcard_cache_size },
};

static int nprefs = XtNumber(prefs);
static int nchoice_prefs = XtNumber(choice_prefs);
static int ntext_prefs = XtNumber(text_prefs);
static int nnumber_prefs = XtNumber(number_prefs);

extern Widget w_toplev;

void cb_prefs_item_help();
void cb_prefs_help();
void cb_prefs_ok();
void cb_prefs_apply();
void cb_prefs_cancel();
Widget vertical_sb();


/*
 * initialize_prefs - Initialize the preferences data structures.
 */
initialize_prefs()
{
	int i;
	int j;

	for (i=0; i<nprefs; i++) {
		switch (prefs[i].type) {
		case TEXT:
			for (j=0; j<ntext_prefs; j++)
				if (!strcmp(text_prefs[j].file_label, prefs[i].file_label)) {
					prefs[i].part.text = &text_prefs[j];
					break;
				}
			if (j == ntext_prefs)
				fatal_error("Programming bug in initialize_prefs()");
			break;
		case CHOICE:
			for (j=0; j<nchoice_prefs; j++)
				if (!strcmp(choice_prefs[j].file_label, prefs[i].file_label)) {
					prefs[i].part.choice = &choice_prefs[j];
					break;
				}
			if (j == nchoice_prefs)
				fatal_error("Programming bug in initialize_prefs()");
			break;
		case NUMBER:
			for (j=0; j<nnumber_prefs; j++)
				if (!strcmp(number_prefs[j].file_label, prefs[i].file_label)) {
					prefs[i].part.number = &number_prefs[j];
					break;
				}
			if (j == nnumber_prefs)
				fatal_error("Programming bug in initialize_prefs()");
			break;
		}
	}
}


/*
 * cb_preferences - Callback to pop up User Preferences dialog.
 */
void
cb_preferences(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    /* Clear error flag */
    raise_okflag();

	create_prefs_dialog();

	/* Pop up preferences window */
	display_current_prefs();
	XtManageChild(pref.w_dialog);
	add_dialog_to_list(pref.w_dialog);
}


/*
 * create_prefs_dialog - Create User Preferences dialog.
 */
create_prefs_dialog()
{
	static int initialized = False;
	int i;
	int j;
	char *label;
	XmString string;
	Widget w_frame;
	Widget w_form;
	Widget w_label;
	Widget w_radioBoxFrame;
	Widget w_radioBox;
	int indx;
	int len;
	int x;
	int y;
	struct choice_st *c_st;
	struct number_st *n_st;
	XtTranslations translations;
	Dimension form_width;
	int toggle_offset;
	Dimension sw_width;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

	/* Create form dialog for displaying user preferences */
	pref.w_dialog = XmCreateFormDialog(w_toplev, "prefs", NULL, 0);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(pref.w_dialog, cb_prefs_cancel, NULL);

	/* Create form for action area */
    pref.w_actionArea = XtVaCreateWidget(
        "actionArea",
        xmFormWidgetClass,
        pref.w_dialog,
		XmNmarginHeight,		10,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
        NULL
    );

	/* Create OK pushbutton */
    pref.w_okButton = XtVaCreateManagedWidget(
        "okButton",
        xmPushButtonWidgetClass,
        pref.w_actionArea,
		XmNdefaultButtonShadowThickness,	1,
		XmNmarginHeight,					4,
		XmNtopAttachment,					XmATTACH_FORM,
        XmNbottomAttachment,    			XmATTACH_FORM,
        XmNleftAttachment,      			XmATTACH_POSITION,
        XmNleftPosition,        			5,
        XmNrightAttachment,     			XmATTACH_POSITION,
        XmNrightPosition,       			20,
        NULL
    );
    XtAddCallback(
        pref.w_okButton,
        XmNactivateCallback,
        cb_prefs_ok,
        (XtPointer)NULL
    );

	/* Create Apply pushbutton */
    pref.w_applyButton = XtVaCreateManagedWidget(
        "applyButton",
        xmPushButtonWidgetClass,
        pref.w_actionArea,
		XmNdefaultButtonShadowThickness,	1,
		XmNmarginHeight,					4,
		XmNtopAttachment,					XmATTACH_FORM,
        XmNbottomAttachment,    			XmATTACH_FORM,
        XmNleftAttachment,      			XmATTACH_POSITION,
        XmNleftPosition,        			30,
        XmNrightAttachment,     			XmATTACH_POSITION,
        XmNrightPosition,       			45,
        NULL
    );
    XtAddCallback(
        pref.w_applyButton,
        XmNactivateCallback,
        cb_prefs_apply,
        (XtPointer)NULL
    );

	/* Create Cancel pushbutton */
    pref.w_cancelButton = XtVaCreateManagedWidget(
        "cancelButton",
        xmPushButtonWidgetClass,
        pref.w_actionArea,
		XmNdefaultButtonShadowThickness,	1,
		XmNmarginHeight,					4,
		XmNtopAttachment,					XmATTACH_FORM,
        XmNbottomAttachment,    			XmATTACH_FORM,
        XmNleftAttachment,      			XmATTACH_POSITION,
        XmNleftPosition,        			55,
        XmNrightAttachment,     			XmATTACH_POSITION,
        XmNrightPosition,       			70,
        NULL
    );
    XtAddCallback(pref.w_cancelButton, XmNactivateCallback, cb_prefs_cancel,
        (XtPointer)NULL);

	/* Create Help pushbutton */
    pref.w_helpButton = XtVaCreateManagedWidget(
        "helpButton",
        xmPushButtonWidgetClass,
        pref.w_actionArea,
		XmNdefaultButtonShadowThickness,	1,
		XmNmarginHeight,					4,
		XmNtopAttachment,					XmATTACH_FORM,
        XmNbottomAttachment,    			XmATTACH_FORM,
        XmNleftAttachment,      			XmATTACH_POSITION,
        XmNleftPosition,        			80,
        XmNrightAttachment,     			XmATTACH_POSITION,
        XmNrightPosition,       			95,
        NULL
    );
    XtAddCallback(
        pref.w_helpButton,
        XmNactivateCallback,
        cb_prefs_help,
        (XtPointer)NULL
    );

	XtManageChild(pref.w_actionArea);

    /* Create separator */
    pref.w_separator = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        pref.w_dialog,
        XmNbottomAttachment,    XmATTACH_WIDGET,
        XmNbottomWidget,        pref.w_actionArea,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );

	/* Create scrolled window to hold preferences */
	pref.w_scrolledWindow = XtVaCreateManagedWidget(
		"scrolledWindow",
		xmScrolledWindowWidgetClass,
		pref.w_dialog,
		XmNscrollingPolicy,			XmAUTOMATIC,
		XmNtopAttachment,			XmATTACH_FORM,
		XmNtopOffset,				10,
		XmNbottomAttachment,		XmATTACH_WIDGET,
		XmNbottomWidget,			pref.w_separator,
		XmNbottomOffset,			10,
		XmNleftAttachment,			XmATTACH_FORM,
		XmNleftOffset,				10,
		XmNrightAttachment,			XmATTACH_FORM,
		XmNrightOffset,				10,
		NULL
	);

	/* Calcuate some component dimensions based on scrolled window size */
	XtVaGetValues(pref.w_scrolledWindow, XmNwidth, &sw_width, NULL);
	form_width = sw_width-41;
	toggle_offset = (int)(.46*(.56*form_width-7));

	/* Create rowcolumn widget to hold preferences */
	pref.w_rowColumn = XtVaCreateWidget(
		"rowColumn",
		xmRowColumnWidgetClass,
		pref.w_scrolledWindow,
		NULL
	);

	/* Layout widgets for individual preferences */
	for (i=0; i<nprefs; i++) {
		indx = prefs_order[i];
		w_frame = XtVaCreateWidget("frame", xmFrameWidgetClass,
			pref.w_rowColumn, NULL);
		w_form = XtVaCreateWidget("form", xmFormWidgetClass, w_frame,
			XmNwidth,	form_width,
			NULL
		);
		len = strlen(prefs[indx].file_label);
		label = XtNewString(prefs[indx].file_label);
		for (j=0; j<len; j++)
			if (label[j] == '_')
				label[j] = ' ';
		string = XmStringCreateSimple(label);
		XtFree(label);
		w_label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, w_form,
			XmNlabelString,			string,
			XmNtopAttachment,		XmATTACH_FORM,
			XmNbottomAttachment,	XmATTACH_FORM,
			XmNleftAttachment,		XmATTACH_FORM,
			XmNleftOffset,			6,
			NULL
		);
		XmStringFree(string);
		translations = XtParseTranslationTable(prefsTranslations);
		XtOverrideTranslations(w_label, translations);
		XtAddCallback(w_label, XmNhelpCallback, cb_prefs_item_help,
			(XtPointer)((long)indx));
		switch (prefs[indx].type) {
		case TEXT:
			prefs[indx].widget = XtVaCreateManagedWidget(
				"textField",
				xmTextFieldWidgetClass,
				w_form,
				XmNtopAttachment,		XmATTACH_FORM,
				XmNbottomAttachment,	XmATTACH_FORM,
				XmNrightAttachment,		XmATTACH_FORM,
				XmNleftAttachment,		XmATTACH_POSITION,
				XmNleftPosition,		44,
				XmNtopOffset,			6,
				XmNbottomOffset,		6,
				XmNrightOffset,			6,
				NULL
			);
			set_bg_to_sb_trough_color(prefs[indx].widget,
				vertical_sb(pref.w_scrolledWindow));
			break;
		case CHOICE:
			w_radioBoxFrame = XtVaCreateWidget(
				"radioBoxFrame",
				xmFrameWidgetClass,
				w_form,
				XmNtopAttachment,		XmATTACH_FORM,
				XmNbottomAttachment,	XmATTACH_FORM,
				XmNleftAttachment,		XmATTACH_POSITION,
				XmNleftPosition,		44,
				XmNrightAttachment,		XmATTACH_FORM,
				XmNtopOffset,			8,
				XmNbottomOffset,		8,
				XmNrightOffset,			8,
				NULL
			);
			w_radioBox = XtVaCreateWidget(
				"radioBox",
				xmRowColumnWidgetClass,
				w_radioBoxFrame,
				XmNisHomogeneous,	True,
				XmNentryClass,		xmToggleButtonGadgetClass,
				XmNpacking,			XmPACK_NONE,
				XmNradioBehavior,	True,
				NULL
			);
			c_st = prefs[indx].part.choice;
			for (j=0; j<c_st->nchoices; j++) {
				string = XmStringCreateSimple(c_st->choices[j].label);
				if (j%2 == 0) 
					x = 0;
				else
					x = toggle_offset;
				y = 20*(j/2);
				c_st->choices[j].widget = XtVaCreateManagedWidget(
					"",
					xmToggleButtonGadgetClass,
					w_radioBox,
					XmNlabelString,		string,
					XmNmarginHeight,	0,
					XmNx,				x,
					XmNy,				y,
					NULL
				);
				XmStringFree(string);
			}
			XtManageChild(w_radioBox);
			XtManageChild(w_radioBoxFrame);
			break;
		case NUMBER:
			n_st = prefs[indx].part.number;
			prefs[indx].widget = XtVaCreateManagedWidget(
				"scale",
				xmScaleWidgetClass,
				w_form,
                XmNtopAttachment,       XmATTACH_FORM,
                XmNbottomAttachment,    XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
				XmNleftAttachment,		XmATTACH_POSITION,
				XmNleftPosition,		44,
                XmNtopOffset,           6,
                XmNbottomOffset,        6,
                XmNrightOffset,         6,
		        XmNshowValue,           True,
		        XmNorientation,         XmHORIZONTAL,
				XmNminimum,				n_st->min,
				XmNmaximum,				n_st->max,
				NULL
			);
			break;
		default:
			fatal_error("Bug in create_prefs_dialog()");
		}
		XtManageChild(w_form);
		XtManageChild(w_frame);
	}

	XtManageChild(pref.w_rowColumn);
	XtManageChild(pref.w_scrolledWindow);
}


/*
 * construct_prefs_filename - Form preferences file name by concatenating
 *                            .xftprc to $HOME.
 */
construct_prefs_filename()
{
	char *homedir = getenv("HOME");

	prefs_file = XtMalloc(strlen(homedir)+strlen(PROPFILE)+2);
	strcpy(prefs_file, homedir);
	strcat(prefs_file, "/");
	strcat(prefs_file, PROPFILE);
}


/*
 * read_prefs_from_file - Initializes current preferences with values
 *                        from .xftprc.
 */
read_prefs_from_file()
{
	static FILE *fp;
	char prefs_line[MAXPLINE+1];
	char *q;
	int len;
	int i;
	int j;
	int position = 0;
	struct choice_st *c_st;
	struct number_st *n_st;
	int value;
	int new_prefs = False;

	/* Set default values and unset preference positions */
	for (i=0; i<nprefs; i++) {
		prefs[i].position = -1;
		switch (prefs[i].type) {
		case TEXT:
			*prefs[i].part.text->value = 
				XtNewString(prefs[i].part.text->def_value);
			break;
		case CHOICE:
			c_st = prefs[i].part.choice;
			for (j=0; j<c_st->nchoices; j++)
				if (strcmp(c_st->def_value, c_st->choices[j].label) == 0) {
					*c_st->value = c_st->choices[j].value;
					break;
				}
			if (j == c_st->nchoices)
				fatal_error("Programming bug in read_prefs_from_file()");
			break;
		case NUMBER:
			n_st = prefs[i].part.number;
			if (n_st->def_value < n_st->min || n_st->def_value > n_st->max)
				fatal_error("Bug in read_prefs_from_file()");
			*n_st->value = n_st->def_value;
			break;
		default:
			fatal_error("Bug in read_prefs_from_file()");
		}
	}

	/* Try to open preferences file */
	if ((fp = fopen(prefs_file, "r")) == NULL)
		goto positions;

	/* Process preferences */
	while (fgets(prefs_line, MAXPLINE+1, fp) != NULL) {
		len = strlen(prefs_line);
		if (prefs_line[len-1] != '\n')
			continue;
		if ((q=strtok(prefs_line, ":")) == NULL)
			continue;
		for (i=0; i<nprefs; i++)
			if (strcmp(q,prefs[i].file_label) == 0 && prefs[i].position == -1) {
				if ((q = strtok(NULL, "\n\t ")) == NULL)
					break;
				switch (prefs[i].type) {
				case TEXT:
					XtFree(*prefs[i].part.text->value);
					*prefs[i].part.text->value = XtNewString(q);
					prefs[i].position = position++;
					break;
				case CHOICE:
					c_st = prefs[i].part.choice;
					for (j=0; j<c_st->nchoices; j++)
						if (strcmp(q, c_st->choices[j].label) == 0) {
							*c_st->value = c_st->choices[j].value;
							prefs[i].position = position++;
							break;
						}
					break;
				case NUMBER:
					n_st = prefs[i].part.number;
					sscanf(q, "%d", &value);
					if (value >= n_st->min && value <= n_st->max) {
						*n_st->value = value;
						prefs[i].position = position++;
					}
					break;
				default:
					fatal_error("Bug in read_prefs_from_file()");
				}
				break;
			}
	}

	fclose(fp);

positions:

	/* Establish the position of preferences not in file */
	for (i=0; i<nprefs; i++)
		if (prefs[i].position == -1) {
			new_prefs = True;
			prefs[i].position = position++;
		}

	/* Determine the order to list the preferences */
	prefs_order = (int *)XtMalloc(sizeof(int)*nprefs);
	for (i=0; i<nprefs; i++)
		prefs_order[prefs[i].position] = i;

	/* If new prefs added by programmer, save prefs */
	if (new_prefs && save_prefs())
		fatal_error("Unable to save user preferences.");
}


/*
 * display_current_prefs - Update the display with current preferences.
 */
display_current_prefs()
{
	int i;
	int j;
	struct choice_st *c_st;

	for (i=0; i<nprefs; i++)
		switch (prefs[i].type) {
		case TEXT:
			XmTextFieldSetString(prefs[i].widget, *prefs[i].part.text->value);
			break;
		case CHOICE:
			c_st = prefs[i].part.choice;
			for (j=0; j<c_st->nchoices; j++)
				if (*c_st->value == c_st->choices[j].value) {
					XmToggleButtonSetState(
						c_st->choices[j].widget,
						True,
						True
					);
					break;
				}
			if (j == c_st->nchoices)
				fatal_error(
					"Bug in display_current_prefs() - Contact programmer");
			break;
		case NUMBER:
			XmScaleSetValue(prefs[i].widget, *prefs[i].part.number->value);
			break;
		default:
			fatal_error("Bug in display_current_prefs()");
		}
}


/*
 * save_prefs - Write current preferences to .xftprc.  Returns -1 if
 *              successful, else -1.
 */
save_prefs()
{
	static FILE *fp;
	int i;
	int j;
	int indx;
	struct choice_st *c_st;

	if ((fp = fopen(prefs_file, "w")) == NULL)
		return -1;

	for (i=0; i<nprefs; i++) {
		indx = prefs_order[i];
		fprintf(fp, "%s: ", prefs[indx].file_label);
		switch (prefs[indx].type) {
		case TEXT:
			fprintf(fp, "%s\n", *prefs[indx].part.text->value);
			break;
		case CHOICE:
			c_st = prefs[indx].part.choice;
			for (j=0; j<c_st->nchoices; j++)
				if (*c_st->value == c_st->choices[j].value) {
					fprintf(fp, "%s\n", c_st->choices[j].label);
					break;
				}
            if (j == c_st->nchoices)
                fatal_error("Bug in save_prefs() - Contact programmer\n");
			break;
		case NUMBER:
			fprintf(fp, "%d\n", *prefs[indx].part.number->value);
			break;
		default:
			fatal_error("Bug in save_prefs() - Contact programmer\n");
		}
	}

	fclose(fp);
	need_to_save_prefs = False;
	return 0;
}


/*
 * apply - Set current preferences to displayed values.
 */
apply()
{
	int i;
	int j;
	struct choice_st *c_st;

	need_to_save_prefs = True;

	for (i=0; i<nprefs; i++)
		switch (prefs[i].type) {
		case TEXT:
			XtFree(*prefs[i].part.text->value);
			*prefs[i].part.text->value = XmTextFieldGetString(prefs[i].widget);
			break;
		case CHOICE:
			c_st = prefs[i].part.choice;
			for (j=0; j<c_st->nchoices; j++)
				if (XmToggleButtonGetState(c_st->choices[j].widget))
					break;
			if (j<c_st->nchoices)
				*c_st->value = c_st->choices[j].value;
			else
				fatal_error("Bug in apply().  Contact programmer.\n");
			break;
		case NUMBER:
			XmScaleGetValue(prefs[i].widget, prefs[i].part.number->value);
			break;
		default:
			fatal_error("Bug in apply().  Contact programmer.\n");
		}
	
	/* Adjust sensitivity of the host controls */
	for (i=0; i<NUMHOSTS; i++)
		update_host_controls(i);
}


/*
 * cb_prefs_ok - Callback that applies current user preferences and then
 *               closes User Preferences dialog.
 */
void
cb_prefs_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	apply();
	XtUnmanageChild(pref.w_dialog);
}


/*
 * cb_prefs_apply - Callback that applies current user preferences.
 */
void
cb_prefs_apply(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	apply();
}


/*
 * cb_prefs_cancel - Callback that restores old user preferences and then
 *                   closes User Preferences dialog.
 */
void
cb_prefs_cancel(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	display_current_prefs();
	XtUnmanageChild(pref.w_dialog);
}


/*
 * cb_save_prefs - Callback that saves current user preferences and viewer
 *                 preferences to a disk file.
 */
void
cb_save_prefs(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	/* Clear error flag */
	raise_okflag();

	if (save_prefs())
		warning_error("Unable to save user preferences.");

	if (save_viewprefs())
		warning_error("Unable to save viewer preferences.");
}

