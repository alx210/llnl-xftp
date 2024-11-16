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

#include <sys/time.h>
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include "startup.h"

#define VERSION   "Version 2.1"
#define DATE      "October 19, 1995"

#define STARTUP_WIDTH     486
#define STARTUP_HEIGHT    170
#define BORDER_WIDTH      5
#define DIVIDER_WIDTH     1
#define SIDE_WIDTH        144
#define CENTER_WIDTH      (STARTUP_WIDTH-2*SIDE_WIDTH-2*DIVIDER_WIDTH)
#define MIN_STARTUP_TIME  2

static struct {
    Widget w_dialog;
    Widget w_form;
	Widget w_frame;
    Widget w_startup;
    Pixmap startup_pixmap;
} about;

static char *about_xftp[] = {
    "LLNL XFTP is a graphical user interface to FTP (File Transfer",
    "Protocol).  LLNL XFTP does not execute the FTP program; instead,",
    "it directly communicates with remote FTP servers.  ",
    "LLNL XFTP is an X client and follows the OSF/Motif",
    "look-and-feel.\n",
    "\n",
    "LLNL XFTP was written by Neale Smith, a member of the Livermore",
    "Computing Department at Lawrence Livermore National Laboratory.\n",
    "\n",
	"Because our resources are limited, we will not be able to give",
	"individual assistance in building and using LLNL XFTP.  However,",
	"your comments, suggestions and bug reports are valuable in",
	"helping us in determining how best to direct our efforts.  ",
	"Our email address is\n",
	"\n",
    "                           llnlxftp@llnl.gov\n",
    "\n",
    "Select USING HELP in the HELP menu for",
    "instructions on using LLNL XFTP's online help package.\n",
	"\n",
	"\n",
	"AVAILABILITY\n",
	"\n",
	"Source code for LLNL XFTP is tarred and compressed, and is",
	"available as follows:\n",
	"\n",
	"        ftp://coral.ocf.llnl.gov/pub/ia/llnlxftp/latest_xftp.tar.Z\n",
	"\n",
	"        http://www.llnl.gov/liv_comp/xftp.html\n",
	"\n",
	"\n",
	"COPYRIGHT NOTICE\n",
	"\n",
	"(c) 1993-1995.  The Regents of the University of California.  All",
	"rights reserved.\n",
	"\n",
	"This work was produced at the University of California, Lawrence",
	"Livermore National Laboratory (UC LLNL) under contract no.",
	"W-7405-ENG-48 (Contract 48) between the U.S. Department of Energy",
	"(DOE) and The Regents of the University of California (University)",
	"for the operation of UC LLNL.  Copyright is reserved to the",
	"University for purposes of controlled dissemination,",
	"commercialization through formal licensing, or other disposition",
	"under terms of Contract 48; DOE policies, regulations and orders;",
	"and U.S. statutes.  The rights of the Federal Government are",
	"reserved under Contract 48 subject to the restrictions agreed upon",
	"by the DOE and University.\n",
	"\n",
	"                           DISCLAIMER\n",
	"\n",
	"This software was prepared as an account of work sponsored by an",
	"agency of the United States Government.  Neither the United States",
	"Government nor the University of California nor any of their",
	"employees, makes any warranty, express or implied, or assumes any",
	"liability or responsibility for the accuracy, completeness, or",
	"usefulness of any information, apparatus, product, or process",
	"disclosed, or represents that its specific commercial products,",
	"process, or service by trade name, trademark, manufacturer, or",
	"otherwise, does not necessarily constitute or imply its",
	"endorsement, recommendation, or favoring by the United States",
	"Government or the University of California. The views and opinions",
	"of the authors expressed herein do not necessarily state or reflect",
	"those of the United States Government or the University of",
	"California, and shall not be used for advertising or product",
	"endorsement purposes.\n",
	"\n",
	"Permission to use, copy, modify and distribute this software and its",
	"documentation for any non-commercial purpose, without fee, is",
	"hereby granted, provided that the above copyright notice and this",
	"permission notice appear in all copies of the software and",
	"supporting documentation, and that all UC LLNL identification in",
	"the user interface remain unchanged.  The title to copyright LLNL",
	"XFTP shall at all times remain with The Regents of the University",
	"of California and users agree to preserve same. Users seeking the",
	"right to make derivative works with LLNL XFTP for commercial",
	"purposes may obtain a license from the Lawrence Livermore National",
	"Laboratory's Technology Transfer Office, P.O. Box 808, L-795,",
	"Livermore, CA 94550.\n",
	NULL
};

