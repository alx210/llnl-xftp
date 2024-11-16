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

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#if defined(SYSV) || defined(SVR4)
#include <netdb.h>
#endif
#include <ctype.h>
#include <X11/Intrinsic.h>
#include "xftp.h"
#include "str.h"

extern int max_ftp_retries;
extern int diagnostics;
extern struct st_host_info hinfo[];
extern int maxt;


/*
 * remote_cd - Change remote working directory to specified directory.
 *             "host" is the host id.  Set process_xevents to True if
 *             X events should be processed while waiting for a reply,
 *             else use False.  Set print_error_msg to True if FTP-
 *             generated error messages are to be written to the log
 *             when not in verbose mode.  Returns 0 if successful,
 *             -3 for broken connection, and -1 for other errors.
 */
remote_cd(host, directory, process_xevents, print_error_msg)
int host;
char *directory;
int process_xevents;
int print_error_msg;
{
	char reply[MAXFTPREPLY];
	char cmd[MAXPATHLEN+5];
	int code;
	int retval;
	int retries = 0;
	int scode;

retry:

	/* Send CWD command to FTP server */
	sprintf(cmd, "CWD %s", directory);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code,maxt,process_xevents);
	switch (scode) {
	case 2:
		return 0;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Change directory failed.  Will retry.\n");
			goto retry;
		}
		if (diagnostics < VERBOSE && print_error_msg)
			print_ftp_reply(host, reply);
		return -1;
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE && print_error_msg)
			print_ftp_reply(host, reply);
		return -1;
	}
}


/*
 * remote_mkdir - Create remote directory.  "host" is the host id.  Returns
 *                0 if successful, -3 for broken connection, and -1 for
 *                other errors.
 */
remote_mkdir(host, path)
int host;
char *path;
{
	char reply[MAXFTPREPLY];
	char cmd[MAXPATHLEN+5];
	int code;
	int retval;
    char *dir;
    char *entry;
	int retries = 0;
	int scode;

retry:

    /* "cd" into directory */
    parse_path(hinfo[host].system, path, &dir, &entry);
    retval = remote_cd(host, dir, False, True);
    XtFree(dir);
    switch (retval) {
    case 0:
        break;
    case -3:
        XtFree(entry);
        return -3;
    default:
        XtFree(entry);
        return -1;
    }

	/* Send MKD command to FTP server */
	if (hinfo[host].system == SYS_VMS)
		sprintf(cmd, "MKD %s", path);
	else {
		sprintf(cmd, "MKD %s", entry);
		XtFree(entry);
	}
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		return 0;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Make directory failed.  Will retry.\n");
			goto retry;
		}
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}
}


/*
 * remote_delete - Delete file/directory from remote host.  "host" is
 *                 the host id.  Set process_xevents to True if X events
 *                 should be processed while waiting for a reply, else
 *                 use False.  Returns 0 if successful, -3 for
 *                 broken connection, and -1 for other errors.
 */
remote_delete(host, path, process_xevents)
int host;
char *path;
int process_xevents;
{
	char reply[MAXFTPREPLY];
	char cmd[MAXPATHLEN+6];
	int code;
	int retval;
	char *dir;
	char *entry;
	int retries = 0;
	int scode;

retry:

    /* "cd" into directory */
    parse_path(hinfo[host].system, path, &dir, &entry);
    retval = remote_cd(host, dir, False, True);
    XtFree(dir);
    switch (retval) {
    case 0:
        break;
    case -3:
        XtFree(entry);
        return -3;
    default:
        XtFree(entry);
        return -1;
    }

	/* Send DELE command to FTP server */
	sprintf(cmd, "DELE %s", entry);
	XtFree(entry);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return -1;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code,maxt,process_xevents);
	switch (scode) {
	case 2:
		return 0;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Delete failed.  Will retry.\n");
			goto retry;
		}
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}
}


/*
 * remote_rmdir - Remove remote directory.  "host" is the host id.  Set
 *                process_xevents to True if X events should be processed
 *                while waiting for a reply, elseuse False.  Returns 0 if
 *                successful, -3 for broken connection, and -1 for other
 *                errors.
 */
remote_rmdir(host, path, process_xevents)
int host;
char *path;
int process_xevents;
{
	char reply[MAXFTPREPLY];
	char cmd[MAXPATHLEN+5];
	int code;
	int retval;
	char *dir;
	char *entry;
	int retries = 0;
	int scode;

retry:

    /* "cd" into directory */
    parse_path(hinfo[host].system, path, &dir, &entry);
    retval = remote_cd(host, dir, False, True);
    XtFree(dir);
    switch (retval) {
    case 0:
        break;
    case -3:
        XtFree(entry);
        return -3;
    default:
        XtFree(entry);
        return -1;
    }

	/* Send RMD command to FTP server */
	sprintf(cmd, "RMD %s", entry);
	XtFree(entry);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return -1;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code,maxt,process_xevents);
	switch (scode) {
	case 2:
		return 0;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Remove directory failed.  Will retry.\n");
			goto retry;
		}
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}
}


/*
 * remote_pwd - Get remote working directory name.  If successful, "name"
 *              is set to point to a string allocated by remote_pwd, else
 *              "name" is undefined.   "host" is the host id.
 *              Use XtFree() to release memory pointed to by "name".
 *              Returns 0 if successful, -3 for broken connection, and
 *              -1 for other errors.
 */
