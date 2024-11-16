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
#include <Xm/TextF.h>
#include <netdb.h>
#include "xftp.h"
#ifdef __osf__
#include <netdnet/dnetdb.h>
#endif
#include "list.h"
#include "str.h"

#define FTPCTRLPORT 21

extern struct st_host_info hinfo[];
extern int use_last_dir;
extern int diagnostics;

char *encrypt_password();
char *decrypt_password();
char *get_password();
char *get_account();
char *system_name();
char *server_name();


/*
 * do_connect - Establish connection to remote host.   Use supplied
 *              password and account if non-null, otherwise prompt
 *              user for them.  Returns 0 if successful, else -1.
 *              If a password is supplied, it must be encrypted.
 *              "w_pseudo_parent" is the dialog to center the password
 *              and account dialogs on.  "w_pseudo_parent" is ignored
 *              if both "password" and "account" are non-NULL.
 */
do_connect(host, hostname, username, password, account, w_pseudo_parent)
int host;
char *hostname;
char *username;
char *password;
char *account;
Widget w_pseudo_parent;
{
	char *acct = NULL;
	char *passwrd = NULL;
	char buf[100];
	char *wd;
	int retval;
	struct sl_struct *list;
	char msg[MAXPATHLEN+40];
	char *host_part;
	int port_part;

	/* This might take some time */
	use_busy_cursor();

	/* Initialize FTP server information */
	hinfo[host].system = SYS_UNIX;
	hinfo[host].server = SERVER_UNKNOWN;

	/* Open connection with FTP server */
	if (parse_hostname(hostname, &host_part, &port_part))
		fatal_error("Bug in do_connect()");
	retval = ftp_open(host, host_part, port_part);
	XtFree(host_part);
	if (retval < 0) {
		sprintf(buf, "Unable to connect to %s", hostname);
		warning_error(buf);
		restore_prev_cursor();
		return -1;
	}

	/* Try to login user */
	switch (ftp_user(host, username)) {
	case 0:
		goto logged_on;
	case 1:
		break;
	default:
		warning_error("Unable to log onto FTP server");
		close(hinfo[host].ctrl_fd);
		restore_prev_cursor();
		return -1;
	}

	/* Password required */
	if (password)   /* Use last password */
		passwrd = decrypt_password(password);
	else {          /* Get password from user */
		if ((passwrd = get_password(w_pseudo_parent)) == NULL) {
			close(hinfo[host].ctrl_fd);
			restore_prev_cursor();
			return -1;
		}
	}

	/* Send password to FTP server */
	retval = ftp_password(host, passwrd);
	switch (retval) {
	case 0:
		goto logged_on;
	case 1:
		break;
	default:
		warning_error("Unable to log onto FTP server");
		close(hinfo[host].ctrl_fd);
		restore_prev_cursor();
		bzero(passwrd, (int)strlen(passwrd));
		XtFree(passwrd);
		return -1;
	}

	/* Account required.  Get it from user */
	if (account)   /* Use last account */
		acct = XtNewString(account);
	else {         /* Get account from user */
		if ((acct = get_account(w_pseudo_parent)) == NULL) {
			close(hinfo[host].ctrl_fd);
			restore_prev_cursor();
			bzero(passwrd, (int)strlen(passwrd));
			XtFree(passwrd);
			return -1;
		}
	}

	/* Send account to FTP server */
	if (ftp_account(host, acct) < 0) {
		warning_error("Unable to log onto FTP server");
		XtFree(acct);
		close(hinfo[host].ctrl_fd);
		restore_prev_cursor();
		bzero(passwrd, (int)strlen(passwrd));
		XtFree(passwrd);
		return -1;
	}

logged_on:

	/* Try to determine system type.  If failure, assume Unix */
	ftp_system(host, False);
	if (diagnostics == DEBUG) {
		write_log("*** System is ");
		write_log(system_name(hinfo[host].system));
		write_log("\n");
		write_log("*** Server is ");
		write_log(server_name(hinfo[host].server));
		write_log("\n");
	}

	/* Determine current working directory */
	if (use_last_dir) {
		retrieve_cache(DIRECTORY, hostname, &list, False);
		if (list->nentries > 0) {
			if (remote_cd(host, list->entries[0], False, True) != 0) {
				sprintf(msg, "Unable to change directory to %s",
					list->entries[0]);
				warning_error(msg);
			}
		}
		release_array_list(list);
	}
	if (remote_pwd(host, &wd) < 0) {
		warning_error("Unable to get working directory.  Will log out");
		close(hinfo[host].ctrl_fd);
		restore_prev_cursor();
		if (passwrd) {
			bzero(passwrd, (int)strlen(passwrd));
			XtFree(passwrd);
		}
		if (acct)
			XtFree(acct);
		return -1;
	}

	/* Success */
	hinfo[host].type = REMOTE;
	XtFree(hinfo[host].wd);
	hinfo[host].wd = wd;
	XtFree(hinfo[host].hostname);
	hinfo[host].hostname = XtNewString(hostname);
	XtFree(hinfo[host].username);
	hinfo[host].username = XtNewString(username);
    if (hinfo[host].move_wd) {
        XtFree(hinfo[host].move_wd);
        hinfo[host].move_wd = NULL;
    }
	if (hinfo[host].password)
		XtFree(hinfo[host].password);
	if (passwrd) {
		hinfo[host].password = encrypt_password(passwrd);
		bzero(passwrd, (int)strlen(passwrd));
		XtFree(passwrd);
	} else
		hinfo[host].password = NULL;
	if (hinfo[host].account)
		XtFree(hinfo[host].account);
	hinfo[host].account = acct;
	restore_prev_cursor();
	return 0;
}


