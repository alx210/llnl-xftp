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
#include <Xm/Xm.h>
#include <Xm/List.h>
#include "xftp.h"
#include "list.h"

extern Widget w_dirList[];
extern struct st_host_info hinfo[];
extern int symbol_mode;

struct sl_struct *create_null_array_list();
char *merge_paths();
char *cstring_to_text();


/*
 * update_dir_displays - Update the list widget containing directory entries.
 *                       update_dir_list first deletes existing entries, and 
 *                       then generates the new entry names.  If preserve is
 *                       True, then the selection is preserved.  Returns 0 for
 *                       success, -3 for broken connection, and -1 for other
 *                       errors.
 */
update_dir_displays(host, preserve)
int host;
int preserve;
{
	struct sl_struct *dlist;
	int retval;
    XmStringTable selected_items;
    XmStringTable sitems;
    int nselected_items;
    int i;

	/* This might take some time */
	use_busy_cursor();

	/* Get short directory list */
	switch (hinfo[host].type) {
	case NEITHER:
		dlist = create_null_array_list();
		break;
	case LOCAL:
		if (local_ls(hinfo[host].wd, &dlist, symbol_mode) != 0)
			fatal_error("Unable to get local directory list");
		break;
	case REMOTE:
		if ((retval = remote_ls(host, hinfo[host].wd, &dlist, symbol_mode))) {
			restore_prev_cursor();
			return retval;
		}
	}

    /* Preserve current selection from main window? */
	if (preserve) {
	    XtVaGetValues(
	        w_dirList[host],
	        XmNselectedItemCount, &nselected_items,
	        XmNselectedItems,     &selected_items,
	        NULL
	    );
	    sitems = (XmStringTable)XtMalloc(nselected_items*sizeof(XmString));
	    for (i=0; i<nselected_items; i++)
	        sitems[i] = XmStringCopy(selected_items[i]);
	}
		
	/* Place directory entries into main window */
	update_dirlist(host, dlist);

	/* Place directory entries into tabular display */
	if (dirtable_in_use(host))
		update_dirtable(host, dlist);

	/* Free directory list */
	release_array_list(dlist);

    /* Reselect items? */
	if (preserve) {
	    for (i=0; i<nselected_items; i++) {
	        XmListSelectItem(w_dirList[host], sitems[i], True);
	        XmStringFree(sitems[i]);
		}
	    XtFree((char *)sitems);
	}

	/* Update long directory display */
	if ((retval = update_dirlong(host)) < 0) {
		restore_prev_cursor();
		return retval;
	}

	restore_prev_cursor();
	return 0;
}


/*
 * update_dirlist - Update the directory list for "host" in the main
 *                  window using directory entry information in "dlist".
 */
update_dirlist(host, dlist)
int host;
struct sl_struct *dlist;
{
	int i;
	XmString string;

	/* Delete scrollinglist items */
	reset_list(w_dirList[host]);
		
	/* Place directory entries into short list */
    for (i=0; i<dlist->nentries; i++) {
        string = XmStringCreateSimple(dlist->entries[i]);
        XmListAddItem(w_dirList[host], string, 0);
        XmStringFree(string);
    }
}


/*
 * reset_list - Clear the scrolled list.  Contains kludge to get around
 *              XmListDeleteAllItems() bug in Apollo Motif.
 */
reset_list(w_list)
Widget w_list;
{
	XmListDeselectAllItems(w_list);
	XmListDeleteAllItems(w_list);
}


/*
 * cb_double_click - Callback to handle double click of entry in either
 *                   of the scrolled directory lists in the main window.
 *                   An attempt is made to "cd" into the indicated 
 *                   directory.
 */
void
cb_double_click(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);
	XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
	char *entry;
	char *new_wd;

	/* Clear error flag */
	raise_okflag();

	/* Clear all selected entries */
	clear_selected_entries(host);

	/* Get entry that was double-clicked */
	entry = cstring_to_text(cbs->item);
	strip_off_symbol(hinfo[host].system, hinfo[host].server, entry);

	/* Form full path name */
	new_wd = merge_paths(hinfo[host].system, hinfo[host].wd, entry);
	XtFree(entry);

    /* Try to cd to specified directory */
	change_directory(host, new_wd);
	XtFree(new_wd);
}


/*
 * cb_multiple_selection - Callback to handle click on an entry in either
 *                         scrolled directory list in the main window.
 *                         The selection of the entry is toggled, and
 *                         the corresponding entry in the host's directory
 *                         table is also toggled.
 */
void
cb_multiple_selection(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int i;
    int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

	/* Update the tabular directory list display */
	if (dirtable_in_use(host)) {
		for (i=0; i<cbs->selected_item_count; i++)
			if (cbs->item_position == cbs->selected_item_positions[i]) {
				select_table_entry(host, cbs->item_position-1);
				break;
			}
		if (i == cbs->selected_item_count)
			deselect_table_entry(host, cbs->item_position-1);
	}

    /* Clear other host's selected directory entries */
    if (host == LEFT)
        clear_selected_entries(RIGHT);
    else
        clear_selected_entries(LEFT);

    /* Update the display */
    update_xfer_controls();
	for (i=0; i<NUMHOSTS; i++)
    	update_host_controls(i);
}


/*
 * clear_selected_entries - Clear all selected directory entries of
 *                          specified host.
 */
clear_selected_entries(host)
int host;
{
    XmListDeselectAllItems(w_dirList[host]);

	/* Clear directory table entries too */
	if (dirtable_in_use(host))
		clear_selected_table_entries(host);
}