static Window startup_window;
static Cursor startup_cursor;
static struct timeval start_time;

extern Display *display;
extern Window root_window;
extern Visual *visual;
extern int screen;
extern int depth;
extern Widget w_toplev;

void cb_about_help();
void cb_about_close();


/*
 * create_mainlogo_pixmap - Creates a pixmap containing the main logo
 *                          using the specified colors.  Caller is
 *                          responsible for calling XFreePixmap() when
 *                          done with pixmap.
 */
Pixmap
create_mainlogo_pixmap(foreground, background)
unsigned long foreground;
unsigned long background;
{
	return XCreatePixmapFromBitmapData(display, root_window,
		(char *)mainlogo_bits, mainlogo_width, mainlogo_height, foreground,
		background, depth);
}


/*
 * create_startup_pixmap - Creates and returns a pixmap containing startup
 *                         info.  Does it in color if possible.  Caller
 *                         responsible for calling XFreePixmap() when done
 *                         with pixmap.
 */
Pixmap
create_startup_pixmap()
{
	static char dash[] = { 2, 1 };
	Pixmap pixmap;
	Pixel default_fg = BlackPixel(display, screen);
	Pixel default_bg;
	Pixmap startup_pixmap;
	GC gc;
	GC inv_gc;
	XFontStruct *font_struct;
	int width;
	Colormap cmap = DefaultColormap(display, screen);
	XColor exact;
	XColor color;
	Pixel fg;
	int class = visual->class;
	int use_color = !(depth <8 || class == GrayScale || class == StaticGray);
	int x;
	int y;

	/* Set background color */
	if (use_color && XAllocNamedColor(display, cmap, "ivory", &exact, &color))
		default_bg = color.pixel;
	else
		default_bg = WhitePixel(display, screen);

	/* Load a font for the version number and date */
	font_struct = XLoadQueryFont(display, "variable");
	if (font_struct == NULL) {
		font_struct = XLoadQueryFont(display, "fixed");
		if (font_struct == NULL)
			fatal_error("Unable to get font for startup dialog");
	}

	/* Create startup pixmap */
	startup_pixmap = XCreatePixmap(display, root_window, STARTUP_WIDTH,
		STARTUP_HEIGHT, depth);

    /* Create graphics contexts */
    gc = XCreateGC(display, startup_pixmap, (unsigned long)0, NULL);
    XSetForeground(display, gc, default_fg);
    XSetBackground(display, gc, default_bg);
	XSetFont(display, gc, font_struct->fid);
	XSetLineAttributes(display, gc, 0, LineOnOffDash, CapButt,
		JoinMiter);
	XSetDashes(display, gc, 0, dash, 2);
    inv_gc = XCreateGC(display, startup_pixmap, (unsigned long)0, NULL);
    XSetForeground(display, inv_gc, default_bg);
    XSetBackground(display, inv_gc, default_fg);

	/* Clear startup pixmap */
	XFillRectangle(display, startup_pixmap, inv_gc, 0, 0, STARTUP_WIDTH,
		STARTUP_HEIGHT);

	/* Divide startup pixmap into three panes */
	XDrawLine(display, startup_pixmap, gc, SIDE_WIDTH, 0, SIDE_WIDTH,
		STARTUP_HEIGHT-1);
	XDrawLine(display, startup_pixmap, gc,
		SIDE_WIDTH+DIVIDER_WIDTH+CENTER_WIDTH, 0,
		SIDE_WIDTH+DIVIDER_WIDTH+CENTER_WIDTH, STARTUP_HEIGHT-1);

	/* Place Lab Logo into startup pixmap */
	if (use_color && XAllocNamedColor(display, cmap, "blue", &exact, &color))
		fg = color.pixel;
	else
		fg = default_fg;
	pixmap = create_mainlogo_pixmap(fg, default_bg);
	XFillRectangle(display, pixmap, inv_gc, 54, 5, 41, 42);
	x = (SIDE_WIDTH-mainlogo_width)/2;
	y = 22;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0,
		mainlogo_width, mainlogo_height, x, y);
	XFreePixmap(display, pixmap);
	
	/* Place Lab name into startup pixmap */
	pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)llnllong_bits, llnllong_width, llnllong_height, default_fg,
		default_bg, depth);
	x = (SIDE_WIDTH-llnllong_width)/2;
	y = 137;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0,
		llnllong_width, llnllong_height, x, y);
	XFreePixmap(display, pixmap);
	
	/* Place Lab's initials into startup pixmap */
	pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)llnlshort_bits, llnlshort_width, llnlshort_height, default_fg,
		default_bg, depth);
	x = SIDE_WIDTH+DIVIDER_WIDTH+(CENTER_WIDTH-llnlshort_width)/2;
	y = 20;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0,
		llnlshort_width, llnlshort_height, x, y);
	XFreePixmap(display, pixmap);
	
	/* Place Xftp's name into startup pixmap */
	if (use_color && XAllocNamedColor(display, cmap, "red", &exact, &color))
		fg = color.pixel;
	else
		fg = default_fg;
	pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)name_bits, name_width, name_height, fg, default_bg, depth);
	x = SIDE_WIDTH+DIVIDER_WIDTH+(CENTER_WIDTH-name_width)/2;
	y = 52;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0, name_width,
		name_height, x, y);
	XFreePixmap(display, pixmap);

	/* Place version number into startup pixmap */
	width = XTextWidth(font_struct, VERSION, strlen(VERSION));
	x = SIDE_WIDTH+DIVIDER_WIDTH+(CENTER_WIDTH-width)/2;
	y = 121;
	XDrawString(display, startup_pixmap, gc, x, y, VERSION,
		strlen(VERSION));

	/* Place date into startup pixmap */
	width = XTextWidth(font_struct, DATE, strlen(DATE));
	x = SIDE_WIDTH+DIVIDER_WIDTH+(CENTER_WIDTH-width)/2;
	y = 138;
	XDrawString(display, startup_pixmap, gc, x, y, DATE, strlen(DATE));
	
	/* Place UC's name into startup pixmap */
	pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)uc_bits, uc_width, uc_height, default_fg, default_bg, depth);
	x = SIDE_WIDTH+DIVIDER_WIDTH+(CENTER_WIDTH-uc_width)/2;
	y = 151;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0, uc_width,
		uc_height, x, y);
	XFreePixmap(display, pixmap);
	
	/* Place Neale's mug into startup pixmap */
	if (use_color && XAllocNamedColor(display, cmap, "brown", &exact, &color))
		fg = color.pixel;
	else
		fg = default_fg;
	pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)smith_bits, smith_width, smith_height, fg, default_bg, depth);
	x = SIDE_WIDTH+CENTER_WIDTH+2*DIVIDER_WIDTH+(SIDE_WIDTH-smith_width)/2;
	y = 7;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0,
		smith_width, smith_height, x, y);
	XFreePixmap(display, pixmap);
	
	/* Place author's name into startup pixmap */
	pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)author_bits, author_width, author_height, default_fg,
		default_bg, depth);
	x = SIDE_WIDTH+CENTER_WIDTH+2*DIVIDER_WIDTH+(SIDE_WIDTH-author_width)/2;
	y = 139;
	XCopyArea(display, pixmap, startup_pixmap, gc, 0, 0, author_width,
		author_height, x, y);
	XFreePixmap(display, pixmap);

	/* Free up memory */
	XFreeGC(display, gc);
	XFreeGC(display, inv_gc);
	XFreeFont(display, font_struct);

	return startup_pixmap;
}


