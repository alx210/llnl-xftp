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
#if defined(_AIX) || defined(__QNX__) 
#include <sys/select.h>
#endif
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include "xftp.h"
#include "str.h"

#define DM   242   /* Telnet commands */
#define IP   244
#define WILL 251
#define WONT 252
#define DO   253
#define DONT 254
#define IAC  255

int sig_pipe_invoked = False;

extern struct st_host_info hinfo[];
extern int diagnostics;
extern XtAppContext app;
extern Display *display;


/*
 * write_tcp - Writes "n" bytes from "buf" to the TCP connection on
 *             socket "fd" to "host".  Returns n if successful, -3 if
 *             connection broken, else -1.
 */
write_tcp(host, fd, buf, n)
int host;
int fd;
char *buf;
int n;
{

	int total_nbytes = 0;
	int nbytes;
	int saved_errno;

	while (total_nbytes < n)
		if ((nbytes = iwrite(fd, &buf[total_nbytes], n-total_nbytes)) < 0) {
			saved_errno = errno;
			report_perror(host, "Writing connection");
			if (saved_errno == EPIPE)
				return -3;
			else
				return -1;
		} else
			total_nbytes += nbytes;

	return total_nbytes;
}


/*
 * read_tcp - Reads "n" bytes to "buf" from the TCP connection on socket
 *            "fd" to "host".  Set process_xevents to True if X events
 *            should be processed while waiting for a reply, else
 *            use False.  Returns number of bytes read if no error,
 *            socket "fd".  If no data is received during time interval,
 *            -2 is returned.  If connection is lost, -3 is returned.
 *            Returns -1 for other errors.
 */
read_tcp(host, fd, buf, n, maxtime, process_xevents)
int host;
int fd;
char *buf;
int n;
int maxtime;
int process_xevents;
{

	int total_nbytes = 0;
	int nbytes;
	int saved_errno;

	while (total_nbytes < n)
		switch (select_on_read(fd, maxtime, process_xevents)) {
		case 1:
		    if ((nbytes = iread(fd, &buf[total_nbytes], n-total_nbytes)) < 0) {
				saved_errno = errno;
		    	report_perror(host, "Reading connection");
				if (saved_errno == ECONNRESET)
					return -3;
				else
		    		return -1;
		    } else if (nbytes == 0)
		    	return total_nbytes;
			else
				total_nbytes += nbytes;
		case 0:
			if (total_nbytes)
				return total_nbytes;
			else
				return -2;
		case -1:
			return -1;
	}
	return total_nbytes;
}


/*
 * select_on_read - Waits for file descriptor fd to be readable.  maxtime
 *                  is the number of seconds to wait.  Set process_xevents
 *                  to True if X events should be processed while waiting 
 *                  for a reply, else use False.  Returns values are 1
 *                  for fd is readable, 0 for timeout, and -1 for error.
 */
select_on_read(fd, maxtime, process_xevents)
int fd;
int maxtime;
int process_xevents;
{
	fd_set readfds;
	fd_set exceptfds;
	struct timeval timeout;
	int nfds;
	int xfd;

	/* Get file descriptor for display connection socket */
	xfd = ConnectionNumber(display);

	while (1) {
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		if (process_xevents)
			FD_SET(xfd, &readfds);
		FD_ZERO(&exceptfds);
		FD_SET(fd, &exceptfds);
		if (process_xevents)
			FD_SET(xfd, &exceptfds);
		timeout.tv_sec = maxtime;
		timeout.tv_usec = 0;
#if defined(_INCLUDE_HPUX_SOURCE) && defined(_PROTOTYPES)
		nfds = select(max_fds(), (int*)&readfds, NULL, (int*)&exceptfds,
			&timeout);
#else
		nfds = select(max_fds(), &readfds, NULL, &exceptfds, &timeout);
#endif
		if (nfds > 0) {
			if (FD_ISSET(xfd, &readfds) || FD_ISSET(xfd,&exceptfds))
				XtAppProcessEvent(app, (XtInputMask)XtIMAll);
			else
				return 1;
		} else
			return nfds;
	}
}


/*
 * get_ftp_reply - Returns in "reply" a string containing the next reply
 *                 from the FTP daemon.  The function return value is the
 *                 first digit of the reply code.  "host" is the host id.
 *                 Input argument "maxreply" is the maximum length string
 *                 to be returned.  "code" is the full reply code.
 *                 The input argument "maxtime" is the maximum allowable
 *                 interval in seconds in which no data is received.
 *                 Set process_xevents to True if X events should be
 *                 processed while waiting for a reply, else use False.
 *                 Other return values are -2 for timeout, -3 for
 *                 broken connection, and -1 for other failures.
 */
get_ftp_reply(host, reply, maxreply, code, maxtime, process_xevents)
int host;
char *reply;
int maxreply;
int *code;
int maxtime;
int process_xevents;
{
	int fd = hinfo[host].ctrl_fd;
	int firstline = True;
	int origcode;
	int replycode;
	int nbytes;
	int lenline;
	int lenreply;
	char line[MAXFTPLINE];
	static char *vms_timeout_pat = "500 FTP connection terminated";
	int len_vms_timeout_pat = strlen(vms_timeout_pat);

	reply[0] = '\0';

	while (1) {
		if ((replycode = get_ftp_line(host, fd, line, MAXFTPLINE, maxtime,
			process_xevents)) < 0)
			return replycode;
		lenline = strlen(line);
		lenreply = strlen(reply);
		nbytes = MIN(maxreply-lenreply-1, lenline);
		strncat(reply, line, nbytes);
		if (firstline) {
			if (replycode == 0)
				return -1;
			if (line[3] == '-') {
				origcode = replycode;
				firstline = False;
			} else
				break;
		} else if (replycode == origcode && line[3] == ' ')
			break;
	}

	if (diagnostics >= VERBOSE)
		print_ftp_reply(host, reply);

	/* Check for server timeout */
	if (hinfo[host].system == SYS_VMS) {
		if ((replycode == 421) ||
				!strncmp(line, vms_timeout_pat, len_vms_timeout_pat)) {
			write_log("*** FTP server on remote host timed out.\n");
			return -3;
		}
	} else if (replycode == 421) {
		write_log("*** FTP server on remote host timed out.\n");
		return -3;
	}

	*code = replycode;
	return replycode/100;
}


