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

#ifndef __pulldown_h
#define __pulldown_h

#include <Xm/Xm.h>

typedef struct _menu_item {
    char        *label;         /* the label for the item */
    WidgetClass *class;         /* pushbutton, label, separator... */
    char         mnemonic;      /* mnemonic; NULL if none */
    char        *accelerator;   /* accelerator; NULL if none */
    char        *accel_text;    /* to be converted to compound string */
    void       (*callback)();   /* routine to call; NULL if none */
    struct _menu_item *subitems; /* pullright menu items, if not NULL */
	Widget      *widget;        /* widget for menu item */
} MenuItem;

#endif  /* __pulldown_h */