/*
 * display_startup_dialog - Display the startup dialog.  Call
 *                          close_startup_dialog() to destroy the startup
 *                          dialog.  This function forces the dialog to
 *                          actually be displayed before returning.
 */
display_startup_dialog()
{
	Screen *screenptr = ScreenOfDisplay(display, screen);
	int x = (WidthOfScreen(screenptr)-STARTUP_WIDTH)/2;
	int y = (HeightOfScreen(screenptr)-STARTUP_HEIGHT)/2;
	unsigned long valuemask;
	XSetWindowAttributes attributes;
	XEvent event;
	Pixmap startup_pixmap;
	Pixmap cursor_pixmap;
	GC gc;
	XColor fg_color;
	XColor bg_color;
	Colormap cmap = DefaultColormap(display, screen);
	Pixel black = BlackPixel(display, screen);
	Pixel white = WhitePixel(display, screen);
	int width;
	int height;
	int i;
	int class = visual->class;
	int use_color = !(depth <8 || class == GrayScale || class == StaticGray);
	XColor exact;
	XColor color;

	/* Create startup window */
	valuemask = CWOverrideRedirect|CWEventMask;
	attributes.override_redirect = True;
	attributes.event_mask = ExposureMask;
	width = STARTUP_WIDTH+2*BORDER_WIDTH;
	height = STARTUP_HEIGHT+2*BORDER_WIDTH;
	startup_window = XCreateWindow(display, root_window, x, y, width, height,
		1, depth, InputOutput, visual, valuemask, &attributes);
	if (use_color && XAllocNamedColor(display, cmap, "grey76", &exact,
			&color))
		XSetWindowBackground(display, startup_window, color.pixel);
	else
		XSetWindowBackground(display, startup_window, white);

    /* Create graphics context */
    gc = XCreateGC(display, startup_window, (unsigned long)0, NULL);
    XSetForeground(display, gc, black);
    XSetBackground(display, gc, white);
	XSetLineAttributes(display, gc, 1, LineSolid, CapButt, JoinMiter);

	/* Create LLL cursor */
	cursor_pixmap = XCreatePixmapFromBitmapData(display, root_window,
		(char *)lllcursor_bits, lllcursor_width, lllcursor_height,
		(unsigned long)1, (unsigned long)0, (unsigned int)1);
	fg_color.pixel = black;
	XQueryColor(display, cmap, &fg_color);
	bg_color.pixel = white;
	XQueryColor(display, cmap, &bg_color);
	startup_cursor = XCreatePixmapCursor(display, cursor_pixmap, None,
		&fg_color, &bg_color, lllcursor_x_hot, lllcursor_y_hot);
	XFreePixmap(display, cursor_pixmap);
	XDefineCursor(display, startup_window, startup_cursor);

	/* Create startup pixmap */
	startup_pixmap = create_startup_pixmap();

	/* Make window visible and wait for expose event */
	XMapRaised(display, startup_window);
	XNextEvent(display, &event);

	/* Clear window to background color */
	XClearWindow(display, startup_window);

	/* Draw inner border */
	for (i=0; i<2; i++) {
		x = BORDER_WIDTH-1-i;
		y = BORDER_WIDTH-1-i;
		width = STARTUP_WIDTH+2*i+1;
		height = STARTUP_HEIGHT+2*i+1;
		XDrawRectangle(display, startup_window, gc, x, y, width, height);
	}

	/* Copy startup pixmap into startup window */
	XCopyArea(display, startup_pixmap, startup_window, gc, 0, 0, STARTUP_WIDTH,
		STARTUP_HEIGHT, BORDER_WIDTH, BORDER_WIDTH);

	/* Free up memory */
	XFreePixmap(display, startup_pixmap);
	XFreeGC(display, gc);

	/* Make sure everything happens before going on */
	XSync(display, 0);

	/* Want to keep startup dialog visible for some minimum time */
	gettimeofday(&start_time, NULL);
}


