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
#include "list.h"


/*
 * add_to_linked_list - Add a link containing the specified string into
 *                      the linked list pointed to by "head".
 */
add_to_linked_list(head, string)
struct entry_link **head;
char *string;
{
	struct entry_link *ptr;

	ptr = XtNew(struct entry_link);
	ptr->entry = XtNewString(string);
	ptr->next = *head;
	*head = ptr;
}


/*
 * release_linked_list - Free all links of the linked list pointed to by
 *                       "head".  Sets "head" to NULL.
 */
release_linked_list(head)
struct entry_link **head;
 {
	struct entry_link *ptr;

	while (*head) {
		ptr = *head;
		*head = (*head)->next;
		XtFree(ptr->entry);
		XtFree((char *)ptr);
	}
 }


/*
 * create_array_list - Create an array of pointers to strings from the
 *                     linked list pointed to by "head".  Returns a
 *                     pointer to a struct containing the array and
 *                     entry count.  This function releases the linked
 *                     list and sets "head" to NULL.
 */
struct sl_struct *
create_array_list(head)
struct entry_link **head;
{
	struct sl_struct *list;
	struct entry_link *ptr;
	int nentries = 0;
	int indx = 0;

	/* Allocate structure */
	list = XtNew(struct sl_struct);

	/* Count the number of entries */
	ptr = *head;
	while (ptr) {
		nentries++;
		ptr = ptr->next;
	}
    list->nentries = nentries;

    /* Allocate array of pointers. */
    list->entries = (char **)XtMalloc(sizeof(char *)*nentries);

    /* Move entries names into arrays from linked list */
    indx = nentries-1;
    while (*head) {
        list->entries[indx--] = (*head)->entry;
        ptr = *head;
        *head = (*head)->next;
        XtFree((char *)ptr);
    }

	return list;
}


/*
 * create_null_array_list - Create an empty array of pointers to strings.
 *                          Returns a pointer to a struct containing the
 *                          array and entry count.
 */
struct sl_struct *
create_null_array_list()
{
    struct sl_struct *list;

    list = XtNew(struct sl_struct);
    list->nentries = 0;
    list->entries = (char **)XtMalloc(0);
    return list;
}


/*
 * release_array_list - Release the memory occupied by the structs returned
 *                      by create_array_list() and create_null_array_list().
 */
release_array_list(list)
struct sl_struct *list;
{
    int i;

    for (i=0; i<list->nentries; i++)
        XtFree(list->entries[i]);

    XtFree((char *)list->entries);
    XtFree((char *)list);
}


/*
 * add_to_array_list - Adds string to end of an array of strings.
 */
add_to_array_list(list, string)
struct sl_struct **list;
char *string;
{
	struct sl_struct *new_list;
	int i;

	/* Allocate new structure */
	new_list = XtNew(struct sl_struct);

	/* Allocate new array of pointers. */
	new_list->nentries = (*list)->nentries+1;
	new_list->entries = (char **)XtMalloc(sizeof(char *)*(new_list->nentries));

	/* Move strings from old list to new */
	for (i=0; i<(*list)->nentries; i++)
		new_list->entries[i] = (*list)->entries[i];

	/* Add new entry to the end */
	new_list->entries[new_list->nentries-1] = XtNewString(string);

	/* Delete old list */
	XtFree((char *)(*list)->entries);
	XtFree((char *)(*list));

	/* Return new list */
	*list = new_list;
}


/*
 * delete_from_array_list - Delete the "n"th string from an array of strings.
 */
delete_from_array_list(list, n)
struct sl_struct **list;
int n;
{
	int i;

	/* Sanity check */
	if (n > (*list)->nentries)
		fatal_error("Trouble in add_to_array_list()");

	/* Delete entry */
	XtFree((*list)->entries[n]);

	/* Move other entries up */
	for (i=n; i<(*list)->nentries-1; i++)
		(*list)->entries[i] = (*list)->entries[i+1];

	/* Decrement count */
	(*list)->nentries--;
}


/*
 * duplicate_array_list - Return a pointer to a copy array list.
 */
struct sl_struct *
duplicate_array_list(list)
struct sl_struct *list;
{
	struct sl_struct *new_list;
	int i;

	/* Allocate structure */
	new_list = XtNew(struct sl_struct);

    /* Allocate array of pointers. */
    new_list->entries = (char **)XtMalloc(sizeof(char *)*list->nentries);

	/* Copy strings */
	for (i=0; i<list->nentries; i++)
		new_list->entries[i] = XtNewString(list->entries[i]);

	/* Set count */
	new_list->nentries = list->nentries;

	return new_list;
}

