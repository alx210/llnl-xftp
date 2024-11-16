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
#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/List.h>
#include "xftp.h"
#include "list.h"

#define FUDGE		7

#define MARGIN      4
#define SPACING     2
#define TMARGIN		2

#define UNSELECTED  0
#define SELECTED    1

struct table_entry {
	int state;
	char *name;
};

static struct {
	Widget w_dialog;
	Widget w_scrolledWindow;
	Widget w_drawingArea;
	Widget w_separator;
	Widget w_closeButton;
	Widget w_helpButton;
	int entry_width;
	int entry_height;
	XFontStruct *font_info;
	GC gc_normal;
	GC gc_inverse;
	int nentries;
	int ncols;
	int nrows;
} dirtable[NUMHOSTS];

static int dirtable_dialog_created[] = { False, False };
static struct table_entry *table_info[] = { NULL, NULL };

static char *dirtable_dialog_name[] = {
    "leftDirTable",
    "rightDirTable"
};

static String scrolledWindowTranslations =
	"<Configure>:	ResizeDirtable()";

static String drawingAreaTranslations =
	"<Btn1Down>:			DirtableMouse(down)\n\
	 <Btn1Motion>:			DirtableMouse(motion)";

static char *dirtable_help[] = {
    "The \"tabular\" directory list allows you to view",
	" the contents of the current",
    "directory in tabular form.  This list may be used to view",
    "and modify the directory's selected entries.  Entries may be",
    "selected/deselected either by clicking or by moving the cursor",
    "over the entries with the mouse button down.  Moving the mouse",
    "too quickly can cause some of the entries to be skipped.  If an",
    "entry is double-clicked, an attempt is made to change the",
    "current directory to that entry.  The tabular list is automatically",
    "updated as the current directory changes.",
	NULL
};

extern Widget w_toplev;
extern Widget w_dirList[];
extern struct st_host_info hinfo[];
extern Display *display;
extern Window root_window;

void cb_redraw_dirtable();
void cb_dirtable_close();
void cb_dirtable_help();
char *merge_paths();


/*
 * cb_display_dirtable - Callback to display the specified host's
 *                       directory entries in tabular format.
 */
void
cb_display_dirtable(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

    create_dirtable_dialog(host);

	XtManageChild(dirtable[host].w_dialog);
	add_dialog_to_list(dirtable[host].w_dialog);
	force_update(dirtable[host].w_drawingArea);

    /* Update the display */
    if (update_dir_displays(host, True) < 0)
        lost_connection(host);
}


/*
 * create_dirtable_dialog - Create a dialog for the specified host that
 *                          displays the entries of the current working
 *                          directory.
 */
