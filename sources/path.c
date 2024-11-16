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

#include <X11/Intrinsic.h>
#include "xftp.h"
#include "str.h"


/*
 * unix_path_to_links - Parses the UNIX path name into its constituent links. 
 *                      unix_path_to_links returns a pointer to an array of
 *                		pointers to the link names.  The last entry in the
 *                 		array is a NULL pointer.  Call release_path_links()
 *                      to free memory returned by unix_path_to_links().
 */
char **
unix_path_to_links(path)
char *path;
{
	char *p;
	char *q;
	char *pathcopy;
	char **links;
	int nlinks;

	/* Check for null path name */
	if (path[0] == '\0')
		fatal_error("Trouble in unix_path_to_links()");

	/* Need to copy path because routine strtok() is destructive */
	pathcopy = XtNewString(path);

	/* Allocate space for array of link names */
	links = (char **)XtMalloc(sizeof(char *)*(MAXLINKS+1));
		
	/* Special case first link of full path name */
	if (pathcopy[0] == '/') {
#ifdef apollo
		if (pathcopy[1] == '/') {
			links[0] = XtNewString("//");
			p = &pathcopy[2];
		}
#else
		links[0] = XtNewString("/");
		p = &pathcopy[1];
#endif
		nlinks = 1;
	} else {
		p = pathcopy;
		nlinks = 0;
	}

	/* Loop through links of path name */
	while ((q=strtok(p, "/")) != NULL) {
		if (nlinks == MAXLINKS)
			fatal_error("Trouble in unix_path_to_links()");
		links[nlinks++] = XtNewString(q);
		p = NULL;
	}

	/* Success */
	links[nlinks] = NULL;
	XtFree(pathcopy);
	return links;
}


/*
 * dos_path_to_links - Parses the DOS path name into its constituent links. 
 *                     dos_path_to_links returns a pointer to an array of
 *              	   pointers to the link names.  The last entry in the
 *                 	   array is a NULL pointer.  Call release_path_links()
 *                     to free memory returned by dos_path_to_links().
 */
char **
dos_path_to_links(path)
char *path;
{
	char *p;
	char *q;
	char *pathcopy;
	char **links;
	int nlinks;
	int len;

	/* Check for null path name */
	if (path[0] == '\0')
		fatal_error("Trouble in dos_path_to_links()");

	/* Need to copy path because routine strtok() is destructive */
	pathcopy = XtNewString(path);

	/* Allocate space for array of link names */
	links = (char **)XtMalloc(sizeof(char *)*(MAXLINKS+1));
		
	/* Special case first link of full path name */
	len = strlen(pathcopy);
	p = pathcopy;
	if ((len > 1) && (pathcopy[1] == ':')) {
		links[0] = XtMalloc(4);
		links[0][0] = *p++;
		links[0][1] = *p++;
		if ((len > 2) && (pathcopy[2] == '\\')) {
			links[0][2] = *p++;
			links[0][3] = '\0';
		} else
			links[0][2] = '\0';
		nlinks = 1;
	} else
		nlinks = 0;

	/* Loop through links of path name */
	while ((q=strtok(p, "\\")) != NULL) {
		if (nlinks == MAXLINKS)
			fatal_error("Trouble in dos_path_to_links()");
		links[nlinks++] = XtNewString(q);
		p = NULL;
	}

	/* Success */
	links[nlinks] = NULL;
	XtFree(pathcopy);
	return links;
}


/*
 * vms_path_to_links - Parses the path name into its constituent links. 
 *                     vms_path_to_links returns a pointer to an array of
 *                	   pointers to the link names.  The last entry in the
 *                 	   array is a NULL pointer.  Call release_path_links()
 *                     to free memory returned by vms_path_to_links().
 *                     Some examples:
 *
 *                        a::b:[c.d]e --> a::, b:, c.dir, d.dir, e
 *                        b:[c.d]e    --> b:, c.dir, d.dir, e
 *                        [.c.d]e     --> c.dir, d.dir, e
 *                        [.c]        --> c.dir
 *                        e           --> e
 *                        c.dir       --> c.dir
 */
