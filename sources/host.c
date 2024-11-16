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
#include <Xm/LabelG.h>
#include "xftp.h"

extern struct st_host_info hinfo[];
extern Widget w_hostName[];
extern Widget w_connectToLocalItem[];
extern Widget w_connectToRemoteItem[];
extern Widget w_disconnectItem[];
extern Widget w_dirMenu[];
extern Widget w_selectMenu[];
extern Widget w_opsMenu[];
extern Widget w_dirName[];
extern Widget w_deleteSelectedEntriesItem[];
extern Widget w_rDeleteSelectedEntriesItem[];
extern Widget w_moveSelectedEntriesItem[];
extern Widget w_renameSelectedEntriesItem[];
extern Widget w_sendQuotedCommandItem[];
extern Widget w_dirList[];
extern int rdelete_enabled;


/*
 * update_hostname - Update the name of the indicated host on the display.
 */
update_hostname(host)
int host;
{
	XmString name;

	if (hinfo[host].type == NEITHER)
		name = XmStringCreateSimple("Not Connected");
	else
		name = XmStringCreateSimple(hinfo[host].hostname);

	XtVaSetValues(w_hostName[host], XmNlabelString, name, NULL);

	XmStringFree(name);
}


/*
 * lost_connection - This routine is called when a lost control connection
 *                   to "host" is detected.  The user is informed, and a
 *                   reconnection is attempted if he/she wishes.
 */
lost_connection(host)
int host;
{
	char msg[250];
	static char *side[] = { "left", "right" };
	char *wd;
	char *current_wd;
	XmStringTable selected_items;
	XmStringTable sitems;
	int nselected_items;
	int i;
	int retval;
	char *hostname;
	char *username;
	char *password;
	char *account;

	/* Close down connection */
	close(hinfo[host].ctrl_fd);
	hinfo[host].type = NEITHER;
   
	/* Does the user wish to reconnect? */
	sprintf(msg, "Lost connection to \"%s\" host.  Do you wish to reconnect?\n(If so, you might need to resubmit your last command.)",
		side[host]);
	use_normal_cursor();
	if (question_dialog(msg) == False) {
		if (update_dir_displays(host, False) < 0)
			fatal_error("Programming bug in lost_connection()");
		update_xfer_controls();
		update_dirname_menu(host);
		update_dirpath(host);
		update_hostname(host);
		update_host_controls(host);
		restore_prev_cursor();
		return;
	}
	restore_prev_cursor();

	/* This might take some time */
	use_busy_cursor();

	/* Preserve current working directory for host */
	wd = XtNewString(hinfo[host].wd);

    /* Preserve current selection for host */
    XtVaGetValues(
        w_dirList[host],
        XmNselectedItemCount, &nselected_items,
        XmNselectedItems,     &selected_items,
        NULL
    );
    sitems = (XmStringTable)XtMalloc(nselected_items*sizeof(XmString));
    for (i=0; i<nselected_items; i++)
        sitems[i] = XmStringCopy(selected_items[i]);

	/* Try to reconnect */
	hostname = XtNewString(hinfo[host].hostname);
	username = XtNewString(hinfo[host].username);
	password = XtNewString(hinfo[host].password);
	account = XtNewString(hinfo[host].account);
	retval = do_connect(host, hostname, username, password, account, NULL);
	XtFree(hostname);
	XtFree(username);
	XtFree(password);
	XtFree(account);
	if (retval < 0
		|| remote_cd(host, wd, False, True) < 0
		|| remote_pwd(host, &current_wd) < 0) {
		close(hinfo[host].ctrl_fd);
		hinfo[host].type = NEITHER;
		if (update_dir_displays(host, False) < 0)
			fatal_error("Programming bug in lost_connection()");
		update_xfer_controls();
		update_dirname_menu(host);
		update_dirpath(host);
		update_hostname(host);
		update_host_controls(host);
		XtFree(wd);
		for (i=0; i<nselected_items; i++)
			XmStringFree(sitems[i]);
		XtFree((char *)sitems);
		restore_prev_cursor();
		return;
	}
	XtFree(hinfo[host].wd);
	hinfo[host].wd = current_wd;

	/* Update directory lists and restore selection */
	if (update_dir_displays(host, False) < 0) {
		warning_error("Unable to reconnect to remote host");
		close(hinfo[host].ctrl_fd);
		hinfo[host].type = NEITHER;
		if (update_dir_displays(host, False)) 
			fatal_error("Programming bug in lost_connection()");
	} else
	    for (i=0; i<nselected_items; i++)
			XmListSelectItem(w_dirList[host], sitems[i], True);

	update_xfer_controls();
    update_dirname_menu(host);
	update_dirpath(host);
    update_hostname(host);
    update_host_controls(host);

	/* Free memory */
	XtFree(wd);
	for (i=0; i<nselected_items; i++)
		XmStringFree(sitems[i]);
	XtFree((char *)sitems);

	restore_prev_cursor();
}


/*
 * update_host_controls - Set the sensitivity of the hosts controls.  This
 *                        routine should be invoked (1) after connecting
 *                        to a host, (2) after disconnecting from a host,
 *                        (3) after selecting a directory entry, (4) after
 *                        changing directories, and (5) after updating
 *                        user preferences.
 */
update_host_controls(host)
int host;
{
	int sensitivity;

    if (hinfo[host].type == NEITHER) {
        XtSetSensitive(w_disconnectItem[host], False);
        XtSetSensitive(w_dirMenu[host], False);
        XtSetSensitive(w_selectMenu[host], False);
        XtSetSensitive(w_opsMenu[host], False);
	} else {
        XtSetSensitive(w_disconnectItem[host], True);
        XtSetSensitive(w_dirMenu[host], True);
        XtSetSensitive(w_selectMenu[host], True);
        XtSetSensitive(w_opsMenu[host], True);
        sensitivity = host_selected(host);
        XtSetSensitive(w_deleteSelectedEntriesItem[host], sensitivity);
		if (rdelete_enabled)
			XtSetSensitive(w_rDeleteSelectedEntriesItem[host], sensitivity);
		else
			XtSetSensitive(w_rDeleteSelectedEntriesItem[host], False);
        XtSetSensitive(w_moveSelectedEntriesItem[host], sensitivity);
        XtSetSensitive(w_renameSelectedEntriesItem[host], sensitivity);
	}

    if (hinfo[host].type == LOCAL) {
        XtSetSensitive(w_connectToLocalItem[host], False);
		XtSetSensitive(w_sendQuotedCommandItem[host], False);
    } else {
        XtSetSensitive(w_connectToLocalItem[host], True);
		XtSetSensitive(w_sendQuotedCommandItem[host], True);
	}
}


/*
 * host_selected - Returns True if a directory entry for the host is
 *                 selected, else False.
 */
host_selected(host)
int host;
{
    if (selection_count(host))
        return True;
    else
        return False;
}


/*
 * selection_count - Returns the number of selected directory entries
 *                   for the indicated host.
 */
selection_count(host)
int host;
{
    int count;

    XtVaGetValues(w_dirList[host], XmNselectedItemCount, &count, NULL);

	return count;
}


