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

extern struct xfer_ctrl_block xc;
extern struct st_host_info hinfo[];
extern int diagnostics;
extern int store_unique;
extern int maxt;
extern int preserve_vms_ver_nums;

long extract_file_len();


/*
 * init_remote_copy - Initialize transfer of file from remote host to
 *                    remote host.  "xc" is used for input and output.
 *                    Returns 0 if successful, -3 for lost source
 *                    connection, -4 for lost sink connection, -5 for
 *                    lost source and sink connections, and -1 for
 *                    other errors.  The transfer will work only if at
 *                    least one of the FTP servers supports the PASV
 *                    command.
 */
init_remote_copy()
{
	char *src_dir;
	char *src_file;
	char *snk_dir;
	char *snk_file;
	char *port;
	char *q;
	char cmd[MAXPATHLEN+5];
	char reply[MAXFTPREPLY];
	int code;
	int retval;
	int src_scode = 0;
	int snk_scode = 0;
	long file_len;
	int passive_mode_host;
	char *src_path;
	char *snk_path;

    /* "cd" into source directory */
	src_paths_of_xfer_node(xc.current_node, NULL, &src_path);
    parse_path(xc.src_host_system, src_path, &src_dir, &src_file);
	XtFree(src_path);
    retval = remote_cd(xc.src_host, src_dir, True, True);
    XtFree(src_dir);
    switch (retval) {
    case 0:
        break;
    case -3:
        XtFree(src_file);
        return -3;
    default:
        XtFree(src_file);
        return -1;
    }

    /* "cd" into sink directory */
	snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
    parse_path(xc.snk_host_system, snk_path, &snk_dir, &snk_file);
	XtFree(snk_path);
	if ((xc.src_host_system == SYS_VMS) && !preserve_vms_ver_nums) {
		if ((q = strrchr(snk_file, ';')))
			*q = '\0';
	}
    retval = remote_cd(xc.snk_host, snk_dir, True, True);
    XtFree(snk_dir);
    switch (retval) {
    case 0:
        break;
    case -3:
		XtFree(src_file);
        XtFree(snk_file);
        return -4;
    default:
		XtFree(src_file);
        XtFree(snk_file);
        return -1;
    }

	/* Try to put either sink or source host into passive mode */
	if ((hinfo[xc.snk_host].server != SERVER_UNIX_UNITREE) &&
			(hinfo[xc.snk_host].server != SERVER_UNIX_NSL_UNITREE) &&
			(retval = ftp_passive(xc.snk_host, &port)) == 0) {
		passive_mode_host = xc.snk_host;
		if ((retval = ftp_port(xc.src_host, port)) < 0) {
			XtFree(src_file);
			XtFree(snk_file);
			XtFree(port);
			return retval;
		}
	} else if ((!hinfo[xc.snk_host].server != SERVER_UNIX_UNITREE) &&
			(!hinfo[xc.snk_host].server != SERVER_UNIX_NSL_UNITREE) &&
			(retval == -3)) {
		XtFree(src_file);
		XtFree(snk_file);
		return -4;
	} else if ((retval = ftp_passive(xc.src_host, &port)) == 0) {
		passive_mode_host = xc.src_host;
		if ((retval = ftp_port(xc.snk_host, port)) == -3) {
			XtFree(src_file);
			XtFree(snk_file);
			XtFree(port);
			return -4;
		} else if (retval < 0) {
			XtFree(src_file);
			XtFree(snk_file);
			XtFree(port);
			return retval;
		}
	} else {
		XtFree(src_file);
		XtFree(snk_file);
		return retval;
	}
	XtFree(port);

	if (passive_mode_host == xc.src_host) {

	    /* Send STOR command to sink FTP server */
		if (store_unique)
			sprintf(cmd, "STOU %s", snk_file);
		else
   		 	sprintf(cmd, "STOR %s", snk_file);
		XtFree(snk_file);
		switch (send_ftp_cmd(xc.snk_host, cmd)) {
		case -1:
			XtFree(src_file);
			return -1;
		case -3:
			XtFree(src_file);
			return -4;
		}

    	/* Get STOR reply from sink FTP server */
		snk_scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, maxt,
			True);
		switch (snk_scode) {
		case -3:
			XtFree(src_file);
			return -4;
		case 1:
			break;
		case -2:
			report_client_timeout(xc.snk_host);
		default:
			if (snk_scode > 0 && diagnostics < VERBOSE)
				print_ftp_reply(xc.snk_host, reply);
			XtFree(src_file);
			return -1;
		}

    	/* Send RETR command to source FTP server */
    	sprintf(cmd, "RETR %s", src_file);
		XtFree(src_file);
		switch (send_ftp_cmd(xc.src_host, cmd)) {
		case -1:
			return abort_rcopy(False, True);
		case -3:
			if (abort_rcopy(False, True) == -4)
				return -5;
			else
				return -3;
		}

    	/* Get RETR reply from source FTP server */
		src_scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, maxt,
			True);
		switch (src_scode) {
		case -3:
			if (abort_rcopy(False, True) == -4)
				return -5;
			else
				return -3;
		case 1:
			file_len = extract_file_len(reply);
			xc.substate = 0;    /* Need to wait for file source response */
			xc.file_index = -1;
			xc.file_len = file_len;
			return 0;
		case -2:
			report_client_timeout(xc.src_host);
		default:
			if (src_scode > 0 && diagnostics < VERBOSE)
				print_ftp_reply(xc.src_host, reply);
			return abort_rcopy(False, True);
		}

	} else {     /* Sink host is in passive mode */

    	/* Send RETR command to source FTP server */
    	sprintf(cmd, "RETR %s", src_file);
		XtFree(src_file);
   	 	if ((retval = send_ftp_cmd(xc.src_host, cmd)) < 0)
			return retval;

    	/* Get RETR reply from source FTP server */
		src_scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, maxt,
			True);
		switch (src_scode) {
		case -3:
			XtFree(snk_file);
			return -3;
		case 1:
			file_len = extract_file_len(reply);
			break;
		case -2:
			report_client_timeout(xc.src_host);
		default:
			if (src_scode > 0 && diagnostics < VERBOSE)
				print_ftp_reply(xc.src_host, reply);
			XtFree(snk_file);
			return -1;
		}

	    /* Send STOR command to sink FTP server */
		if (store_unique)
			sprintf(cmd, "STOU %s", snk_file);
		else
   		 	sprintf(cmd, "STOR %s", snk_file);
		XtFree(snk_file);
		switch (send_ftp_cmd(xc.snk_host, cmd)) {
		case -1:
			return abort_rcopy(True, False);
			break;
		case -3:
			if (abort_rcopy(True, False) == -3)
				return -5;
			else
				return -4;
		}

    	/* Get STOR reply from sink FTP server */
		snk_scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, maxt,
			True);
		switch (snk_scode) {
		case -3:
			if (abort_rcopy(True, False) == -3)
				return -5;
			else
				return -4;
		case 1:
			xc.substate = 0;    /* Need to wait for file source response */
			xc.file_index = -1;
			xc.file_len = file_len;
			return 0;
		case -2:
			report_client_timeout(xc.snk_host);
		default:
			if (snk_scode > 0 && diagnostics < VERBOSE)
				print_ftp_reply(xc.snk_host, reply);
			return abort_rcopy(True, False);
		}

	}
}


