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
#include <sys/stat.h>
#ifdef apollo
#include <sys/dir.h>
#else
#ifdef NEXT
#include <bsd/sys/dir.h>
#else
#include <dirent.h>
#endif
#endif
#include <errno.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

#ifdef apollo
#define dirent direct
#endif

#ifndef S_ISLNK
#define S_ISLNK(mode)   (((mode)&0170000) == 0120000) /* Kludge for nonPOSIX */
#endif

#ifndef S_ISSOCK
#define S_ISSOCK(mode)  (((mode)&0170000) == 0140000) /* Kludge for nonPOSIX */
#endif

#define MAXDIRLINE  200

#define MAXTCPBUF 1024

extern int max_ftp_retries;
extern int diagnostics;
extern struct st_host_info hinfo[];
extern int sort_long_lists_by_date;
extern int maxt;

struct sl_struct *create_array_list();
char *ifgets();
int cmp_symbol_entries();

 
/* 
 * local_ls - Sets "dlist" to a pointer to a structure that contains a
 *            sorted list of directory entries in "directory" on the
 *            local host.  Set sym_mode to True if type symbol is to be
 *            appended to directory entries.  Returns 0 if successful,
 *            else returns -1.  If unable to read the directory because
 *            of lack of permission, returns 0 and a single entry named
 *            ".unreadable".
 */ 
local_ls(directory, dlist, sym_mode)
char *directory;
struct sl_struct **dlist;
int sym_mode;
{ 
	struct entry_link *head = NULL;
	DIR *dirp;
	struct dirent *dp;
	char entry[MAXPATHLEN];
	struct stat status;
	int retval;

	if ((dirp = opendir(directory)) == NULL) {
		if (errno == EACCES) {
			add_to_linked_list(&head, ".unreadable");
			goto done;
		} else
			return -1;
	}

	if (sym_mode)
		retval = chdir(directory);

	for (dp=readdir(dirp); dp!=NULL; dp=readdir(dirp))
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
			if (sym_mode && !retval && !lstat(dp->d_name, &status)) {
				strcpy(entry, dp->d_name);
				if (S_ISREG(status.st_mode)) {
					if ((S_IXUSR | S_IXGRP | S_IXOTH) & status.st_mode)
						strcat(entry, "*");
				} else if (S_ISDIR(status.st_mode))
					strcat(entry, "/");
				else if (S_ISLNK(status.st_mode))
					strcat(entry, "@");
				else if (S_ISSOCK(status.st_mode))
					strcat(entry, "=");
				else if ((S_IXUSR | S_IXGRP | S_IXOTH) & status.st_mode)
					strcat(entry, "*");
				add_to_linked_list(&head, entry);
			} else
	       		add_to_linked_list(&head, dp->d_name);
		}
	
	closedir(dirp);

done:
 
    /* Convert linked list into array */
    *dlist = create_array_list(&head);
 
    /* Sort entries */
	if (sym_mode)
		quicksort((*dlist)->entries, (*dlist)->nentries, cmp_symbol_entries);
	else
    	quicksort((*dlist)->entries, (*dlist)->nentries, strcmp);

    return 0;
}


/* 
 * local_dir - Sets "dlist" to a pointer to a structure that contains a
 *             sorted list of directory entries in "directory" on the
 *             local host.  Returns 0 if successful, else returns -1.
 */ 
local_dir(directory, dlist)    
char *directory;
struct sl_struct **dlist;
{
    struct entry_link *head = NULL;
    char dirline[MAXDIRLINE+1];
    FILE *fp;
    int len;
    int dummy;
	char cmd[MAXPATHLEN+15];

	if (sort_long_lists_by_date)
		sprintf(cmd, "/bin/ls -galt %s", directory);
	else
		sprintf(cmd, "/bin/ls -gal %s", directory);
	save_sigcld_handler();
    if ((fp = popen(cmd, "r")) == NULL) {
		restore_sigcld_handler();
		return -1;
	}

    while (ifgets(dirline, MAXDIRLINE, fp) != NULL) {
        len = strlen(dirline);
        if (dirline[len-1] == '\n')
            dirline[--len] = '\0';
		if (sscanf(dirline, "total %d", &dummy) == 1)
			continue;
		else if (len >= 2 && strcmp(&(dirline[len-2]), " .") == 0)
			continue;
		else if (len >= 3 && strcmp(&(dirline[len-3]), " ..") == 0)
			continue;
		else
			add_to_linked_list(&head, dirline);
    }
	restore_sigcld_handler();

    if (ferror(fp)) {
		release_linked_list(&head);
		return -1;
	}

    pclose(fp);

    /* Convert linked list into array */
    *dlist = create_array_list(&head);

	return 0;
}


/*
 * remote_dirlist - Set "dlist" to a pointer to a structure that
 *                  contains a list of entries in "directory" on the
 *                  remote host.  Set "cmd" to either "NLST" or
 *                  "LIST", optionally followed by an option
 *                  (e.g., "NLST -lagt").  "host" is the host id.
 *                  Returns 0 if successful, -3 for broken
 *                  connection, and -1 for other errors.
 */
