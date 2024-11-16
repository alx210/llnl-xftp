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
#ifdef HIPPI
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#include <unistd.h>
#include <Xm/Xm.h>
#include "xftp.h"
#include "xfer.h"
#include "str.h"

static char *tcpbuf = NULL;

extern struct xfer_ctrl_block xc;
extern int diagnostics;
extern int store_unique;
extern int maxt;
extern int preserve_vms_ver_nums;

long extract_file_len();
char *create_unique_name();


/*
 * init_get - Initialize transfer of file from remote host to local host.
 *            "xc" is used for input and output.  Returns 0 if successful,
 *            -3 for lost source connection, and -1 for other errors.
 */
init_get()
{
	char *src_dir;
	char *src_file;
    char snk_path[MAXPATHLEN+1];
	char *tmp_snk_path;
	char *q;
	int data_fd;
	int code;
	char reply[MAXFTPREPLY];
	int sock;
	FILE *fp;
	char cmd[MAXPATHLEN+5];
	int retval;
	long file_len;
	int scode;
	char *dir;
	int sys_tcpbuflen = XFER_SYS_TCPBUFLEN;
	char *full_path;
#ifdef HIPPI
	int winshift = XFER_TCPWINSHIFT;
#endif

	/* Get path of sink file */
	snk_paths_of_xfer_node(xc.current_node, NULL, &tmp_snk_path);
	strcpy(snk_path, tmp_snk_path);
	XtFree(tmp_snk_path);
	if ((xc.src_host_system == SYS_VMS) &&
			(!preserve_vms_ver_nums || (xc.operation == VIEW))) {
		if ((q = strrchr(snk_path, ';')))
			*q = '\0';
	}

	/* Get unique file name if requested */
	if (store_unique) {
		if ((tmp_snk_path = create_unique_name(snk_path)) == NULL)
			return -1;
		strcpy(snk_path, tmp_snk_path);
	}

	/* Create temporary directory for view file */
	if (xc.operation == VIEW) {
		parse_path(SYS_UNIX, snk_path, &dir, NULL);
		retval = local_mkdir(xc.snk_host, dir, 0700);
		XtFree(dir);
		if (retval)
			return -1;
	}

	/* Verify that a writable sink file can be created */
	if (!can_create_sinkfile(xc.snk_host, snk_path))
		return -1;

    /* "cd" into source directory */
	src_paths_of_xfer_node(xc.current_node, NULL, &full_path);
    parse_path(xc.src_host_system, full_path, &src_dir, &src_file);
	XtFree(full_path);
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

	/* Initialize data connection */
	if ((sock = init_data_conn(xc.src_host)) < 0) {
		XtFree(src_file);
		return sock;
	}

	/* Send RETR command to FTP server */
	sprintf(cmd, "RETR %s", src_file);
	XtFree(src_file);
	if ((retval = send_ftp_cmd(xc.src_host, cmd)) < 0) {
		close(sock);
		return retval;
	}

	/* Get initial response from FTP server */
	scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 1:
		file_len = extract_file_len(reply);
		break;
    case 4:
		if (diagnostics < VERBOSE)
			print_ftp_reply(xc.src_host, reply);
		close(sock);
        return -1;
	case -3:
		close(sock);
		return -3;
	case -2:
		report_client_timeout(xc.src_host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(xc.src_host, reply);
		close(sock);
		return -1;
	}

	/* Establish data connection */
	data_fd = accept_data_conn(xc.src_host, sock);
	close(sock);
	if (data_fd < 0)
		return -1;

	/* Attempt to increase TCP receive buffer size to improve performance */
#if defined(SOL_SOCKET) && defined(SO_RCVBUF)
	setsockopt(data_fd, SOL_SOCKET, SO_RCVBUF, (char *)&sys_tcpbuflen,
		sizeof(sys_tcpbuflen));
#endif

	/* Optimize tranfer rate for HIPPI channel on UNICOS Cray */
#ifdef HIPPI
	setsockopt(data_fd, IPPROTO_TCP, TCP_WINSHIFT, (char *)&winshift,
		sizeof(winshift));
#endif

	/* Create sink file */
	if ((fp = fopen(snk_path, "w")) == NULL) {
		report_perror(xc.snk_host, snk_path);
		return abort_get();
	}

    /* Allocate user TCP buffer (but first perform sanity check) */
    if (tcpbuf)
        fatal_error("Programming bug in init_get() involving tcpbuf");
    tcpbuf = XtMalloc(XFER_USER_TCPBUFLEN);

    xc.data_fd = data_fd;
	xc.snk_fp = fp;
	xc.file_index = 0;
    xc.file_len = file_len;

	return 0;
}


