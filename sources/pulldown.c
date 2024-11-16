/*
 * This source module is derived from an example program from Chapter 15 of
 * Volume Six A of the O'Reilly & Associates, Inc. book, "Motif Programming
 * Manual", by Dan Heller & Paula M. Ferguson.  Here is the required copy-
 * right notice:
 *
 *      Written by Dan Heller and Paula Ferguson.  
 *      Copyright 1994, O'Reilly & Associates, Inc.
 *      Permission to use, copy, and modify this program without
 *      restriction is hereby granted, as long as this copyright
 *      notice appears in each copy of the program source code.
 *      This program is freely distributable without licensing fees and
 *      is provided without guarantee or warrantee expressed or implied.
 *      This program is -not- in the public domain.
 */

#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include "pulldown.h"

/* Pulldown menus are built from cascade buttons, so this function
 * also includes pullright menus.  Create the menu, the cascade button
 * that owns the menu, and then the submenu items.
 */
Widget
BuildPulldownMenu(parent, menu_title, menu_mnemonic, items, callback_data)
Widget parent;
char *menu_title;
KeySym menu_mnemonic;
MenuItem *items;
XtPointer callback_data;
{
    Widget PullDown, cascade, widget;
    int i;
    XmString str;

    PullDown = XmCreatePulldownMenu(parent, "pulldown", NULL, 0);

    str = XmStringCreateSimple(menu_title);
    cascade = XtVaCreateManagedWidget(menu_title,
        xmCascadeButtonWidgetClass, parent,
        XmNsubMenuId,   PullDown,
        XmNlabelString, str,
        XmNmnemonic,    menu_mnemonic,
        NULL);
    XmStringFree(str);

    /* Now add the menu items */
    for (i = 0; items[i].label != NULL; i++) {
        /* If subitems exist, create the pull-right menu by calling this
         * function recursively.  Since the function returns a cascade
         * button, the widget returned is used..
         */
        if (items[i].subitems)
            widget = BuildPulldownMenu(PullDown,
                items[i].label, items[i].mnemonic, items[i].subitems, NULL);
        else
            widget = XtVaCreateManagedWidget(items[i].label,
                *items[i].class, PullDown,
                NULL);
		if (items[i].widget != NULL)
			*(items[i].widget) = widget;
        /* Whether the item is a real item or a cascade button with a
         * menu, it can still have a mnemonic.
         */
        if (items[i].mnemonic != '\0')
            XtVaSetValues(widget, XmNmnemonic, items[i].mnemonic, NULL);
        /* any item can have an accelerator, except cascade menus. But,
         * we don't worry about that; we know better in our declarations.
         */
        if (items[i].accelerator) {
            str = XmStringCreateSimple(items[i].accel_text);
            XtVaSetValues(widget,
                XmNaccelerator, items[i].accelerator,
                XmNacceleratorText, str,
                NULL);
            XmStringFree(str);
        }
        /* again, anyone can have a callback -- however, this is an
         * activate-callback.  This may not be appropriate for all items.
         */
        if (items[i].callback)
            XtAddCallback(widget, XmNactivateCallback,
                items[i].callback, callback_data);
    }
    return cascade;
}
