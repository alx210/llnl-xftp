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

#include <Xm/SelectioB.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include "xftp.h"
#include "xfer.h"
#include "list.h"
#include "str.h"

struct del_ctrl_block del_ctrl;

extern struct st_host_info hinfo[];
extern int beep_when_ops_done;
extern Widget w_toplev;
extern int diagnostics;
extern int inquire_on_delete;
extern Widget w_dirList[];
extern XtAppContext app;

char *merge_paths();
char *links_to_path();
int cb_delete_entries();
char *cstring_to_text();


/*
 * cb_delete - Callback that deletes selected file(s).
 */
void
cb_delete(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	init_delete(host, False);
}


/*
 * cb_rdelete - Callback that recursively deletes selected entries.
 */
void
cb_rdelete(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	init_delete(host, True);
}


/*
 * init_delete - Initialize delection(s).
 */
init_delete(host, recursive)
int host;
int recursive;
{
	XmStringTable selected_items;
	int nselected_items;
	int i;
	struct entry_link *head;
	struct entry_link *ptr;
	int retval;

	/* Clear error flag */
	raise_okflag();

	/* Ask the user if okay to delete? */
	if (inquire_on_delete) {
		if (recursive)
			retval = verify_selection(host,
				"Do you really want to recursively delete these items?");
		else
			retval = verify_selection(host,
				"Do you really want to delete these items?");
		if (retval == False)
			return;
	}

	/* This might take some time */
	use_busy_cursor();

	/* Pop up deletion monitor */
	show_mp_monitor("Now Deleting:");
	clear_mp_monitor();

	/* Disable controls */
	enable_controls(False);

	/* Get list of entries to delete */
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

    /* Set up deletion control block */
    del_ctrl.recursive = recursive;
    del_ctrl.level = 0;
    del_ctrl.host = host;
    del_ctrl.head[0] = head;

    /* Perform deletion */
    XtAppAddWorkProc(app, (XtWorkProc)cb_delete_entries, NULL);
}


/*
 * clean_up_del_ctrl - Cleans up the deletion control block data structure.
 *                     This routine should be called after servicing a
 *                     deletion request. 
 */
clean_up_del_ctrl()
{
	int i;

	if (del_ctrl.rel_path != NULL) {
		XtFree(del_ctrl.rel_path);
		del_ctrl.rel_path = NULL;
	}

	if (del_ctrl.full_path != NULL) {
		XtFree(del_ctrl.full_path);
		del_ctrl.full_path = NULL;
	}

	for(i=0; i<=del_ctrl.level; i++)
		release_linked_list(&del_ctrl.head[i]);
}


/*
 * cb_delete_entries - Work procedure for executing deletion request.
 *                     State information is kept in "del_ctrl", the
 *                     deletion control block data structure.
 */
