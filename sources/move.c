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
#include "xftp.h"
#include "xfer.h"

struct {
	int host;
	struct entry_link *head;
} move_ctrl;

extern Widget w_dirList[];
extern struct st_host_info hinfo[];
extern int inquire_on_move;
extern XtAppContext app;
extern int beep_when_ops_done;
extern int diagnostics;
extern Widget w_toplev;

int cb_move_entries();
char *merge_paths();
char *equivalent_file_path();
char *get_destination_dir();
char *cstring_to_text();


/*
 * cb_move_selected_entries - Callback to move selected entries.
 */
void
cb_move_selected_entries(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	char *sink_dir;
	XmStringTable selected_items;
	int nselected_items;
	int i;
	struct entry_link *ptr;
	struct entry_link *head;

    /* Clear error flag */
    raise_okflag();

	/* Ask the user if okay to move */
	if (inquire_on_move) {
		if (!verify_selection(host, "Do you really want to move these items?"))
			return;
	}

	/* Get destination directory */
	sink_dir = get_destination_dir(host, hinfo[host].type, hinfo[host].move_wd,
		hinfo[host].wd, w_toplev);
	if (sink_dir) {
		if (hinfo[host].move_wd)
			XtFree(hinfo[host].move_wd);
		hinfo[host].move_wd = sink_dir;
	} else
		return;


	/* Check for identical source and sink directories */
	if (strcmp(hinfo[host].move_wd, hinfo[host].wd) == 0) {
		warning_error("Source and destination directories are the same");
		return;
	}

	/* This might take some time */
	use_busy_cursor();

	/* Pop up multipurpose monitor */
	show_mp_monitor("Now Moving:");
	clear_mp_monitor();

	/* Disable controls */
	enable_controls(False);

	/* Form linked list of selected entries */
    XtVaGetValues(
        w_dirList[host],
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );
	head = NULL;
	for (i=nselected_items-1; i>=0; i--) {
		ptr = XtNew(struct entry_link);
		ptr->entry = cstring_to_text(selected_items[i]);
        strip_off_symbol(hinfo[host].system, hinfo[host].server, ptr->entry);
        ptr->next = head;
        head = ptr;
	}

	/* Set up move control block */
	move_ctrl.host = host;
	move_ctrl.head = head;

	/* Perform move */
	XtAppAddWorkProc(app, (XtWorkProc)cb_move_entries, NULL);
}


/*
 * cb_move_entries - Work proc to actually move directory entries.
 */
cb_move_entries()
{
    struct entry_link *ptr;
	char *src_path;
	char *snk_path;
	int retval;
	char msg[MAXPATHLEN+40];
	char *temp_path;

    /* Are we done? */
    if (move_ctrl.head == NULL)
        goto done;

	/* Did user push abort button? */
	if (mp_abort_requested()) {
		hide_abort_dialog();
	    warning_error("Move(s) aborted");
		goto done;
	}

	/* Let user know where we are */
	update_mp_monitor(move_ctrl.head->entry);

	/* Create source and sink paths */
	temp_path = merge_paths(hinfo[move_ctrl.host].system,
		hinfo[move_ctrl.host].wd, move_ctrl.head->entry);
	src_path = equivalent_file_path(hinfo[move_ctrl.host].system, temp_path);
	XtFree(temp_path);
	temp_path = merge_paths(hinfo[move_ctrl.host].system,
		hinfo[move_ctrl.host].move_wd, move_ctrl.head->entry);
	snk_path = equivalent_file_path(hinfo[move_ctrl.host].system, temp_path);
	XtFree(temp_path);

	/* Move entry */
	if (hinfo[move_ctrl.host].type == LOCAL)
		retval = local_rename(move_ctrl.host, src_path, snk_path);
	else
		retval = remote_rename(move_ctrl.host, src_path, snk_path);
	XtFree(src_path);
	XtFree(snk_path);
	switch (retval) {
	case -3:
		goto lost;
	case -1:
		sprintf(msg, "Unable to move %s", move_ctrl.head->entry);
		warning_error(msg);
		break;
	case 0:
		if (diagnostics >= NORMAL) {
			sprintf(msg, "*** Successfully moved:  %s\n",move_ctrl.head->entry);
			write_log(msg);
		}
	}
	ptr = move_ctrl.head;
	move_ctrl.head = move_ctrl.head->next;
	XtFree(ptr->entry);
	XtFree((char *)ptr);
	return False;

done:

	release_linked_list(&move_ctrl.head);
	hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
	enable_controls(True);
    if (update_dir_displays(move_ctrl.host, False) < 0)
        goto lost;
    update_xfer_controls();
    update_host_controls(move_ctrl.host);
    if (beep_when_ops_done)
        beep();
    restore_prev_cursor();
    return True;

lost:

    release_linked_list(&move_ctrl.head);
    hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
    enable_controls(True);
    clear_selected_entries(move_ctrl.host);
    restore_prev_cursor();
    lost_connection(move_ctrl.host);
    if (beep_when_ops_done)
        beep();
    return True;
}