create_dirtable_dialog(host)
int host;
{
	Arg args[1];
	int i;
	XtTranslations translations;
	XmStringCharSet charset;
	XmFontList fontlist;
	XmFontContext context;
	Pixel fg;
	Pixel bg;

    /* Create dialog only once */
    if (dirtable_dialog_created[host])
        return;
    dirtable_dialog_created[host] = True;

	/* Create form dialog */
	i = 0;
	XtSetArg(args[i], XmNautoUnmanage, False); i++;
	dirtable[host].w_dialog = XmCreateFormDialog(
		w_dirList[host],
		dirtable_dialog_name[host],
		args,
		i
	);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(dirtable[host].w_dialog, cb_dirtable_close, 
		(XtPointer)((long)host));

	/* Create "Close" pushbutton */
	dirtable[host].w_closeButton = XtVaCreateManagedWidget(
		"closeButton",
		xmPushButtonWidgetClass,
		dirtable[host].w_dialog,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,		10,
		XmNleftAttachment,		XmATTACH_POSITION,
		XmNleftPosition,		20,
		XmNrightAttachment,		XmATTACH_POSITION,
		XmNrightPosition,		36,
		NULL
	);
    XtAddCallback(
        dirtable[host].w_closeButton,
        XmNactivateCallback,
        cb_dirtable_close,
        (XtPointer)((long)host)
    );
   
	/* Create "Help" pushbutton */
	dirtable[host].w_helpButton = XtVaCreateManagedWidget(
		"helpButton",
		xmPushButtonWidgetClass,
		dirtable[host].w_dialog,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,		10,
		XmNleftAttachment,		XmATTACH_POSITION,
		XmNleftPosition,		64,
		XmNrightAttachment,		XmATTACH_POSITION,
		XmNrightPosition,		80,
		NULL
	);
    XtAddCallback(
        dirtable[host].w_helpButton,
        XmNactivateCallback,
        cb_dirtable_help,
        (XtPointer)((long)host)
    );

	/* Create separator */
	dirtable[host].w_separator = XtVaCreateManagedWidget(
		"separator",
		xmSeparatorWidgetClass,
		dirtable[host].w_dialog,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,		dirtable[host].w_closeButton,
		XmNbottomOffset,		10,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);

    /* Create scrolled window to hold drawing area widget */
    dirtable[host].w_scrolledWindow = XtVaCreateManagedWidget(
        "scrolledWindow",
        xmScrolledWindowWidgetClass,
        dirtable[host].w_dialog,
        XmNscrollingPolicy,		XmAUTOMATIC,
		XmNuserData,			(long)host,
		XmNtopAttachment,		XmATTACH_FORM,
		XmNtopOffset,			7,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,        dirtable[host].w_separator,
		XmNbottomOffset,		6,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNleftOffset,			7,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNrightOffset,			6,
        NULL
    );

	/* Add translations to scrolled window to handle resize events */
	translations = XtParseTranslationTable(scrolledWindowTranslations);
	XtOverrideTranslations(dirtable[host].w_scrolledWindow, translations);

	/* Create drawing area widget to hold directory entries */
	dirtable[host].w_drawingArea = XtVaCreateManagedWidget(
		"drawingArea",
		xmDrawingAreaWidgetClass,
		dirtable[host].w_scrolledWindow,
		XmNuserData,		(long)host,
		XmNresizePolicy,	XmRESIZE_NONE,
		XmNtranslations,	XtParseTranslationTable(drawingAreaTranslations),
		NULL
	);
	XtAddCallback(dirtable[host].w_drawingArea, XmNexposeCallback,
		cb_redraw_dirtable, (XtPointer)((long)host));

    /* Get foreground and background colors of drawing area */
    XtVaGetValues(
        dirtable[host].w_drawingArea,
        XmNbackground,  &bg,
        XmNforeground,  &fg,
        NULL
    );

    /* Get fontlist */
    XtVaGetValues(dirtable[host].w_dialog, XmNlabelFontList, &fontlist, NULL);

    /* Get font */
    if (XmFontListInitFontContext(&context, fontlist) == False)
        fatal_error("Unable to get font");
    if (!XmFontListGetNextFont(context, &charset, &dirtable[host].font_info))
        fatal_error("Unable to get font");
    XtFree(charset);

    /* Create First Graphics Context displaying entries */
    dirtable[host].gc_normal = XCreateGC(display, XtWindow(w_toplev),
		(unsigned long)0, NULL);
    XSetForeground(display, dirtable[host].gc_normal, fg);
    XSetBackground(display, dirtable[host].gc_normal, bg);
    XSetFont(display, dirtable[host].gc_normal, dirtable[host].font_info->fid);

    /* Create Second Graphics Context for displaying entries */
    dirtable[host].gc_inverse = XCreateGC(display, XtWindow(w_toplev),
		(unsigned long)0, NULL);
    XSetForeground(display, dirtable[host].gc_inverse, bg);
    XSetBackground(display, dirtable[host].gc_inverse, fg);
    XSetFont(display, dirtable[host].gc_inverse, dirtable[host].font_info->fid);

    XmFontListFreeFontContext(context);
}


/*
 * ResizeDirtable - Action procedure for rearranging the entries in the
 *                  directory table to better fit the new size of the
 *                  scrolled window widget.
 */
void
ResizeDirtable(widget, event, args, num_args)
Widget widget;
XEvent *event;
String *args;
Cardinal *num_args;
{
	long host;

	XtVaGetValues(widget, XmNuserData, &host, NULL);

	calc_dirtable_geometry(host);

	XClearArea(display, XtWindow(dirtable[host].w_drawingArea), 0, 0, 0, 0,
		True);
}


/*
 * DirtableMouse - Action procedure for handling mouse events in the
 *                 drawing area.
 */
