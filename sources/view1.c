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

#include <Xm/ToggleB.h>
#include "xftp.h"
#include "xfer.h"
#include "str.h"

int init_local_copy();
int do_local_copy();
int abort_local_copy();

int init_get();
int do_get();
int abort_get();

static int (*init_fn[2])() =  { init_local_copy,    init_get  };
static int (*do_fn[2])() =    { do_local_copy,      do_get    };

char *temp_file_name();
extern Widget w_dirList[];
extern Widget w_ascii;
char *cstring_to_text();
int cb_view_xfer_file();
double stop_timer();

extern struct st_host_info hinfo[];
extern struct xfer_ctrl_block xc;
extern XtAppContext app;
extern Display *display;
extern int diagnostics;


/*
 * cb_view_selected_file - Callback to initialize file transfer of selected
 *                         file.
 */
void
cb_view_selected_file(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int xfer_mode;
	int src_host;
	int snk_host;
	char *q;
	XmStringTable selected_items;
	int nselected_items;
	struct forest_node *ptr;

	/* Clear error flag */
	raise_okflag();

	/* Determine source host */ 
	if (host_selected(LEFT))
		src_host = LEFT;
	else
		src_host = RIGHT;

	/* Make sink host same as source for error reporting */
	snk_host = src_host;

	/* This might take some time */
	use_busy_cursor();

	/* Set transfer mode */
	if (XmToggleButtonGetState(w_ascii))
		xfer_mode = ASCII;
	else
		xfer_mode = BINARY;
	if (hinfo[src_host].type == REMOTE)
		if (ftp_type(src_host, xfer_mode) < 0) {
			restore_prev_cursor();
			lost_connection(src_host);
			return;
		}

	/* Disable controls */
	enable_controls(False);

	/* Get selected entry */
	XtVaGetValues(
		w_dirList[src_host],
		XmNselectedItemCount, &nselected_items,
		XmNselectedItems,     &selected_items,
		NULL
	);
	if (nselected_items != 1)
		fatal_error("Bug in cb_view_selected_file()");  /* Sanity check */
	ptr = XtNew(struct forest_node);
	ptr->parent = NULL;
	ptr->next = NULL;
	ptr->first_child = NULL;
	ptr->status = NOT_PROCESSED;
	ptr->position = AT_START_NODE;
	ptr->entry = cstring_to_text(selected_items[0]);
	strip_off_symbol(hinfo[src_host].system, hinfo[src_host].server,ptr->entry);
	ptr->type = TYPE_FILE;
	if (hinfo[src_host].system == SYS_VMS) {
		if ((q = strstr(ptr->entry, ".dir")))
			*q = '\0';   /* Shave off ".dir;n" */
	}

	/* Set up file control block */
	xc.operation = VIEW;
	xc.mode = xfer_mode;
	xc.level = 0;
	xc.state = 0;
	xc.nretries = 0;
	xc.src_host = src_host;
	xc.snk_host = snk_host;
	xc.src_host_type = hinfo[src_host].type;
	xc.src_host_system = hinfo[src_host].system;
	xc.src_host_server = hinfo[src_host].server;
	xc.src_host_wd = XtNewString(hinfo[src_host].wd);
	xc.snk_host_type = LOCAL;
	xc.snk_host_system = SYS_UNIX;
	xc.snk_host_wd = XtNewString("");
	xc.forest = ptr;
	xc.s0ptr = ptr;
	xc.node[0] = ptr;
	xc.ignore_create_dir_errors = False;
	xc.nstages = 0;
	xc.more_files = True;

	/* Transfer files */
	show_xfermon();
	set_xfermon_status(STATUS_PASS1);
	set_xfermon_name_label("File Name:");
	set_xfermon_progress((long)-1, (long)-1);
	XtAppAddWorkProc(app, (XtWorkProc)cb_view_xfer_file, NULL);
}


