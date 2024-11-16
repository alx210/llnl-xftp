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

#include <Xm/Xm.h>
#include "xftp.h"

extern Widget w_toplev;

static char *using_help[] = {
	"Select LLNL XFTP OVERVIEW in the HELP menu for a description",
	"of LLNL XFTP.\n",
	"\n",
	"Select GETTING STARTED in the HELP menu for a brief",
	"discussion on how to use LLNL XFTP.\n",
	"\n",
	"Select CONTEXTUAL HELP in the HELP menu to change the cursor",
	"to a pointing hand.  Move the hand and click on any graphical",
	"element in the main window you wish to know more about.\n",
	"\n",
	"Many popup dialogs have  help packages that can be accessed",
	"by clicking on their HELP buttons.\n",
	"\n",
	"Select ABOUT LLNL XFTP in the FILE menu for version and author",
	"information.",
	NULL
};

static char *getting_started[] = {
	"Use the CONNECT menu to connect the two sides to the desired",
	"combination of local and/or remote hosts.  (Depending on how",
	"the user preferences are set, when LLNL XFTP comes up some",
	"combination of the left and right sides might already be",
	"connected to the local host.)\n",
	"\n",
	"Once connected, traverse through a host's directory structure",
	"by (1) using the \"directory name\" menu (the menu just above",
	"the directory list), (2) double-clicking on entries in the",
	"directory list, (3) selecting the CHANGE DIRECTORY item",
	"in the host's DIR menu, or (4) selecting an item from the",
	"GO TO PREVIOUS DIRECTORY submenu in the host's DIR menu.\n",
	"\n",
	"Obtain different types of views into a host's current directory",
	"by selecting the DISPLAY DIRECTORY (LONG) or DISPLAY DIRECTORY",
	"(TABLE) items in the host's DIR menu.\n",
	"\n",
	"Select/deselect directory entries to operate on by (1)",
	"single-clicking on entries in the main directory list,",
	"(2) single-clicking on entries in the tabular directory",
	"list, or (3) dragging the mouse with the left button held down",
	"over entries in the tabular directory list, or (4) by selecting",
	"any of the items in the host's SELECT menu.\n",
	"\n",
	"Transfer selected items by clicking on the COPY or RCOPY",
	"(recursive copy) buttons.  Be sure to first set the file",
	"transfer mode by toggling the appropriate button (ASCII or",
	"BINARY) in the main window.\n",
	"\n",
	"Perform other types of operations on selected items by",
	"selecting any one of the items in the host's OPS menu.\n",
	"\n",
	"Configure LLNL XFTP by selecting the PREFERENCES item in the",
	"OPTIONS menu.",
	NULL
};