remote_dirlist(host, directory, cmd, dlist)
int host;
char *directory;
char *cmd;
struct sl_struct **dlist;
{
    char ftpline[MAXFTPLINE+1];
    struct entry_link *head;
    int data_fd;
    char ch;
    int nbytes;
    int code;
    char reply[MAXFTPREPLY];
    int sock;
    int linelen;
    char tcpbuf[MAXTCPBUF];
    int len_tcpbuf;
    int index_tcpbuf;
    int retval;
	int retries = 0;
	int scode;
	char temp[20];
	char *request;

    /* Verify that request is valid */
	strcpy(temp, cmd);
	if ((request = strtok(temp, " ")) == NULL)
		return -1;
    if (strcmp(request, "NLST") && strcmp(request, "LIST"))
        return -1;

retry:

	/* Initialize linked list of directory entries */
	head = NULL;

    /* "cd" to directory */
    if ((retval = remote_cd(host, directory, False, True)) < 0)
        return retval;

    /* Initialize data connection */
    if ((sock = init_data_conn(host)) < 0)
        return sock;

    /* Send command to FTP server */
    if ((retval = send_ftp_cmd(host, cmd)) < 0) {
        close(sock);
        return retval;
    }

    /* Get initial response from FTP server */
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
    switch (scode) {
    case 1:
        break;
    case 4:
		close(sock);
        if (code >= 450 && code < 460 && retries++ < max_ftp_retries) {
            if (diagnostics >= VERBOSE)
                write_log("*** Directory list failed.  Will retry.\n");
            goto retry;
        }
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
        return -1;
	case 5:
		close(sock);
		if (code == 550)     /* Kludge to handle empty UNICOS directories */
			goto convert;
		else if (code == 501)
			goto convert;    /* Kludge to handle empty MacOS directories */
		if (diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		return -1;
    case -3:
        close(sock);
        return -3;
	case -2:
		report_client_timeout(host);
    default:
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
        close(sock);
        return -1;
    }

    /* Establish data connection */
    data_fd = accept_data_conn(host, sock);
    close(sock);
    if (data_fd < 0)
        return -1;

    /* Get directory list from FTP server */
    len_tcpbuf = 0;
    index_tcpbuf = 0;
    linelen = 0;
    while (1) {
        if (index_tcpbuf >= len_tcpbuf) {
			nbytes = read_tcp(host, data_fd, tcpbuf, MAXTCPBUF, maxt, False);
			switch (nbytes) {
			case -2:
                release_linked_list(&head);
                goto abort;
			case -1:
			case -3:
                release_linked_list(&head);
                close(data_fd);
                return nbytes;
			case 0:
				goto got_list;
			}
            index_tcpbuf = 0;
            len_tcpbuf = nbytes;
        }
        ch = tcpbuf[index_tcpbuf++];
        if (ch == '\n') {
            ftpline[linelen] = '\0';
            if (diagnostics == DEBUG) {
                write_log(ftpline);
                write_log("\n");
            }
            add_to_linked_list(&head, ftpline);
            linelen = 0;
        } else if (ch != '\r') {
            if (linelen >= MAXFTPLINE) {
                release_linked_list(&head);
                close(data_fd);
                return -1;
            }
            ftpline[linelen++] = ch;
        }
    }

got_list:

    /* Get final response from FTP server */
	if (hinfo[host].server == SERVER_MAC_FTPD)
		close(data_fd);
	scode = get_ftp_reply(host, reply, MAXFTPREPLY, &code, maxt, False);
    switch (scode) {
    case 2:
        close(data_fd);
        break;
    case -3:
		release_linked_list(&head);
        close(data_fd);
        return -3;
	case -2:
		report_client_timeout(host);
    default:
		if (scode == 4 && code/10 == 45 && retries++ < max_ftp_retries) {
            if (diagnostics >= VERBOSE)
                write_log("*** Directory list failed.  Will retry.\n");
			release_linked_list(&head);
			close(data_fd);
            goto retry;
        }
		if (code == 550) {    /* Kludge to handle empty VMS directories */
			close(data_fd);
			break;
		}
		if (scode > 0 && diagnostics < VERBOSE)
			print_ftp_reply(host, reply);
		release_linked_list(&head);
        close(data_fd);
        return -1;
    }

convert:

    /* Convert linked list into array */
    *dlist = create_array_list(&head);

    return 0;

abort:

    /* Send urgent abort to server */
    urgent_abort(host);

    /* Clear out all incoming data */
    while (read_tcp(host, data_fd, tcpbuf, MAXTCPBUF, 10, False) > 0);
    close(data_fd);

    /* Get responses from server */
    retval = get_ftp_reply(host, reply, MAXFTPREPLY, &code, 15, False);
    if (retval > 0)
        retval = get_ftp_reply(host, reply, MAXFTPREPLY, &code, 5, False);

    if (retval == -3)
        return -3;
    else if (retval == -2)
		report_client_timeout(host);

    return -1;
}


/*
 * remote_ls - Sets "dlist" to a pointer to a structure that contains
 *             a sorted list of directory entries in "directory" on the
 *             remote host.  "host" is the host id.  Set sym_mode to
 *             True if type symbol is to be appended to directory
 *             entries.  Returns 0 if successful, -3 for broken
 *             connection, and -1 for other errors.
 */
remote_ls(host, directory, dlist, sym_mode)
int host;
char *directory;
struct sl_struct **dlist;
int sym_mode;
{
    int retval;
	int i;
	int j;
	int len;
	char *dirline;

    /* Get short list */
	if (sym_mode && ((hinfo[host].system == SYS_UNIX)
			|| (hinfo[host].server == SERVER_MAC_FTPD)
			|| (hinfo[host].system == SYS_NT)))
		retval = remote_dirlist(host, directory, "NLST -aF", dlist);
	else if (hinfo[host].system == SYS_UNIX)
		retval = remote_dirlist(host, directory, "NLST -a", dlist);
	else
		retval = remote_dirlist(host, directory, "NLST", dlist);

    if (retval < 0)
        return retval;

	/* Kludge for certain VMS FTP servers */
	if (hinfo[host].server == SERVER_VMS_3_0)
		for (i=0; i<(*dlist)->nentries; i++) {
			len = strlen((*dlist)->entries[i]);
			for (j=0; j<len; j++)
				(*dlist)->entries[i][j] = tolower((*dlist)->entries[i][j]);
		}

	/* Remove trailing slashes from certain FTP servers */
	if (!sym_mode && (hinfo[host].server == SERVER_MAC_VERSATERM 
			|| hinfo[host].server == SERVER_MAC_NCSATELNET
			|| hinfo[host].server == SERVER_DOS_WINQVT))
		for (i=0; i<(*dlist)->nentries; i++) {
			len = strlen((*dlist)->entries[i]);
			if (len > 0 && (*dlist)->entries[i][len-1] == '/')
				(*dlist)->entries[i][len-1] = '\0';
		}

	/* Delete unwanted entries */
	if (hinfo[host].system != SYS_VMS)
		for (i=(*dlist)->nentries-1; i>=0; i--) {
			dirline = (*dlist)->entries[i];
			if (sym_mode) {
				if (!strcmp(dirline, "./") || !strcmp(dirline, "../"))
					delete_from_array_list(dlist, i);
			} else {
				if (!strcmp(dirline, ".") || !strcmp(dirline, ".."))
					delete_from_array_list(dlist, i);
			}
		}
   
    /* Sort entries */
	if (sym_mode && ((hinfo[host].system == SYS_UNIX)
			|| (hinfo[host].system == SYS_NT)
			|| (hinfo[host].server == SERVER_MAC_FTPD)
			|| (hinfo[host].server == SERVER_MAC_VERSATERM)
			|| (hinfo[host].server == SERVER_MAC_NCSATELNET)
			|| (hinfo[host].server == SERVER_DOS_WINQVT)))
		quicksort((*dlist)->entries, (*dlist)->nentries, cmp_symbol_entries);
	else
    	quicksort((*dlist)->entries, (*dlist)->nentries, strcmp);

    return 0;
}


/*
 * remote_dir - Sets "dlist" to a pointer to a structure that contains 
 *              a long list of directory entries in "directory" on the
 *              remote host.  "host" is the host id.  Returns 0 if
 *              successful, -3 for broken connection, and -1 for other
 *				errors.
 */
remote_dir(host, directory, dlist)
int host;
char *directory;
struct sl_struct **dlist;
{
	int i;
	int retval;
	int dummy;
	int len;
	char *dirline;
	char *cmd;

