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
#include <errno.h>
#ifdef NEXT
#include <bsd/sys/unistd.h>
#else
#include <unistd.h>
#endif
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include "xftp.h"
#include "logo50.h"
#include "logo64.h"

#define MAXTRIES  10

struct {
	unsigned char *data;
	int width;
	int height;
} icon_image[] = {
	{ &logo64_bits[0], 64, 64 },
	{ &logo50_bits[0], 50, 50 }
};

static Pixmap icon_pixmap;
static int volume = 0;

extern int symbol_mode;
extern struct st_host_info hinfo[];
extern Widget w_toplev;
extern int diagnostics;
extern int need_to_save_prefs;
extern int need_to_save_viewprefs;
extern Widget w_host[];
extern Display *display;
extern int depth;
extern Window root_window;
extern Widget w_xferOpsLabel;
extern Widget w_xferOpsFrame;
extern Widget w_xferModeLabel;
extern Widget w_xferModeFrame;
extern Widget w_dirList[];
extern Widget w_copyButton;
extern Widget w_rCopyButton;
extern Widget w_viewButton;


/*
 * vertical_sb - Returns the vertical scrollbar of the list widget "w_list".
 */
Widget
vertical_sb(widget)
Widget widget;
{
	Widget  w_vertical_sb;

	XtVaGetValues(widget, XmNverticalScrollBar, &w_vertical_sb, NULL);
	return w_vertical_sb;
}


/*
 * cb_disconnect - Callback to disconnect host.
 */
void
cb_disconnect(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

    /* Clear error flag */
    raise_okflag();

	/* Do it */
	disconnect(host);

}


/*
 * disconnect - Disconnects "host".  The indicated host can be either locally
 *              or remotely connected.
 */
disconnect(host)
int host;
{
	switch (hinfo[host].type) {
	case NEITHER:
		return;
	case REMOTE:
		close(hinfo[host].ctrl_fd);
	}

	hinfo[host].type = NEITHER;

    /* Update the display */
    if (update_dir_displays(host, False) < 0)
		fatal_error("Unexpected error disconnecting from host");
	update_xfer_controls();
    update_dirname_menu(host);
	update_dirpath(host);
    update_hostname(host);
    update_host_controls(host);
}


/*
 * beep - Emit a beep.
 */
beep()
{
	XBell(display, volume);
}


/*
 * cb_quit - Callback to quit application.
 */
void
cb_quit(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	/* See if user preferences need to be saved */
	if ((need_to_save_prefs || need_to_save_viewprefs) &&
			question_dialog("Do you want to save the user preferences?")) {
		if (need_to_save_prefs && save_prefs())
			warning_error("Unable to save user preferences.");
		if (need_to_save_viewprefs && save_viewprefs())
			warning_error("Unable to save viewer preferences.");
	}

	/* Get rid of any remaining temporary view directories */
	delete_all_viewer_dirs();

	save_cache();
	exit(0);
}


/*
 * report_client_timeout - Write notice to diagnostics log that the 
 *                         specified remote "host" has timed out.
 */
report_client_timeout(host)
int host;
{
	if (diagnostics >= VERBOSE) {
		if (host == LEFT)
			write_log("Left host: ");
		else
			write_log("Right host: ");
		write_log("Timed out waiting for response from remote host.\n");
	}
}


/*
 * can_create_sinkfile - Returns True if a writable file specifed by
 *                       "snk_path" can be created; else returns False.
 *                       "host" is the host the file is to be created on.
 */
can_create_sinkfile(host, snk_path)
int host;
char *snk_path;
{
	char *snk_dir;

    parse_path(SYS_UNIX, snk_path, &snk_dir, NULL);
    if (access(snk_path, W_OK) < 0)
        if (errno == ENOENT) {   /* File does not exist */
            if (access(snk_dir, W_OK) < 0) {
                report_perror(host, snk_path);
                XtFree(snk_dir);
                return False;
            }
        } else if (errno == EACCES) {    /* Permission denied */
            if (access(snk_dir, W_OK) < 0) {
                report_perror(host, snk_path);
                XtFree(snk_dir);
                return False;
            }
            if (chmod(snk_path, 0600) < 0) {
                report_perror(host, snk_path);
                XtFree(snk_dir);
                return False;
            }
        } else {
            report_perror(host, snk_path);
            XtFree(snk_dir);
            return False;
        }
    XtFree(snk_dir);

	return True;
}