static char *xftp_overview[] = {
	"LLNL XFTP is a graphical user interface to FTP (File Transfer",
	"Protocol).  LLNL XFTP does not execute the FTP program; instead,",
	"it directly communicates with remote FTP servers.  ",
	"LLNL XFTP is an X client and follows the OSF/Motif",
	"look-and-feel.  LLNL XFTP enables users to transfer files and",
	"perform various directory manipulation commands, both",
	"remotely and locally.  Two especially notable features",
	"are the ability to (1) perform \"third-party\" file",
	"transfers, and (2) transfer or delete entire directory",
	"substructures.\n",
	"\n",
	"LLNL XFTP's main window is divided into five main sections: (1)",
	"the main menu bar at the top of the window, (2) the diagnostics",
	"log at the bottom, (3) the file transfer controls in the",
	"center, and (4 and 5) the \"left\" and \"right\" host sections",
	"to either side of the file transfer controls.\n",
	"\n",
	"LLNL XFTP can be connected to zero, one, or two hosts.  The hosts",
	"may be any combination of local and remote hosts (the local",
	"host is the client host upon which LLNL XFTP is running).\n",
	"\n",
	"Each host section consists of four major graphical elements:",
	"(1) a label which displays the host's name; (2) a menu bar",
	"whose menus contain many commands that operate on the host;",
	"(3) a \"directory\" menu; and (4) a scrollable \"directory",
	"list.\"\n",
	"\n",
	"Each host's CONNECT menu contains commands for",
	"connecting/disconnecting local and remote hosts.\n",
	"\n",
	"Each host's DIR menu contains various commands for creating",
	"new directories, and for changing and displaying the",
	"current directory.\n",
	"\n",
	"Each host's SELECT menu contains commands for",
	"selecting/deselecting entries in the current directory",
	"(the selections appear in the scrollable list of directory",
	"entries).\n",
	"\n",
	"Each host's OPS menu contains several commands that operate",
	"on selected directory entries.\n",
	"\n",
	"Each host's directory menu contains each link of the path",
	"of the current directory (the last link is displayed).  By",
	"selecting a link in the path, the current directory is",
	"correspondingly changed.  (Hence, this is a mechanism to",
	"traverse up the host's directory structure.)\n",
	"\n",
	"Each host's directory list contains the entries of the",
	"current directory.  One way to select/deselect a directory",
	"entry is to click on it.  If an entry is double-clicked,",
	"an attempt is made to change the current directory to that",
	"entry.  (Hence, this is a way to traverse down the host's",
	"directory structure.)\n",
	"\n",
	"The file transfer controls in the middle of the main window allow",
	"you to initiate file transfers on selected entries.  The",
	"transfers can go from left to right or from right to left.  ",
	"Angle brackets in the COPY and RCOPY buttons indicate the",
	"direction of the transfer (e.g., \">>Copy>>\" indicates",
	"that a left-to-right transfer would take place).  The",
	"VIEW button displays the selected file.\n",
	"\n",
	"The scrollable diagnostics log window at the bottom of the main",
	"window displays error and status information.  ",
	"There are ways to save and clear the contents of the",
	"diagnostics window, and to",
	"control the level of verbosity of messages written to it.\n",
	"\n",
	"There is a flag to the left of the diagnostics log window",
	"that indicates whether an operation completes successfully",
	"or not.  If an operation fails then (1) a beep is emitted,",
	"(2) the flag changes from green to red (white to black on",
	"monochrome displays), (3) the \"OK\" in",
	"the flag changes to \"E\", and (4) and error message is",
	"written to the diagnostics log window.  The next operation",
	"will automatically clear the error flag.\n",
	"\n",
	"LLNL XFTP automatically caches (1) wildcard expressions, (2) paths",
	"of current directories, (3) host names and user names used",
	"in connecting to remote hosts, and (4) quoted commands.  The",
	"purpose of each cache is to reduce the amount of typing",
	"required.  The cache is automatically preserved across LLNL XFTP",
	"sessions in a file named \".xftpcache\" in your home directory.\n",
	"\n",
	"Finally, selecting item PREFERENCES in the OPTIONS menu pops",
	"up a dialog that allows you to configure LLNL XFTP according to",
	"your preferences.",
	NULL
};


static char *availability[] = {
	"Source code for LLNL XFTP is tarred and compressed, and is",
	"available as follows:\n",
	"\n",
	"        ftp://coral.ocf.llnl.gov/pub/ia/llnlxftp/latest_xftp.tar.Z\n",
	"\n",
	"        http://www.llnl.gov/liv_comp/xftp.html\n",
	NULL,
};

static char *help_file_menu[] = {
	"The FILE menu contains the following items:\n",
	"\n",
	"\n",
	"ABOUT LLNL XFTP\n",
	"\n",
	"Displays information about this version of XFTP.\n",
	"\n",
	"\n",
	"QUIT\n",
	"\n",
	"Terminates the application.\n",
	NULL
};

static char *help_log_menu[] = {
	"The LOG menu contains the following items:\n",
	"\n",
	"\n",
	"CLEAR LOG\n",
	"\n",
	"Clears the diagnostics log window.\n",
	"\n",
	"\n",
	"SAVE LOG\n",
	"\n",
	"Writes the contents of the diagnostics window to a user-specified",
	"file.  The existing contents of the diagnostics window are not",
	"removed.\n",
	NULL
};

static char *help_options_menu[] = {
	"The OPTIONS menu contains the following items:\n",
	"\n",
	"\n",
	"PREFERENCES\n",
	"\n",
	"Displays the user preferences dialog.\n",
	"\n",
	"\n",
	"VIEWER PREFERENCES\n",
	"\n",
	"Displays the viewer preferences dialog.\n",
	"\n",
	"\n",
	"SAVE PREFERENCES\n",
	"\n",
	"Saves the current settings of the user and viewer",
	"preferences.\n",
	NULL
};