void
DirtableMouse(widget, event, args, num_args)
Widget widget;
XButtonEvent *event;
String *args;
Cardinal *num_args;
{
	long host;
	static int state = 0;
	static int prev_index;
	static Time prev_time;
	char *entry;
	char *new_wd;
	int indx;
	static int last_motion_index;

	/* Sanity check */
	if (*num_args != 1)
		fatal_error("Bug in DirtableMouse()");

	/* Get host */
	XtVaGetValues(widget, XmNuserData, &host, NULL);

	/* Which entry did the event occur in? */
	indx = entry_index(host, event->x, event->y);

	/* Handle mouse motion event */
	if (strcmp(args[0], "motion") == 0) {
		if (indx == -1 || !point_in_clipwindow(host, event->x, event->y))
			last_motion_index = -1;
		else if (indx != last_motion_index) {
			toggle_table_entry(host, indx);
			last_motion_index = indx;
		}
		return;
	}

	/* Must be a "button press" event */
	last_motion_index = indx;

	/* Ignore button presses outside of entries */
	if (indx == -1)
		return;

	/* Distinguish between single and double clicks */
	if (state == 0 || indx != prev_index
		|| event->time-prev_time >= XtGetMultiClickTime(display)) {
		toggle_table_entry(host, indx);
		prev_index = indx;
		prev_time = event->time;
		state = 1;
		return;
	}

	/* It's a double click!  Clear all selected entries */ 
	clear_selected_entries(host);

	/* Get entry that was double-clicked */
	entry = XtNewString(table_info[host][indx].name);
	strip_off_symbol(hinfo[host].system, hinfo[host].server, entry);

    /* Form full path name */
	new_wd = merge_paths(hinfo[host].system, hinfo[host].wd, entry);
	XtFree(entry);

    /* Try to cd to specified directory */
    change_directory(host, new_wd);
    XtFree(new_wd);

	state = 0;
}


/*
 * update_dirtable - Update the host's directory table.  update_dirtable()
 *                   first deletes existing entries, and then displays
 *                   the new entry names.
 */
update_dirtable(host, dlist)
int host;
struct sl_struct *dlist;
{
	int i;
	int width;

	/* Delete existing directory entries */
	delete_dirtable_entries(host);

	/* Allocate array to hold directory table info */
	if (dlist->nentries)
		table_info[host] =(struct table_entry *)XtMalloc(dlist->nentries*
			sizeof(struct table_entry));
	else
		table_info[host] = NULL;

    /* Determine size of directory entries */
    dirtable[host].entry_width = 0;
    for (i=0; i<dlist->nentries; i++) {
        table_info[host][i].name = XtNewString(dlist->entries[i]);
        width = XTextWidth(dirtable[host].font_info, dlist->entries[i],
            strlen(dlist->entries[i]));
        if (width > dirtable[host].entry_width)
            dirtable[host].entry_width = width;
		table_info[host][i].state = UNSELECTED;
    }
	dirtable[host].entry_width += 2*TMARGIN;
	dirtable[host].entry_height = dirtable[host].font_info->ascent+
		dirtable[host].font_info->descent+2*TMARGIN;
	dirtable[host].nentries = dlist->nentries;

	calc_dirtable_geometry(host);

	XClearArea(display, XtWindow(dirtable[host].w_drawingArea), 0, 0, 0, 0,
		True);
}


/*
 * cb_dirtable_close - Callback for closing directory table dialog.
 */
void
cb_dirtable_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	delete_dirtable_entries(host);

	XtUnmanageChild(dirtable[host].w_dialog);
}


/*
 * cb_dirtable_help - Callback for invoking directory table dialog's
 *                    help package.
 */
void
cb_dirtable_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
    char *text;

    XtVaGetValues(XtParent(dirtable[host].w_dialog), XmNtitle, &text, NULL);
    help_dialog(dirtable[host].w_dialog, False, text, dirtable_help);
}


/*
 * delete_dirtable_entries - Release memory used to hold information
 *                           about the directory entries.
 */
delete_dirtable_entries(host)
int host;
{
	int i;

    if (table_info[host]) {
        for (i=0; i<dirtable[host].nentries; i++)
            XtFree(table_info[host][i].name);
        XtFree((char *)table_info[host]);
        table_info[host] = NULL;
    }
	dirtable[host].nentries = 0;
}


/*
 * select_table_entry - Select the label (specified by the entry's
 *                      position) in the host's directory list table.
 */
select_table_entry(host, indx)
int host;
int indx;
{
	table_info[host][indx].state = SELECTED;
	draw_entry(host, indx);
}


/*
 * deselect_table_entry - Select the label (specified by the entry's
 *                        position) in the host's directory list table.
 */
deselect_table_entry(host, indx)
int host;
int indx;
{
	table_info[host][indx].state = UNSELECTED;
	draw_entry(host, indx);
}


/*
 * toggle_table_entry - Toggle the selection of the label widget
 *                      for "host" specified by "indx". Also
 *                      update directory display in main window.
 */