/*
 * do_remote_copy - Transfer next part of file from remote host to remote
 *                  host.  "xc" is used for input and output.  Returns 0
 *                  if file transfer complete, 1 if file transfer not
 *                  complete, -3 for lost source connection, -4 for lost
 *                  sink connection, -5 for lost source and sink
 *                  connections, -6 for a "552" reply (for LLNL Archive
 *                  retry mechanism), and -1 for other errors.
 */
do_remote_copy()
{
	char reply[MAXFTPREPLY];
	int code;
	int scode;

    /* Get final response from source FTP server */
	if (xc.substate == 0) {
		scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, DELTA,
			True);
		switch (scode) {
		case -3:
			if (abort_rcopy(False, True) == -4)
				return -5;
			else
				return -3;
		case -2:
			return 1;
		case 2:
			xc.substate = 1;
			break;
		default:
			if (scode > 0 && diagnostics < VERBOSE)
				print_ftp_reply(xc.src_host, reply);
			return abort_rcopy(False, True);
		}
	}

    /* Get final response from sink FTP server */
	scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, DELTA, True);
	switch (scode) {
	case -3:
		return -4;
	case -2:
		return 1;
	case 2:
		return 0;
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(xc.snk_host, reply);
		if (code == 552)        /* LLNL Archive retry logic */
			return -6;
		else
			return -1;
	}
}


/*
 * abort_remote_copy - Abort transfer of file from remote host to remote
 *                     host.  "xc" is used for input and output.  Returns
 *                     -3 for lost source connection, -4 for lost sink
 *                     connection, -5 for lost source and sink connections,
 *                     else -1.
 */
abort_remote_copy()
{
	abort_rcopy(True, True);
}


/*
 * abort_rcopy - Abort transfer of file from remote host to remote host.
 *               "abort_src" specifies whether to send abort request to
 *               source host.  "abort_snk" specifies whether to send
 *               abort request to sink host.  "xc" is used for all other
 *               input and output.  Returns -3 for lost source connection,
 *               -4 for lost sink connection, -5 for lost source and sink
 *               connections, else -1.
 */
abort_rcopy(abort_src, abort_snk)
int abort_src;
int abort_snk;
{
	char reply[MAXFTPREPLY];
	int code;
	int src_scode = 0;
	int snk_scode = 0;

	/* Is the source FTP server already done? */
	if (abort_src) {
		src_scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, 5,
			True);
		switch (src_scode) {
		case 2:
		case 4:
		case 5:
			abort_src = False;
			src_scode = 0;
			break;
		case -3:
			abort_src = False;
		}
	}

	/* Is the sink FTP server already done? */
	if (abort_snk) {
		snk_scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, 5,
			True);
		switch (snk_scode) {
		case 2:
		case 4:
		case 5:
			abort_snk = False;
			snk_scode = 0;
			break;
		case -3:
			abort_snk = False;
		}
	}

	/* Send abort request to source FTP server */
	if (abort_src)
		urgent_abort(xc.src_host);

	/* Send abort request to sink FTP server */
	if (abort_snk)
		urgent_abort(xc.snk_host);

	/* Get responses from source FTP server */
	if (abort_src) {
    	src_scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, 15,
			True);
		if (src_scode == 2)
			get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, 5, True);
    	else if (src_scode > 0)
        	src_scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code,
				15, True);
		if (src_scode == -2) {
			report_client_timeout(xc.src_host);
			src_scode = -3;
		}
	}

	/* Get responses from sink FTP server */
	if (abort_snk) {
    	snk_scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, 15,
			True);
		if (snk_scode == 2)
			get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, 5, True);
    	else if (snk_scode > 0)
       		snk_scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code,
				15, True);
		if (snk_scode == -2) {
			report_client_timeout(xc.snk_host);
			snk_scode = -3;
		}
	}

	/* Return appropriate error value */
	if (src_scode == -3 && snk_scode == -3)
		return -5;
	else if (src_scode == -3)
		return -3;
	else if (snk_scode == -3)
		return -4;
	else
		return -1;
}

