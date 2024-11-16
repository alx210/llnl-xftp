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

#include "prefs.h"

char *help_add_to_cache[] = {
	"Specifies whether newly referenced items should be added to the",
	"directory, quoted command, wildcard, and host/user caches.  ",
	"It is usually desirable to choose YES.  Choose NO if",
	"referencing a series of seldom-referenced items that would",
	"clear the cache of your favorite entries.",
	NULL
};

char *help_anonymous_ftp_password[] = {
	"The value to initialize the \"Password\" field of the \"Connect",
	"to Anonymous\" dialog to.  This is normally your email address.  ",
	"The \"Connect to Anonymous\" dialog is accessible via each",
	"host's \"Connect\" menu.",
	NULL
};

char *help_beep_when_ops_done[] = {
	"Specifies whether to beep when each set of deletion, move,",
	"quoted command, and file transfer operations completes.",
	NULL
};

char *help_diagnostics[] = {
	"Specifies the level of diagnostics information to be written",
	"to the diagnostics log window.  Choose QUIET for error messages,",
	"only.  Choose NORMAL for error messages and success messages.  ",
	"Choose VERBOSE for error messages, success messages, and replies",
	"received from the FTP servers.  Choose DEBUG for error",
	"messages, replies received from the FTP servers,  commands",
	"sent to the FTP servers, and other assorted diagnostics.",
	NULL
};

char *help_directory_cache_size[] = {
	"The maximum number of previously referenced directory paths",
	"cached per host.  The cache is preserved across LLNL XFTP sessions.",
	NULL
};

char *help_enhance_colors[] = {
	"Specifies whether to \"pretty up\" some of the graphical",
	"elements (such as scrolled lists and text fields) by changing",
	"the normal background color.  This preference has",
	"no effect on monochrome displays.  Choose YES to enhance the",
	"appearance, or NO for the usual Motif look.",
	NULL
};

char *help_enter_dir_upon_creation[] = {
	"Specifies whether to automatically enter a newly created",
	"directory.",
	NULL
};

char *help_host_cache_size[] = {
	"The maximum number of hosts to cache items for.  The cache is preserved",
	"across LLNL XFTP sessions.",
	NULL
};

char *help_initial_log_file_name[] = {
	"The default file name to be presented to the user in the",
	"\"Save Log\" dialog.",
	NULL
};

char *help_initial_transfer_mode[] = {
	"The default file transfer mode (ASCII or BINARY).",
	NULL
};

char *help_initial_wildcard_mode[] = {
	"The default mode of the \"Use Wildcard\" dialog.  Choose",
	"REPLACE to make \"Replace Filename Selection\" the default.  ",
	"Choose ADD to make \"Add to Filename Selection\" the default.",
	NULL
};

char *help_inquire_on_copy[] = {
	"Specifies whether to pop up a dialog that asks the user to",
	"confirm that the indicated transfers should take place.",
	NULL
};

char *help_inquire_on_delete[] = {
	"Specifies whether to pop up a dialog that asks the user to",
	"confirm that the indicated deletions should take place.",
	NULL
};

char *help_inquire_on_move[] = {
	"Specifies whether to pop up a dialog that asks the user to",
	"confirm that the indicated moves should take place.",
	NULL
};

char *help_inquire_on_quote[] = {
	"Specifies whether to pop up a dialog that asks the user to",
	"confirm that the indicated commands should be sent.",
	NULL
};

char *help_left_auto_local_login[] = {
	"Specifies whether to automatically connect the left side to",
	"the local (client) host upon starting LLNL XFTP.",
	NULL
};

char *help_max_ftp_retries[] = {
	"Specifies the maximum number of retries LLNL XFTP will attempt",
	"if it receives a response from an FTP server that indicates",
	"the desired operation failed because of some temporary",
	"condition on the remote host.",
	NULL
};

char *help_max_time_for_ftp_reply[] = {
	"Specifies the maximum time in seconds that LLNL XFTP will wait",
	"for a response from an FTP server before concluding that the",
	"connection has been broken.",
	NULL
};

char *help_overwrite_dir_on_copy[] = {
	"Specifies whether it is okay to overwrite an existing directory",
	"when recursively copying another directory by the same name.  If a sink",
	"directory is overwritten then none of its subdirectories will be",
	"deleted, but new files and subdirectories might be added to it,",
	"and some existing files might be replaced.",
	NULL
};

char *help_preserve_vms_ver_nums[] = {
	"Specifies whether to preserve VMS file version numbers when",
	"transferring files from a VMS system.  This is only an issue",
	"for those VMS FTP servers that provide version numbers (not",
	"all do).",
	NULL,
};

char *help_print_password_in_debug[] = {
	"Specifies whether to print passwords in the log window when",
	"the user preference DIAGNOSTICS is set to DEBUG.",
	NULL,
};

char *help_quote_cache_size[] = {
	"The number of quoted commands to cache.  The cache is preserved",
	"across LLNL XFTP sessions.",
	NULL
};