toggle_table_entry(host, indx)
int host;
int indx;
{
	int i;

	/* Clear error flag */
	raise_okflag();

    /* First clear other host's selected directory entries */
    if (host == LEFT)
        clear_selected_entries(RIGHT);
    else
        clear_selected_entries(LEFT);

	/* Now toggle item */
	if (table_info[host][indx].state == UNSELECTED) {
		select_table_entry(host, indx);
		XmListDeselectPos(w_dirList[host], indx+1);
		XmListSelectPos(w_dirList[host], indx+1, False);
	} else {
		deselect_table_entry(host, indx);
		XmListDeselectPos(w_dirList[host], indx+1);
	}

	/* Update the display */
	update_xfer_controls();
    for (i=0; i<NUMHOSTS; i++)
        update_host_controls(i);
}


/*
 * dirtable_in_use - Returns True if the host's tabular list dialog is
 *                   in use, else returns False.
 */
dirtable_in_use(host)
int host;
{
	if (dirtable_dialog_created[host] && XtIsManaged(dirtable[host].w_dialog))
		return True;
	else
		return False;
}


/*
 * clear_selected_table_entries - Clear all selected directory entries of
 *                                the specified host.
 */
clear_selected_table_entries(host)
int host;
{
	int i;

	for (i=0; i<dirtable[host].nentries; i++)
		if (table_info[host][i].state == SELECTED)
			deselect_table_entry(host, i);
}


/*
 * cb_redraw_dirtable - Callback to redraw portion of drawing area in
 *                      response to an expose event.
 */
void
cb_redraw_dirtable(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct *)call_data;
	int x = cbs->event->xexpose.x;
	int y = cbs->event->xexpose.y;
	int width = cbs->event->xexpose.width;
	int height = cbs->event->xexpose.height;
	int start_col;
	int end_col;
	int start_row;
	int end_row;
	int col;
	int row;
	int indx;

	/* Determine which entries to redraw */
	start_col = (x-MARGIN)/(dirtable[host].entry_width+SPACING);
	if (start_col < 0)
		start_col = 0;
	end_col = (x-MARGIN+width)/(dirtable[host].entry_width+SPACING);
	if (end_col >= dirtable[host].ncols)
		end_col = dirtable[host].ncols-1;
	start_row = (y-MARGIN)/(dirtable[host].entry_height+SPACING);
	if (start_row < 0)
		start_row = 0;
	end_row = (y-MARGIN+height)/(dirtable[host].entry_height+SPACING);
	if (end_row >= dirtable[host].nrows)
		end_row = dirtable[host].nrows-1;

	for (col=start_col; col<=end_col; col++)
		for (row=start_row; row<=end_row; row++) {
			indx = col*dirtable[host].nrows+row;
			if (indx >= dirtable[host].nentries)
				break;
			draw_entry(host, indx);
		}
}


/*
 * calc_dirtable_geometry - Determine the layout of entries in the
 *                          drawing area.  Must be called before
 *                          entries are drawn.
 */
calc_dirtable_geometry(host)
int host;
{
	int ncols;
	int nrows;
	int nrows_visible;
	Dimension scrolledwindow_width;
	Dimension scrolledwindow_height;
	Dimension vertical_sb_width;
	Widget w_vertical_sb;

	/* Get dimensions of scrolled window */
	XtVaGetValues(
		dirtable[host].w_scrolledWindow,
		XmNwidth,	&scrolledwindow_width,
		XmNheight,	&scrolledwindow_height,
		NULL
	);

	if (dirtable[host].nentries == 0) {
		ncols = 0;
		nrows = 0;
		XtVaSetValues(dirtable[host].w_drawingArea,
			XmNwidth,	1,
			XmNheight,	1,
			NULL
		);
	} else {
		ncols = (int)(scrolledwindow_width-2*MARGIN)
			/(dirtable[host].entry_width+SPACING);
		if (ncols == 0)
			ncols = 1;
		nrows = (dirtable[host].nentries+ncols-1)/ncols;
		nrows_visible = (int)(scrolledwindow_height -2*MARGIN)
			/(dirtable[host].entry_height+SPACING);
		if (nrows > nrows_visible) {
	        XtVaGetValues(dirtable[host].w_scrolledWindow, XmNverticalScrollBar,
	            &w_vertical_sb, NULL);
	        XtVaGetValues(w_vertical_sb, XmNwidth, &vertical_sb_width, NULL);
			ncols = (int)(scrolledwindow_width-vertical_sb_width-FUDGE-2*MARGIN)
				/(dirtable[host].entry_width+SPACING);
			if (ncols == 0)
				ncols = 1;
			nrows = (dirtable[host].nentries+ncols-1)/ncols;
		}
		XtVaSetValues(dirtable[host].w_drawingArea,
			XmNwidth,	ncols*(dirtable[host].entry_width+SPACING)+2*MARGIN,
			XmNheight,	nrows*(dirtable[host].entry_height+SPACING)+2*MARGIN,
			NULL
		);
	}

	dirtable[host].ncols = ncols;
	dirtable[host].nrows = nrows;
}