char **
vms_path_to_links(path)
char *path;
{
	char *pathcopy;
	char **links;
	int nlinks = 0;
	char *q;
	char *p;
	int len;
	char *filename;
	int i;

	/* Check for null path name */
	if (path[0] == '\0')
		fatal_error("Trouble in vms_path_to_links()");

	/* Need to copy path because routine strtok() is destructive */
	pathcopy = XtNewString(path);

	/* Allocate space for array of link names */
	links = (char **)XtMalloc(sizeof(char *)*(MAXLINKS+1));

    /* Network node */
    p = pathcopy;
    if ((q = strstr(p, "::"))) {
        len = q-p+2;
        links[nlinks] = XtMalloc(len+1);
        for (i=0; i<len; i++)
            links[nlinks][i] = p[i];
        links[nlinks][len] = '\0';
        nlinks++;
        p = q+2;
    }

    /* Device */
    if ((q = strchr(p, ':'))) {
        len = q-p+1;
        links[nlinks] = XtMalloc(len+1);
        for (i=0; i<len; i++)
            links[nlinks][i] = p[i];
        links[nlinks][len] = '\0';
        nlinks++;
        p = q+1;
    }

    /* Directory and file */
    if (*p == '[') {
        p++;
		if (*p == '.') {
			if (nlinks != 0)
				fatal_error("Trouble in vms_path_to_links()");
			p++;
		} else if (nlinks == 0)
			fatal_error("Trouble in vms_path_to_links()");
        filename = strchr(pathcopy, ']');
        if (filename == NULL || filename == p)
            fatal_error("Trouble in vms_path_to_links()");
        *filename++ = '\0';
        while ((q=strtok(p, ".")) != NULL) {
            if (nlinks == MAXLINKS)
                fatal_error("Trouble in vms_path_to_links()");
            links[nlinks] = XtMalloc(strlen(q)+5);
            strcpy(links[nlinks], q);
            strcat(links[nlinks], ".dir");
            nlinks++;
            p = NULL;
        }
        if (strlen(filename)) {
            if (nlinks == MAXLINKS)
                fatal_error("Trouble in vms_path_to_links()");
            links[nlinks++] = XtNewString(filename);
        }
    } else {
        links[nlinks++] = XtNewString(p);
    }

	/* Success */
	links[nlinks] = NULL;
	XtFree(pathcopy);
	return links;
}


/*
 * path_to_links - Parses the path name into its constituent links. 
 *                 path_to_links returns a pointer to an array of
 *                 pointers to the link names.  The last entry in the
 *                 array is a NULL pointer.  Call release_path_links()
 *                 to free memory returned by path_to_links().  "system"
 *                 is identifies the system (e.g.,  SYS_UNIX).
 */
char **
path_to_links(system, path)
int system;
char *path;
{
	switch (system) {
	case SYS_VMS:
		return vms_path_to_links(path);
	case SYS_DOS:
		return dos_path_to_links(path);
	default:
		return unix_path_to_links(path);
	}
}


/*
 * release_path_links - Release the memory returned by path_to_links().
 */
release_path_links(links)
char **links;
{
	char **ptr = links;
	
	/* Check for null pointer */
	if (links == NULL)
		return;

	/* Release memory */
	while (*ptr)
		XtFree(*ptr++);
	XtFree ((char *)links);
	links = NULL;
}


/*
 * reverse_link_order - Reverse the order of the directory links in the
 *                      array returned by path_to_links().
 */
reverse_link_order(links)
char **links;
{
	int nlinks = 0;
	int nlim;
	int i;
	char **ptr = links;
	char *temp;

	while (*ptr++)
		nlinks++;

	nlim = nlinks/2;

	for (i=0; i<nlim; i++) {
		temp = links[i];
		links[i] = links[nlinks-1-i];
		links[nlinks-1-i] = temp;
	}
}


/*
 * unix_links_to_path - Construct a Unix path from its constituent links.
 *                      "links" is a pointer to an array of pointers to the
 *                      link names.  "nlinks" is the number of entries in
 *                      "links".  Call XtFree() to free the memory returned
 *                      by unix_links_to_path().
 */
char *
unix_links_to_path(links, nlinks)
char **links;
int nlinks;
{
	int len = 0;
	int i;
	char *path;

    /* Determine length of path name */
    for (i=0; i<nlinks; i++)
        len += strlen(links[i])+1;

    /* Build path name for new working directory */
    path = XtMalloc(len+1);
    strcpy(path, "");
    for (i=0; i<nlinks; i++) {
        strcat(path, links[i]);
        if (path[strlen(path)-1] != '/' && i < nlinks-1)
            strcat(path, "/");
    };

	return path;
}


/*
 * nt_links_to_path - Construct NT path from its constituent links.
 *                    "links" is a pointer to an array of pointers to the
 *                    link names.  "nlinks" is the number of entries in
 *                    "links".  Call XtFree() to free the memory returned
 *                    by nt_links_to_path().
 */
