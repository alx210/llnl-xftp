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
#include <fcntl.h>
#include <sys/stat.h>
#include <Xm/Xm.h>
#include "xftp.h"
#include "xfer.h"

static char *tcpbuf = NULL;
static char *filebuf = NULL;

extern struct xfer_ctrl_block xc;
extern int max_ftp_retries;
extern int diagnostics;
extern int store_unique;
extern int maxt;
extern struct st_host_info hinfo[];


/*
 * init_put - Initialize transfer of file from local host to remote host.
 *            "xc" is used for input and output.  Returns 0 if successful,
 *            -4 for lost sink connection, and -1 for other errors.
 */
init_put()
{
	char *src_path;
	char *snk_dir;
	char *snk_file;
	int file_fd;
	int data_fd;
	long file_len;
	struct stat status;
	int code;
	char reply[MAXFTPREPLY];
	int sock;
	char cmd[MAXPATHLEN+5];
	int retval;
	char msg[MAXPATHLEN+30];
	int retries = 0;
	int scode;
    int sys_tcpbuflen = XFER_SYS_TCPBUFLEN;
	char *snk_path;
#ifdef HIPPI
    int winshift = XFER_TCPWINSHIFT;
#endif

retry:

	/* Open source file and verify that it is a "real" file */
	src_paths_of_xfer_node(xc.current_node, NULL, &src_path);
	if ((file_fd = iopen2(src_path, O_RDONLY)) < 0) {
		report_perror(xc.src_host, src_path);
		XtFree(src_path);
		return -1;
	}
	if (fstat(file_fd, &status) < 0 || !S_ISREG(status.st_mode)) {
		sprintf(msg, "%s is not a \"real\" file", src_path);
		XtFree(src_path);
		warning_error(msg);
		close(file_fd);
		return -1;
	}
	XtFree(src_path);

	/* Get length of file */
	file_len = status.st_size;

	/* "cd" into sink directory */
	snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
	parse_path(xc.snk_host_system, snk_path, &snk_dir, &snk_file);
	XtFree(snk_path);
	retval = remote_cd(xc.snk_host, snk_dir, True, True);
	XtFree(snk_dir);
	switch (retval) {
	case 0:
		break;
	case -3:
		XtFree(snk_file);
		close(file_fd);
		return -4;
	default:
		XtFree(snk_file);
		close(file_fd);
		return -1;
	}

	/* Initialize data connection */
	if ((sock = init_data_conn(xc.snk_host)) < 0) {
		XtFree(snk_file);
		close(file_fd);
		if (sock == -3)
			return -4;
		else
			return -1;
	}

	/* Send ALLO command to FTP server */
	if (((hinfo[xc.snk_host].server == SERVER_UNIX_UNITREE) ||
			(hinfo[xc.snk_host].server == SERVER_UNIX_NSL_UNITREE)) &&
			(xc.mode == BINARY)) {
		if ((retval = ftp_allocate(xc.snk_host, file_len)) < 0) {
			close(file_fd);
			close(sock);
			if (retval == -3)
				return -4;
			else
				return -1;
		}
	}

	/* Send STOR command to FTP server */
	if (store_unique)
		sprintf(cmd, "STOU %s", snk_file);
	else
		sprintf(cmd, "STOR %s", snk_file);
	XtFree(snk_file);
	if ((retval = send_ftp_cmd(xc.snk_host, cmd)) < 0) {
		close(file_fd);
		close(sock);
		if (retval == -3)
			return -4;
		else
			return -1;
	}

	/* Get initial response from FTP server */
	scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 1:
		break;
    case 4:
		close(file_fd);
		close(sock);
        if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
            if (diagnostics >= VERBOSE)
                write_log("*** Unable to initialize put.  Will retry.\n");
            goto retry;
        }
		if (diagnostics < VERBOSE)
			print_ftp_reply(xc.snk_host, reply);
        return -1;
	case -3:
		close(file_fd);
		close(sock);
		return -4;
	case -2:
		report_client_timeout(xc.snk_host);
	default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(xc.snk_host, reply);
		close(file_fd);
		close(sock);
		return -1;
	}

	/* Establish data connection */
	data_fd = accept_data_conn(xc.snk_host, sock);
	close(sock);
	if (data_fd < 0) {
		close(file_fd);
		return -1;
	}

	/* Attempt to increase TCP send buffer size to improve performance */
#if defined(SOL_SOCKET) && defined(SO_SNDBUF)
    setsockopt(data_fd, SOL_SOCKET, SO_SNDBUF, (char *)&sys_tcpbuflen,
        sizeof(sys_tcpbuflen));
#endif

    /* Optimize tranfer rate for HIPPI channel on UNICOS Cray */
#ifdef HIPPI
    setsockopt(data_fd, IPPROTO_TCP, TCP_WINSHIFT, (char *)&winshift,
        sizeof(winshift));
#endif

	/* Allocate user buffers (but first perform sanity check) */
	if (tcpbuf)
		fatal_error("Programming bug in init_put() involving tcpbuf");
	tcpbuf = XtMalloc(XFER_USER_TCPBUFLEN);
	if (filebuf)
		fatal_error("Programming bug in init_put() involving filebuf");
	filebuf = XtMalloc(XFER_FILEBUFLEN);

	xc.data_fd = data_fd;
	xc.src_file_fd = file_fd;
	xc.file_len = file_len;
	xc.file_index = 0;

	return 0;
}