/*
 * cb_view_xfer_file - Work proc for transferring and viewing file.
 */
cb_view_xfer_file()
{
	long current_time;
	struct forest_node *tmp_node;
	char *rel_path;
	char *full_path;
	char msg[MAXPATHLEN+40];
	int i;
	int retval;
	double ftime;
	char *temp_ptr;
	char *snk_path;
	char *q;

	/* Perform next step of file transfers */
	switch (xc.state) {
	case 0:   /* No xfer in progress */
		if (xfer_abort_requested()) {
			xfer_done(False, False);
			return True;
		}
		if ((xc.nstages == MAXSTAGES) || (xc.nstages && !xc.more_files)) {
			src_paths_of_xfer_node(xc.staged_file[0].node, &rel_path, NULL);
			set_xfermon_status(STATUS_WAIT);
			set_xfermon_name(rel_path);
			set_xfermon_progress((long)-1, (long)-1);
			XSync(display, 0);
			XtFree(rel_path);
			current_time = time_of_day();
			if (current_time < xc.staged_file[0].time+STAGE_DELTA) {
				sleep(DELTA);
				return False;
			}
			src_paths_of_xfer_node(xc.staged_file[0].node, NULL, &full_path);
			retval = stage_unitree_file(xc.src_host, full_path);
			XtFree(full_path);
			switch (retval) {
			case -3:
				xfer_done(True, False);
				return True;
			case -1:
				src_paths_of_xfer_node(xc.staged_file[0].node, &rel_path, NULL);
				sprintf(msg, "Unable to initialize transfer of %s", rel_path);
				XtFree(rel_path);
				warning_error(msg);
				for (i=1; i<xc.nstages; i++) {
					xc.staged_file[i-1].node = xc.staged_file[i].node;
					xc.staged_file[i-1].time = xc.staged_file[i].time;
				}
				xc.nstages--;
				break;
			case 0:
				xc.current_node = xc.staged_file[0].node;
				for (i=1; i<xc.nstages; i++) {
					xc.staged_file[i-1].node = xc.staged_file[i].node;
					xc.staged_file[i-1].time = xc.staged_file[i].time;
				}
				xc.nstages--;
				xc.state = 1;
				break;
			case 1:
				tmp_node = xc.staged_file[0].node;
				for (i=1; i<xc.nstages; i++) {
					xc.staged_file[i-1].node = xc.staged_file[i].node;
					xc.staged_file[i-1].time = xc.staged_file[i].time;
				}
				xc.staged_file[xc.nstages-1].node = tmp_node;
				xc.staged_file[xc.nstages-1].time = current_time;
			}
			return False;
		} else if (xc.more_files) {
			while (1) {
				if (xc.node[xc.level] == NULL) {
					xc.level--;
					if (xc.level < 0) {
						xc.more_files = False;
						return False;
					}
					xc.node[xc.level] = xc.node[xc.level]->next;
				} else if (xc.node[xc.level]->type == TYPE_DIR) {
					xc.node[xc.level+1] = xc.node[xc.level]->first_child;
					xc.level++;
				} else if (xc.node[xc.level]->type == TYPE_FILE) {
					if ((xc.src_host_server == SERVER_UNIX_UNITREE) ||
							(xc.src_host_server == SERVER_UNIX_NSL_UNITREE)) {
						current_time = time_of_day();
						src_paths_of_xfer_node(xc.node[xc.level], &rel_path,
							&full_path);
						set_xfermon_status(STATUS_STAGE);
						set_xfermon_name(rel_path);
						set_xfermon_progress((long)-1, (long)-1);
						XSync(display, 0);
						retval = stage_unitree_file(xc.src_host, full_path);
						XtFree(rel_path);
						XtFree(full_path);
						switch (retval) {
						case -3:
							xfer_done(True, False);
							return True;
						case -1:
							src_paths_of_xfer_node(xc.node[xc.level], &rel_path,
								NULL);
							sprintf(msg, "Unable to initialize transfer of %s",
								rel_path);
							XtFree(rel_path);
							warning_error(msg);
							xc.node[xc.level] = xc.node[xc.level]->next;
							return False;
						case 1:
							xc.staged_file[xc.nstages].node = xc.node[xc.level];
							xc.staged_file[xc.nstages].time = current_time;
							xc.nstages++;
							xc.node[xc.level] = xc.node[xc.level]->next;
							return False;
						}
					}
					xc.current_node = xc.node[xc.level];
					xc.node[xc.level] = xc.node[xc.level]->next;
					xc.state = 1;
					return False;
				} else
					fatal_error("Bug in cb_xfer_files()");
			}
		} else {
			xfer_done(False, False);
			return True;
		}
	case 1:   /* Initialize file transfer */
		if (xfer_abort_requested()) {
			xfer_done(False, False);
			return True;
		}
		if ((temp_ptr = temp_file_name()) == NULL) {
			src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
			sprintf(msg, "Can't create temp directory for %s.", rel_path);
			warning_error("Unable to create temporary directory for viewer");
			XtFree(rel_path);
			xc.state = 0;
			xc.nretries = 0;
			return False;
		}
		XtFree(xc.snk_host_wd);
		xc.snk_host_wd = temp_ptr;
		src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
		set_xfermon_status(STATUS_XFER);
		set_xfermon_name(rel_path);
		set_xfermon_progress((long)-1, (long)-1);
		XSync(display, 0);
		XtFree(rel_path);
		retval = (*init_fn[xc.src_host_type])();
		switch (retval) {
		case -3:
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
			delete_file_and_its_dir(snk_path);
			XtFree(snk_path);
			xfer_done(True, False);
			return True;
		case -1:
			src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
			sprintf(msg, "Unable to initialize transfer of %s", rel_path);
			warning_error(msg);
			XtFree(rel_path);
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
			delete_file_and_its_dir(snk_path);
			XtFree(snk_path);
			xc.state = 0;
			xc.nretries = 0;
			break;
		case 0:
			set_xfermon_progress(xc.file_len, xc.file_index);
			XSync(display, 0);
			start_timer();
			xc.state = 2;
		}
		return False;
	case 2:  /* File transfer in progress */
		if (xfer_abort_requested()) {
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
			xfer_abort();
			delete_file_and_its_dir(snk_path);
			XtFree(snk_path);
			return True;
		}
		retval = (*do_fn[xc.src_host_type])();
		switch (retval) {
		case -3:
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
			delete_file_and_its_dir(snk_path);
			XtFree(snk_path);
			xfer_done(True, False);
			return True;
		case -1:
			src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
			sprintf(msg, "Unable to complete transfer of %s", rel_path);
			warning_error(msg);
			XtFree(rel_path);
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
			delete_file_and_its_dir(snk_path);
			XtFree(snk_path);
			xc.state = 0;
			xc.nretries = 0;
			return False;
		case 0:   /* Transfer complete */
			set_xfermon_progress(xc.file_len, xc.file_index);
			XSync(display, 0);
			ftime = stop_timer();
			if (diagnostics >= NORMAL) {
				src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
				sprintf(msg, "*** Successfully transferred file:  %s\n",
					rel_path);
				write_log(msg);
				XtFree(rel_path);
				print_xfer_stats(ftime, xc.file_len);
			}
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
			if (xc.src_host_system == SYS_VMS) {
				if ((q = strrchr(snk_path, ';')))
					*q = '\0';
			}
			view_file(xc.src_host, snk_path);
			XtFree(snk_path);
			xc.state = 0;
			xc.nretries = 0;
			return False;
		case 1:   /* More to transfer */
			set_xfermon_progress(xc.file_len, xc.file_index);
			return False;
		}
	}

	return True;   /* Make gcc compiler happy (shouldn't get here) */
}