static char *help_connect_menu[] = {
	"The CONNECT menu contains the following commands for",
	"connecting/disconnect local and remote hosts:\n",
	"\n",
	"\n",
	"CONNECT TO LOCAL\n",
	"\n",
	"Connects to the local (client) host.  If currently connected, the",
	"connection is first closed.\n",
	"\n",
	"\n",
	"CONNECT TO REMOTE\n",
	"\n",
	"Displays a dialog that enables the user to connect to a remote host.\n",
	"\n",
	"\n",
	"CONNECT TO ANONYMOUS\n",
	"\n",
	"Displays a dialog that enables the user to easily connect to an",
	"anonymous FTP site.\n",
	"\n",
	"\n",
	"DISCONNECT\n",
	"\n",
	"Closes the currently connected host.\n",
	NULL
};

static char *help_dir_menu[] = {
	"The DIR menu contains the following commands for creating new",
	"directories, and for changing and displaying the current",
	"directory:\n",
	"\n",
	"\n",
	"GO TO PREVIOUS DIRECTORY\n",
	"\n",
	"Displays a pullright menu that allows the user to easily change",
	"the current directory to a previously referenced directory.\n",
	"\n",
	"\n",
	"CHANGE DIRECTORY\n",
	"\n",
	"Displays a dialog that enables the user to change the current",
	"directory.\n",
	"\n",
	"\n",
	"MAKE DIRECTORY\n",
	"\n",
	"Displays a dialog to prompt for the name of a directory to make",
	"in the current directory.\n",
	"\n",
	"\n",
	"DISPLAY DIRECTORY PATH\n",
	"\n",
	"Pops up a dialog that displays the current directory for that",
	"host.  It will be automatically updated as the current directory",
	"changes.\n",
	"\n",
	"\n",
	"DISPLAY DIRECTORY (LONG)\n",
	"\n",
	"Pops up a dialog that displays the contents of the current",
	"directory in a scrolled list.  If supported by the host (and",
	"this is almost always the case), additional information about",
	"each directory entry is displayed, such as file length and",
	"access restrictions.  This display is automatically updated",
	"as the current directory changes.  This directory display does",
	"not support selecting entries or displaying the current",
	"selection.\n",
	"\n",
	"WARNING: Using this feature can increase network traffic, put an",
	"additional load on the remote host, and slow down interactivity.\n",
	"\n",
	"\n",
	"DISPLAY DIRECTORY (TABLE)\n",
	"\n",
	"Pops up a dialog that displays the contents of the current",
	"directory in tabular form.  This dialog may be used to view",
	"and modify the directory's selected entries.  Entries may be",
	"selected/deselected either by clicking or by moving the cursor",
	"over the entries with the mouse button down.  Moving the mouse",
	"too quickly can cause some of the entries to be skipped.  If an",
	"entry is double-clicked, an attempt is made to change the",
	"current directory to that entry.  This display is automatically",
	"updated as the current directory changes.\n",
	NULL
};

static char *help_select_menu[] = {
	"The SELECT menu contains the following commands for",
	"selecting/deselecting entries in the current directory:\n",
	"\n",
	"\n",
	"SELECT ALL ENTRIES\n",
	"\n",
	"Selects all the entries in the host's current directory.\n",
	"\n",
	"\n",
	"DESELECT ALL ENTRIES\n",
	"\n",
	"Deselects all the entries in the host's current directory.\n",
	"\n",
	"\n",
	"USE WILDCARD\n",
	"\n",
	"Pops up a dialog to prompt for a wildcard expression to use",
	"in selecting entries in the host's current directory list.",
	NULL
};

static char *help_ops_menu[] = {
	"The OPS menu contains the following commands that operate",
	"on selected directory entries:\n",
	"\n",
	"\n",
	"DELETE SELECTED ENTRY(S)\n",
	"\n",
	"Deletes the selected entries.  A dialog pops up to enable the",
	"user to monitor the progress of the deletions.  The dialog",
	"contains a button for aborting the deletions.\n",
	"\n",
	"\n",
	"MOVE SELECTED ENTRY(S)\n",
	"\n",
	"Moves selected entries from the current directory to another",
	"directory on the same host.  This is similar to the UNIX \"mv\"",
	"command, but without rename capability.  (Use RENAME SELECTED",
	"ENTRY(S) to rename entries.)\n",
	"\n",
	"\n",
	"RENAME SELECTED ENTRY(S)\n",
	"\n",
	"Pops up a dialog to individually rename selected entries.  ",
	"This is similar to the UNIX \"mv\" command except that the",
	"renamed entries cannot be moved into another directory.  ",
	"(Use MOVE SELECTED ENTRY(S) to move entries.)\n",
	"\n",
	"\n",
	"SEND QUOTED COMMAND\n",
	"\n",
	"Pops up a dialog to prompt for a \"raw\" command to send to",
	"the FTP server of the remote host.\n",
	"\n",
	"\n",
	"RECURSIVELY DELETE SELECTED ENTRY(S)\n",
	"\n",
	"Recursively deletes the selected entries.  A dialog pops up",
	"to enable the user to monitor the progress of the deletions.\n",
	"\n",
	"WARNING: Exercise extreme caution when using this command",
	"because it deletes entire directory subtrees.  To minimize",
	"the chance of an unintentional deletion, this menu item",
	"is always greyed out unless the user preference RECURSIVE DELETE",
	"ENABLED is turned on.",
	NULL
};

