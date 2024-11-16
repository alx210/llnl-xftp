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
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#include "xftp.h"
#include "str.h"

static Widget w_saveLogDialog;

static char *help_save_diagnostics_log[] = {
    "This dialog enables you to write the contents of the",
    "diagnostics window to the file specified in the \"Log File",
    "Name\" field.  If the file already exists, it will be",
    "overwritten.  The default file name is set by the user",
    "preference INITIAL LOGFILE NAME.\n",
    "\n",
    "Click on the OK button to perform the operation and",
    "remove the dialog.  Click on the CANCEL button to remove",
    "the dialog without performing the operation.",
    NULL
};


extern Widget w_log;
extern int diagnostics;
extern Widget w_toplev;
extern char *initial_logfile_name;
extern Display *display;
extern int print_password_in_debug;

#if !defined(__FreeBSD__) && !defined(__MACHTEN_PPC__) && !defined(__linux__)
extern int sys_nerr;
extern char *sys_errlist[];
#endif

void cb_save_log_ok();
void cb_help_save_diagnostics_log();


/*
 * write_log - Write the string "msg" to the log window.
 */
write_log(msg)
char *msg;
{
    XmTextInsert(w_log, XmTextGetLastPosition(w_log), msg);
    XmTextShowPosition(w_log, XmTextGetLastPosition(w_log));
	XFlush(display);
}


/*
 * cb_clear_log - Callback to clear the log window.
 */
void
cb_clear_log(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	/* Clear error flag */
	raise_okflag();

	XmTextSetString(w_log, "");
}


/*
 * report_perror - Write "msg" to log followed by error message corresponding
 *                 to "errno".  "host" is the host identifier.
 */
report_perror(host, msg)
int host;
char *msg;
{
	char buf[10];
	int saved_errno = errno;

	switch (host) {
	case LEFT:
        write_log("Left host:  ");
		break;
	case RIGHT:
        write_log("Right host: ");
	}

	write_log("(Local) ");
	if (msg != NULL && strlen(msg) != 0) {
		write_log(msg);
		write_log(" - ");
	}
	#if !defined(__FreeBSD__) && !defined(__MACHTEN_PPC__) && !defined(__linux__)
	if (saved_errno < sys_nerr) {
		write_log(sys_errlist[saved_errno]);
		write_log("\n");
	} else {
		sprintf(buf, "errno = %d\n", saved_errno);
		write_log(buf);
	}
	#else
	write_log(strerror(saved_errno));
	write_log("\n");
	#endif
}


/*
 * print_ftp_reply - Write the string "reply" to the log window with a
 *                   "host" designator.
 */
print_ftp_reply(host, reply)
int host;
char *reply;
{
    if (host == LEFT)
        write_log("Left host:  ");
    else
        write_log("Right host: ");
    if (diagnostics == DEBUG)
        write_log("<-- ");
    write_log(reply);
}


/*
 * print_ftp_cmd - Write the string "cmd" to the log window with a "host"
 *                 designator.
 */
print_ftp_cmd(host, cmd)
int host;
char *cmd;
{
	char *temp = XtNewString(cmd);
	char *q;

	/* Identify host */
    if (host == LEFT)
        write_log("Left host:  ");
    else
        write_log("Right host: ");
    write_log("--> ");

	/* Should the password be displayed? */
	if (!print_password_in_debug && (q = strstr(temp, "PASS")))
		q[4] = '\0';

    write_log(temp);
    write_log("\n");
	XtFree(temp);
}


/*
 * cb_save_log - Callback to save the contents of the log window into
 *               a disk file.
 */
void
cb_save_log(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{

    /* Clear error flag */
    raise_okflag();

    /* Create "Save Log" dialog */
    create_save_log_dialog();

    /* Pop up dialog */
    XtManageChild(w_saveLogDialog);
    add_dialog_to_list(w_saveLogDialog);
}


/*
 * create_save_log_dialog - Create the "Save Log" dialog.
 */
create_save_log_dialog()
{
	static int initialized = False;
    Widget widget;
	char *buf;
	char *homedir;

    /* Create dialog only once */
    if (initialized)
        return;
    initialized = True;

    /* Create file selection dialog to get log file name */
    w_saveLogDialog = XmCreatePromptDialog(w_toplev, "saveLog", NULL, 0);
    XtAddCallback(w_saveLogDialog, XmNokCallback, cb_save_log_ok,
		(XtPointer)NULL);
	XtAddCallback(w_saveLogDialog, XmNhelpCallback,
		cb_help_save_diagnostics_log, (XtPointer)NULL);

	/* Propose path for logfile */
	widget = XmSelectionBoxGetChild(w_saveLogDialog, XmDIALOG_TEXT);
	if (strlen(initial_logfile_name) && initial_logfile_name[0] == '/')
		XmTextSetString(widget, initial_logfile_name);
	else {
		homedir = getenv("HOME");
		buf = XtMalloc(strlen(homedir)+strlen(initial_logfile_name)+2);
		strcpy(buf, homedir);
		strcat(buf, "/");
		strcat(buf, initial_logfile_name);
		XmTextSetString(widget, buf);
		XtFree(buf);
	}
}


/*
 * cb_save_log_ok - This callback actually saves the log file.
 */
void
cb_save_log_ok(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	char *logfile_name;
	FILE *fp;
	char *buf;
	char msg[MAXPATHLEN+25];

    /* Get logfile name */
    logfile_name = XmTextGetString(XmSelectionBoxGetChild(w_saveLogDialog,
        XmDIALOG_TEXT));

	/* Open file for writing */
    if ((fp = fopen(logfile_name, "w")) == NULL) {
		sprintf(msg, "Trouble saving log to %s\n", logfile_name);
		warning_error(msg);
		return;
	}

    /* Get copy of log buffer */
    buf = XmTextGetString(w_log);

    /* Write buffer to file */
    fprintf(fp, "%s", buf);
    fclose(fp);

    /* Free memory */
    XtFree(buf);
    XtFree(logfile_name);
}


/*
 * cb_help_save_diagnostics_log - Callback to display a help message for
 *                                the "Save Diagnostics Log" dialog.
 */
void
cb_help_save_diagnostics_log(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    help_dialog(w_saveLogDialog, False, "Save Diagnostics Log",
		help_save_diagnostics_log);
}


/*
 * show_top_of_log - Scroll to top of log.
 */
show_top_of_log()
{
	XtVaSetValues(w_log, XmNtopCharacter, 0, NULL);
}