char *
nt_links_to_path(links, nlinks)
char **links;
int nlinks;
{
	int len = 0;
	int i;
	char *path;

	/* Determine length of path name */
	for (i=0; i<nlinks; i++)
		len += strlen(links[i])+1;

	/* Build path name for new working directory */
	path = XtMalloc(len+1);
	strcpy(path, "");
	for (i=0; i<nlinks; i++) {
		strcat(path, links[i]);
		if (i < nlinks-1)
			strcat(path, "/");
	};

	/* Make sure that "x:" looks like "x:/" */
	if (nlinks == 1 && strlen(path) == 2 && path[1] == ':')
		strcat(path, "/");

	return path;
}


/*
 * dos_links_to_path - Construct a DOS path from its constituent links.
 *                     "links" is a pointer to an array of pointers to the
 *                     link names.  "nlinks" is the number of entries in
 *                     "links".  Call XtFree() to free the memory returned
 *                     by dos_links_to_path().
 */
char *
dos_links_to_path(links, nlinks)
char **links;
int nlinks;
{
	int len = 0;
	int i;
	char *path;

    /* Determine length of path name */
    for (i=0; i<nlinks; i++)
        len += strlen(links[i])+1;

    /* Build path name for new working directory */
    path = XtMalloc(len+1);
    strcpy(path, "");
    for (i=0; i<nlinks; i++) {
        strcat(path, links[i]);
        if (path[strlen(path)-1] != '\\' && i < nlinks-1)
            strcat(path, "\\");
    };

	return path;
}


/*
 * vms_links_to_path - Construct a VMS path from its constituent links.
 *                     "links" is a pointer to an array of pointers to the
 *                     link names.  "nlinks" is the number of entries in
 *                     "links".  Call XtFree() to free the memory returned
 *                     by vms_links_to_path().
 */
char *
vms_links_to_path(links, nlinks)
char **links;
int nlinks;
{
	int len; 
	int i;
	int state = 0;
	char *path;
	char *q;
	char *temp;

	/* Conservatively allocate enough memory to hold path */
	len = 2;
	for (i=0; i<nlinks; i++)
		len += strlen(links[i])+1;
	path = XtMalloc(len+1);

    /* Construct VMS path */
    path[0] = '\0';
	for (i=0; i<nlinks; i++) {
        len = strlen(links[i]);
        if (len == 0)
            fatal_error("Problem in vms_links_to_path()");
        switch (state) {
        case 0:
            if ((len > 2) && !strcmp(&links[i][len-2], "::")) {
                strcat(path, links[i]);
                state = 1;
            } else if ((len > 1) && (links[i][len-1] == ':')) {
                strcat(path, links[i]);
                state = 2;
            } else if (strstr(links[i], ".dir")) {
                strcat(path, "[.");
				temp = XtNewString(links[i]);
				q = strstr(temp, ".dir");
                *q = '\0';
                strcat(path, temp);
				XtFree(temp);
                state = 3;
            } else {
                strcat(path, links[i]);
                state = 4;
        }
            break;
        case 1:
            if ((len > 1) && (links[i][len-1] == ':')) {
                strcat(path, links[i]);
                state = 2;
            } else if (strstr(links[i], ".dir")) {
                strcat(path, "[.");
				temp = XtNewString(links[i]);
				q = strstr(temp, ".dir");
				*q = '\0';
                strcat(path, temp);
				XtFree(temp);
                state = 3;
            } else {
                strcat(path, links[i]);
                state = 4;
            }
            break;
        case 2:
            if (strstr(links[i], ".dir")) {
                strcat(path, "[");
				temp = XtNewString(links[i]);
				q = strstr(temp, ".dir");
				*q = '\0';
                strcat(path, temp);
				XtFree(temp);
                state = 3;
            } else {
                strcat(path, links[i]);
                state = 4;
            }
            break;
        case 3:
            if (strstr(links[i], ".dir")) {
                strcat(path, ".");
				temp = XtNewString(links[i]);
				q = strstr(temp, ".dir");
				*q = '\0';
                strcat(path, temp);
				XtFree(temp);
            } else {
                strcat(path, "]");
                strcat(path, links[i]);
                state = 4;
            }
            break;
        case 4:
            fatal_error("Problem in vms_links_to_path()");
        }
    }

    /* Is a closed bracket needed? */
    if (state == 3)
        strcat(path, "]");

    return path;
}


/*
 * links_to_path - Construct a  path from its constituent links.  "system"
 *                 identifies the system (e.g., SYS_UNIX).  "links" is a
 *                 pointer to an array of pointers to the link names.
 *                 "nlinkes" is the number of entries in "links".  Call 
 *                 XtFree() tofree the memory returned by
 *                 links_to_path().
 */