/*
 * strip_off_symbol - Strips symbol off end of string if code is running
 *                    in "symbol mode".  When xftp is in symbol mode, it
 *                    asks the FTP server to append symbols ('/', '*',
 *                    '@', and '=' to directory entries so that the user
 *                    can easily identify file type.  Whether symbols
 *                    are actually stripped depends on the type of
 *                    system (e.g., SYS_UNIX).
 */
strip_off_symbol(system, server, string)
int system;
int server;
char *string;
{
	int len;

	if (symbol_mode) {
		len = strlen(string);
		if (system == SYS_UNIX) {
			switch (string[len-1]) {
			case '/':
			case '*':
			case '@':
			case '=':
				string[len-1] = '\0';
			}
		} else if ((server == SERVER_MAC_NCSATELNET)
				|| (server == SERVER_MAC_VERSATERM)
				|| (server == SERVER_DOS_WINQVT)) {
			if (string[len-1] == '/')
				string[len-1] = '\0';
		}
	}
}


/*
 * enable_controls - If "enable" is FALSE, certain controls that should
 *                   not be used during a file transfer are turned off.
 *                   If "enable" is TRUE, these controls are turned back
 *                   on.
 */
enable_controls(enable)
int enable;
{
    int i;

    /* Enable/disable all file transfer controls */
    XtSetSensitive(w_xferOpsLabel, enable);
    XtSetSensitive(w_xferOpsFrame, enable);
    XtSetSensitive(w_xferModeLabel, enable);
    XtSetSensitive(w_xferModeFrame, enable);

    /* Enable/disable all host controls */
    for (i=0; i<NUMHOSTS; i++) {
        XtSetSensitive(w_host[i], enable);
		if (dirtable_in_use(i))
			enable_dirtable(i, enable);
	}
}


/*
 * create_unique_name - Create a unique file name by, if necessary,
 *                      prefixing name with "." followed by a integer
 *                      that ranges from 1 to 99.  If successful, a
 *                      pointer to the new name is returned.  The
 *                      memory is in a static buffer local to this
 *                      routine.  If unsuccessful, NULL is returned.
 *                      "file_path" should be the full path name of
 *                      the file.
 */
char *
create_unique_name(file_path)
char *file_path;
{
	static char new_path[MAXPATHLEN+1];
	int i;

	/* First see if supplied name is unique */
	if (access(file_path, 0) < 0) {
		if (errno == ENOENT) {
			strcpy(new_path, file_path);
			return new_path;
		} else
			return NULL;
	}

	/* Try concatenating a "." followed by an integer */
	for (i=1; i<100; i++) {
		sprintf(new_path, "%s.%d", file_path, i);
		if (access(new_path, 0) < 0) {
			if (errno == ENOENT)
				return new_path;
			else
				return NULL;
		}
	}

	/* Ran out of suffices */
	return NULL;
}


/*
 * create_icon - Create custom icon for use with Mwm.  This routine will
 *               try to create either a 50x50 icon or a 64x64 icon, depending
 *               on what size the window manager prefers.
 */
create_icon()
{
    XIconSize *list;
    int count;
    Status status;
    int i;
    int j;
    int width;
    int height;

    /* Only create custom icon if window manager is MWM */
    if (!XmIsMotifWMRunning(w_toplev))
        return;

    /* Get window manager's list of preferred icon sizes */
    status = XGetIconSizes(display, root_window, &list, &count);
    if (status == 0)
        return;

    /* See if we can handle one of the preferred sizes */
    for (i=0; i<XtNumber(icon_image); i++)
        for (j=0; j<count; j++) {
            width = icon_image[i].width;
            height = icon_image[i].height;
            if (width >= list[j].min_width &&
                width <= list[j].max_width &&
                (width-list[j].min_width)%list[j].width_inc == 0 &&
                height >= list[j].min_height &&
                height <= list[j].max_height &&
                (height-list[j].min_height)%list[j].height_inc == 0) {
				icon_pixmap = XCreatePixmapFromBitmapData(
			        display,
			        root_window,
			        (char *)icon_image[i].data,
			        icon_image[i].width,
			        icon_image[i].height,
			        BlackPixelOfScreen(XtScreen(w_toplev)),
			        WhitePixelOfScreen(XtScreen(w_toplev)),
			        depth
			    );
                XtVaSetValues(w_toplev, XmNiconPixmap, icon_pixmap, NULL);
                XFree(list);
                return;
            }
        }

    XFree(list);
}


