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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "xftp.h"
#include "xfer.h"

#define MAXFILEBUF 2048

extern struct xfer_ctrl_block xc;
extern int store_unique;

char *create_unique_name();


/*
 * init_local_copy - Initialize transfer of file from local host to local
 *                   host.  "xc" is used for input and output.  Returns 0
 *                   if successful, else -1.
 */
init_local_copy()
{
	int src_file_fd;
	int snk_file_fd;
	char *snk_path;
	char *src_file;
	char snk_file[MAXPATHLEN+1];
	char *tmp_snk_file;
	long file_len;
	struct stat status;
	char msg[MAXPATHLEN+30];
	char *dir;
	int retval;

	/* Get path of sink file */
	snk_paths_of_xfer_node(xc.current_node, NULL, &tmp_snk_file);
	strcpy(snk_file, tmp_snk_file);
	XtFree(tmp_snk_file);

    /* Get unique file name if requested */
    if (store_unique) {
        if ((tmp_snk_file = create_unique_name(snk_file)) == NULL)
            return -1;
		strcpy(snk_file, tmp_snk_file);
    }

    /* Create temporary directory for view file */
    if (xc.operation == VIEW) {
		snk_paths_of_xfer_node(xc.current_node, NULL, &snk_path);
        parse_path(SYS_UNIX, snk_path, &dir, NULL);
		XtFree(snk_path);
        retval = local_mkdir(xc.snk_host, dir, 0700);
        XtFree(dir);
        if (retval)
            return -1;
    }

    /* Verify that a writable sink file can be created */
    if (!can_create_sinkfile(xc.snk_host, snk_file))
        return -1;

    /* Open source file and verify that source file is a "real" file */
	src_paths_of_xfer_node(xc.current_node, NULL, &src_file);
    if ((src_file_fd = iopen2(src_file, O_RDONLY)) < 0) {
        report_perror(xc.src_host, src_file);
		XtFree(src_file);
        return -1;
    }
    if (fstat(src_file_fd, &status) < 0 || !S_ISREG(status.st_mode)) {
		sprintf(msg, "%s is not a \"real\" file", src_file);
		warning_error(msg);
        close(src_file_fd);
		XtFree(src_file);
        return -1;
    }
	XtFree(src_file);

    /* Get length of file */
    file_len = status.st_size;

	/* Create sink file */
	if (unlink(snk_file) == -1)
		if (errno != ENOENT) {
			report_perror(xc.snk_host, snk_file);
			close(src_file_fd);
			return -1;
		}
	if ((snk_file_fd = iopen3(snk_file, O_CREAT|O_WRONLY, 0644)) < 0) {
		report_perror(xc.snk_host, snk_file);
		close(src_file_fd);
		return -1;
	}

	xc.src_file_fd = src_file_fd;
	xc.snk_file_fd = snk_file_fd;
	xc.file_len = file_len;
    xc.file_index = 0;

    return 0;
}


/*
 * do_local_copy - Transfer next part of file from local host to local
 *                 host.  "xc" is used for input and output.  Returns 0
 *                 if file transfer complete, 1 if file transfer not
 *                 complete, and -1 for error.
 */
do_local_copy()
{
	int nbytes;
	char filebuf[MAXFILEBUF];
	char *src_file;
	char *snk_file;

	if (xc.file_index < xc.file_len) {
		nbytes = MIN(xc.file_len-xc.file_index, MAXFILEBUF);
		if (iread(xc.src_file_fd, filebuf, nbytes) != nbytes) {
			src_paths_of_xfer_node(xc.current_node, NULL, &src_file);
			report_perror(xc.src_host, src_file);
			XtFree(src_file);
			close(xc.src_file_fd);
			close(xc.snk_file_fd);
			return -1;
		}
		if (iwrite(xc.snk_file_fd, filebuf, nbytes) != nbytes) {
			snk_paths_of_xfer_node(xc.current_node, NULL, &snk_file);
			report_perror(xc.snk_host, snk_file);
			XtFree(snk_file);
			close(xc.src_file_fd);
			close(xc.snk_file_fd);
			return -1;
		}
		xc.file_index += nbytes;
	}

    /* Have all the bytes been transferred? */
    if (xc.file_index < xc.file_len)
        return 1;

    /* Close files */
    close(xc.src_file_fd);
	close(xc.snk_file_fd);

	return 0;
}


/*
 * abort_local_copy - Abort transfer of file from local host to local host.
 *                    "xc" is used for input and output.  Returns -1.
 */
abort_local_copy()
{
	close(xc.src_file_fd);
	close(xc.snk_file_fd);
	return -1;
}

