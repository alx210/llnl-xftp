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
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include "xftp.h"
#include "str.h"

extern struct st_host_info hinfo[];
extern int diagnostics;
extern int maxt;


/*
 * ftp_open - Open a control connection with the ftp server specified
 *            by "ip_addr" and "port".  "host" specifies LEFT or RIGHT host.
 *            If successful, the function value is 0 and hinfo[host].ctrl_fd
 *            is set to the file descriptor used for the control connection
 *            socket; else -1 is returned.  If successful, updates
 *            hinfo[host].system and hinfo[host].server.
 */
ftp_open(host, ip_addr, port)
int host;
char *ip_addr;
int port;
{
	int ctrl_fd;
	struct sockaddr_in remote;
	int code;
	char reply[MAXFTPREPLY];
	struct hostent *hostptr;
	char msg[50];
	int scode;
	unsigned long temp;

	/* Form destination address */
	bzero(&remote, (int)sizeof(remote));
	if ((temp = inet_addr(ip_addr)) != ~0L)
		remote.sin_addr.s_addr = temp;
	else {
		if ((hostptr = gethostbyname(ip_addr)) == NULL)
			return -1;
		if (hostptr->h_addrtype != AF_INET)
			return -1;
		remote.sin_addr.s_addr =
	           	((struct in_addr*)hostptr->h_addr_list[0])->s_addr;
	}
	remote.sin_family = AF_INET;
	remote.sin_port = htons((short)port);
	if (diagnostics == DEBUG) {
		sprintf(msg, "*** dest_addr = %lx\n",
		        (unsigned long)remote.sin_addr.s_addr);
		write_log(msg);
	}

	/* Create socket */
	if ((ctrl_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		report_perror(host, "Opening socket stream");
		return -1;
	}

	/* Connect socket */
	if (diagnostics == DEBUG)
		write_log("*** Will try to connect to remote\n");
	if (iconnect(ctrl_fd, (struct sockaddr *)&remote,(int)sizeof(remote)) < 0) {
		report_perror(host, "Connecting socket stream");
		close(ctrl_fd);
		return -1;
	}

	if (diagnostics == DEBUG)
		write_log("*** Was able to connect to remote\n");

	/* Get response from FTP server */
	hinfo[host].ctrl_fd = ctrl_fd;
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		determine_server_from_open(host, reply);
		return 0;
	case -2:
		report_client_timeout(host);
	default:
		close(hinfo[host].ctrl_fd);
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
	}
}


/*
 * ftp_user - Sends USER command to FTP server.  "host" is the host id.
 *            Returns 0 if successful, 1 if successful but a password is
 *            required, -3 for broken connection, and -1 for other
 *            errors.
 */
