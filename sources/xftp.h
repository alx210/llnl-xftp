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

#ifndef __xftp_h
#define __xftp_h

#include <sys/param.h>
#ifdef ISC    /* SunSoft ISC Unix 4.0 */
#include <sys/limits.h>
#endif

#define LLNL  1   /* Flags LLNL-specific logic to be removed later */

#define LEFT	     0
#define RIGHT	     1
#define LOCAL_HOST   2
#define NUMHOSTS     2

#define SYS_UNIX 0
#define SYS_VMS  1
#define SYS_MAC  2
#define SYS_DOS  3
#define SYS_NT   4

#define SERVER_UNKNOWN           0
#define SERVER_VMS_3_0           1
#define SERVER_VMS_MULTINET      2
#define SERVER_MAC_VERSATERM     3
#define SERVER_MAC_NCSATELNET    4
#define SERVER_MAC_FTPD          5
#define SERVER_DOS_WINQVT        6
#define SERVER_NT                7
#define SERVER_UNIX_UNITREE      8
#define SERVER_UNIX_NSL_UNITREE  9

#define LOCAL	0
#define REMOTE	1

#define NEITHER 2

#define ASCII  0
#define BINARY 1

#define REPLACE 0
#define ADD     1

#define QUIET	0
#define NORMAL	1
#define VERBOSE	2
#define DEBUG	3

#define DIRECTORY	0
#define WILDCARD	1
#define QUOTE		2
#define USER		3
#define NUMCACHES	4

#define MAXDIRCACHE 30

#define MAXFTPREPLY 500
#define MAXFTPLINE  200

#define MAXLINKS	20

#ifndef MAXPATHLEN
#ifdef PATH_MAX
#define MAXPATHLEN PATH_MAX
#else
#ifdef PATHSIZE
#define MAXPATHLEN PATHSIZE
#endif
#endif
#endif

#ifndef MIN
#define MIN(a, b) ((a)<(b))?(a):(b)
#endif

#if defined (SYSV) || defined (SYSV386) || defined (SVR4)
#define bzero(b, n)         memset(b, 0, n)
#define bcopy(b1, b2, n)    memcpy(b2, b1, n)
#endif

struct st_host_info {
	int		type;		/* LOCAL, REMOTE, or NEITHER */
	int		ctrl_fd;	/* For remote hosts */
	int		system;     /* System of FTP server (e.g., SYS_UNIX) */
	int		server;		/* FTP server */
	char	*wd;		/* Current working directory */
	char	*hostname;	/* Name of host */
	char	*username;	/* Name of user */
	char	*move_wd;	/* Last destination directory used in move operation */
	char	*password;	/* Password */
	char 	*account;	/* Account */
};

#endif  /* __xftp_h */

