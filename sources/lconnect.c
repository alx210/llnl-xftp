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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <Xm/Xm.h>
#include "xftp.h"
#include "list.h"

extern struct st_host_info hinfo[];
extern int use_last_dir;

char *local_host_name();
char* get_login(void);

/*
 * connect_to_local - Make the left or right side (as specified by "host")
 *                    of the main window be a local connection.
 */
connect_to_local(host)
int host;
{
	char *username;
	struct sl_struct *list;
	char msg[MAXPATHLEN+40];

	/* Now connected to local host */
	hinfo[host].type = LOCAL;

	/* Can assume that local host is Unix */
	hinfo[host].system = SYS_UNIX;
	hinfo[host].server = SERVER_UNKNOWN;

	/* Get host name */
	XtFree(hinfo[host].hostname);
	hinfo[host].hostname = local_host_name();

	/* Get user name */
	username = get_login();
	XtFree(hinfo[host].username);
	hinfo[host].username = XtNewString(username);

	/* Set working directory */
	XtFree(hinfo[host].wd);
	if (use_last_dir) {
		retrieve_cache(DIRECTORY, hinfo[host].hostname, &list, False);
		if (list->nentries > 0) {
			if (local_cd(host, list->entries[0], True) == 0)
				hinfo[host].wd = XtNewString(list->entries[0]);
			else {
				sprintf(msg, "Unable to change directory to %s",
					list->entries[0]);
				warning_error(msg);
				hinfo[host].wd = XtNewString(getenv("HOME"));
			}
		} else
			hinfo[host].wd = XtNewString(getenv("HOME"));
		release_array_list(list);
	} else
		hinfo[host].wd = XtNewString(getenv("HOME"));

	/* Reset move destination directory name to NULL */
	if (hinfo[host].move_wd != NULL) {
		XtFree(hinfo[host].move_wd);
		hinfo[host].move_wd = NULL;
	}

    /* Update the display */
    if (update_dir_displays(host, False) < 0)
		fatal_error("*** Bug in connect_to_local()");
	update_xfer_controls();
    update_dirname_menu(host);
	update_dirpath(host);
	update_dir_cache(host);
    update_hostname(host);
    update_host_controls(host);
}


/*
 * cb_connect_to_local - Callback to "connect" to the local host.
 */
void
cb_connect_to_local(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int	host = (int)((long)client_data);

	/* Clear error flag */
	raise_okflag();

    /* First disconnect if already logged in */
    if (hinfo[host].type != NEITHER)
        disconnect(host);

	/* Do it */
	connect_to_local(host);
}

char* get_login(void)
{
	static char *login = NULL;
	
	if(!login) {
		struct passwd *pwd;
		pwd = getpwuid(getuid());
		if(pwd) login = XtNewString(pwd->pw_name);
	}
	return login;
}