/*
 * draw_entry - Draw the entry in the drawing area.  If the entry is
 *              flagged as "selected" it is displayed in inverse video.
 */
draw_entry(host, indx)
int host;
int indx;
{
	Window window = XtWindow(dirtable[host].w_drawingArea);
	int col = indx/dirtable[host].nrows;
	int row = indx-col*dirtable[host].nrows;
	int x = MARGIN+col*(dirtable[host].entry_width+SPACING);
	int y = MARGIN+row*(dirtable[host].entry_height+SPACING);
	GC gc1;
	GC gc2;

    if (table_info[host][indx].state == UNSELECTED) {
		gc1 = dirtable[host].gc_normal;
		gc2 = dirtable[host].gc_inverse;
	} else {
		gc1 = dirtable[host].gc_inverse;
		gc2 = dirtable[host].gc_normal;
	}

	XFillRectangle(display, window, gc2, x, y, dirtable[host].entry_width,
		dirtable[host].entry_height);
    XDrawString(display, window, gc1, x+TMARGIN,
		y+dirtable[host].font_info->ascent+TMARGIN,
		table_info[host][indx].name, strlen(table_info[host][indx].name));
}


/*
 * entry_index - Return the index of the entry at coordinates (x, y).
 *               Return -1 if no entry at this position.
 */
entry_index(host, x, y)
int host;
int x;
int y;
{
	int col;
	int row;
	int offset;
	int indx;

	if (dirtable[host].nentries == 0)
		return -1;

	if (x < MARGIN || y < MARGIN)
		return -1;

	/* Determine column */
	x -= MARGIN;
	col = x/(dirtable[host].entry_width+SPACING);
	if (col >= dirtable[host].ncols)
		return -1;
	offset = x-col*(dirtable[host].entry_width+SPACING);
	if (offset >= dirtable[host].entry_width)
		return -1;

	/* Determine row */
	y -= MARGIN;
	row = y/(dirtable[host].entry_height+SPACING);
	if (row >= dirtable[host].nrows)
		return -1;
	offset = y-row*(dirtable[host].entry_height+SPACING);
	if (offset >= dirtable[host].entry_height)
		return -1;

	/* Determine index */
	indx = col*dirtable[host].nrows+row;
	if (indx >= dirtable[host].nentries)
		return -1;

	return indx;
}


/*
 * point_in_clipwindow - Returns true if drawing area point (x,y) is in
 *                       the scrolled window's clip window (i.e., it
 *                       is visible.
 */
point_in_clipwindow(host, x, y)
int host;
int x;
int y;
{
	Widget w_clipWindow;
    Dimension clipwindow_width;
    Dimension clipwindow_height;
	Window child;
	int x_clip;
	int y_clip;
	int x_point;
	int y_point;

	/* Get coordinates of clip window origin */
    XtVaGetValues(dirtable[host].w_scrolledWindow, XmNclipWindow,
        &w_clipWindow, NULL);
    XtVaGetValues(
        w_clipWindow,
        XmNwidth,   &clipwindow_width,
        XmNheight,  &clipwindow_height,
        NULL);
	XTranslateCoordinates(display, XtWindow(w_clipWindow), root_window,
		0, 0, &x_clip, &y_clip, &child);
	
	/* Convert drawing area point into global coordinates */
	XTranslateCoordinates(display, XtWindow(dirtable[host].w_drawingArea),
		root_window, x, y, &x_point, &y_point, &child);

	/* Is point in clip window? */
	if (x_point < x_clip || x_point >= x_clip+(int)clipwindow_width
		|| y_point < y_clip || y_point >= y_clip+(int)clipwindow_height)
		return False;
	else
		return True;
}


/*
 * enable_dirtable - If "enable" is FALSE, the user is no longer able to
 *                   change directories by double-clicking on a directory
 *                   table entry.  If "enable" is TRUE, this capability
 *                   is turned back on.
 */
enable_dirtable(host, enable)
int host;
int enable;
{
	XtSetSensitive(dirtable[host].w_drawingArea, enable);
}

