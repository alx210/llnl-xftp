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
#include "xftp.h"

extern struct st_host_info hinfo[];
extern int diagnostics;

/*
 * init_data_conn - Initialize a data connection between the FTP server and
 *                  the client.  "host" is the host id.  Returns the file
 *                  of the data connection socket if successful,
 *                  -3 for broken connection, and -1 for other errors.
 */
init_data_conn(host)
int host;
{
    int sock;
    struct sockaddr_in data;
	int addrlen;
	char buf[32];
	unsigned char *h;
	unsigned char port[2];
	int retval;
	unsigned int hostport;

    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        report_perror(host, "Opening stream socket");
     	return -1;
    }

    /* Bind socket.  Use same IP address as for control connection */
	addrlen = sizeof(data);
#ifdef UNIXWARE
	if (getsockname(hinfo[host].ctrl_fd, (struct sockaddr *)&data,
			(size_t *)&addrlen) < 0) {
#else
	if (getsockname(hinfo[host].ctrl_fd, (struct sockaddr *)&data, &addrlen)
			< 0) {
#endif
		report_perror(host, "Gettin socket name");
		close(sock);
		return -1;
	}
    data.sin_port = 0;
	if (bind(sock, (struct sockaddr *)&data, (int)sizeof(data)) < 0) {
		report_perror(host, "Binding stream socket");
		close(sock);
   		return -1;
	}

	/* Listen for connection */
	if (listen(sock, 1) < 0) {
		report_perror(host, "Listen");
		close(sock);
		return -1;
	}

	/* Tell FTP server what host/port to connect to */
#ifdef UNIXWARE
	if (getsockname(sock, (struct sockaddr *)&data, (size_t *)&addrlen) < 0) {
#else
	if (getsockname(sock, (struct sockaddr *)&data, &addrlen) < 0) {
#endif
		report_perror(host, "Getting socket name");
		close(sock);
		return -1;
	}
	h = (unsigned char *)&data.sin_addr;
	hostport = ntohs(data.sin_port);
	port[0] = (hostport&0xff00)>>8;
	port[1] = hostport&0xff;
	sprintf(buf, "%d,%d,%d,%d,%d,%d", h[0], h[1], h[2], h[3],
			port[0], port[1]);
	if ((retval = ftp_port(host, buf)) < 0) {
		close(sock);
		return retval;
	}

	return sock;
}

/*
 * accept_data_conn - Establish a data connection between the FTP server and
 *                    the client.  "data_fd" is the data connection socket.
 *                    "host" is the host to connect to.  Returns the file
 *                    descriptor of the data connection socket
 *                    if successful, else -1.  "data_fd" is closed.
 */
accept_data_conn(host, data_fd)
int host;
int data_fd;
{
	struct sockaddr_in remote;
	int addrlen;
	int new_data_fd;

	/* Accept connection */
	if (diagnostics == DEBUG)
		write_log("*** Will try to accept connection\n");
	addrlen = sizeof(remote);
	new_data_fd = iaccept(data_fd, (struct sockaddr *)&remote, &addrlen);
	if (new_data_fd < 0) {
		report_perror(host, "accept");
		close(data_fd);
		return -1;
	}
	if (diagnostics == DEBUG)
		write_log("*** Was able to accept connection\n");

	/* Success */
	close(data_fd);
	return new_data_fd;
}

