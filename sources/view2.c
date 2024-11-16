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
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

#define MAXBUF       1024
#define MAXARGV      50
#define VINTERVAL    15*1000
#define PLACEHOLDER  "<>"

struct view_dialog {
	struct view_dialog *next;
	Widget w_dialog;
	Widget w_separator;
	Widget w_saveButton;
	Widget w_closeButton;
	Widget w_helpButton;
	Widget w_text;
};

struct decompressor_st {
	char *extension;
	char *command_line;
};

struct viewdir_link {
	struct viewdir_link *next;
	struct viewdir_link *prev;
	int pid;
	int ready_for_removal;
	char *dir;
};

static struct decompressor_st decompressor_mappings[] = {
	{ ".Z",		"uncompress %i"			},
	{ ".btoa",	"atob < %i > %o"		},
	{ ".gz",	"gunzip %i"				},
	{ ".z",		"gunzip %i"     		},
	{ ".uu",	"cd %d ; uudecode %i"	}
};

static ndecompressor_mappings = XtNumber(decompressor_mappings);

static struct view_dialog *free_view_head = NULL;

static struct viewdir_link *viewdir_head = NULL;
static struct viewdir_link *viewdir_tail = NULL;

static int fake_pid = 0;
static int garbage_collector_running = False;
static char *viewsave_wd = NULL;

static char *view_help[] = {
	"You can use the \"Viewer Preferences\" dialog (accessible",
	"via the \"Options\" menu) to control whether a file will",
	"be displayed in this built-in viewer or in an external",
	"viewer of the user's choice (e.g., emacs or xv).\n",
	"\n",
	"Click on the SAVE button to pop up a dialog for selecting",
	"a directory to save the viewed file in.  If the original file was",
	"decompressed prior to viewing, it is this decompressed file that",
	"will be saved.",
	NULL
};

extern Widget w_toplev;
extern Display *display;
extern struct sl_struct *viewer_mappings;
extern XtAppContext app;

void cb_internal_viewer_save(Widget, XtPointer, XtPointer);
void cb_internal_viewer_close(Widget, XtPointer, XtPointer);
void cb_internal_viewer_help(Widget, XtPointer, XtPointer);
Widget vertical_sb();
struct view_dialog *create_internal_viewer_dialog();
char *decompress_file();
char *merge_paths();
void cb_view_dir_remover(XtPointer, XtIntervalId*);
char *get_destination_dir();


/*
 * create_internal_viewer_dialog - Returns a pointer to structure for a 
 *                                 dialog for viewing a text file.   If an
 *                                 unused dialog is available, it is returned;
 *                                 otherwise, a new dialog is created.
 */
