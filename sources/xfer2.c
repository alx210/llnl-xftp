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
#include "str.h"


int cb_expand_snk_forest();
int cb_examine_snk_forest();
int cb_xfer_files();

extern XtAppContext app;
extern int overwrite_dirs;
extern Display *display;
extern struct xfer_ctrl_block xc;


/*
 * cb_examine_src_forest
 */
cb_examine_src_forest()
{
	char *rel_path;
	char *full_path;
	char *rel_path_parent;
	int retval;
	int i;
	struct forest_node *ptr;
	struct forest_node *head;
	struct sl_struct *list;
	char *q;
	char msg[MAXPATHLEN+40];

	/* Determine types of current directory's entries */
	if (xc.state == 0) {
		while (xc.s0ptr) {
			if (xfer_abort_requested()) {
				xfer_done(False, False);
				return True;
			}
			if (xc.s0ptr->type == TYPE_UNKNOWN) {
				if (xc.src_host_system == SYS_VMS)
					fatal_error("Bug in cb_examine_src_forest()");
				if (xc.s0ptr->parent)
					src_paths_of_xfer_node(xc.s0ptr->parent, &rel_path_parent,
						NULL);
				else
					rel_path_parent = XtNewString(xc.src_host_wd);
				set_xfermon_name(rel_path_parent);
				XtFree(rel_path_parent);
				src_paths_of_xfer_node(xc.s0ptr, NULL, &full_path);
				retval = test_for_dir(xc.src_host, full_path);
				XtFree(full_path);
				switch (retval) {
				case -3:
					xfer_done(True, False);
					return True;
				case 0:
					xc.s0ptr->type = TYPE_DIR;
					break;
				case -1:
					xc.s0ptr->type = TYPE_FILE;
				}
			}
			xc.s0ptr = xc.s0ptr->next;
		}
		xc.state = 1;
	}
		
	/* Build forest */
	while (1) {
		if (xfer_abort_requested()) {
			xfer_done(False, False);
			return True;
		}
		if (xc.node[xc.level] == NULL) {
			xc.level--;
			if (xc.level < 0) {
				xc.node[0] = xc.forest;
				xc.level = 0;
				if (dir_in_xfer_forest()) {
					if (overwrite_dirs) {
						set_xfermon_status(STATUS_PASS3);
						XtAppAddWorkProc(app, (XtWorkProc)cb_expand_snk_forest,
							NULL);
					} else {
						set_xfermon_status(STATUS_PASS2);
						XtAppAddWorkProc(app, (XtWorkProc)cb_examine_snk_forest,
							NULL);
					}
				} else {
					show_xfermon();
/*
					sleep(1);
*/
					xc.state = 0;
					set_xfermon_status(STATUS_XFER);
					set_xfermon_name_label("File Name:");
					XtAppAddWorkProc(app, (XtWorkProc)cb_xfer_files, NULL);
				}
				return True;
			}
			xc.node[xc.level] = xc.node[xc.level]->next;
		} else if ((xc.node[xc.level]->type == TYPE_DIR)
				&& (xc.node[xc.level]->position != ABOVE_START_NODE)) {
			/*** FOR XDIR, IF OP==COPY THEN ASK USER ***/
			if (xc.level == MAXLEVELS-1) {
				warning_error( "File transfer recurses too deeply - aborted.");
				xfer_done(False, False);
				return True;
			}
			/* Get directory list */
			src_paths_of_xfer_node(xc.node[xc.level], &rel_path, &full_path);
			set_xfermon_name(rel_path);
			if (xc.src_host_type == LOCAL)
				retval = local_ls(full_path, &list, False);
			else
				retval = remote_ls(xc.src_host, full_path, &list, False);
			XtFree(rel_path);
			XtFree(full_path);
			switch (retval) {
			case -3:
				xfer_done(True, False);
				return True;
			case -1:
				src_paths_of_xfer_node(xc.node[xc.level], &rel_path, NULL);
				sprintf(msg, "Unable to get list of source directory %s",
					rel_path);
				warning_error(msg);
				XtFree(rel_path);
				xfer_done(False, False);
				return True;
			}
			/* Add source directory list to new level */
			head = NULL;
			for (i=list->nentries-1; i>=0; i--) {
				ptr = XtNew(struct forest_node);
				ptr->parent = xc.node[xc.level];
				ptr->first_child = NULL;
				ptr->status = NOT_PROCESSED;
				ptr->position = BELOW_START_NODE;
				ptr->entry = XtNewString(list->entries[i]);
				if (xc.src_host_system == SYS_VMS) {
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
			release_array_list(list);
			xc.node[xc.level]->first_child = head;
			xc.level++;
			xc.node[xc.level] = head;
			xc.state = 0;
			xc.s0ptr = head;
			return False;
		} else
			xc.node[xc.level] = xc.node[xc.level]->next;
	}
}


/*
 * dir_in_xfer_forest - Scans the completed source forest and returns True
 *                      if it contains a directory that is at or below a
 *                      "start node".
 */
dir_in_xfer_forest()
{
	struct forest_node *node[MAXLEVELS];
	int level = 0;
	
	node[0] = xc.forest;
	while (1) {
		if (node[level] == NULL) {
			level--;
			if (level < 0)
				return False;
			node[level] = node[level]->next;
		} else if (node[level]->type == TYPE_DIR) {
			if (node[level]->position == ABOVE_START_NODE) {
				node[level+1] = node[level]->first_child;
				level++;
			} else
				return True;
		} else {
			node[level] = node[level]->next;
		}
	}
}