/*
 * close_startup_dialog - Destroys the startup dialog after making sure
 *                        that the startup dialog has been visible for
 *                        some minimal time interval.
 */
close_startup_dialog()
{
	struct timeval current_time;
	double fstart_time;
	double fcurrent_time;

	/* Want to keep startup dialog visible for some minimum time */
	while (1) {
		gettimeofday(&current_time, NULL);
		fstart_time = (double)start_time.tv_sec
			+(double)start_time.tv_usec/1000000.;
		fcurrent_time = (double)current_time.tv_sec
			+(double)current_time.tv_usec/1000000.;
		if (fcurrent_time-fstart_time > (double)MIN_STARTUP_TIME)
			break;
		sleep(1);
	}

	/* Get rid of startup window */
	XDestroyWindow(display, startup_window);
	XFreeCursor(display, startup_cursor);
}


/*
 * write_copyright - Write copyright notice to log.
 */
write_copyright()
{
    write_log("LLNL XFTP ");
    write_log(VERSION);
	write_log(",  Lawrence Livermore National Laboratory\n");
	write_log("(c) 1993-1995, The Regents of the University of California\n");
	show_top_of_log();
}


/*
 * cb_about_xftp - Callback to pop up the "About Xftp" dialog.
 */
void
cb_about_xftp(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    /* Clear error flag */
    raise_okflag();

    /* Create about dialog */
    create_about_dialog();
    XtManageChild(about.w_dialog);
}


