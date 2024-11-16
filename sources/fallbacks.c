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

static String non_color_fallbacks[] = {
	"XFtp*defaultFontList: *hel*-b*-r-*-12-*,variable",
	"XFtp*fontList: *hel*-b*-r-*-12-*,variable",
	"XFtp*leftDirLong*dirList.fontList: 7x13bold,fixed",
	"XFtp*rightDirLong*dirList.fontList: 7x13bold,fixed",
	"XFtp*view*text.fontList: 7x13bold,fixed",
	"XFtp*borderWidth: 0",
	"XFtp.title: LLNL XFTP",
	"*main.width: 506",
	"*main.height: 444",
	"*main*dirName.labelString: Dir:",
	"*main*dirList.scrollBarDisplayPolicy: STATIC",
	"*main*dirList.visibleItemCount: 10",
	"*main*xferModeLabel.labelString: Xfer Mode",
	"*main*ascii.labelString: ASCII",
	"*main*binary.labelString: Binary",
	"*main*xferOpsLabel.labelString: Xfer Ops",
	"*main*xferOps.entryAlignment: ALIGNMENT_CENTER",
	"*main*xferOps.viewButton.labelString: View",
	"*main*bottomForm.paneMinimum: 79",
	"*main*resultFlagFrame.shadowType: XmSHADOW_IN",
	"*main*log.editable: False",
	"*main*log.marginHeight: 3",
	"*wildcard_popup.title: Wildcard",
	"*wildcard*listLabel.labelString: Recently Referenced Wildcards:",
	"*wildcard*list.visibleItemCount: 6",
	"*wildcard*list.scrollBarDisplayPolicy: STATIC",
	"*wildcard*list.width: 220",
	"*wildcard*textLabel.labelString: Wildcard to Apply:",
	"*wildcard*replaceToggle.labelString: Replace Filename Selection",
	"*wildcard*addToggle.labelString: Add to Filename Selection",
	"*rConnect_popup.title: Connect to Remote Host",
	"*rConnect.okLabelString: Connect",
	"*rConnect*hostListLabel.labelString: Recently Referenced Hosts:",
	"*rConnect*hostList.visibleItemCount: 6",
	"*rConnect*hostList.scrollBarDisplayPolicy: STATIC",
	"*rConnect*hostNameLabel.labelString: Host Name:",
	"*rConnect*userNameLabel.labelString: User Name:",
	"*aConnect_popup.title: Connect to Anonymous",
	"*aConnect.okLabelString: Connect",
	"*aConnect*hostListLabel.labelString: Recently Referenced Hosts:",
	"*aConnect*hostList.visibleItemCount: 6",
	"*aConnect*hostList.scrollBarDisplayPolicy: STATIC",
	"*aConnect*hostNameLabel.labelString: Host Name:",
	"*aConnect*userNameLabel.labelString: User Name:",
	"*aConnect*passwordLabel.labelString: Password:",
	"*password_popup.title: Password",
	"*password.selectionLabelString: Type password",
	"*account_popup.title: Account",
	"*account.selectionLabelString: Type account",
	"*changeDir_popup.title: Change Directory",
	"*changeDir.listLabelString: Recently Referenced Directories:",
	"*changeDir.selectionLabelString: Directory:",
	"*makeDir_popup.title: Make Directory",
	"*makeDir.selectionLabelString: Directory Name:",
	"*verify_popup.title: Verify Selection",
	"*verify*list.visibleItemCount: 8",
	"*verify*list.scrollBarDisplayPolicy: STATIC",
	"*destdir_popup.title: Destination Directory",
	"*destdir*instruction.labelString: Display Destination Directory",
	"*destdir*dirName.labelString: Dir:",
	"*destdir*dirList.scrollBarDisplayPolicy: STATIC",
	"*destdir*dirList.width: 228",
	"*destdir*form.height: 250",
	"*rename_popup.title: Rename",
	"*rename.okLabelString: Rename",
	"*rename.applyLabelString: Skip",
	"*rename*oldNameLabel.labelString: Old name:",
	"*rename*newNameLabel.labelString: New name:",
	"*quote_popup.title: Send Quoted Command",
	"*quote.okLabelString: Send",
	"*quote*listLabel.labelString: Recently Referenced Commands:",
	"*quote*list.visibleItemCount: 6",
	"*quote*list.scrollBarDisplayPolicy: STATIC",
	"*quote*list.width: 220",
	"*quote*textLabel.labelString: Low-level FTP Command:",
	"*quote*warningLabel.labelString: WARNING!",
	"*quote*warningFrame.shadowType: SHADOW_ETCHED_OUT",
	"*quote*warningMsg1.labelString: Do not send commands directly to the",
	"*quote*warningMsg1.alignment: ALIGNMENT_BEGINNING",
	"*quote*warningMsg2.labelString: FTP server unless you know what you",
	"*quote*warningMsg2.alignment: ALIGNMENT_BEGINNING",
	"*quote*warningMsg3.labelString: are doing.  Press Help for details.",
	"*quote*warningMsg3.alignment: ALIGNMENT_BEGINNING",
	"*question_popup.title: Question",
	"*question.okLabelString: Yes",
	"*question.cancelLabelString: No",
	"*abort_popup.title: Abort in Progress",
	"*abort.abortMessage.labelString: ABORT OPERATION IN PROGRESS\\n\\n------------\\n\\nPLEASE BE PATIENT",
	"*saveLog_popup.title: Save Diagnostics Log",
	"*saveLog.selectionLabelString: Log File Name:",
	"*xfermon_popup.title: File Transfer Monitor",
	"*xfermon.okLabelString: Abort",
	"*xfermon.width: 400",
	"*xfermon.height: 221",
	"*xfermon.resizePolicy: RESIZE_NONE",
	"*xfermon*statusLabel.labelString: Status:",
	"*xfermon*status.leftOffset: 95",
	"*xfermon*nameLabel.labelString: File Name:",
	"*xfermon*name.leftOffset: 95",
	"*xfermon*lengthLabel.labelString: File Length:",
	"*xfermon*length.leftOffset: 95",
	"*xfermon*bytesMovedLabel.labelString: Bytes Moved:",
	"*xfermon*bytesMoved.leftOffset: 95",
	"*prefs_popup.title: Xftp Preferences",
	"*prefs*okButton.labelString: OK",
	"*prefs*applyButton.labelString: Apply",
	"*prefs*cancelButton.labelString: Cancel",
	"*prefs*helpButton.labelString: Help",
	"*prefs.scrolledWindow.height: 280",
	"*prefs.scrolledWindow*rowColumn.frame.shadowType: SHADOW_OUT",
	"*prefs.scrolledWindow*radioBoxFrame.shadowType: SHADOW_ETCHED_IN",
	"*prefs.scrolledWindow.width: 447",
	"*leftDirPath_popup.title: \"Left\" Host Directory",
	"*leftDirPath.cancelLabelString: Close",
	"*leftDirPath.width: 350",
	"*leftDirPath.height: 100",
	"*rightDirPath_popup.title: \"Right\" Host Directory",
	"*rightDirPath.cancelLabelString: Close",
	"*rightDirPath.width: 350",
	"*rightDirPath.height: 100",
	"*leftDirLong_popup.title: \"Left\" Long Directory List",
	"*leftDirLong.closeButton.labelString: Close",
	"*leftDirLong.helpButton.labelString: Help",
	"*leftDirLong*dirList.columns: 75",
	"*leftDirLong*dirList.rows: 12",
	"*rightDirLong_popup.title: \"Right\" Long Directory List",
	"*rightDirLong.closeButton.labelString: Close",
	"*rightDirLong.helpButton.labelString: Help",
	"*rightDirLong*dirList.columns: 75",
	"*rightDirLong*dirList.rows: 12",
	"*leftDirTable_popup.title: \"Left\" Tabular Directory List",
	"*leftDirTable.closeButton.labelString: Close",
	"*leftDirTable.helpButton.labelString: Help",
	"*leftDirTable.scrolledWindow.width: 400",
	"*leftDirTable.scrolledWindow.height: 200",
	"*rightDirTable_popup.title: \"Right\" Tabular Directory List",
	"*rightDirTable.closeButton.labelString: Close",
	"*rightDirTable.helpButton.labelString:  Help",
	"*rightDirTable.scrolledWindow.width: 400",
	"*rightDirTable.scrolledWindow.height: 200",
	"*about_popup.title: About LLNL XFTP",
	"*about.cancelLabelString: Close",
	"*about.helpLabelString: More",
	"*help_popup.title: Help",
	"*help.width: 450",
	"*help.height: 300",
	"*information_popup.title: Information",
	"*mpMonitor_popup.title: Monitor",
	"*mpMonitor.okLabelString: Abort",
	"*mpMonitor.width: 350",
	"*mpMonitor.height: 100",
	"*view.saveButton.labelString: Save",
	"*view.closeButton.labelString: Close",
	"*view.helpButton.labelString: Help",
	"*view*text.columns: 70",
	"*view*text.rows: 18",
	"*viewPrefs_popup.title: Viewer Preferences",
	"*viewPrefs*listLabel.labelString: Extensions:",
	"*viewPrefs*list.visibleItemCount: 8",
	"*viewPrefs*list.scrollBarDisplayPolicy: STATIC",
	"*viewPrefs*extensionLabel.labelString: Extension:",
	"*viewPrefs*commandLineLabel.labelString: Viewer Command Line:",
	"*viewPrefs*editFrame.shadowType: SHADOW_ETCHED_IN",
	"*viewPrefs*addButton.labelString: Add",
	"*viewPrefs*replaceButton.labelString: Replace",
	"*viewPrefs*deleteButton.labelString: Delete",
	"*viewPrefs*messageFrame.shadowType: SHADOW_ETCHED_OUT",
	"*viewPrefs*message1.labelString: If \"Viewer Command Line\" is null then the built-in",
	"*viewPrefs*message1.alignment: ALIGNMENT_BEGINNING",
	"*viewPrefs*message2.labelString: viewer will be used.    Use \"<>\" for file name.",
	"*viewPrefs*message2.alignment: ALIGNMENT_BEGINNING",
};