    /* Get extended list */
	if (sort_long_lists_by_date) {
		if (hinfo[host].server == SERVER_UNIX_UNITREE)
			cmd = XtNewString("NLST -alt");
		else if (hinfo[host].system == SYS_UNIX)
			cmd = XtNewString("NLST -galt");
		else if (hinfo[host].system == SYS_NT)
			cmd = XtNewString("NLST -galtr");
		else
			cmd = XtNewString("LIST");
	} else {
		if (hinfo[host].server == SERVER_UNIX_UNITREE)
			cmd = XtNewString("NLST -al");
		else if (hinfo[host].server == SERVER_UNIX_NSL_UNITREE)
			cmd = XtNewString("LIST");
		else if (hinfo[host].system == SYS_UNIX)
			cmd = XtNewString("NLST -gal");
		else
			cmd = XtNewString("LIST");
	}
	retval = remote_dirlist(host, directory, cmd, dlist);
	XtFree(cmd);

	/* Delete unwanted lines */
	if ((retval == 0) && (hinfo[host].system == SYS_UNIX)) 
		for (i=(*dlist)->nentries-1; i>=0; i--) {
			dirline = (*dlist)->entries[i];
			len = strlen(dirline);
			if ((sscanf(dirline, "total %d", &dummy) == 1)
					|| (len >= 2 && !strcmp(&dirline[len-2], " ."))
					|| (len >= 3 && !strcmp(&dirline[len-3], " ..")))
				delete_from_array_list(dlist, i);
		}

	return retval;
}

