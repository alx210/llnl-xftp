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

#include <Xm/RowColumn.h>
#include "xftp.h"

extern Widget w_dirName[];
extern struct st_host_info hinfo[];
extern Widget w_dirNameMenuItem[][MAXLINKS];

char **path_to_links();
char *links_to_path();


/*
 * update_dirname_menu - Update directory name menu for "host" in main
 *                       window.
 */
update_dirname_menu(host)
int host;
{
	XmString label;
	char **wd_links;
	int nlinks = 0;
	int i;
	int len;

	/* If not connected, create "None" menu item */
	if (hinfo[host].type == NEITHER) {
		label = XmStringCreateSimple("None");
		XtVaSetValues(w_dirNameMenuItem[host][0], XmNlabelString, label, NULL);
		XmStringFree(label);
		XtManageChild(w_dirNameMenuItem[host][0]);
		for (i=1; i<MAXLINKS; i++)
			XtUnmanageChild(w_dirNameMenuItem[host][i]);
		XtVaSetValues(
			w_dirName[host],
			XmNmenuHistory,	w_dirNameMenuItem[host][0],
			NULL
		);
		return;
	}

	/* Parse working directory path. */
	wd_links = path_to_links(hinfo[host].system, hinfo[host].wd);

    /* Count number of links in path name */
    while (wd_links[nlinks])
        nlinks++;

	/* Truncate ".dir" from VMS links */
	if (hinfo[host].system == SYS_VMS) {
		for (i=0; i<nlinks; i++) {
			len = strlen(wd_links[i]);
			if ((len > 4) && !strcmp(&wd_links[i][len-4], ".dir"))
				wd_links[i][len-4] = '\0';
		}
	}

    /* Add the path links to menu in reverse order */
	for (i=0; i<nlinks; i++) {
		label = XmStringCreateSimple(wd_links[nlinks-1-i]);
		XtVaSetValues(w_dirNameMenuItem[host][i],
			XmNlabelString,	label,
			XmNuserData,	(long)nlinks-i,
			NULL
		);
		XmStringFree(label);
		XtManageChild(w_dirNameMenuItem[host][i]);
	}

	/* Unmanage unused menu items */
	for (i=nlinks; i<MAXLINKS; i++)
		XtUnmanageChild(w_dirNameMenuItem[host][i]);

    /* Free up memory returned by path_to_links() */
    release_path_links(wd_links);

	/* Make first menu item current */
	XtVaSetValues(
		w_dirName[host],
		XmNmenuHistory,	w_dirNameMenuItem[host][0],
		NULL
	);
}


/*
 * cb_set_dir - Change working directory to correspond to the
 *              menu item selected in the directory menu.
 *              If unable to cd into directory, this routine
 *              acts as a no-op.
 */
void
cb_set_dir(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    long indx;
    char **wd_links;
    char *new_wd;
    int host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

	/* Do nothing if not connected */
	if (hinfo[host].type == NEITHER)
		return;

    /* Get position of selected link in path */
    XtVaGetValues(widget, XmNuserData, &indx, NULL);

    /* Parse working directory path */
    wd_links = path_to_links(hinfo[host].system, hinfo[host].wd);

    /* Build path name for new working directory */
    new_wd = links_to_path(hinfo[host].system, wd_links, indx);

    /* Free up memory returned by path_to_links() */
    release_path_links(wd_links);

    /* Try to cd to specified directory */
	change_directory(host, new_wd);
	XtFree(new_wd);
}