char *
links_to_path(system, links, nlinks)
int system;
char **links;
int nlinks;
{
	switch (system) {
	case SYS_VMS:
		return vms_links_to_path(links, nlinks);
	case SYS_DOS:
		return dos_links_to_path(links, nlinks);
	case SYS_NT:
		return nt_links_to_path(links, nlinks);
	default:
		return unix_links_to_path(links, nlinks);
	}
}


/*
 * merge_paths - Merge paths "full_path" and "rel_path".  "full_path"
 *               should be a full directory path and "rel_path" should be
 *               relative to "full_path".  The intent is to return a full
 *               path that is equivalent to referencing "rel_path" from
 *               directory "full_path".  "system" identifies the system
 *               (e.g., "SYS_UNIX").  Call XtFree() to free memory returned
 *               by merge_paths().
 */
char *
merge_paths(system, full_path, rel_path)
int system;
char *full_path;
char *rel_path;
{
	char **full_path_links;
	char **rel_path_links;
	char **links;
	char **ptr;
	char *path;
	int nlinks = 0;
	int indx;

	if (!(full_path_links = path_to_links(system, full_path)))
		fatal_error("Trouble in merge_paths()");
	if (!(rel_path_links = path_to_links(system, rel_path)))
		fatal_error("Trouble in merge_paths()");

	/* Allocate space for array of link names */
	ptr = full_path_links;
	while (*ptr++)
		nlinks++;
	ptr = rel_path_links;
	while (*ptr++)
		nlinks++;
	links = (char **)XtMalloc(sizeof(char *)*(nlinks+1));

	/* Merge links */
	indx = 0;
	ptr = full_path_links;
	while (*ptr) {
		links[indx++] = XtNewString(*ptr);
		ptr++;
	}
	ptr = rel_path_links;
	while (*ptr) {
		links[indx++] = XtNewString(*ptr);
		ptr++;
	}
	links[indx] = NULL;

	/* Create a merged path */
	path = links_to_path(system, links, nlinks);

	/* Free memory before returning */
	release_path_links(full_path_links);
	release_path_links(rel_path_links);
	release_path_links(links);

	return path;
}


/*
 * parse_path - Parses path name "path" into its constituent directory
 *              "dir" and entry "entry".  The caller is responsible
 *              for calling XtFree() to release memory.  "system" identifies
 *              the system (e.g., SYS_UNIX).  If "dir" or "entry" is NULL,
 *              no corresponding value is returned.
 */
parse_path(system, path, dir, entry)
int system;
char *path;
char **dir;
char **entry;
{
	char **links;
	char **dir_links;
	char **ptr;
	int nlinks = 0;
	int i;

	/* Break path into links and count them */
	if (!(links = path_to_links(system, path)))
		fatal_error("Trouble in parse_path()");
	ptr = links;
	while (*ptr++)
		nlinks++;
	
	/* Sanity check */
	if (nlinks < 2)
		fatal_error("Problem in parse_path()");

	/* Create directory path from links */
	dir_links = (char **)XtMalloc(sizeof(char *)*nlinks);
	for (i=0; i<nlinks-1; i++)
		dir_links[i] = XtNewString(links[i]);
	dir_links[nlinks-1] = NULL;
	if (dir)
		*dir = links_to_path(system, dir_links, nlinks-1);

	/* Pick off entry */
	if (entry)
		*entry = XtNewString(links[nlinks-1]);

	/* Free memory before returning */
	release_path_links(links);
	release_path_links(dir_links);
}


/*
 * unix_rel_dir_path_to_dos - Convert the Unix relative directory path to
 *                            the format used by the DOS system.  Call
 *                            XtFree() to release returned memory.
 */
char *
unix_rel_dir_path_to_dos(path)
char *path;
{
	char *dos_path;
	char *p;

	/* Same as Unix relative path, with the /'s changed to \'s */
	dos_path = XtNewString(path);
	p = dos_path;
	while (*p) {
		if (*p == '/')
			*p = '\\';
		p++;
	}

	return dos_path;
}


/*
 * unix_rel_dir_path_to_vms - Convert the Unix relative directory path to
 *                            the format used by the VMS system.  Call
 *                            XtFree() to release returned memory.
 */