char *help_quote_placeholder[] = {
	"This is used as a placeholder in a quoted command to represent",
	"items selected in the directory list.  For example, the quoted",
	"command \"stage 1 <>\" will send a STAGE command to the remote",
	"host for each selected item.  The default value is \"<>\",",
	"but the placeholder can be set to any nonnull printable string.",
	NULL
};

char *help_recursive_delete_enabled[] = {
	"Specifies whether to enable the RECURSIVELY DELETE SELECTED",
	"ENTRY(S) item in the OPS menus.  Choose NO to help prevent",
	"accidental recursive deletions.",
	NULL
};

char *help_right_auto_local_login[] = {
	"Specifies whether to automatically connect the right side to",
	"the local (client) host upon starting LLNL XFTP.",
	NULL
};

char *help_use_last_dir[] = {
	"Specifies whether to automatically re-enter the most recently",
	"referenced directory after connecting to a host.  If this",
	"option is requested, but is not possible to perform (e.g.,",
	"because the directory no longer exists), an error message",
	"will be generated, and your home directory will be entered.",
	NULL
};

char *help_sort_caches[] = {
	"Specifies whether to sort (by ASCII-collating sequence) cached",
	"items when they are displayed in dialogs.  If YES, then sort.  ",
	"If NO, then the most recently used items are placed at the top",
	"of the list.",
	NULL
};

char *help_sort_long_lists_by_date[] = {
	"Specifies whether to attempt to sort the \"long\" directory",
	"lists by time modified (with most recent listed first)",
	"instead of by name.  The YES option is considered to be",
	"unsafe because some FTP servers do not support this option",
	"and may give unpredictable results.  (Most UNIX hosts",
	"support this option.)",
	NULL
};

char *help_store_unique[] = {
	"Specifies whether existing sink files are to be renamed before",
	"file transfer occurs.  If YES, then the existing sink file",
	"is typically renamed by appending a \".\" (period) followed",
	"by an integer between 1 and 99.",
	NULL
};

char *help_symbols_on_dir_entries[] = {
	"Specifies whether to append single characters to directory",
	"entries in order to distinguish their types.  If YES(UNSAFE)",
	"is choosen, \"/\" is appended to directories, \"*\" is",
	"appended to executables, \"@\" is appended to symbolic links",
	"and \"=\" is appended to AF_UNIX address family sockets.  ",
	"The YES option is considered to be unsafe because some FTP",
	"servers do not support this option and may give unpredictable",
	"results.  (Most UNIX hosts support this option.)  ",
	"This option is ignored when used with VMS hosts (and, hence,",
	"is safe to use with VMS.)  ",
	"Furthermore, if you are referencing a directory or directory",
	"entry whose name ends with one of these special symbols,",
	"you will need to use the NO option.  When in",
	"doubt, select NO(SAFE).  One further note: use of this",
	"feature can dramatically degrade interactivity.",
	NULL
};

char *help_user_cache_size[] = {
	"The number of user names to cache (for remote hosts).  The",
	"cache is preserved across LLNL XFTP sessions.",
	NULL
};

char *help_wildcard_cache_size[] = {
	"The number of wildcard expressions to cache.  The cache is",
	"preserved across LLNL XFTP sessions.",
	NULL
};

char *help_prefs_dialog[] = {
	"This dialog allows the user to configure LLNL XFTP according to",
	"his/her preferences.  The user is presented with a number",
	"of configuration parameters, which are set by either toggling,",
	"sliding, or typing.  Click the APPLY button to cause the",
	"new values to take effect.  The OK button has the same",
	"effect as APPLY, but the dialog is also closed.",
	"The CANCEL button undoes changes made since OK or APPLY",
	"were last pressed.\n",
	"\n",
	"The preferences can be preserved across LLNL XFTP sessions in",
	"a text file named \".xftprc\" in the user's home directory",
	"(select SAVE PREFERENCES in the main window's OPTIONS menu).\n",
	"\n",
	"The user may alter the order the preferences are presented",
	"in the dialog by (1) terminating LLNL XFTP, (2) using a text",
	"editor to change the order of lines in .xftprc, and (3)",
	"restarting LLNL XFTP.\n",
	"\n",
	"IMPORTANT: An online description of each user preference can be viewed",
	"by clicking on the name of the desired user preference.",
	NULL
};

extern struct prefs_struct prefs[];


/*
 * cb_prefs_item_help - Callback that displays help information about the
 *                      user preference clicked on.
 */
void
cb_prefs_item_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    int indx = (int)((long)client_data);
	int len;
	char *label;
	int i;

    len = strlen(prefs[indx].file_label);
    label = XtNewString(prefs[indx].file_label);
    for (i=0; i<len; i++)
        if (label[i] == '_')
            label[i] = ' ';
	help_dialog(widget, False, label, prefs[indx].help_text);
    XtFree(label);
}


/*
 * cb_prefs_help - Callback that displays help information about the
 *                 User Preferences dialog.
 */
cb_prefs_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	help_dialog(widget, False, "Xftp Preferences", help_prefs_dialog);
}

