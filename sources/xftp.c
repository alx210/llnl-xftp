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
#include <signal.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include "xftp.h"

#ifndef SIGCLD
#define SIGCLD  SIGCHLD
#endif

Widget w_toplev;
struct st_host_info hinfo[NUMHOSTS];
XtAppContext app;
Display *display;
int screen;
int depth;
Visual *visual;
Window root_window;

void ResizeDirtable();
void DirtableMouse();
int cb_create_dialogs();
int create_rconnect_dialog();
int create_aconnect_dialog();
int create_verify_dialog();
int create_prefs_dialog();
int create_xfermon_dialog();
int create_mp_monitor_dialog();
int create_changedir_dialog();
int create_dirtable_dialog();
int create_dirlong_dialog();
void cb_quit();
void reapchild();

static XtActionsRec actions[] = {
	{"ResizeDirtable", ResizeDirtable},
	{"DirtableMouse", DirtableMouse}
};

static struct {
	int (*function)();
	int uses_host_arg;
} dialog_creation[] = {
	{ create_rconnect_dialog,		False },
	{ create_aconnect_dialog,		False },
	{ create_verify_dialog,			False },
	{ create_prefs_dialog,			False },
	{ create_xfermon_dialog,		False },
	{ create_mp_monitor_dialog,		False },
	{ create_changedir_dialog,		False },
	{ create_dirtable_dialog,		True  },
	{ create_dirlong_dialog,		True  },
	{ NULL,							0     }
};

static void (*saved_sigcld_handler)();
String *get_fallbacks();

extern Widget w_bottomForm;
extern int auto_local_login[];
extern Widget w_main;


/*
 * main - Initialize and run.
 */
main(argc, argv)
int argc;
char **argv;
{
	int i;
	int use_color = False;
	String *fallbacks;

	/* Initialize SOCKS */
#ifdef SOCKS
	SOCKSinit(argv[0]);
#endif

	/* Verify that environment variable HOME is set */
	if (getenv("HOME") == NULL)
		fatal_error("Environment variable HOME not set");

	/* Initialize preferences from .xftprc */
	initialize_prefs();
	construct_prefs_filename();
	read_prefs_from_file();

	/* Initialize viewer preferences from .xftpview */
	construct_viewprefs_filename();
	read_viewprefs_from_file();

	/* Initialize directory cache from .xftpcache */
	construct_cache_filename();
	read_cache_from_file();

	/* Initialize host information table */
	for (i=0; i<NUMHOSTS; i++) {
		hinfo[i].type = NEITHER;
		hinfo[i].system = SYS_UNIX;
		hinfo[i].wd = XtNewString("");
		hinfo[i].username = XtNewString("");
		hinfo[i].hostname = XtNewString("");
		hinfo[i].move_wd = NULL;
		hinfo[i].password = NULL;
		hinfo[i].account = NULL;
	}

	/* Should fallback resources contain color information? */
	for (i=0; i<argc; i++)
		if (!strcmp(argv[i], "-color")) {
			use_color = True;
			break;
		}
	fallbacks = get_fallbacks(use_color);

    /* Initialize the toolkit and create the toplevel shell */
    w_toplev = XtVaAppInitialize(&app, "XFtp", NULL, 0,
#if XtSpecificationRelease > 4
        (int *)&argc,
#else
        (Cardinal *)&argc,
#endif
        argv, fallbacks, NULL);

    /* Get the display and screen characteristics */
    display = XtDisplay(w_toplev);
    screen = XDefaultScreen(display);
    depth = XDefaultDepth(display, screen);
    visual = XDefaultVisual(display, screen);
    root_window = XRootWindowOfScreen(XtScreen(w_toplev));

    /* Add callback for the WM_DELETE_WINDOW protocol */
    add_wm_delete_window_cb(w_toplev, cb_quit, NULL);

	/* Create main window */
	create_main_window();

	/* Make widgets "real" */
	XtRealizeWidget(w_toplev);
	add_dialog_to_list(w_toplev);

	/* Kludge to enable user to make lower pane in main window small */
	XtVaSetValues(w_bottomForm, XmNpaneMinimum, 1, NULL);

	/* Initialize controls */
	for (i=0; i<NUMHOSTS; i++) {
		if (update_dir_displays(i, False) < 0)
			fatal_error("Major problem during initialization");
		update_hostname(i);
		update_host_controls(i);
	}
	update_xfer_controls();

	/* Automatically "log onto" local host if specified by user */
	for (i=0; i<NUMHOSTS; i++)
		if (auto_local_login[i])
			connect_to_local(i);

	/* Ignore signals from broken pipes (and sockets) */
	signal(SIGPIPE, SIG_IGN);

	XtAppAddActions(app, actions, XtNumber(actions));

	/* Create custom icon */
	create_icon();

	/* Display copyright notice */
	write_copyright();

	/* Create often-used dialogs in background */
	XtAppAddWorkProc(app, (XtWorkProc)cb_create_dialogs, NULL);

	/* Will need to reap children */
	signal(SIGCLD, reapchild);

	/* Turn over control */
	XtAppMainLoop(app);
}


/*
 * fatal_error - Writes string "msg" to standard error and then terminates.
 */
fatal_error(msg)
char *msg;
{
    fprintf(stderr, "*** FATAL ERROR: %s\n", msg);
    exit(-1);
}


/*
 * warning_error - Writes string "msg" to diagnostics log and prepends
 *                 a warning message.
 */
warning_error(msg)
char *msg;
{
	char *buf;

	buf = XtMalloc(strlen(msg)+30);
	sprintf(buf, "*** WARNING: %s\n", msg);
    write_log(buf);
	XtFree(buf);
	raise_errorflag();
	beep();
}


/*
 * cb_create_dialogs - Work procedure to create often-used dialogs in
 *                     the background.
 */
cb_create_dialogs()
{
	static int indx = 0;
	int i;

	if (dialog_creation[indx].function) {
		if (dialog_creation[indx].uses_host_arg)
			for (i=0; i<NUMHOSTS; i++)
				dialog_creation[indx].function(i);
		else
			dialog_creation[indx].function();
		indx++;
		return False;
	} else
		return True;
}


/*
 * reapchild - Reap the child file viewer process in order to free up the
 *             temporary directory and file(s).
 */
void
reapchild(signo)
int signo;
{
	int pid = iwait(NULL);

	if (pid != -1)
		mark_viewdir_for_removal(pid);

	/* Reset signal handler for AIX, IRIX, etc. */
	signal(SIGCLD, reapchild);
	
}


/*
 * system_with_preserve_sigcld - Same as standard function, system(), but
 *                               with some changes to preserve the SIGCLD
 *                               signal handling.
 */
system_with_preserve_sigcld(string)
char *string;
{
	void (*save_fn)();
	int retval;

	save_fn = signal(SIGCLD, SIG_DFL);
	retval = system(string);
	signal(SIGCLD, save_fn);

	return retval;
}


/*
 * save_sigcld_handler - Save current signal handler for SIGCLD.
 */
save_sigcld_handler()
{
	saved_sigcld_handler = signal(SIGCLD, SIG_DFL);
}


/*
 * restore_sigcld_handler - Restore signal handler for SIGCLD that was
 *                          saved by save_sigcld_handler().
 */
restore_sigcld_handler()
{
	signal(SIGCLD, saved_sigcld_handler);
}