remote_pwd(host, name)
int host;
char **name;
{
	char reply[MAXFTPREPLY];
	int code;
	int nbytes;
	char *start;
	char *end;
	int retval;
	int retries = 0;
	int len;

retry:

	/* Send PWD command to FTP server */
	if ((retval = send_ftp_cmd(host, "PWD")) < 0)
		return retval;

	/* Get response from FTP server */
    retval = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
    switch (retval) {
    case 2:
        break;
	case 1:
    case 3:
	case 5:
        return -1;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Print working directory failed.  Will retry.\n");
			goto retry;
		}
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
    case -2:
		report_client_timeout(host);
        return -1;
    default:
		if (retval > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
        return retval;
    }

	/* Extract directory path */
	if ((start = strchr(reply, '\"'))) {
		start++;
		if ((end = strrchr(reply, '\"')) == NULL)
			return -1;
		if ((nbytes = end-start) <= 0)
			return -1;
		*name = XtMalloc(nbytes+1);
		bcopy(start, *name, nbytes);
		(*name)[nbytes] = '\0';
	} else {
		if (strtok(reply, " ") == NULL)
			return -1;
		if ((start = strtok(NULL, " ")) == NULL)
			return -1;
		*name = XtNewString(start);
	}

	/* Kludge for NCSA Telnet FTP server on Mac */
	if (hinfo[host].server == SERVER_MAC_NCSATELNET) {
		len = strlen(*name);
		if ((len>1) && (((*name)[len-1] == '/') || ((*name)[len-1] == '\001')))
			(*name)[len-1] = '\0';
	}

	return 0;
}


/*
 * remote_rename - Rename/move a directory entry from "old_path" to
 *                 "new_path".  "host" is the host id. 
 *                 Returns 0 if successful, -3 for broken connection,
 *                 and -1 for other errors.
 */
remote_rename(host, old_path, new_path)
int host;
char *old_path;
char *new_path;
{
	char reply[MAXFTPREPLY];
	char cmd[MAXPATHLEN+6];
	int code;
	int retval;
	int retries = 0;
	int scode;
	char *dir;
	char *entry;

retry:

	/* Send RNFR command to FTP server */
	if (hinfo[host].server == SERVER_MAC_VERSATERM) {
		parse_path(hinfo[host].system, old_path, &dir, &entry);
   		retval = remote_cd(host, dir, False, True);
    	XtFree(dir);
    	switch (retval) {
    	case 0:
       		break;
    	case -3:
        	XtFree(entry);
        	return -3;
    	default:
        	XtFree(entry);
        	return -1;
    	}
		sprintf(cmd, "RNFR %s", entry);
		XtFree(entry);
	} else
		sprintf(cmd, "RNFR %s", old_path);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 3:
		break;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Rename failed.  Will retry.\n");
			goto retry;
		}
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}

	/* Send RNTO command to FTP server */
	if (hinfo[host].server == SERVER_MAC_VERSATERM) {
		parse_path(hinfo[host].system, new_path, &dir, &entry);
   		retval = remote_cd(host, dir, False, True);
    	XtFree(dir);
    	switch (retval) {
    	case 0:
       		break;
    	case -3:
        	XtFree(entry);
        	return -3;
    	default:
        	XtFree(entry);
        	return -1;
    	}
		sprintf(cmd, "RNTO %s", entry);
		XtFree(entry);
	} else
		sprintf(cmd, "RNTO %s", new_path);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 2:
		return 0;
	case 4:
		if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
			if (diagnostics >= VERBOSE)
				write_log("*** Rename failed.  Will retry.\n");
			goto retry;
		} else {
	        if (diagnostics < VERBOSE)
	            print_ftp_reply(host, reply);
			return -1;
		}
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}
}


/*
 * quote_cmd - Send command to ftp server.  "host" is the host id.  Returns
 *             0 if successful, -3 for broken connection, and -1 for other
 *             errors.
 */
quote_cmd(host, cmd)
int host;
char *cmd;
{
	char reply[MAXFTPREPLY];
	int code;

	/* Send command to FTP server */
	if (send_ftp_cmd(host, cmd) < 0)
		return -1;

	/* Get response from FTP server */
	while (1)
		switch (get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, True)) {
		case 1:
		case 3:
			if (diagnostics < VERBOSE)
				print_ftp_reply(host, reply);
			continue;
		case -1:
			return -1;
		case -2:
			report_client_timeout(host);
			return -1;
		case -3:
			return -3;
		default:
			if (diagnostics < VERBOSE)
				print_ftp_reply(host, reply);
			return 0;
		}
}


/*
 * stage_unitree_file - Tell Unitree server to stage the file specified by
 *                      "path".  "host" is the host id.  Returns 0 if file
 *                      is already on disk, 1 if stage is successfully
 *                      initiated, -3 for lost connection, and -1 for
 *                      other errors.
 */
stage_unitree_file(host, path)
int host;
char *path;
{
	char reply[MAXFTPREPLY];
	char cmd[MAXPATHLEN+9];
	int code;
	int retval;
	int scode;

	/* Send STAGE command to FTP server */
	sprintf(cmd, "STAGE 1 %s", path);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 2:
		return 0;
	case 4:
	case 5:
		if (code == 405)
			return 1;
		if ((code == 505) && (hinfo[host].server == SERVER_UNIX_NSL_UNITREE) &&
				strstr(reply, ": is being moved from the archive..."))
			return 1;
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	case -3:
		return -3;
	case -2:
		report_client_timeout(host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}
}