/*
 * get_ftp_line - Returns in "line" a string containing the next line
 *                from the FTP daemon.  The function return value is the
 *                reply code.  "fd" is the file descriptor of the socket
 *                used for the control connection.  The input argument
 *                "maxline" is the maximum length string to be returned.
 *                The input argument "maxtime" is the maximum allowable
 *                interval in seconds in which no data is received.
 *                Set process_xevents to True if X events should be
 *                processed while waiting for a reply, else use False.
 *                "host" is the host to get the FTP response from.
 *                The return value is for lines containing no reply code.
 *                Other return values are -2 for timeout, -3 for broken
 *                connection, and -1 for other failures.
 */
get_ftp_line(host, fd, line, maxline, maxtime, process_xevents)
int host;
int fd;
char *line;
int maxline;
int maxtime;
int process_xevents;
{
	int cnt;
	unsigned char ch;
	char buf[3];
	int code = 0;
	int indx = 0;
	int retval;

	while (1) {
		if ((cnt = read_tcp(host, fd, (char *)&ch, 1, maxtime, process_xevents))
				< 0)
			return cnt;
		else if (cnt == 0)
			return -3;
		if (ch == IAC) {
			if ((cnt = read_tcp(host, fd, (char *)&ch, 1, maxtime,
					process_xevents)) < 0)
				return cnt;
			if (cnt == 0)
				return -3;
			switch (ch) {
			case DO:
			case DONT:
				if ((cnt = read_tcp(host, fd, (char *)&ch, 1, maxtime,
						process_xevents)) < 0)
					return cnt;
				if (cnt == 0)
					return -3;
				buf[0] = IAC;
				buf[1] = DONT;
				buf[2] = ch;
				if ((retval = write_tcp(host, fd, buf, 3)) != 3)
					return retval;
				break;
			case WILL:
			case WONT:
				if ((cnt = read_tcp(host, fd, (char *)&ch, 1, maxtime,
						process_xevents)) < 0)
					return cnt;
				if (cnt == 0)
					return -3;
				buf[0] = IAC;
				buf[1] = WONT;
				buf[2] = ch;
				if ((retval = write_tcp(host, fd, buf, 3)) != 3)
					return retval;
			}
			continue;
		}
		if (ch != '\r' && indx < maxline-1)
			line[indx++] = ch;
		if (ch == '\n') {
			if (indx >= 4 && (line[3] == ' ' || line[3] == '-')
						   && isdigit(line[0]) && isdigit(line[1])
						   && isdigit(line[2]))
				code = (line[0]-'0')*100+(line[1]-'0')*10+(line[2]-'0');
			line[indx] = '\0';
			return code;
		}
	}
}


/*
 * send_ftp_cmd - Sends command "cmd" to the FTP server.  "host" is the host
 *                id.  "\r\n" is appended to cmd before it is sent.  Returns
 *                0 for success, -3 for broken socket, and -1 for other
 *                errors.
 */
send_ftp_cmd(host, cmd)
int host;
char *cmd;
{
	int fd = hinfo[host].ctrl_fd;
	int len = strlen(cmd);
	int retval;
	char *buf;

	if (diagnostics == DEBUG)
		print_ftp_cmd(host,cmd);

	buf = XtMalloc(len+3);
	strcpy(buf, cmd);
	strcat(buf, "\r\n");
	retval = write_tcp(host, fd, buf, len+2);
	bzero(buf, (int)strlen(buf));
	XtFree(buf);
	if (retval != len+2)
		return retval;
	else
		return 0;
}


/*
 * sig_pipe - Will be invoked if trying to write on a broken connection.
 */
sig_pipe()
{
	sig_pipe_invoked = True;
}


/*
 * urgent_abort - Sends an urgent abort request to the FTP server
 *                connected via the TCP connection for "host".
 *                Returns 0 if successful, -3 for broken connection,
 *                and -1 for other errors.  This routine does not
 *                try to receive a reply from the FTP server.
 */
urgent_abort(host)
int host;
{
	int fd = hinfo[host].ctrl_fd;
	char buf[2];
	int retval;

	if (diagnostics == DEBUG)
		print_ftp_cmd(host, "Urgent Abort");

	/* Send IAC, IP */
	buf[0] = IAC;
	buf[1] = IP;
	if ((retval = write_tcp(host, fd, buf, 2)) != 2)
		return retval;

	/* Send IAC, DM in urgent mode */
	buf[0] = IAC;
	if (isend(fd, buf, 1, MSG_OOB) != 1)
		return -1;
	buf[0] = DM;

	/* Send ABOR command */
	if ((retval = write_tcp(host, fd, buf, 1)) != 1)
		return retval;
	if ((retval = write_tcp(host, fd, "ABOR\r\n", 6)) != 6)
		return retval;

	return 0;
}