/*
 * add_wm_delete_window_cb - Add callback for the WM_DELETE_WINDOW protocol
 *                           for the shell that is the ancestor of widget.
 */
add_wm_delete_window_cb(widget, callback, client_data)
Widget widget;
XtCallbackProc callback;
XtPointer client_data;
{
	Atom WM_DELETE_WINDOW;
	Widget shell;

	/* First find the dialog's shell widget */
	for (shell = widget; !XtIsShell(shell); shell = XtParent(shell));

	/* Don't want toolkit to handle Close action */
	XtVaSetValues(shell, XmNdeleteResponse, XmDO_NOTHING, NULL);

	/* Install the WM_DELETE_WINDOW atom */
	WM_DELETE_WINDOW = XmInternAtom(display, "WM_DELETE_WINDOW", False);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, callback, client_data);
}


/*
 * center_dialog_over_dialog - Center dialog "w_top_dialog" over
 *                             "w_bottom_dialog.
 */
center_dialog_over_dialog(w_top_dialog, w_bottom_dialog)
Widget w_top_dialog;
Widget w_bottom_dialog;
{
    Dimension width;
    Dimension height;
    Position x;
    Position y;
    Dimension w;
    Dimension h;

    /* Center top dialog over bottom dialog */
    XtVaGetValues(w_top_dialog, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaGetValues(w_bottom_dialog, XmNx, &x, XmNy, &y, XmNwidth, &w,
        XmNheight, &h, NULL);
    XtVaSetValues(w_top_dialog, XmNx, x+(int)(w-width)/2, XmNy,
		y+(int)(h-height)/2, NULL);
}


/*
 * cstring_to_text - Convert compound string "string" to a null-delimited
 *                   string, which is returned.  Call XtFree() to free
 *                   returned string;
 */
char *
cstring_to_text(string)
XmString string;
{
	char *text;

#ifdef XmFONTLIST_DEFAULT_TAG    /* Motif 1.2.x */
	if (!XmStringGetLtoR(string, XmFONTLIST_DEFAULT_TAG, &text))
#else							 /* Motif 1.1.x */
	if (!XmStringGetLtoR(string, XmSTRING_DEFAULT_CHARSET, &text))
#endif
		fatal_error("Trouble in cstring_to_text()");

		return text;
}

/*
 * text_to_cstring - Convert null-delimited string "text" to a compound
 *                   string, which is returned.  Call XmStringFree() to
 *                   free returned string.
 */
XmString
text_to_cstring(text)
char *text;
{
#ifdef XmFONTLIST_DEFAULT_TAG    /* Motif 1.2.x */
	return XmStringCreateLtoR(text, XmFONTLIST_DEFAULT_TAG);
#else                            /* Motif 1.1.x */
	return XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
#endif
}


/*
 * update_xfer_controls - Update the sensitivity of the "Copy" and "Rcopy"
 *                        buttons to reflect connection and selection states.
 */
update_xfer_controls()
{
    XmString copy_label;
    XmString rcopy_label;
    XmString str;
    int sensitivity;

	/* Is a copy possible? */
    if (hinfo[LEFT].type == NEITHER || hinfo[RIGHT].type == NEITHER) {
        sensitivity = False;
        copy_label = XmStringCreateSimple("Copy");
        rcopy_label = XmStringCreateSimple("Rcopy");
    } else {
        if (host_selected(LEFT)) {
            sensitivity = True;
            copy_label = XmStringCreateSimple(">>Copy>>");
            rcopy_label = XmStringCreateSimple(">>Rcopy>>");
        } else if (host_selected(RIGHT)) {
            sensitivity = True;
            copy_label = XmStringCreateSimple("<<Copy<<");
            rcopy_label = XmStringCreateSimple("<<Rcopy<<");
        } else {
            sensitivity = False;
            copy_label = XmStringCreateSimple("Copy");
            rcopy_label = XmStringCreateSimple("Rcopy");
        }
    }

    /* Update "Copy" button label only if it has changed */
    XtVaGetValues(w_copyButton, XmNlabelString, &str, NULL);
    if (XmStringCompare(str, copy_label) == False)
        XtVaSetValues(w_copyButton, XmNlabelString, copy_label, NULL);
    XmStringFree(copy_label);
    XmStringFree(str);   /* Yes, this is necessary */
    XtSetSensitive(w_copyButton, sensitivity);

    /* Update "Rcopy" button label only if it has changed */
    XtVaGetValues(w_rCopyButton, XmNlabelString, &str, NULL);
    if (XmStringCompare(str, rcopy_label) == False)
        XtVaSetValues(w_rCopyButton, XmNlabelString, rcopy_label, NULL);
    XmStringFree(rcopy_label);
    XmStringFree(str);   /* Yes, this is necessary */
    XtSetSensitive(w_rCopyButton, sensitivity);

	/* Adjust sensitivity of "View" button */
	if ((selection_count(LEFT) == 1) || (selection_count(RIGHT) == 1))
		XtSetSensitive(w_viewButton, True);
	else
		XtSetSensitive(w_viewButton, False);
}


/*
 * temp_file_name - Create a unique name for a temporary file in the
 *                  directory defined as P_tmpdir in <stdio.h>.  If the
 *                  environment variable TMPDIR is defined, it is used
 *                  as the directory.  If successful, this function returns
 *                  a pointer to the name, which can be released by calling
 *                  XtFree(); if unsuccessful, NULL is returned.  The name
 *                  will begin with the prefix "xftp".
 */
char *
temp_file_name()
{
	char *dir_path = NULL;
	char *file_path;
	char *temp;
	int pid;
	char pid_string[5];
	char suffix[5];
	int i;
	int j;

	/* Get directory path */
	if ((temp = getenv("TMPDIR")))
		dir_path = XtNewString(temp);
	else
		dir_path = XtNewString(P_tmpdir);

	/* Verify that directory is accessible */
	if (access(dir_path, 0) < 0) {
		XtFree(dir_path);
		return NULL;
	}

	/* Convert PID into string */
	pid = getpid();
	sprintf(pid_string, "%04d", pid%10000);

	/* Try several times to create a unique name */
	for (i=0; i<MAXTRIES; i++) {
		for (j=0; j<4; j++)
			suffix[j] = ((rand()&0xff)%26)+'A';
		suffix[4] = '\0';
		file_path = XtMalloc(strlen(dir_path)+14);
		strcpy(file_path, dir_path);
		if (file_path[strlen(file_path)-1] != '/')
			strcat(file_path, "/");
		strcat(file_path, "xftp");
		strcat(file_path, pid_string);
		strcat(file_path, suffix);
		if (access(file_path, 0) < 0) {
			if (errno == ENOENT) {
				XtFree(dir_path);
				return file_path;
			}
		}
		XtFree(file_path);
	}

	XtFree(dir_path);
	return NULL;
}


/*
 * cmp_symbol_entries - Compare strings "string1" and "string2", but
 *                      ignoring the last character if it is a "type"
 *                      symbol (('@', '*', '=', or '/').  Returns
 *                      negative value if "string1" is less than "string2,
 *                      0 if "string1" equals "string2", and positive
 *                      value if "string1" is greater than "string2".
 */
cmp_symbol_entries(string1, string2)
char *string1;
char *string2;
{
	char *str1 = XtNewString(string1);
	char *str2 = XtNewString(string2);
	int retval;
	int len;

	/* String off symbol from first string */
	len = strlen(str1);
	switch (str1[len-1]) {
	case '@':
	case '*':
	case '=':
	case '/':
		str1[len-1] = '\0';
	}

	/* String off symbol from second string */
	len = strlen(str2);
	switch (str2[len-1]) {
	case '@':
	case '*':
	case '=':
	case '/':
		str2[len-1] = '\0';
	}

	/* Make comparison */
	retval = strcmp(str1, str2);
	XtFree(str1);
	XtFree(str2);
	return retval;
}