/*
 * do_get - Transfer next part of file from remote host to local host.
 *          "xc" is used for input and output.  Returns 0 if file transfer
 *          complete, 1 if file transfer not complete, -3 for lost source
 *          connection, and -1 for other errors.
 */
do_get()
{
	char ch;
	int nbytes;
	int code;
	char reply[MAXFTPREPLY];
	int len_tcpbuf = 0;
	int index_tcpbuf = 0;
	int scode;
	char *snk_path;

	if (xc.mode == ASCII) {    /* Receive ASCII file from FTP server */
		switch (nbytes = read_tcp(xc.src_host, xc.data_fd, tcpbuf,
			XFER_USER_TCPBUFLEN, DELTA, True)) {
		case -2:
			return 1;
		case -1:
		case -3:
			return abort_get();
		case 0:
			goto done;
		}
		len_tcpbuf = nbytes;
		while (index_tcpbuf < len_tcpbuf) {
			ch = tcpbuf[index_tcpbuf++];
			if (ch == '\r') {
				if (index_tcpbuf >= len_tcpbuf) {
					do {
						nbytes = read_tcp(xc.src_host, xc.data_fd, tcpbuf, 1,
							DELTA, True);
					} while (nbytes == -2);
					if (nbytes == 1) {
						index_tcpbuf = 0;
						len_tcpbuf = 1;
					} else if (nbytes == 0) {
						putc('\r', xc.snk_fp);
						xc.file_index++;
						goto done;
					} else
						return abort_get();
				}
				ch = tcpbuf[index_tcpbuf];
				if (ch != '\n') {
					putc('\r', xc.snk_fp);
					xc.file_index++;
				}
			} else {
				putc(ch, xc.snk_fp);
				xc.file_index++;
			}
		}
	} else               /* Receive binary file from FTP server */
		switch (nbytes = read_tcp(xc.src_host, xc.data_fd, tcpbuf,
			XFER_USER_TCPBUFLEN, DELTA, True)) {
		case -2:
			return 1;
		case -1:
		case -3:
			return abort_get();
		case 0:
			goto done;
		default:
			if (iwrite(fileno(xc.snk_fp), tcpbuf, nbytes) != nbytes) {
				snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
				report_perror(xc.snk_host, snk_path);
				XtFree(snk_path);
				return abort_get();
			}
			xc.file_index += nbytes;
		}

	return 1;

done:

	/* Close sink file */
	fclose(xc.snk_fp);

    /* Free up user TCP buffer */
    XtFree(tcpbuf);
    tcpbuf = NULL;

	/* Get final response from FTP server */
	if (xc.src_host_server == SERVER_MAC_FTPD)
		close(xc.data_fd);
	scode = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 2:
		close(xc.data_fd);
		return 0;
	case -3:
		close(xc.data_fd);
		return -3;
	case -2:
		report_client_timeout(xc.src_host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(xc.src_host, reply);
		close(xc.data_fd);
		return -1;
	}
}


/*
 * abort_get - Abort transfer of file from remote host to local host.
 *             "xc" is used for input and output.  Returns -3 for lost
 *             source connection, else -1.
 */
abort_get()
{
	int code;
	char reply[MAXFTPREPLY];
	int retval;
	int nsecs;

	/* Close sink file */
	fclose(xc.snk_fp);

	/* Send urgent abort to server */
	urgent_abort(xc.src_host);

	/* Make sure user TCP buffer is allocated */
	if (tcpbuf)
		tcpbuf = XtMalloc(XFER_USER_TCPBUFLEN);

	/* Clear out all incoming data */
	if ((xc.src_host_server != SERVER_UNIX_UNITREE) &&
			(xc.src_host_server != SERVER_UNIX_NSL_UNITREE))
		while (read_tcp(xc.src_host, xc.data_fd, tcpbuf, XFER_USER_TCPBUFLEN,
			10, True) > 0);
	close(xc.data_fd);

	/* Get responses from server */
	nsecs = 15;
	retval = 1;
	while (retval > 0) {
		retval = get_ftp_reply(xc.src_host, reply, MAXFTPREPLY, &code, nsecs,
			True);
		nsecs = 5;
	}

    /* Free up user TCP buffer */
    XtFree(tcpbuf);
    tcpbuf = NULL;

	/* Determine return value */
	if (retval == -3)
		return -3;
	else if (retval == -2)
		report_client_timeout(xc.src_host);
	return -1;
}