ftp_user(host, username)
int host;
char *username;
{
	char cmd[100];
	char reply[MAXFTPREPLY];
	int code;
	int retval;

	/* Send USER command to FTP server */
	sprintf(cmd, "USER %s", username);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Process reply */
	retval = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (retval) {
	case 2:
		return 0;
	case 3:
		return 1;
	case -2:
		report_client_timeout(host);
		return -1;
	default:
		if (retval > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return retval;
	}
}


/*
 * ftp_password - Sends PASS command to FTP server.  "host" is the host
 *                id.  Returns 0 if successful, 1 if successful but an
 *                account is required, -3 for broken connection,
 *                and -1 for other errors.
 */
ftp_password(host, password)
int host;
char *password;
{
	char cmd[100];
	char reply[MAXFTPREPLY];
	int code;
	int retval;
	int scode;

	/* Send PASS command to FTP server */
	sprintf(cmd, "PASS %s", password);
	retval = send_ftp_cmd(host, cmd);
	bzero(cmd, (int)strlen(cmd));
	if (retval < 0)
		return retval;

	/* Process reply */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		return 0;
	case 3:
		return 1;
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
 * ftp_account - Sends ACCT command to FTP server.  "host" is the host id.
 *               Returns 0 if successful, -3 for broken connection, and -1
 *               for other errors.
 */
ftp_account(host, account)
int host;
char *account;
{
	char cmd[100];
	char reply[MAXFTPREPLY];
	int code;
	int retval;
	int scode;

	/* Send ACCT command to FTP server */
	sprintf(cmd, "ACCT %s", account);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Process reply */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		return 0;
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
 * ftp_passive - Tell server to enter passive mode.  If successful,
 *               returns pointer to string containing port address
 *               returned by server in "port", else "port" is undefined.
 *               "host" is the host id.  Use XtFree() to release
 *               memory pointed to by "port".  Returns 0 if successful,
 *               -3 for broken connection, and -1 for other errors.
 */
ftp_passive(host, port)
int host;
char **port;
{
	char reply[MAXFTPREPLY];
	int code;
	int nbytes;
	char *start;
	char *end;
	int retval;

	/* Send PASV command to FTP server */
	if ((retval = send_ftp_cmd(host, "PASV")) < 0)
		return retval;

	/* Get response from FTP server */
    retval = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
    switch (retval) {
    case 2:
        break;
    case -2:
		report_client_timeout(host);
        return -1;
	case -3:
		return -3;
    default:
        return -1;
    }

	/* Extract port address */
	if ((start = strchr(reply, '(')) == NULL)
		return -1;
	start++;
	if ((end = strrchr(start, ')')) == NULL)
		return -1;
	if ((nbytes = end-start) <= 0)
		return -1;
	*port = XtMalloc(nbytes+1);
	bcopy(start, *port, nbytes);
	(*port)[nbytes] = '\0';
	return 0;
}


/*
 * ftp_type - Set file tranfer mode.  "host" is the host id.  Valid values
 *            for "mode" are ASCII and BINARY.  Returns 0 for success,
 *            -3 for broken connection, and -1 for other errors.
 */
ftp_type(host, mode)
int host;
int mode;
{
	char reply[MAXFTPREPLY];
	char cmd[7];
	int code;
	int retval;
	char type;
	int scode;

	/* Send TYPE command to FTP server */
	if (mode == ASCII)
		type = 'A';
	else if (mode == BINARY)
		type = 'I';
	else
		return -1;
	sprintf(cmd, "TYPE %c", (char)type);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		return 0;
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
 * ftp_port - Set the data port to be used in data connection to the
 *            specified value.  "hostport" is a string of the form
 *            h1,h2,h3,h4,p1,p2  where the hi's are the decimal-
 *            valued bytes of the host's Internet address, and the pi's
 *            are the decimal valued bytes of the port's address.
 *            "host" is the host id.  Returns 0 if successful, -3 for
 *            broken connection, and -1 for other errors.
 */
ftp_port(host, hostport)
int host;
char *hostport;
{
	char reply[MAXFTPREPLY];
	char cmd[37];
	int code;
	int retval;
	int scode;

	/* Send PORT command to FTP server */
	sprintf(cmd, "PORT %s", hostport);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		return 0;
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
 * ftp_system - Tries to determine the type of system and server the
 *              remote host has by using the low-level SYST command.
 *              Set "print_error_msg" to True if FTP-generated error
 *              messages are to be written to the log when not in verbose
 *              mode.  Returns 0 for success, -3 for broken connection,
 *              and -1 for other errors.  If successful, and if enough
 *              information is returned, hinfo[host].system and
 *              hinfo[host].server are set.
 */
ftp_system(host, print_error_msg)
int host;
int print_error_msg;
{
	char reply[MAXFTPREPLY];
	int code;
	int retval;
	int scode;

	/* Send SYST command to FTP server */
	if ((retval = send_ftp_cmd(host, "SYST")) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		determine_server_from_syst(host, reply);
		return 0;
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
 * ftp_allocate - Tell the server to reserve "size" bytes of space for a
 *                subsequent store command.  Returns 0 if successful,
 *                -3 for broken connection, and -1 for other errors.
 *                "host" is the host id.
 */
ftp_allocate(host, size)
int host;
long size;
{
	char cmd[100];
	char reply[MAXFTPREPLY];
	int code;
	int scode;
	int retval;

	/* Send ALLO command to FTP server */
	sprintf(cmd, "ALLO %ld", size);
	if ((retval = send_ftp_cmd(host, cmd)) < 0)
		return retval;

	/* Get response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
	switch (scode) {
	case 2:
		return 0;
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

