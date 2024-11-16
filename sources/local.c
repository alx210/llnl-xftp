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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <Xm/Xm.h>
#include "xftp.h"


#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  100
#endif


/*
 * local_cd - Change local working directory to specified directory.
 *            Set print_error_msg to True if error messages are to be
 *            written to the log.  Returns 0 if successful, else -1.
 *            "host" is used only for error reporting.
 */
local_cd(host, directory, print_error_msg)
int host;
char *directory;
int print_error_msg;
{
	int retval;

	if ((retval = chdir(directory)) != 0) {
		if (print_error_msg)
			report_perror(host, directory);
	}

	return retval;
}


/*
 * local_pwd - Get local working directory name.  If successful, "name"
 *             is set to point to a string allocated by local_pwd, else
 *             "name" is undefined.  Returns 0 if successful, else -1.
 *              Caller can release memory by calling XtFree();
 */
local_pwd(name)
char **name;
{
	char pathname[MAXPATHLEN];
	char *retval;

	retval = getcwd(pathname, MAXPATHLEN);

	if (retval == NULL)
		return -1;
	else {
		*name = XtNewString(pathname);
		return 0;
	}
}


/*
 * local_mkdir - Create directory on local host.  Returns 0 if successful,
 *               else -1.  "host" is used only for error reporting.  The
 *               mode mask of directory is initialized from mode.
 */
local_mkdir(host, directory, mode)
int host;
char *directory;
int mode;
{
	int retval;

	if ((retval = mkdir(directory, mode)) != 0)
		report_perror(host, directory);

	return retval;
}


/*
 * local_delete - Delete file/directory from local host.  Returns 0 if
 *                successful, else -1.  "host" is used only for error
 *                reporting.
 */
local_delete(host, path)
int host;
char *path;
{
	char *dir;
	char *entry;
	int result;

	if (unlink(path) == 0)
		return 0;
	else if (errno == EPERM) {
		parse_path(SYS_UNIX, path, &dir, &entry);
		result = (local_cd(host, dir, False) == 0 && rmdir(entry) == 0);
		XtFree(dir);
		XtFree(entry);
		if (result)
			return 0;
	}

	report_perror(host, path);
	return -1;
}


/*
 * local_rename - Rename/move a directory entry from "old_path" to
 *                "new_path" on local host.  Returns 0 if successful,
 *                else -1.  "host" is used only for error reporting.
 */
local_rename(host, old_path, new_path)
int host;
char *old_path;
char *new_path;
{
	int retval;

	if ((retval = rename(old_path, new_path)) != 0)
		report_perror(host, "Rename");

	return retval;
}


/*
 * max_fds - Returns the maximum allowable number of open files.
 */
max_fds()
{
    int max_files;

#ifdef _SC_OPEN_MAX         /* POSIX */
    if ((max_files = sysconf(_SC_OPEN_MAX)) == -1)
        fatal_error("Trouble in max_fds() - sysconf() failed");
#else
#ifdef _NFILE               /* Might be overkill */
    max_files = _NFILE;
#else                       /* Assume BSD */
    max_files = getdtablesize();
#endif
#endif

#if defined(__QNX__)
	/* select will prematurely timeout if value returned by sysconf is used */
	if (max_files > FD_SETSIZE)
		max_files = FD_SETSIZE;
#endif

    return max_files;
}


/*
 * local_host_name - Returns a pointer to a string containing the name
 *                   of the local host.  Call XtFree() to release memory.
 */
char *
local_host_name()
{
    char name[MAXHOSTNAMELEN+1];

    if (gethostname(name, MAXHOSTNAMELEN) == -1)
        fatal_error("Trouble in local_host_name()");

    return XtNewString(name);
}