static char *help_dirname[] = {
	"The directory name menu contains each link of the path of the",
	"current directory (the last link is displayed).  By selecting",
	"a link in the path, the current directory is correspondingly",
	"changed.  Hence, this is a mechanism for traversing up the",
	"host's directory structure.",
	NULL
};

static char *help_dirlist[] = {
	"The directory list displays the entries of the current",
	"directory.  One way to select/deselect a directory entry",
	"is to click on it.  If an entry is double-clicked, an",
	"attempt is made to change the current directory to that",
	"entry.  Hence, this is a mechanism for traversing down the",
	"host's directory structure.",
	NULL
};

static char *help_hostname[] = {
	"If connected, this label displays the name of the host;",
	"otherwise \"Not Connected\" is displayed.",
	NULL
};

static char *help_copy[] = {
	"Transfers the selected files according to the mode (ASCII",
	"or Binary) specified in \"Xfer Mode\".  A dialog pops up to",
	"enable the user to monitor the progress of the transfers.  ",
	"The dialog contains a button for aborting the transfers.",
	NULL
};

static char *help_rcopy[] = {
	"Recursively transfers the selected entries according to",
	"the mode (ASCII or Binary) specified in \"Xfer Mode\".  If",
	"a selected entry is a directory, an attempt is made to",
	"transfer its entire directory subtree.  A dialog pops up to",
	"enable the user to monitor the progress of the transfers.  ",
	"The dialog contains a button for aborting the transfers.",
	NULL
};

static char *help_view[] = {
	"Displays the selected file.  The \"Viewer Preferences\"",
	"dialog (accessible via the OPTIONS menu) can be used to",
	"control whether the file will be displayed using the built-in",
	"viewer or using an external viewer of the user's choice",
	"(e.g., emacs or xv).  The selection of the viewer is",
	"configurable, based on the file extension (e.g., \".gif\"",
	"or \".jpg\").  There is no limit on the number or types",
	"of viewers that can be simultaneously displayed.\n",
	"\n",
	"The file is transferred according to the mode (ASCII or",
	"Binary) specified in \"Xfer Mode\".  A dialog pops up",
	"to enable the user to monitor the progress of the",
	"transfer.  The dialog contains a button for aborting",
	"the transfer.\n",
	"\n",
	"The destination of the transfer is a temporary directory",
	"created in the directory defined as P_tmpdir in the include",
	"file stdio.h.  If the environment variable TMPDIR is",
	"defined, it is used as the directory.  The temporary", 
	"directory, and the files it contains, are automatically",
	"deleted when the user dismisses the viewer or terminates",
	"LLNL XFTP.\n",
	"\n",
	"If the file is recognized as being compressed, it is",
	"expanded before viewing.  LLNL XFTP excutes the following",
	"decompressors based on the file extension: gunzip for",
	"\".z\" and \".gz\"; uncompress for \".Z\"; atob for",
	"\"atob\"; and uudecode for \".uu\".  LLNL XFTP searches",
	"for the decompressor application in the standard way",
	"using the PATH environment variable.",
	NULL
};

static char *help_xfer_mode[] = {
	"Click on a toggle button to select the desired file transfer",
	"mode (ASCII or Binary).  The default file transfer mode can",
	"be set via the user preference INITIAL TRANSFER MODE.",
	NULL
};

static char *help_result_flag[] = {
	"The status flag indicates whether an operation completes",
	"successfully or not.  If an operation fails then (1) a beep",
	"is emitted, (2) the flag changes from green to red",
	"(white to black on monochrome displays), (3) the",
	"\"OK\" in the flag changes to \"E\", and (4) an error",
	"message is written to the diagnostics log window.  The",
	"next operation will automatically clear the error flag.",
	NULL
};

