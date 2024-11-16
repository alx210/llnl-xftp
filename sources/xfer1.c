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

struct xfer_ctrl_block xc;

char *merge_paths();
char *cstring_to_text();
char *links_to_path();
int cb_examine_src_forest();

extern int inquire_on_copy;
extern Widget w_dirList[];
extern struct st_host_info hinfo[];
extern Widget w_ascii;
extern XtAppContext app;
extern int beep_when_ops_done;


/*
 * cb_copy - Callback that copies selected file(s).
 */
void
cb_copy(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int operation = (int)((long)client_data);
	int xfer_mode;
	int src_host;
	int snk_host;
	XmStringTable selected_items;
	int nselected_items;
	int i;
	int retval;
	char *q;
	struct forest_node *head;
	struct forest_node *ptr;

	/* Clear error flag */
	raise_okflag();

	/* Determine source and sink hosts */
	if (host_selected(LEFT)) {
		src_host = LEFT;
		snk_host = RIGHT;
	} else if (host_selected(RIGHT)) {
		src_host = RIGHT;
		snk_host = LEFT;
	} else
		fatal_error("Bug in cb_copy() - Contact programmer");

	/* Ask the user if okay to copy? */
	if (inquire_on_copy) {
        switch (operation) {
        case COPY:
            retval = verify_selection(src_host,
                "Do you really want to transfer these items?");
            break;
        case RCOPY:
            retval = verify_selection(src_host,
                "Do you really want to recursively transfer these items?");
        }
		if (retval == False)
			return;
	}

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
	if (hinfo[snk_host].type == REMOTE)
		if (ftp_type(snk_host, xfer_mode) < 0) {
			restore_prev_cursor();
			lost_connection(snk_host);
			return;
		}

	/* Disable controls */
	enable_controls(False);

	/* Form list of selected entries */
	XtVaGetValues(
		w_dirList[src_host],
		XmNselectedItemCount, &nselected_items,
		XmNselectedItems,     &selected_items,
		NULL
	);
	head = NULL;
	for (i=nselected_items-1; i>=0; i--) {
		ptr = XtNew(struct forest_node);
		ptr->parent = NULL;
		ptr->first_child = NULL;
		ptr->status = NOT_PROCESSED;
		ptr->position = AT_START_NODE;
		ptr->entry = cstring_to_text(selected_items[i]);
		strip_off_symbol(hinfo[src_host].system, hinfo[src_host].server,
			ptr->entry);
		if (operation == COPY) {
			ptr->type = TYPE_FILE;
			if (hinfo[src_host].system == SYS_VMS) {
				if ((q = strstr(ptr->entry, ".dir")))
					*q = '\0';   /* Shave off ".dir;n" */
			}
		} else if (hinfo[src_host].system == SYS_VMS) {
			if ((q = strstr(ptr->entry, ".dir"))) {
				*q = '\0';   /* Shave off ".dir;n" */
				ptr->type = TYPE_DIR;
			} else
				ptr->type = TYPE_FILE;
		} else
			ptr->type = TYPE_UNKNOWN;
		ptr->next = head;
		head = ptr;
	}

	/* Set up file control block */
	xc.operation = operation;
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
	xc.snk_host_type = hinfo[snk_host].type;
	xc.snk_host_system = hinfo[snk_host].system;
	xc.snk_host_wd = XtNewString(hinfo[snk_host].wd);
	xc.forest = head;
	xc.s0ptr = head;
	xc.node[0] = head;
	xc.ignore_create_dir_errors = False;
	xc.nstages = 0;
	xc.more_files = True;

	/* Transfer files */
	show_xfermon();
/*
	sleep(1);
*/
	set_xfermon_status(STATUS_PASS1);
	set_xfermon_name_label("Directory:");
	set_xfermon_progress((long)-1, (long)-1);
	XtAppAddWorkProc(app, (XtWorkProc)cb_examine_src_forest, NULL);
}


/*
 * test_for_dir - Returns 0 if full path is a directory, -1 if path is not a
 *                directory, and -3 if connection is lost.
 */
test_for_dir(host, path)
int host;
char *path;
{
	if (hinfo[host].type == LOCAL) {
		if (local_cd(host, path, False) == 0)
			return 0;
	} else
		switch (remote_cd(host, path, True, False)) {
		case -3:
			return -3;
		case 0:
			return 0;
		}
	return -1;
}


/*
 * rel_path_of_xfer_node - Given an xfer node, return the corresponding
 *                         entry's relative path in native format.
 *                         "which" can be either SOURCE or SINK.
 */
