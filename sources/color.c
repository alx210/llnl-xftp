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

extern int enhance_colors;
extern int depth;
extern Visual *visual;


/*
 * color_display - Returns True if display can handle at least 256 colors,
 *                 else returns False.
 */
color_display()
{
	int class = visual->class;

	/* Can the display handle colors? */
	if (depth <8 || class == GrayScale || class == StaticGray)
		return False;
	else
		return True;
}


/*
 * fix_list_color - Set background of scrolled list to match its
 *                  scrollbar's trough color.
 */
fix_list_color(widget)
Widget widget;
{
	Pixel	sb_trough_color;
	Widget	w_vertical_sb;

	/* Does the user want color enhancement? */
	if (!enhance_colors)
		return;

	/* Can the display handle colors? */
	if (!color_display())
		return;

	XtVaGetValues(XtParent(widget), XmNverticalScrollBar, &w_vertical_sb, NULL);
	
	XtVaGetValues(w_vertical_sb, XmNtroughColor, &sb_trough_color, NULL);

	XtVaSetValues(widget, XmNbackground, sb_trough_color, NULL);
}


/*
 * set_bg_to_sb_trough_color - This strange routine sets the background
 *                             color of "widget" to the trough color of
 *                             the scrollbar widget "w_sb".  The purpose
 *                             of this routine is to enhance the appearance
 *                             of certain types of widgets (e.g., text
 *                             and textfield widgets).
 */
set_bg_to_sb_trough_color(widget, w_sb)
Widget widget;
Widget w_sb;
{
	Pixel sb_trough_color;

	/* Does the user want color enhancement? */
	if (!enhance_colors)
		return;

	/* Can the display handle colors? */
	if (!color_display())
		return;

	XtVaGetValues(w_sb, XmNtroughColor, &sb_trough_color, NULL);
	XtVaSetValues(widget, XmNbackground, sb_trough_color, NULL);
}


/*
 * fix_horizontal_text_sb_color - Kludge to correct problem on IBM RS6000
 *                                with color of horizontal scrollbar in
 *                                scrollable text widget.
 */
fix_horizontal_text_sb_color(widget)
Widget widget;
{
    Widget w_horizontal_sb;
    Pixel text_bg_color;

    XtVaGetValues(XtParent(widget), XmNhorizontalScrollBar, &w_horizontal_sb,
        NULL);
    XtVaGetValues(widget, XmNbackground, &text_bg_color, NULL);
    XtVaSetValues(w_horizontal_sb, XmNforeground, text_bg_color, NULL);
}


/*
 * fix_vertical_text_sb_color - Kludge to correct problem on IBM RS6000
 *                              with color of vertical scrollbar in
 *                              scrollable text widget.
 */
fix_vertical_text_sb_color(widget)
Widget widget;
{
    Widget w_vertical_sb;
    Pixel text_bg_color;

    XtVaGetValues(XtParent(widget), XmNverticalScrollBar, &w_vertical_sb,
        NULL);
    XtVaGetValues(widget, XmNbackground, &text_bg_color, NULL);
    XtVaSetValues(w_vertical_sb, XmNforeground, text_bg_color, NULL);
}