static char *help_log[] = {
	"The diagnostics window displays error messages and status",
	"information.  The user can control the level of verbosity",
	"of messages via the user preference",
	"DIAGNOSTICS.  The contents of this window can be saved to",
	"a file by selecting the SAVE LOG item in the",
	"LOG menu.  Clear this window by selecting the CLEAR",
	"LOG item in the LOG menu.\n",
	"\n",
	"If the status flag indicates that an error has occurred,",
	"refer to the diagnostics window for the details.",
	NULL
};


/*
 * cb_help_file_menu - Callback to display help message for "File" menu.
 */
cb_help_file_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "File Menu", help_file_menu);
}


/*
 * cb_help_log_menu - Callback to display help message for "Log" menu.
 */
void
cb_help_log_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Log Menu", help_log_menu);
}


/*
 * cb_help_options_menu - Callback to display help message for "Options" menu.
 */
void
cb_help_options_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Options Menu", help_options_menu);
}


/*
 * cb_help_connect_menu - Callback to display help message for "Connect" menu.
 */
void
cb_help_connect_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Connect Menu", help_connect_menu);
	else
		help_dialog(w_toplev, False, "Right Connect Menu", help_connect_menu);
}


/*
 * cb_help_dir_menu - Callback to display help message for "Dir" menus.
 */
void
cb_help_dir_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Dir Menu", help_dir_menu);
	else
		help_dialog(w_toplev, False, "Right Dir Menu", help_dir_menu);
}


/*
 * cb_help_select_menu - Callback to display help message for "Select" menus.
 */
void
cb_help_select_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Select Menu", help_select_menu);
	else
		help_dialog(w_toplev, False, "Right Select Menu", help_select_menu);
}


/*
 * cb_help_ops_menu - Callback to display help message for "Ops" menus.
 */
void
cb_help_ops_menu(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Ops Menu", help_ops_menu);
	else
		help_dialog(w_toplev, False, "Right Ops Menu", help_ops_menu);
}


/*
 * cb_help_dirname - Callback to display help message for directory name
 *                   menus.
 */
void
cb_help_dirname(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Directory Name Menu", help_dirname);
	else
		help_dialog(w_toplev, False, "Right Directory Name Menu", help_dirname);
}


/*
 * cb_help_dirlist - Callback to display help message for main window
 *                   directory lists.
 */
void
cb_help_dirlist(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Directory List", help_dirlist);
	else
		help_dialog(w_toplev, False, "Right Directory List", help_dirlist);
}


/*
 * cb_help_hostname - Callback to display help message for host name fields
 *                    in main window.
 */
void
cb_help_hostname(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	int host = (int)((long)client_data);

	if (host == LEFT)
		help_dialog(w_toplev, False, "Left Host Name", help_hostname);
	else
		help_dialog(w_toplev, False, "Right Host Name", help_hostname);
}


/*
 * cb_help_copy - Callback to display help message for "Copy" button in
 *                main window.
 */
void
cb_help_copy(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Copy Button", help_copy);
}


/*
 * cb_help_rcopy - Callback to display help message for Recursive Copy Button
 *                 in main window.
 */
void
cb_help_rcopy(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Recursive Copy Button", help_rcopy);
}


/*
 * cb_help_view - Callback to display help message for View Button in main
 *                window.
 */
void
cb_help_view(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "View Button", help_view);
}


/*
 * cb_help_xfer_mode - Callback to display help message for file transfer
 *                     mode control in main window.
 */
void
cb_help_xfer_mode(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "File Transfer Mode Control", help_xfer_mode);
}


/*
 * cb_help_result_flag - Callback to display help message for result
 *                       flag in main window.
 */
void
cb_help_result_flag(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Status Flag", help_result_flag);
}


/*
 * cb_help_log - Callback to display help message for diagnostics log
 *               window.
 */
void
cb_help_log(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Diagnostics Log Window", help_log);
}


/*
 * cb_xftp_overview - Callback to display help message that gives overview
 *                    of Xftp.
 */
cb_xftp_overview(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "LLNL XFTP Overview", xftp_overview);
}


/*
 * cb_getting_started - Callback to display "Getting Started" help message.
 */
void
cb_getting_started(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Getting Started", getting_started);
}


/*
 * cb_using_help - Callback to display help message describing how to use
 *                 the online help facility.
 */
void
cb_using_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Using Help", using_help);
}


/*
 * cb_availability - Callback to display availability help message.
 */
void
cb_availability(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(w_toplev, False, "Availability", availability);
}