char *
unix_rel_dir_path_to_vms(path)
char *path;
{
	char **unix_links;
	char **vms_links;
	char *vms_path;
	char **ptr;
	int nlinks = 0;
	int i;

	/* Parse Unix relative path into links */
	if (!(unix_links = unix_path_to_links(path)))
		fatal_error("Trouble in unix_rel_dir_path_to_vms()");

	/* Allocate space for array of VMS links names */
	ptr = unix_links;
	while (*ptr++)
		nlinks++;
	vms_links = (char **)XtMalloc(sizeof(char *)*(nlinks+1));

	/* Create VMS links */
	for (i=0; i<nlinks; i++) {
		vms_links[i] = XtMalloc(strlen(unix_links[i])+5);
		strcpy(vms_links[i], unix_links[i]);
		strcat(vms_links[i], ".dir");
	}
	vms_links[nlinks] = NULL;

	/* Create VMS path from links */
	vms_path = vms_links_to_path(vms_links, nlinks);

	/* Release memory before returning */
	release_path_links(unix_links);
	release_path_links(vms_links);

	return vms_path;
}


/*
 * unix_rel_dir_path_to_native - Convert the Unix relative directory path
 *                               to the native "system" format.  "system"
 *                               identifies the system (e.g., SYS_UNIX).
 *                               Call XtFree() to release returned memory.
 */
char *
unix_rel_dir_path_to_native(system, path)
int system;
char *path;
{
	switch (system) {
	case SYS_VMS:
		return unix_rel_dir_path_to_vms(path);
	case SYS_DOS:
		return unix_rel_dir_path_to_dos(path);
	default:
		return XtNewString(path);
	}
}


/*
 * unix_rel_file_path_to_vms - Convert the Unix relative file path to
 *                             the format used by the VMS system.  Call
 *                             XtFree() to release returned memory.
 */
char *
unix_rel_file_path_to_vms(path)
char *path;
{
	char **unix_links;
	char **vms_links;
	char *vms_path;
	char **ptr;
	int nlinks = 0;
	int i;

	/* Parse Unix relative path into links */
	if (!(unix_links = unix_path_to_links(path)))
		fatal_error("Trouble in unix_rel_file_path_to_vms()");

	/* Allocate space for array of VMS links names */
	ptr = unix_links;
	while (*ptr++)
		nlinks++;
	vms_links = (char **)XtMalloc(sizeof(char *)*(nlinks+1));

	/* Create VMS links */
	for (i=0; i<nlinks-1; i++) {
		vms_links[i] = XtMalloc(strlen(unix_links[i])+5);
		strcpy(vms_links[i], unix_links[i]);
		strcat(vms_links[i], ".dir");
	}
	vms_links[nlinks-1] = XtNewString(unix_links[nlinks-1]);
	vms_links[nlinks] = NULL;

	/* Create VMS path from links */
	vms_path = vms_links_to_path(vms_links, nlinks);

	/* Release memory before returning */
	release_path_links(unix_links);
	release_path_links(vms_links);

	return vms_path;
}


/*
 * unix_rel_file_path_to_native - Convert the Unix relative file path
 *                                to the native "system" format.  "system"
 *                                identifies the system (e.g., SYS_UNIX).
 *                                Call XtFree() to release returned memory.
 */
char *
unix_rel_file_path_to_native(system, path)
int system;
char *path;
{
	switch (system) {
	case SYS_VMS:
		return unix_rel_file_path_to_vms(path);
	case SYS_DOS:
		return unix_rel_dir_path_to_dos(path);
	default:
		return XtNewString(path);
	}
}


/*
 * equivalent_file_path - The purpose of this kludgy function is to convert
 *                        a directory path into a file path.  This is a no-op
 *                        if "system" is not SYS_VMS.  If "system" is SYS_VMS,
 *                        the following sort of conversion take place:
 *
 *                            a::b:[c.d.e]   -->  a::b:[c.d]e.dir
 *                            a::b:[c.d]e    -->  a::b:[c.d]e     (no-op)
 *                            a::b:[c]       -->  a::b:c.dir
 *
 *                        Call XtFree() to release returned memory.
 */
char *
equivalent_file_path(system, path)
int system;
char *path;
{
	char **links;
	char **ptr;
	char *dir_path;
	char *new_path;
	int nlinks = 0;
	int len = 0;

	if (system == SYS_VMS) {
		links = vms_path_to_links(path);
		ptr = links;
		while (*ptr++)
			nlinks++;
		len = strlen(links[nlinks-1]);
		if ((len > 4) && !strcmp(&links[nlinks-1][len-4], ".dir")) {
			dir_path = vms_links_to_path(links, nlinks-1);
			new_path = XtMalloc(strlen(dir_path)+len+1);
			strcpy(new_path, dir_path);
			strcat(new_path, links[nlinks-1]);
			XtFree(dir_path);
		} else {
			new_path = XtNewString(path);
		}
		release_path_links(links);
	} else
		new_path = XtNewString(path);

	return new_path;
}

