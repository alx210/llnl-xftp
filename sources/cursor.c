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

#include <X11/cursorfont.h>
#include <Xm/Xm.h>

extern Display *display;

struct dialog_list_st {
	struct dialog_list_st *next;
	struct dialog_list_st *prev;
	Widget dialog_widget;
};

struct cursor_stack_st {
	int shape;
	Cursor cursor;
	struct cursor_stack_st *next;
};

static struct cursor_stack_st *cursor_head = NULL;
static struct dialog_list_st *dialog_list_head = NULL;


/*
 * change_cursor - Change cursor to "shape", which specifies a character
 *                 in the standard cursor font.  A stack of cursors is
 *                 maintained to make it easy to pop back to a previously
 *                 used cursor.  Every dialog in use has to indivually
 *                 have its cursor changed.
 */
change_cursor(shape)
int shape;
{
	struct cursor_stack_st *cptr;
	struct dialog_list_st *dptr;

	/* Push new cursor shape onto cursor stack */
	cptr = XtNew(struct cursor_stack_st);
	cptr->shape = shape;
	cptr->cursor = XCreateFontCursor(display, shape);
	cptr->next = cursor_head;
	cursor_head = cptr;

	/* Maybe cursor is already that shape */
	if (cptr->next && cptr->shape == cptr->next->shape)
		return;

	/* Change cursor */
	XSync(display, 0);
	dptr = dialog_list_head;
	while(dptr) {
		XDefineCursor(display, XtWindow(dptr->dialog_widget), cptr->cursor);
		dptr = dptr->next;
	}
	XSync(display, 0);
}


/*
 * use_busy_cursor - Change cursor to busy cursor shape.
 */
use_busy_cursor()
{
	change_cursor(XC_watch);
}


/*
 * use_normal_cursor - Change cursor to normal cursor shape.
 */
use_normal_cursor()
{
	change_cursor(XC_left_ptr);
}


/*
 * restore_prev_cursor - Restore cursor to the previously used shape.
 */
restore_prev_cursor()
{
	struct cursor_stack_st *current;
	struct dialog_list_st *dptr;

	/* Is there anything to pop off the cursor stack? */
	if (cursor_head == NULL)
		return;

	/* Get the current cursor (and pop it off the stack) */
	current = cursor_head;
	cursor_head = cursor_head->next;
	XFreeCursor(display, current->cursor);

	/* If cursor stack is now empty, revert to inherited cursor */
	if (cursor_head == NULL) {
		dptr = dialog_list_head;
		while(dptr) {
			XUndefineCursor(display, XtWindow(dptr->dialog_widget));
			dptr = dptr->next;
		}
		XtFree((char *)current);
		XSync(display, 0);
		return;
	}

	/* Is previous cursor shape same as current current shape? */
	if (cursor_head->shape == current->shape) {
		XtFree((char *)current);
		return;
	}
	XtFree((char *)current);

	/* Change to previous cursor for all dialogs */
	dptr = dialog_list_head;
	while(dptr) {
		XDefineCursor(display, XtWindow(dptr->dialog_widget),
			cursor_head->cursor);
		dptr = dptr->next;
	}
	XSync(display, 0);
}


/*
 * add_dialog_to_list - Add the dialog referenced by "w_dialog" to the
 *                      list of existing dialogs.  This list is used
 *                      by change_cursor() so that it can change the
 *                      cursor for all existing dialogs.
 */
add_dialog_to_list(w_dialog)
Widget w_dialog;
{
	struct dialog_list_st *dptr;

	/* Don't need to do anything if dialog widget is already in list */
	dptr = dialog_list_head;
	while (dptr) {
		if (w_dialog == dptr->dialog_widget)
			return;
		dptr = dptr->next;
	}

	dptr = XtNew(struct dialog_list_st);
	dptr->dialog_widget = w_dialog;
	dptr->prev = NULL;
	dptr->next = dialog_list_head;
	if (dialog_list_head)
		dialog_list_head->prev = dptr;
	dialog_list_head = dptr;

	/* If non-inherited cursor currently in use, change cursor right now */
	if (cursor_head) {
		XSync(display, 0);
		XDefineCursor(display, XtWindow(w_dialog), cursor_head->cursor);
		XSync(display, 0);
	}
}


/*
 * remove_dialog_from_list - Remove the dialog referenced by "w_dialog" from
 *                           the list of existing dialogs.  This list is used
 *                           by change_cursor() so that it can change the
 *                           cursor for all existing dialogs.
 */
remove_dialog_from_list(w_dialog)
Widget w_dialog;
{
	struct dialog_list_st *dptr;

	/* Disconnect from linked list */
	dptr = dialog_list_head;
	while (dptr) {
		if (dptr->dialog_widget == w_dialog) {
			if (dptr->prev)
				dptr->prev->next = dptr->next;
			else
				dialog_list_head = dptr->next;
			if (dptr->next)
				dptr->next->prev = dptr->prev;
			XtFree((char *)dptr);
			return;
		}
		dptr = dptr->next;
	}
	fatal_error("Bug in remove_dialog_from_list()");
}