/*
 * do_put - Transfer next part of file from local host to remote host.
 *          "xc" is used for input and output.  Returns 0 if file transfer
 *          complete, 1 if file transfer not complete, -4 for lost sink
 *          connection, -6 for a "552" reply (for LLNL Archive retry
 *          mechanism), and -1 for other errors.
 */
do_put()
{
	int tcp_index = 0;
	int nbytes;
	int code;
	char reply[MAXFTPREPLY];
	int i;
	int retval;
	int scode;

	if (xc.mode == ASCII) {
		if (xc.file_index < xc.file_len) {
			nbytes = MIN(xc.file_len-xc.file_index, XFER_FILEBUFLEN);
			if (iread(xc.src_file_fd, filebuf, nbytes) != nbytes) {
				report_perror(xc.src_host, xc.current_node->entry);
				return abort_put();
			}
			for (i=0; i<nbytes; i++)
				if (filebuf[i] == '\n') {
					if (tcp_index+2 >= XFER_USER_TCPBUFLEN) {
						retval = write_tcp(xc.snk_host, xc.data_fd, tcpbuf,
							tcp_index);
						if (retval != tcp_index) {
							if (retval == -3) {
								close(xc.src_file_fd);
								close(xc.data_fd);
								XtFree(tcpbuf);
								tcpbuf = NULL;
								XtFree(filebuf);
								filebuf = NULL;
								return -4;
							} else
								return abort_put();
						}
						tcp_index = 0;
					}
					tcpbuf[tcp_index++] = '\r';
					tcpbuf[tcp_index++] = '\n';
				} else {
					if (tcp_index+1 >= XFER_USER_TCPBUFLEN) {
						retval = write_tcp(xc.snk_host, xc.data_fd, tcpbuf,
							tcp_index);
						if (retval != tcp_index) {
							if (retval == -3) {
								close(xc.src_file_fd);
								close(xc.data_fd);
								XtFree(tcpbuf);
								tcpbuf = NULL;
								XtFree(filebuf);
								filebuf = NULL;
								return -4;
							} else
								return abort_put();
						}
						tcp_index = 0;
					}
					tcpbuf[tcp_index++] = filebuf[i];
				}
			xc.file_index += nbytes;
		}
		if (tcp_index) {
			retval = write_tcp(xc.snk_host, xc.data_fd, tcpbuf, tcp_index);
			if (retval != tcp_index) {
				if (retval == -3) {
					close(xc.src_file_fd);
					close(xc.data_fd);
					XtFree(tcpbuf);
					tcpbuf = NULL;
					XtFree(filebuf);
					filebuf = NULL;
					return -4;
				} else
					return abort_put();
			}
		}
	} else      /* Send binary file to FTP server */
		if (xc.file_index < xc.file_len) {
			nbytes = MIN(xc.file_len-xc.file_index, XFER_FILEBUFLEN);
			if (iread(xc.src_file_fd, filebuf, nbytes) != nbytes) {
				report_perror(xc.src_host, xc.current_node->entry);
				return abort_put();
			}
			retval = write_tcp(xc.snk_host, xc.data_fd, filebuf, nbytes);
			if (retval != nbytes) {
				if (retval == -3) {
					close(xc.src_file_fd);
					close(xc.data_fd);
					XtFree(tcpbuf);
					tcpbuf = NULL;
					XtFree(filebuf);
					filebuf = NULL;
					return -4;
				} else
					return abort_put();
			}
			xc.file_index += nbytes;
		}

	/* Have all the bytes been transferred? */
	if (xc.file_index < xc.file_len)
		return 1;

	/* Close source file */
	close(xc.src_file_fd);

	/* Close data connection */
	close(xc.data_fd);

	/* Free up user buffers */
	XtFree(tcpbuf);
	tcpbuf = NULL;
	XtFree(filebuf);
	filebuf = NULL;

	/* Get final response from FTP server */
	scode = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, maxt, True);
	switch (scode) {
	case 2:
		return 0;
	case -3:
		return -4;
	case -2:
		report_client_timeout(xc.snk_host);
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
 * abort_put - Abort transfer of file from local host to remote host.
 *             "xc" is used for input and output.  Returns -4 for lost
 *             sink connection, else -1.
 */
abort_put()
{
	int code;
	char reply[MAXFTPREPLY];
	int retval;

	/* Close source file */
	close(xc.src_file_fd);

	/* Close data connection */
	close(xc.data_fd);

	/* Make sure that user buffers have been freed */
	if (tcpbuf) {
		XtFree(tcpbuf);
		tcpbuf = NULL;
	}
	if (filebuf) {
		XtFree(filebuf);
		filebuf = NULL;
	}

	/* Get final response from FTP server */
	if ((retval = get_ftp_reply(xc.snk_host, reply, MAXFTPREPLY, &code, maxt,
			True)) == -3)
		return -4;
	else if (retval == -2)
		report_client_timeout(xc.snk_host);

	return -1;
}