/*
 * create_about_dialog - Create dialog that displays information about
 *                       XFTP.
 */
create_about_dialog()
{
    /* Create prompt dialog to display information */
    about.w_dialog = XmCreatePromptDialog(w_toplev, "about", NULL, 0);
    XtAddCallback(about.w_dialog, XmNhelpCallback, cb_about_help,
		(XtPointer)NULL);
    XtAddCallback(about.w_dialog, XmNcancelCallback, cb_about_close,
		(XtPointer)NULL);

    /* Don't show prompt dialog's selection label and text */
    XtUnmanageChild(XmSelectionBoxGetChild(about.w_dialog,
        XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(about.w_dialog, XmDIALOG_TEXT));

    /* Don't need OK button */
    XtUnmanageChild(XmSelectionBoxGetChild(about.w_dialog, XmDIALOG_OK_BUTTON));

    /* Add callback for the WM_DELETE_WINDOW protocol */
    add_wm_delete_window_cb(about.w_dialog, cb_about_close, NULL);

    /* Create form for control area */
    about.w_form = XtVaCreateWidget(
        "form",
        xmFormWidgetClass,
        about.w_dialog,
     	NULL
    );

	/* Create frame for pixmap */
	about.w_frame = XtVaCreateManagedWidget(
		"frame",
		xmFrameWidgetClass,
		about.w_form,
		XmNshadowType,			XmSHADOW_IN,
		XmNtopAttachment,		XmATTACH_FORM,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNrightAttachment,		XmATTACH_FORM,
		NULL
	);
   
	/* Create startup pixmap */
	about.startup_pixmap = create_startup_pixmap();
    about.w_startup = XtVaCreateManagedWidget(
        "startup",
        xmLabelWidgetClass,
        about.w_frame,
        XmNlabelType,			XmPIXMAP,
        XmNlabelPixmap,			about.startup_pixmap,
     NULL
    );

    XtManageChild(about.w_form);
}


/*
 * cb_about_close - Callback to close the "About Xftp" dialog.
 */
void
cb_about_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    close_about();
}


/*
 * cb_about_help - Callback to display basic help information.
 */
void
cb_about_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    close_about();
    help_dialog(w_toplev, False, "About LLNL XFTP", about_xftp);
}


/*
 * close_about - Destroys the "About Xftp" dialog.
 */
close_about()
{
    XtDestroyWidget(about.w_dialog);
    XFreePixmap(display, about.startup_pixmap);
}

