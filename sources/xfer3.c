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

static char *msg1 = "You are attempting to overwrite at least one directory, but\nthe preference OVERWRITE DIR ON COPY is set to NO.\n\nDirectory: %s\n\nOverwrite all such directories anyway?";

static char *msg2 = "Unable to create sink directory:\n\n%s\n\nDo you wish to continue even though this directory\n(and perhaps others) cannot be created?";

int cb_expand_snk_forest();
int cb_xfer_files();

extern XtAppContext app;
extern int diagnostics;
extern Display *display;
extern struct xfer_ctrl_block xc;


/*
 * cb_examine_snk_forest
 */
cb_examine_snk_forest()
{
	char *rel_path;
	char *full_path;
	int retval;
	char msg[MAXPATHLEN+200];

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
				set_xfermon_status(STATUS_PASS3);
				XtAppAddWorkProc(app, (XtWorkProc)cb_expand_snk_forest, NULL);
				return True;
			}
			xc.node[xc.level] = xc.node[xc.level]->next;
		} else if (xc.node[xc.level]->position == ABOVE_START_NODE) {
			xc.node[xc.level+1] = xc.node[xc.level]->first_child;
			xc.level++;
		} else if (xc.node[xc.level]->position == AT_START_NODE) {
			if (xc.node[xc.level]->type == TYPE_DIR) {
				snk_paths_of_xfer_node(xc.node[xc.level], &rel_path,&full_path);
				set_xfermon_name(rel_path);
				retval = test_for_dir(xc.snk_host, full_path);
				XtFree(rel_path);
				XtFree(full_path);
				switch (retval) {
				case -3:
					xfer_done(False, True);
					return True;
				case 0:
					xc.node[xc.level]->status = ALREADY_EXISTS;
					snk_paths_of_xfer_node(xc.node[xc.level], NULL, &full_path);
					sprintf(msg, msg1, full_path);
					XtFree(full_path);
					enable_xfermon_abort(False);
					retval = question_dialog(msg);
					enable_xfermon_abort(True);
					if (retval) {
						xc.node[0] = xc.forest;
						xc.level = 0;
						set_xfermon_status(STATUS_PASS3);
						XtAppAddWorkProc(app, (XtWorkProc)cb_expand_snk_forest,
							NULL);
						return True;
					} else {
						xfer_done(False, False);
						return True;
					}
				}
			}
			xc.node[xc.level] = xc.node[xc.level]->next;
		} else
			fatal_error("Bug in cb_examine_snk_forest()");
	}
}


/*
 * cb_expand_snk_forest
 */
cb_expand_snk_forest()
{
	char *rel_path;
	char *full_path;
	int retval;
	char msg[MAXPATHLEN+200];

	while (1) {
		if (xfer_abort_requested()) {
			xfer_done(False, False);
			return True;
		}
		if (xc.node[xc.level] == NULL) {
			xc.level--;
			if (xc.level < 0) {
/*
				sleep(1);
*/
				xc.node[0] = xc.forest;
				xc.level = 0;
				xc.state = 0;
				set_xfermon_status(STATUS_XFER);
				set_xfermon_name_label("File Name:");
				XtAppAddWorkProc(app, (XtWorkProc)cb_xfer_files, NULL);
				return True;
			}
			xc.node[xc.level] = xc.node[xc.level]->next;
		} else if (xc.node[xc.level]->type == TYPE_DIR) {
			snk_paths_of_xfer_node(xc.node[xc.level], &rel_path, NULL);
			set_xfermon_name(rel_path);
			XtFree(rel_path);
			if ((xc.node[xc.level]->position == ABOVE_START_NODE)
					|| (xc.node[xc.level]->status == ALREADY_EXISTS)) {
				xc.node[xc.level+1] = xc.node[xc.level]->first_child;
				xc.level++;
				continue;
			} else if ((xc.node[xc.level]->position == AT_START_NODE)
					|| (xc.level &&
					(xc.node[xc.level]->parent->status == ALREADY_EXISTS))) {
				snk_paths_of_xfer_node(xc.node[xc.level], NULL, &full_path);
				retval = test_for_dir(xc.snk_host, full_path);
				XtFree(full_path);
				switch (retval) {
				case -3:
					xfer_done(False, True);
					return True;
				case 0:
					xc.node[xc.level]->status = ALREADY_EXISTS;
					xc.node[xc.level+1] = xc.node[xc.level]->first_child;
					xc.level++;
					return False;
				}
			}
			snk_paths_of_xfer_node(xc.node[xc.level], NULL, &full_path);
			if (xc.snk_host_type == LOCAL)
				retval = local_mkdir(xc.snk_host, full_path, 0777);
			else
				retval = remote_mkdir(xc.snk_host, full_path);
			XtFree(full_path);
			switch (retval) {
			case -3:
				xfer_done(False, True);
				return True;
			case -1:
				snk_paths_of_xfer_node(xc.node[xc.level], NULL, &full_path);
				sprintf(msg, "Unable to create directory %s", full_path);
				warning_error(msg);
				sprintf(msg, msg2, full_path);
				XtFree(full_path);
				xc.node[xc.level]->status = UNABLE_TO_CREATE;
				xc.node[xc.level] = xc.node[xc.level]->next;
				if (!xc.ignore_create_dir_errors) {
					enable_xfermon_abort(False);
					retval = question_dialog(msg);
					enable_xfermon_abort(True);
					if (!retval) {
						xfer_done(False, False);
						return True;
					}
					xc.ignore_create_dir_errors = True;
				}
				break;
			case 0:
				if (diagnostics >= NORMAL) {
					snk_paths_of_xfer_node(xc.node[xc.level], &rel_path, NULL);
					sprintf(msg, "*** Successfully created directory: %s\n",
							rel_path);
					write_log(msg);
					XtFree(rel_path);
				}
				xc.node[xc.level]->status = CREATED;
				xc.node[xc.level+1] = xc.node[xc.level]->first_child;
				xc.level++;
			}
			return False;
		} else
			xc.node[xc.level] = xc.node[xc.level]->next;
	}
}