struct view_dialog *
create_internal_viewer_dialog()
{
	int i;
	Arg args[6];
	struct view_dialog *ptr;
	struct view_dialog *view;

	/* If an unused dialog is available, return it */
	if (free_view_head) {
		ptr = free_view_head;
		free_view_head = free_view_head->next;
		return ptr;
	}

	/* Need to create a new dialog */
	view = XtNew(struct view_dialog);

    /* Create form dialog */
    i = 0;
	XtSetArg(args[i], XmNautoUnmanage, False); i++;
    view->w_dialog = XmCreateFormDialog(w_toplev, "view", args, i);

	/* Add callback for the WM_DELETE_WINDOW protocol */
	add_wm_delete_window_cb(view->w_dialog, cb_internal_viewer_close,
		(XtPointer)view);

    /* Create "Save" pushbutton */
    view->w_saveButton = XtVaCreateManagedWidget(
        "saveButton",
        xmPushButtonWidgetClass,
        view->w_dialog,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNbottomOffset,        10,
        XmNleftAttachment,      XmATTACH_POSITION,
        XmNleftPosition,        20,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       32,
        NULL
    );
    XtAddCallback(view->w_saveButton, XmNactivateCallback,
		cb_internal_viewer_save, (XtPointer)view);

    /* Create "Close" pushbutton */
    view->w_closeButton = XtVaCreateManagedWidget(
        "closeButton",
        xmPushButtonWidgetClass,
        view->w_dialog,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNbottomOffset,        10,
        XmNleftAttachment,      XmATTACH_POSITION,
        XmNleftPosition,        44,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       56,
        NULL
    );
    XtAddCallback(view->w_closeButton, XmNactivateCallback,
		cb_internal_viewer_close, (XtPointer)view);

    /* Create "Help" pushbutton */
    view->w_helpButton = XtVaCreateManagedWidget(
        "helpButton",
        xmPushButtonWidgetClass,
        view->w_dialog,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNbottomOffset,        10,
        XmNleftAttachment,      XmATTACH_POSITION,
        XmNleftPosition,        68,
        XmNrightAttachment,     XmATTACH_POSITION,
        XmNrightPosition,       80,
        NULL
    );
    XtAddCallback(view->w_helpButton, XmNactivateCallback,
        cb_internal_viewer_help, (XtPointer)view);

	/* Create separator */
    view->w_separator = XtVaCreateManagedWidget(
        "separator",
        xmSeparatorWidgetClass,
        view->w_dialog,
        XmNbottomAttachment,    XmATTACH_WIDGET,
        XmNbottomWidget,        view->w_closeButton,
		XmNbottomOffset,		8,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL
    );

	/* Create scrolled text window to hold list */
	i = 0;
	XtSetArg(args[i], XmNeditable, False); i++;
	XtSetArg(args[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(args[i], XmNcursorPositionVisible, False); i++;
	view->w_text = XmCreateScrolledText(view->w_dialog,
		"text", args, i);
	XtVaSetValues(XtParent(view->w_text),
		XmNtopAttachment,		XmATTACH_FORM,
		XmNtopOffset,			8,
		XmNbottomAttachment,	XmATTACH_WIDGET,
		XmNbottomWidget,		view->w_separator,
		XmNbottomOffset,		8,
		XmNleftAttachment,		XmATTACH_FORM,
		XmNleftOffset,			8,
		XmNrightAttachment,		XmATTACH_FORM,
		XmNrightOffset,			8,
		NULL
	);
	XtManageChild(view->w_text);
    fix_vertical_text_sb_color(view->w_text);
    fix_horizontal_text_sb_color(view->w_text);
	set_bg_to_sb_trough_color(view->w_text,
		vertical_sb(XtParent(view->w_text)));

	return view;
}


/*
 * close_internal_viewer - Close the internal viewer and delete the file
 *                         being viewed (and its directory);
 */
void
close_internal_viewer(view)
struct view_dialog *view;
{
	long pid;
	char *ptr;

	/* Pop dialog down */
	XtUnmanageChild(view->w_dialog);

	/* Free file name */
	XtVaGetValues(view->w_saveButton, XmNuserData, &ptr, NULL);
	XtFree(ptr);

	/* Delete view directory */
	XtVaGetValues(view->w_dialog, XmNuserData, &pid, NULL);
	delete_viewer_dir(pid);

	/* Put dialog on free list for future reuse */
	view->next = free_view_head;
	free_view_head = view;
}


/*
 * cb_internal_viewer_save - Callback for internal viewer dialog's "Save"
 *                           button.
 */
void
cb_internal_viewer_save(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	char *src_file;
	char *snk_dir;
	char *snk_file;
	char *filename;
	int src_file_fd;
	int snk_file_fd;
	char *home_dir;
	char msg[MAXPATHLEN+30];
	struct stat status;
	long file_len;
	long file_index = 0;
	char *buf;
	int nbytes;
	long pid;
	struct view_dialog *view = (struct view_dialog *)client_data;
	struct viewdir_link *ptr;

	/* Remember pid for validity check after "Destination Dir" dialog */
	XtVaGetValues(view->w_dialog, XmNuserData, &pid, NULL);

	/* Get destination directory */
	home_dir = XtNewString(getenv("HOME"));
	snk_dir = get_destination_dir(LOCAL_HOST, LOCAL, viewsave_wd, home_dir,
		view->w_dialog);
	XtFree(home_dir);
	if (snk_dir) {
		if (viewsave_wd)
			XtFree(viewsave_wd);
		viewsave_wd = snk_dir;
	} else
		return;

	/* Verify that internal viewer not closed by window manager */
	ptr = viewdir_head;
	while (ptr) {
		if (ptr->pid == pid)
			break;
		ptr = ptr->next;
	}
	if (ptr == NULL) {
		warning_error("Viewer not present.  Unable to save file.");
		return;
	}

	/* Determine source and sink file names */
	XtVaGetValues(view->w_saveButton, XmNuserData, &src_file, NULL);
	parse_path(SYS_UNIX, src_file, NULL, &filename);
	snk_file = merge_paths(SYS_UNIX, viewsave_wd, filename);

	/* Open source file */
	if ((src_file_fd = iopen2(src_file, O_RDONLY)) < 0) {
		report_perror(LOCAL_HOST, src_file);
		goto error;
	}

	/* Get length of file */
	if (fstat(src_file_fd, &status) < 0) {
		report_perror(LOCAL_HOST, src_file);
		close(src_file_fd);
		goto error;
	}
	file_len = status.st_size;

    /* Create sink file */
    if (unlink(snk_file) == -1)
        if (errno != ENOENT) {
            report_perror(LOCAL_HOST, snk_file);
			close(src_file_fd);
			goto error;
        }
    if ((snk_file_fd = iopen3(snk_file, O_CREAT|O_WRONLY, 0644)) < 0) {
		report_perror(LOCAL_HOST, snk_file);
		close(src_file_fd);
		goto error;
    }

	/* Copy file */
	buf = XtMalloc(MAXBUF);
	while (file_index < file_len) {
		nbytes = MIN(file_len-file_index, MAXBUF);
		if (iread(src_file_fd, buf, nbytes) != nbytes) {
			report_perror(LOCAL_HOST, src_file);
			close(src_file_fd);
			close(snk_file_fd);
			XtFree(buf);
			goto error;
		}
		if (iwrite(snk_file_fd, buf, nbytes) != nbytes) {
			report_perror(LOCAL_HOST, snk_file);
			close(src_file_fd);
			close(snk_file_fd);
			XtFree(buf);
			goto error;
		}
		file_index += nbytes;
	}
	close(src_file_fd);
	close(snk_file_fd);
	XtFree(buf);

	/* Success */
	write_log("*** Successfully saved file: ");
	write_log(filename);
	write_log("\n");
	goto done;

error:

	sprintf(msg, "Unable to save %s", filename);
	warning_error(msg);

done:

	XtFree(snk_file);
	XtFree(filename);
}


/*
 * cb_internal_viewer_close - Callback for internal viewer dialog's "Close"
 *                            button.
 */
void
cb_internal_viewer_close(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	struct view_dialog *view = (struct view_dialog *)client_data;

	close_internal_viewer(view);
}


/*
 * cb_internal_viewer_help - Callback to display "View" dialog's help package.
 */
void
cb_internal_viewer_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	struct view_dialog *view = (struct view_dialog *)client_data;

    help_dialog(view->w_dialog, False, "Built-in Viewer", view_help);
}


/*
 * delete_viewer_dir - Delete the temporary view directory associated with
 *                     "pid".  (The internal viewers use fake pid's which
 *                     have a negative value.)  If there is no directory
 *                     associated with "pid", this routine is a no-op.
 *                     No error is reported if the directory does not
 *                     exist, or if unable to delete the directory or its
 *                     contents.
 */
delete_viewer_dir(pid)
int pid;
{
	char *dir = NULL;
	struct viewdir_link *ptr;
	struct sl_struct *dlist;
	char *temp;
	int i;

	/* Is there a directory associated with the pid? */
	ptr = viewdir_head;
	while (ptr) {
		if (ptr->pid == pid) {
			dir = ptr->dir;
			if (ptr == viewdir_head)
				viewdir_head = ptr->next;
			else
				ptr->prev->next = ptr->next;
			if (ptr == viewdir_tail)
				viewdir_tail = ptr->prev;
			else
				ptr->next->prev = ptr->prev;
			XtFree((char *)ptr);
			break;
		}
		ptr = ptr->next;
	}
	if (dir == NULL)
		return;

	/* Delete all entries in the directory */
	if (local_ls(dir, &dlist, False) == 0) {
		for (i=0; i<dlist->nentries; i++) {
			temp = merge_paths(SYS_UNIX, dir, dlist->entries[i]);
			unlink(temp);
			XtFree(temp);
		}
		release_array_list(dlist);
	}

	/* Now remove its directory */
	rmdir(dir);
	XtFree(dir);
}


/*
 * delete_all_viewer_dirs - Deletes all temporary view directories.  This
 *                          function should be called before terminating.
 */
delete_all_viewer_dirs()
{
	struct viewdir_link *ptr;
	struct sl_struct *dlist;
	int i;
	char *temp;

	/* Delete all directories from list of active view directories */
	while (viewdir_head) {
		ptr = viewdir_head;
		viewdir_head = viewdir_head->next;
		if (local_ls(ptr->dir, &dlist, False) == 0) {
			for (i=0; i<dlist->nentries; i++) {
				temp = merge_paths(SYS_UNIX, ptr->dir, dlist->entries[i]);
				unlink(temp);
				XtFree(temp);
			}
			release_array_list(dlist);
		}
		rmdir(ptr->dir);
		XtFree(ptr->dir);
		XtFree((char *)ptr);
	}
}


/*
 * use_internal_viewer - Pop up an internal viewer to display "file".  The
 *                       view deletes "file" and its directory when it is
 *                       finished displaying it.   "host" is used only for 
 *                       error reporting purposes.  Returns a phoney pid 
 *                       used to associate the viewer with the temporary 
 *                       view directory.
 */
use_internal_viewer(host, file)
int host;
char *file;
{
	int fd;
	char *name;
	struct view_dialog *view;
	long file_len;
	long file_index = 0;
	char *buf;
	int nbytes;
	struct stat status;
	long pid = --fake_pid;
	int i;

    /* Open file */
    if ((fd = iopen2(file, O_RDONLY)) < 0) {
        report_perror(host, file);
		warning_error("Trouble accessing temporary view file");
        return pid;
    }

	/* Get length of file */
	if (fstat(fd, &status) < 0) {
		warning_error("Trouble accessing temporary view file");
		return pid;
	}
	file_len = status.st_size;

	/* Get a view dialog to work with */
	view = create_internal_viewer_dialog();

	/* Associate fake pid with internal viewer */
	XtVaSetValues(view->w_dialog, XmNuserData, pid, NULL);

	/* Remember file name for possible save operation */
	XtVaSetValues(view->w_saveButton, XmNuserData, (XtPointer)XtNewString(file),
		NULL);

	/* Put the file name in the dialog title */
	parse_path(SYS_UNIX, file, NULL, &name);
	XtVaSetValues(XtParent(view->w_dialog), XmNtitle, name, NULL);
	XtFree(name);

	/* Display file */
	XmTextSetString(view->w_text, "");
	buf = XtMalloc(MAXBUF+1);
	while (file_index < file_len) {
		nbytes = MIN(file_len-file_index, MAXBUF);
		if (iread(fd, buf, nbytes) != nbytes) {
			report_perror(host, file);
			warning_error("Trouble accessing temporary view file");
			close_internal_viewer(view);
			XtFree(buf);
			return pid;
		}
#if !defined(_UNICOS) && !defined(__QNX__)   /* Compilers don't like toasci() */
		for (i=0; i<nbytes; i++)
			buf[i] = toascii(buf[i]);
#endif
		buf[nbytes] = '\0';
		XmTextInsert(view->w_text, XmTextGetLastPosition(view->w_text), buf);
		file_index += nbytes;
	}
	XFlush(display);
	XtFree(buf);

	/* Close file */
	close(fd);

	/* Show beginning of file */
	XmTextShowPosition(view->w_text, (XmTextPosition)0);

	/* Pop up viewer */
    XtManageChild(view->w_dialog);
    add_dialog_to_list(view->w_dialog);

	return pid;
}


/*
 * view_file - Pop up a viewer to display "file".  The view deletes "file"
 *             (and its directory when it is finished displaying it.  If
 *             unable to display the file, the file (and its directory)
 *             are deleted.  "host" is used only for error reporting
 *             purposes.
 */
view_file(host, file)
int host;
char *file;
{
	struct viewdir_link *ptr;
	char *viewable_file;
	char *dir;
	int i;
	char *ext;
	char *q;
	int indx;
	char *ext_command;
	char *extension;
	char *command_line;
	char *p;

	/* See if file needs to be decompressed */
	viewable_file = decompress_file(file);
	if (viewable_file == NULL) {
		warning_error("Unable to expand file for viewing.");
		viewable_file = XtNewString(file);
	}

	/* Add directory which contains file to list of active view directories */
	parse_path(SYS_UNIX, viewable_file, &dir, NULL);
	ptr = XtNew(struct viewdir_link);
	ptr->dir = dir;
	ptr->ready_for_removal = False;
	ptr->prev = NULL;
	ptr->next = viewdir_head;
	if (viewdir_head)
		viewdir_head->prev = ptr;
	viewdir_head = ptr;
	if (viewdir_tail == NULL)
		viewdir_tail = ptr;

	/* Determine which viewer to use */
	indx = -1;
	if ((q = strrchr(viewable_file, '.')))
		ext = XtNewString(q);
	else
		ext = XtNewString("");
	for (i=0; i<viewer_mappings->nentries; i++) {
		ext_command = XtNewString(viewer_mappings->entries[i]);
		extension = strtok(ext_command, " ");
		if (!strcmp(extension, "OTHERS"))
			indx = i;
		else if (!strcmp(extension, ext)) {
			indx = i;
			XtFree(ext_command);
			break;
		}
		XtFree(ext_command);
	}
	if (indx == -1)
		fatal_error("Trouble in view_file.");
	XtFree(ext);

	/* View file */
	if ((p = strchr(viewer_mappings->entries[indx], ' '))) {
		command_line = ++p;
		ptr->pid = use_external_viewer(host, command_line, viewable_file);
	} else
		ptr->pid = use_internal_viewer(host, viewable_file);

	XtFree(viewable_file);
}


/*
 * decompress_file - Decompresses "file" and returns the name of the 
 *                   decompressed file.  Returns NULL if unsuccessful.
 *                   If successful, the original file is destroyed.
 *                   If file is not recognized as compressed, the original
 *                   file name is returned.  Use XtFree() to free returned
 *                   string.
 */
char *
decompress_file(file)
char *file;
{
	int i;
	int len_ext;
	int len_filename;
	char *output_file;
	char *command_line;
	int retval;
	char *temp;
	char *q;
	char *dir;
	char *entry;
	struct sl_struct *dlist;
	char *uncompressed_file;
	char *full_path;

	/* Does file extension indicate compression? */
	len_filename = strlen(file);
	for (i=0; i<ndecompressor_mappings; i++) {
		len_ext = strlen(decompressor_mappings[i].extension);
		if (len_filename > len_ext && !strcmp(&file[len_filename-len_ext],
				decompressor_mappings[i].extension))
			break;
	}
	if (i == ndecompressor_mappings)
		return XtNewString(file);

	/* Construct command line for decompressor */
	parse_path(SYS_UNIX, file, &dir, &entry);
	command_line = XtMalloc(strlen(decompressor_mappings[i].command_line)+
		len_filename+MAXPATHLEN+1);
	temp = XtNewString(decompressor_mappings[i].command_line);
	q = strtok(temp, " ");
	strcpy(command_line, q);
	while ((q = strtok(NULL, " "))) {
		strcat(command_line, " ");
		if (!strcmp(q, "%i"))
			strcat(command_line, file);
		else if (!strcmp(q, "%o")) {
			output_file = XtNewString(file);
			output_file[len_filename-len_ext] = '\0';
			strcat(command_line, output_file);
			XtFree(output_file);
		} else if (!strcmp(q, "%d"))
			strcat(command_line, dir);
		else
			strcat(command_line, q);
	}
	XtFree(temp);

	/* Decompress file */
	retval = system_with_preserve_sigcld(command_line);
	XtFree(command_line);
	if (retval) {
		XtFree(dir);
		XtFree(entry);
		return NULL;
	}

	/* Determine name of decompressed file */
	if (local_ls(dir, &dlist, False) != 0) {
		XtFree(dir);
		XtFree(entry);
		return NULL;
	}
	if (dlist->nentries == 0)
		uncompressed_file = NULL;
	else if (dlist->nentries == 1)
		uncompressed_file = dlist->entries[0];
	else {
		for (i=0; i<dlist->nentries; i++)
			if (strcmp(dlist->entries[i], entry)) {
				uncompressed_file = dlist->entries[i];
				break;
			}
		if (i == dlist->nentries)
			fatal_error("Trouble in decompress_file()");
	}
	if (uncompressed_file)
		full_path = merge_paths(SYS_UNIX, dir, uncompressed_file);
	else
		full_path = NULL;
	XtFree(entry);
	XtFree(dir);
	release_array_list(dlist);
	return full_path;
}


/*
 * delete_file_and_its_dir - Delete "file" and the "dir" that contains it.
 *                           If "file" is NULL, this routine is a no-op.
 *                           No errors are reported if the file and/or
 *                           directory do not exist, or if unable to
 *                           delete either.
 */
delete_file_and_its_dir(file)
char *file;
{
	char *dir;

	/* First delete the file */
	unlink(file);

	/* Now remove its directory */
	parse_path(SYS_UNIX, file, &dir, NULL);
	rmdir(dir);
	XtFree(dir);
}


/*
 * use_external_viewer - Pop up an external viewer to display "file".  
 *                       "command_line" is the command to be executed,
 *                       after replacing all instances of "<>" with 
 *                       the file name.  "file" and its directory are
 *                       deleted when the viewer terminates (or when
 *                       xftp terminates).  "host" is used only for
 *                       error reporting purposes.  Returns the external
 *                       viewer's pid, which is used to associate the
 *                       viewer with the temporary view directory.
 */
use_external_viewer(host, command_line, file)
int host;
char *command_line;
char *file;
{
	int count;
	char *q;
	char *p;
	char *cmd;
	int indx;
	int filename_len = strlen(file);
	int i;
	char *argv[MAXARGV+1];
	int argc = 0;
	int pid;

	/* Count number of "<>" patterns in command line */
	count = 0;
	q = command_line;
	while ((q = strstr(q, PLACEHOLDER))) {
		count++;
		q += strlen(PLACEHOLDER);
	}

	/* Now build real command line */
	cmd = XtMalloc(strlen(command_line)+count*filename_len+1);
	indx = 0;
	p = command_line;
	while ((q = strstr(p, PLACEHOLDER))) {
		while (p < q)
			cmd[indx++] = *p++;
		p = q+strlen(PLACEHOLDER);
		for (i=0; i<filename_len; i++)
			cmd[indx++] = file[i];
	}
	while (*p)
		cmd[indx++] = *p++;
	cmd[indx] = '\0';

	/* Parse execute line into tokens */
    if ((argv[argc++] = strtok(cmd, " ")) == NULL)
        fatal_error("Trouble in use_external_viewer()");
    while (argc < MAXARGV && (argv[argc] = strtok(NULL, " ")) != NULL)
        argc++;
    if (argc == MAXARGV)
        warning_error("Can't launch viewer -- Bad execute line");

	/* Start up view directory garbage collector */
	if (!garbage_collector_running) {
		set_view_dir_remover_timer();
		garbage_collector_running = True;
	}

    /* Fork and exec */
    if ((pid = fork()) == 0)
        if (execvp(argv[0], argv) == -1) {
			fprintf(stderr, "\007*** Unable to launch viewer %s: ", argv[0]);
            perror("");
            exit(-1);
        }

	XtFree(cmd);

	return pid;
}


/*
 * mark_viewdir_for_removal - Flag the directory used by the external
 *                            viewer with "pid" as no longer needed.
 */
mark_viewdir_for_removal(pid)
int pid;
{
	struct viewdir_link *ptr = viewdir_head;

	while (ptr) {
		if (ptr->pid == pid) {
			ptr->ready_for_removal = True;
			return;
		}
		ptr = ptr->next;
	}
}


/*
 * cb_view_dir_remover - Callback that removes directories used by external
 *                       viewers when they are no longer needed.
 */
void
cb_view_dir_remover(client_data, id)
XtPointer client_data;
XtIntervalId *id;
{
	struct viewdir_link *ptr;

	/* Delete flagged directories used by terminated external viewers */
	ptr = viewdir_head;
	while (ptr)
		if (ptr->ready_for_removal) {
			delete_viewer_dir(ptr->pid);
			ptr = viewdir_head;   /* Yes, this is correct */
		} else
			ptr = ptr->next;

	/* Reset timer if there still more active external viewers */
	ptr = viewdir_head;
	while (ptr) {
		if (ptr->pid > 0) {
			set_view_dir_remover_timer();
			return;
		}
		ptr = ptr->next;
	}

	/* Through garbage collecting for now */
	garbage_collector_running = False;
}


/*
 * set_view_dir_remover_timer - Set the timer to invoke cb_view_dir_remover().
 */
set_view_dir_remover_timer()
{
	XtAppAddTimeOut(app, (unsigned long)VINTERVAL, cb_view_dir_remover,
		(XtPointer)NULL);
}