char *
rel_path_of_xfer_node(system, node, which)
int system;
struct forest_node *node;
int which;
{
	struct forest_node *ptr;
	char **links;
	char *rel_path;
	int i;
	int nlinks;

	/* Determine number of links */
	nlinks = 0;
	ptr = node;
	while (ptr) {
		nlinks++;
		if ((which == SINK) && (ptr->position == AT_START_NODE))
			break;
		ptr = ptr->parent;
	}
	links = (char **)XtMalloc(sizeof(char *)*(nlinks+1));
	ptr = node;
	for (i=nlinks-1; i>=0; i--) {
		if (system == SYS_VMS) {
			switch (ptr->type) {
			case TYPE_DIR:
				links[i] = XtMalloc(strlen(ptr->entry)+5);
				strcpy(links[i], ptr->entry);
				strcat(links[i], ".dir");
				break;
			case TYPE_FILE:
				links[i] = XtNewString(ptr->entry);
				break;
			case TYPE_UNKNOWN:
				fatal_error("Bug in rel_path_of_xfer_node()");
			}
		} else
			links[i] = XtNewString(ptr->entry);
		ptr = ptr->parent;
	}
	links[nlinks] = NULL;		
	rel_path = links_to_path(system, links, nlinks);
	release_path_links(links);
	return rel_path;
}


/*
 * src_paths_of_xfer_node - Given an xfer node, return the corresponding
 *                          entry's source relative path and source full
 *                          path in native format.  "rel_path" and
 *                          "full_path" may be NULL, if that value is not
 *                          to be returned.  Use XtFree() to release the
 *                          returned memory.
 */
src_paths_of_xfer_node(node, rel_path, full_path)
struct forest_node *node;
char **rel_path;
char **full_path;
{
	char *tmp_rel_path;
	char *tmp_full_path;

	/* Create paths */
	tmp_rel_path = rel_path_of_xfer_node(xc.src_host_system, node, SOURCE);
	tmp_full_path = merge_paths(xc.src_host_system, xc.src_host_wd,
		tmp_rel_path);

	/* Return relative path? */
	if (rel_path)
		*rel_path = tmp_rel_path;
	else
		XtFree(tmp_rel_path);

	/* Return full path? */
	if (full_path)
		*full_path = tmp_full_path;
	else
		XtFree(tmp_full_path);
}


/*
 * snk_paths_of_xfer_node - Given an xfer node, return the corresponding
 *                          entry's sink relative path and sink full
 *                          path in native format.  "rel_path" and
 *                          "full_path" may be NULL, if that value is not
 *                          to be returned.  Use XtFree() to release the
 *                          returned memory.
 */
snk_paths_of_xfer_node(node, rel_path, full_path)
struct forest_node *node;
char **rel_path;
char **full_path;
{
	char *tmp_rel_path;
	char *tmp_full_path;

	/* Create paths */
	tmp_rel_path = rel_path_of_xfer_node(xc.snk_host_system, node, SINK);
	tmp_full_path = merge_paths(xc.snk_host_system, xc.snk_host_wd,
		tmp_rel_path);

	/* Return relative path? */
	if (rel_path)
		*rel_path = tmp_rel_path;
	else
		XtFree(tmp_rel_path);

	/* Return full path? */
	if (full_path)
		*full_path = tmp_full_path;
	else
		XtFree(tmp_full_path);
}


/*
 * print_xfer_forest
 */
print_xfer_forest()
{
	struct forest_node *node[MAXLEVELS];
	int level = 0;
	int i;
	
	node[0] = xc.forest;
	while (1) {
		if (node[level] == NULL) {
			level--;
			if (level < 0)
				return;
			node[level] = node[level]->next;
		} else if (node[level]->type == TYPE_DIR) {
			for (i=0; i<3*level; i++)
				printf(" ");
			if (node[level]->position == ABOVE_START_NODE)
				printf("P %s\n", node[level]->entry);
			else
				printf("D %s\n", node[level]->entry);
			node[level+1] = node[level]->first_child;
			level++;
		} else if (node[level]->type == TYPE_UNKNOWN) {
			for (i=0; i<3*level; i++)
				printf(" ");
			printf("U %s\n", node[level]->entry);
			node[level] = node[level]->next;
		} else {
			for (i=0; i<3*level; i++)
				printf(" ");
			printf("F %s\n", node[level]->entry);
			node[level] = node[level]->next;
		}
	}
		
}


/*
 * release_xfer_forest - Release memory used by transfer forest.
 */
release_xfer_forest()
{
	release_xfer_subforest(xc.forest);
}


/*
 * release_xfer_subforest - Release memory used by forest starting at "node".
 */
release_xfer_subforest(node)
struct forest_node *node;
{
	struct forest_node *ptr1;
	struct forest_node *ptr2;

	/* If forest is empty, nothing to do */
	if (!node)
		return;

	/* March through siblings */
	ptr1 = node;
	while (ptr1) {
		release_xfer_subforest(ptr1->first_child);
		XtFree(ptr1->entry);
		ptr2 = ptr1;
		ptr1 = ptr1->next;
		XtFree((char *)ptr2);
	}
}