/*
 * parse_hostname - Parse "hostname" into its host name and port address
 *                  components.  "hostname" can either consist of a blank-
 *                  delimited host name / port address pair, or just a
 *                  host name.  If the port address part is not given,
 *                  the default control connection port will be used.
 *                  If successful, "host_part" is set to point to a
 *                  string (which the caller can free by calling XtFree()),
 *                  "port_part" is set to the specified port, and a value
 *                  of 0 is returned.  If no host name is given, -1 is
 *                  returned.  If an invalid port designator is given,
 *                  -2 is returned.
 */
parse_hostname(hostname, host_part, port_part)
char *hostname;
char **host_part;
int *port_part;
{
    char *temp;
    char *hp;
    char *pp;
    int port;
    int len;
    int i;

    /* Make copy of hostname */
    temp = XtNewString(hostname);

    /* Get host part */
    if ((hp = strtok(temp, " ")) == NULL) {
        XtFree(temp);
        return -1;
    }

    /* Get port part */
    if ((pp = strtok(NULL, " ")) == NULL)
        port = FTPCTRLPORT;
    else {
        port = 0;
        len = strlen(pp);
        for (i=0; i<len; i++)
            if (pp[i] >= '0' && pp[i] <= '9')
                port = 10*port+(pp[i]-'0');
            else {
                XtFree(temp);
                return -2;
            }
        if (strtok(NULL, " ") != NULL) {
            XtFree(temp);
            return -2;
        }
    }

    /* Everything looks good */
    *host_part = XtNewString(hp);
    XtFree(temp);
    *port_part = port;
    return 0;
}


/*
 * parse_hostuser - Parse the hostname/username pair in "hostuser" to
 *                  "host_part" and "user_part".  Use XtFree() to free
 *                  returned memory.  Returns -1 if either part is null,
 *                  otherwise 0 is returned.
 */
parse_hostuser(hostuser, host_part, user_part)
char *hostuser;
char **host_part;
char **user_part;
{
	int i;
	char *p;
	char *text = XtNewString(hostuser);

	/* Host comes before the first "(" */
    if ((p = strtok(text, "(")) == NULL) {
		XtFree(text);
		return -1;
	}
    for (i=strlen(p)-1; i>=0; i--)
        if (p[i] == ' ')
            p[i] = '\0';
        else
            break;
    if (strlen(p) == 0) {
		XtFree(text);
		return -1;
	}
	*host_part = XtNewString(p);

    /* Get user name.  Spaces must be embedded */
    if ((p = strtok(NULL, "()")) == NULL) {
		XtFree(text);
		XtFree(*host_part);
		return -1;
    }
	while (*p == ' ')
		p++;
    for (i=strlen(p)-1; i>=0; i--)
        if (p[i] == ' ')
            p[i] = '\0';
        else
            break;
    if (strlen(p) == 0) {
		XtFree(text);
		XtFree(*host_part);
		return -1;
	}
	*user_part = XtNewString(p);

	XtFree(text);
	return 0;
}

