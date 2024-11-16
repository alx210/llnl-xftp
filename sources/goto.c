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

#include <Xm/PushBG.h>
#include "xftp.h"
#include "list.h"

Widget w_gotoPrevDirMenuItem[NUMHOSTS][MAXDIRCACHE];

extern int sort_caches;
extern Widget w_gotoPrevDirMenu[];
extern struct st_host_info hinfo[];

char *cstring_to_text();


/*
 * cb_goto_prev_dir - Callback to go to the directory selected in the
 *                    "Go to Previous Directory" menu.
 */
void
cb_goto_prev_dir(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	XmString string;
	char *new_wd;
	int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

	XtVaGetValues(widget, XmNlabelString, &string, NULL);
	new_wd = cstring_to_text(string);
	XmStringFree(string);   /* Yes, this is necessary */

	/* Try to cd to specified directory */
	change_directory(host, new_wd);
	XtFree(new_wd);
}


/*
 * initialize_goto_menu - Initialize the "Go to Previous Directory" menu
 *                        by creating a bunch of placeholder menu items.
 */
initialize_goto_menu(host)
int host;
{
	XmString string;
	int i;
	Widget w_pulldown;

	/* Get pulldown menu */
	XtVaGetValues(w_gotoPrevDirMenu[host], XmNsubMenuId, &w_pulldown, NULL);

	/* Now create the menu items, but leave them unmanaged */
	string = XmStringCreateSimple("Dummy");
	for (i=0; i<MAXDIRCACHE; i++) {
		w_gotoPrevDirMenuItem[host][i] = XtVaCreateWidget(
            "",
            xmPushButtonGadgetClass,
            w_pulldown,
            XmNlabelString, string,
            NULL
        );
        XtAddCallback(w_gotoPrevDirMenuItem[host][i], XmNactivateCallback,
            cb_goto_prev_dir, (XtPointer)((long)host));
    }
    XmStringFree(string);
}


/*
 * update_goto_menu - Update the "Go to Previous Directory" menu with the
 *                    latest values from the directory cache.
 */
update_goto_menu(host)
int host;
{
	int i;
	struct sl_struct *list;
	XmString label;

    /* Get rid of current "Dir" menu items */
    for (i=0; i<MAXDIRCACHE; i++)
        XtUnmanageChild(w_gotoPrevDirMenuItem[host][i]);

    /* Done if not connected to host */
    if (hinfo[host].type == NEITHER)
        return;

    /* Retrieve cached directory paths */
    retrieve_cache(DIRECTORY, hinfo[host].hostname, &list, sort_caches);

    /* Enter directory paths into "Dir" menu in main window */
    for (i=0; i<list->nentries; i++) {
        label = XmStringCreateSimple(list->entries[i]);
        XtVaSetValues(w_gotoPrevDirMenuItem[host][i], XmNlabelString, label,
			NULL);
        XmStringFree(label);
        XtManageChild(w_gotoPrevDirMenuItem[host][i]);
    }

	/* Free stuff */
	release_array_list(list);
}


/*
 * cb_dir_menu - Callback to update the "Go to Previous Directory" menu 
 *               with the latest values from the directory cache.
 */
void
cb_dir_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	update_goto_menu(host);
}