cb_delete_entries()
{
	struct entry_link *ptr;
	int host = del_ctrl.host;
	int host_type = hinfo[del_ctrl.host].type;
	struct sl_struct *list;
	int retval;
	int len;
	int i;
	int is_a_directory;
	char msg[MAXPATHLEN+40];
	int already_handled_dir = False;
	char **rel_path_links;
	char *temp;
	char *q;

	del_ctrl.full_path = NULL;
	del_ctrl.rel_path = NULL;
		
	/* Did user push abort button? */
	if (mp_abort_requested())
		goto abort;

	if (del_ctrl.head[del_ctrl.level] == NULL) {
		if (del_ctrl.level == 0)
			goto done;
		else {
			del_ctrl.level--;
			already_handled_dir = True;
		}
	}

	/* Build relative path name */
	rel_path_links = (char **)XtMalloc(sizeof(char *)*(del_ctrl.level+2));
	for (i=0; i<=del_ctrl.level; i++)
		rel_path_links[i] = XtNewString(del_ctrl.head[i]->entry);
	rel_path_links[del_ctrl.level+1] = NULL;
	del_ctrl.rel_path = links_to_path(hinfo[host].system, rel_path_links,
		del_ctrl.level+1);
	release_path_links(rel_path_links);

	/* Build full path name */
	del_ctrl.full_path = merge_paths(hinfo[host].system, hinfo[host].wd,
		del_ctrl.rel_path);

	if (del_ctrl.recursive && !already_handled_dir) {
		/* Test for directory by trying to cd to it */
		is_a_directory = False;
		if (host_type == LOCAL) {
			if (local_cd(host, del_ctrl.full_path, False) == 0)
				is_a_directory = True;
		} else
			switch (remote_cd(host, del_ctrl.full_path, True, False)) {
			case -3:
				goto lost;
			case 0:
				is_a_directory = True;
			}
		if (is_a_directory) {
			/* Get directory list */
			if (host_type == LOCAL) {
				if (local_ls(del_ctrl.full_path, &list, False) != 0) {
					warning_error("Unable to get directory list");
					goto done;
				}
			} else {
				switch (remote_ls(host, del_ctrl.full_path, &list, False)) {
				case -3:
					goto lost;
				case -1:
					warning_error("Unable to get directory list");
					goto done;
				}
			}
			/* Add directory list to new level */
			del_ctrl.level++;
			if (del_ctrl.level == MAXLEVELS) {
				warning_error("Deletion recurses too deeply - aborted.");
				goto done;
			}
			del_ctrl.head[del_ctrl.level] = NULL;
			for (i=list->nentries-1; i>=0; i--) {
				ptr = XtNew(struct entry_link);
				ptr->entry = XtNewString(list->entries[i]);
				ptr->next = del_ctrl.head[del_ctrl.level];
				del_ctrl.head[del_ctrl.level] = ptr;
			}
			release_array_list(list);
		    XtFree(del_ctrl.full_path);
		    XtFree(del_ctrl.rel_path);
			return False;
		}
	}

	/* Delete next directory entry */
	update_mp_monitor(del_ctrl.rel_path);
	if (hinfo[host].type == LOCAL)
		retval = local_delete(host, del_ctrl.full_path);
	else {
		len = strlen(del_ctrl.full_path);
		if ((hinfo[host].system == SYS_VMS)
				&& (del_ctrl.full_path[len-1] == ']'))
			if (hinfo[host].server == SERVER_VMS_3_0) {    /* Kludge */
				temp = XtMalloc(strlen(del_ctrl.full_path)+8);
				strcpy(temp, del_ctrl.full_path);
				if ((q = strrchr(temp, '.')) == NULL)
					fatal_error("Trouble in cb_delete_entries()");
				*q = '\0';
				strcat(temp, "]");
				strcat(temp, del_ctrl.head[del_ctrl.level]->entry);
				retval = remote_rmdir(host, temp, True);
				XtFree(temp);
			} else
				retval = remote_rmdir(host, del_ctrl.full_path, True);
		else if (hinfo[host].server == SERVER_MAC_FTPD)
			switch (remote_cd(host, del_ctrl.full_path, True, False)) {
			case -3:
				goto lost;
			case 0:
				retval = remote_rmdir(host, del_ctrl.full_path, True);
				break;
			default:
				retval = remote_delete(host, del_ctrl.full_path, True);
			}
		else
			retval = remote_delete(host, del_ctrl.full_path, True);
	}
    switch (retval) {
    case -3:
		goto lost;
    case -1:
        sprintf(msg, "Unable to delete %s", del_ctrl.rel_path);
        warning_error(msg);
		break;
	case 0:
		if (diagnostics >= NORMAL) {
        	sprintf(msg, "*** Successfully deleted:  %s\n", del_ctrl.rel_path);
        	write_log(msg);
		}
    }
    XtFree(del_ctrl.full_path);
    XtFree(del_ctrl.rel_path);
    ptr = del_ctrl.head[del_ctrl.level];
    del_ctrl.head[del_ctrl.level] = ptr->next;
    XtFree(ptr->entry);
    XtFree((char *)ptr);
    return False;

done:
	
	clean_up_del_ctrl();
	hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
	enable_controls(True);
	if (update_dir_displays(host, False) < 0)
		goto lost;
	update_xfer_controls();
	update_host_controls(host);
	if (beep_when_ops_done)
		beep();
	restore_prev_cursor();
	return True;

abort:

	hide_abort_dialog();
	warning_error("Deletion(s) aborted");
	clean_up_del_ctrl();
	hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
	enable_controls(True);
	if (update_dir_displays(host, False) < 0)
		goto lost;
	update_xfer_controls();
	update_host_controls(host);
	if (beep_when_ops_done)
		beep();
	restore_prev_cursor();
	return True;

lost:

	clean_up_del_ctrl();
	hide_mp_monitor();
	if (mp_abort_requested())
		hide_abort_dialog();
	enable_controls(True);
	clear_selected_entries(host);
	restore_prev_cursor();
	lost_connection(host);
	use_busy_cursor();
	update_xfer_controls();
	update_host_controls(host);
	if (beep_when_ops_done)
		beep();
	restore_prev_cursor();
	return True;
}