static String color_fallbacks[] = {
	"*background: grey76",
	"*foreground: brown",
	"*main*logo.foreground: Blue",
};

static int nnon_color_fallbacks = XtNumber(non_color_fallbacks);
static int ncolor_fallbacks = XtNumber(color_fallbacks);


/*
 * get_fallbacks - Return set of built-in fallback resources.  If 
 *                 "use_color" is True, the resources will include
 *                 include some color specifications; otherwise, not.
 *                 Use XtFree() to release returned memory.
 */
String *
get_fallbacks(use_color)
int use_color;
{
	String *fallbacks;
	int i;

	if (use_color) {
		fallbacks = (String *)XtMalloc(sizeof(String)
			*(nnon_color_fallbacks+ncolor_fallbacks+1));
		for (i=0; i<nnon_color_fallbacks; i++)
			fallbacks[i] = non_color_fallbacks[i];
		for (i=0; i<ncolor_fallbacks; i++)
			fallbacks[nnon_color_fallbacks+i] = color_fallbacks[i];
		fallbacks[nnon_color_fallbacks+ncolor_fallbacks] = NULL;
	} else {
		fallbacks = (String *)XtMalloc(sizeof(String)*(nnon_color_fallbacks+1));
		for (i=0; i<nnon_color_fallbacks; i++)
			fallbacks[i] = non_color_fallbacks[i];
		fallbacks[nnon_color_fallbacks] = NULL;
	}
	return fallbacks;
}

