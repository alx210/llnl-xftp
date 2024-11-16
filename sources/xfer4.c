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
#include <ctype.h>
#include <Xm/Xm.h>
#include "xftp.h"
#include "xfer.h"
#include "str.h"

int init_local_copy();
int do_local_copy();
int abort_local_copy();

int init_put();
int do_put();
int abort_put();

int init_get();
int do_get();
int abort_get();

int init_remote_copy();
int do_remote_copy();
int abort_remote_copy();

long time_of_day();
double stop_timer();

int (*init_fn[2][2])() = {
	{ init_local_copy,  init_put         },
	{ init_get,         init_remote_copy }
};

int (*do_fn[2][2])() = {
	{ do_local_copy,    do_put         },
	{ do_get,           do_remote_copy }
};

int (*abort_fn[2][2])() = {
	{ abort_local_copy, abort_put         },
	{ abort_get,        abort_remote_copy }
};

static struct timeval start_time;
static struct timeval stop_time;

extern struct xfer_ctrl_block xc;
extern int max_ftp_retries;
extern int diagnostics;
extern int beep_when_ops_done;
extern Display *display;


/*
 * cb_xfer_files - Work proc for transferring files.
 */
cb_xfer_files()
{
	long current_time;
	struct forest_node *tmp_node;
	char *rel_path;
	char *full_path;
	char msg[MAXPATHLEN+40];
	int i;
	int retval;
	double ftime;

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
					if (xc.node[xc.level]->status == UNABLE_TO_CREATE)
						xc.node[xc.level] = xc.node[xc.level]->next;
					else {
						xc.node[xc.level+1] = xc.node[xc.level]->first_child;
						xc.level++;
					}
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
		src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
		set_xfermon_status(STATUS_XFER);
		set_xfermon_name(rel_path);
		set_xfermon_progress((long)-1, (long)-1);
		XSync(display, 0);
		XtFree(rel_path);
		retval = (*init_fn[xc.src_host_type][xc.snk_host_type])();
		switch (retval) {
		case -5:
			xfer_done(True, True);
			return True;
		case -4:
			xfer_done(False, True);
			return True;
		case -3:
			xfer_done(True, False);
			return True;
		case -1:
			src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
			sprintf(msg, "Unable to initialize transfer of %s", rel_path);
			warning_error(msg);
			XtFree(rel_path);
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
			xfer_abort();
			return True;
		}
		retval = (*do_fn[xc.src_host_type][xc.snk_host_type])();
		switch (retval) {
		case -5:
			xfer_done(True, True);
			return True;
		case -4:
			xfer_done(False, True);
			return True;
		case -3:
			xfer_done(True, False);
			return True;
		case -6:    /* Unitree retry logic */
			if (xc.nretries++ < max_ftp_retries)  {
				xc.state = 1;
				src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
				sprintf(msg, "*** Transfer of %s failed.  Will retry.\n",
					rel_path);
				write_log(msg);
				XtFree(rel_path);
				return False;
			}
		case -1:
			src_paths_of_xfer_node(xc.current_node, &rel_path, NULL);
			sprintf(msg, "Unable to complete transfer of %s", rel_path);
			warning_error(msg);
			XtFree(rel_path);
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


/*
 * extract_file_len - Extracts the file length usually returned in a "150"
 *                    ftp reply.  This routine looks for a pattern of the
 *                    form "(xxx bytes)", "(xxx)", or "(xxxk) near the end
 *                    of the reply, where the "xxx" represent digits.  If
 *                    this pattern is found, the file length is returned,
 *                    otherwise -1 is returned.  "reply" is the "150"
 *                    ftp reply to be scanned.
 */
long
extract_file_len(reply)
char *reply;
{
    static char *pat1 = " bytes)";
    int lenpat1 = strlen(pat1);
	static char *pat2 = ")";
	int lenpat2 = strlen(pat2);
	static char *pat3 = "k)";
	int lenpat3 = strlen(pat3);
	char *ptr;
	long len;

    if ((ptr = strrchr(reply, '(')) && isdigit(*++ptr)) {
        len = 0;
        while (*ptr && isdigit(*ptr))
            len = (len*10)+(*ptr++-'0');
        if ((int)(strlen(reply)-strlen(ptr)) >= lenpat1)
            if (strncmp(ptr, pat1, lenpat1) == 0)
                return len;
        if ((int)(strlen(reply)-strlen(ptr)) >= lenpat2)
            if (strncmp(ptr, pat2, lenpat2) == 0)
                return len;
        if ((int)(strlen(reply)-strlen(ptr)) >= lenpat3)
            if (strncmp(ptr, pat3, lenpat3) == 0)
                return len*1024;
    }

	return -1;
}


/*
 * start_timer - Starts the microsecond timer.  Needs to be called at
 *               start of interval to be timed.  Call stop_timer() at
 *               end of interval.
 *
 */
start_timer()
{
    gettimeofday(&start_time, NULL);
}


/*
 * stop_timer - Returns the number of microseconds that have elapsed
 *              since start_timer() was last called.  Undefined results
 *              if start_timer() is not called first.
 */
double
stop_timer()
{
    double fstart_time;
    double fstop_time;

    gettimeofday(&stop_time, NULL);
    fstart_time = (double)start_time.tv_sec+(double)start_time.tv_usec/1000000.;
    fstop_time = (double)stop_time.tv_sec+(double)stop_time.tv_usec/1000000.;
    return fstop_time-fstart_time;
}


/*
 * print_xfer_stats - Prints statistics about a file transfer.  "ftime"
 *                    is the number of seconds the transfer took.  "nbytes"
 *                    is the number of bytes transferred.  Set "nbytes"
 *                    to -1 if file length not known.
 */
print_xfer_stats(ftime, nbytes)
double ftime;
long nbytes;
{
	char msg[120];

	write_log("      ");
	if (nbytes == -1)
		write_log("?");
	else {
		sprintf(msg, "%ld", nbytes);
		write_log(msg);
	}
	sprintf(msg, " bytes transferred in %3.1f seconds", ftime);
	write_log(msg);
	if (nbytes != -1) {
		if (ftime <= 0)
			write_log(" (Very Fast)");
		else {
			sprintf(msg, " (%3.1f Kbytes/s)", (double)nbytes/(ftime*1000.));
			write_log(msg);
		}
	}
	write_log("\n");
}


/*
 * time_of_day - Returns time of day in seconds.
 */
long
time_of_day()
{
	struct timeval now;

    gettimeofday(&now, NULL);
	return (long)(now.tv_sec+(now.tv_usec+500000)/1000000);
}


/*
 * xfer_done - Clean up after prematurely terminating during a file
 *             transfer pass.  Set "lost_src_host" to True if
 *             connection to source host is lost; otherwise set to
 *             False.  Set "lost_snk_host" to True if connection to
 *             sink host is lost; otherwise set to False.
 */
xfer_done(lost_src_host, lost_snk_host)
int lost_src_host;
int lost_snk_host;
{
	/* Hide file transfer dialog */
	hide_xfermon();

	/* Hide abort dialog if it is being used and connection was lost */
	if ((lost_src_host || lost_snk_host) && xfer_abort_requested()) {
		hide_abort_dialog();
		warning_error("File transfer(s) aborted");
	}

	/* Try to restore lost connections and update sink directory list */
	if (lost_src_host)
		lost_connection(xc.src_host);
	if (lost_snk_host)
		lost_connection(xc.snk_host);
	else if (xc.operation != VIEW) {    /* Update sink directory */
		if (update_dir_displays(xc.snk_host, False) < 0) {
			if (!lost_src_host && xfer_abort_requested()) {
				hide_abort_dialog();
				warning_error("File transfer(s) aborted");
			}
			lost_connection(xc.snk_host);
		}
	}

	/* Finish cleaning up */
	release_xfer_forest();
	XtFree(xc.src_host_wd);
	XtFree(xc.snk_host_wd);
	enable_controls(True);
	clear_selected_entries(xc.src_host);
	update_xfer_controls();
	update_host_controls(xc.src_host);
	if (beep_when_ops_done)
		beep();
	restore_prev_cursor();

	/* Hide abort dialog if it is still being used */
	if (!lost_src_host && !lost_snk_host && xfer_abort_requested()) {
		hide_abort_dialog();
		warning_error("File transfer(s) aborted");
	}
}


/*
 * xfer_abort - Abort active file transfer and then clean up.
 */
xfer_abort()
{
	/* Hide file transfer dialog */
	hide_xfermon();

	/* Send abort messages to remote host(s) involved in transfer */
	switch ((*abort_fn[xc.src_host_type][xc.snk_host_type])()) {
	case -5:
		xfer_done(True, True);
		break;
	case -4:
		xfer_done(False, True);
		break;
	case -3:
		xfer_done(True, False);
		break;
	default:
		xfer_done(False, False);
	}
}

