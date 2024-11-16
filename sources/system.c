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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>


/*
 * iaccept - Same as accept() system call except for automatic recovery from
 *           EINTR errors.
 */
iaccept(s, addr, addrlen)
int s;
struct sockaddr *addr;
int *addrlen;
{
	int ret;

	while (1) {
#ifdef UNIXWARE
		ret = accept(s, addr, (size_t *)addrlen);
#else
		ret = accept(s, addr, addrlen);
#endif
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * iconnect - Same as connect() system call except for automatic recovery from
 *            EINTR errors.
 */
iconnect(s, name, namelen)
int s;
struct sockaddr *name;
int namelen;
{
	int ret;

	while (1) {
		ret = connect(s, name, namelen);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * iopen2 - Same as open() system call with 2 arguments except for
 *          automatic recovery from EINTR errors.
 */
iopen2(path, flags)
char *path;
int flags;
{
	int ret;

	while (1) {
		ret = open(path, flags);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * iopen3 - Same as open() system call with 3 arguments except for
 *          automatic recovery from EINTR errors.
 */
iopen3(path, flags, mode)
char *path;
int flags;
int mode;
{
	int ret;

	while (1) {
		ret = open(path, flags, mode);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * iread - Same as read() system call except for automatic recovery from
 *         EINTR errors.
 */
iread(fd, buf, nbyte)
int fd;
char *buf;
int nbyte;
{
	int ret;

	while (1) {
		ret = read(fd, buf, nbyte);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * isend - Same as send() system call except for automatic recovery from
 *         EINTR errors.
 */
isend(s, msg, len, flags)
int s;
char *msg;
int len;
int flags;
{
	int ret;

	while (1) {
		ret = send(s, msg, len, flags);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * iwrite - Same as write() system call except for automatic recovery from
 *          EINTR errors.
 */
iwrite(fd, buf, nbyte)
int fd;
char *buf;
int nbyte;
{
	int ret;

	while (1) {
		ret = write(fd, buf, nbyte);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}


/*
 * iwait - Same as wait() system call except for automatic recovery from
 *         EINTR errors.
 */
iwait(status)
#if defined(ultrix)
union wait *status;
#else
int *status;
#endif
{
	int ret;

	while (1) {
		ret = wait(status);
		if ((ret < 0) && (errno == EINTR))
			continue;
		else
			return ret;
	}
}

/*
 * ifgets - Same as fgets() except for automatic recovery from EINTR errors.
 */
char *
ifgets(s, n, stream)
char *s;
int n;
FILE *stream;
{
	char *ret;

	while (1) {
		ret = fgets(s, n, stream);
		if (ret == NULL && !feof(stream) && ferror(stream) && errno == EINTR)
			clearerr(stream);
		else
			return ret;
	}
}

