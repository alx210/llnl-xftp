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
#include <Xm/Xm.h>
#include "xftp.h"
#include "list.h"
#include "str.h"

#define CACHEFILE ".xftpcache"
#define MAXCLINE  200

struct cache_link {
	struct cache_link *next;
	struct cache_link *prev;
	char *value;
};

struct cache_st {
	struct cache_link *head;
	struct cache_link *tail;
	int count;
};

struct host_link {
	struct host_link *next;
	struct host_link *prev;
	char *host_name;
	struct cache_st cache[NUMCACHES];
};

struct hostuser_link {
	struct hostuser_link *next;
	struct hostuser_link *prev;
	char *hostuser;
};

static struct host_link *host_head = NULL;
static struct host_link *host_tail = NULL;
static int host_count = 0;
static char *cache_file;

static char *cache_label[] = {
	"Dir:",
	"Wildcard:",
	"Quote:",
	"User:"
};

extern struct st_host_info hinfo[];
extern int max_cache_hosts;
extern int max_cache[];
extern int add_to_cache_ok;


/*
 * construct_cache_filename - Form preferences file name by concatenating
 *                            .xftprc to $HOME.
 */
construct_cache_filename()
{
    char *homedir = getenv("HOME");

    cache_file = XtMalloc(strlen(homedir)+strlen(CACHEFILE)+2);
    strcpy(cache_file, homedir);
    strcat(cache_file, "/");
    strcat(cache_file, CACHEFILE);
}


/*
 * save_cache - Write current cache entries to .xftpcache.
 */
save_cache()
{
	static FILE *fp;
	struct host_link *hptr;
	struct cache_link *cptr;
	int i;

	if ((fp = fopen(cache_file, "w")) == NULL) {
		warning_error("Unable to save cache.");
		return;
	}

	/* Save caches */
	hptr = host_tail;
	while (hptr) {
		fprintf(fp, "Host: %s\n", hptr->host_name);
		for (i=0; i<NUMCACHES; i++) {
			cptr = hptr->cache[i].tail;
			while (cptr) {
				fprintf(fp, "   %s %s\n", cache_label[i],  cptr->value);
				cptr = cptr->prev;
			}
		}
		hptr = hptr->prev;
	}

	fclose(fp);
}


/*
 * read_cache_from_file - Initialize cache entries with info from .xftpcache.
 */
read_cache_from_file()
{
	FILE *fp;
	char cache_line[MAXCLINE+1];
	int len;
	char *id;
	char *value;
	char *host_name = NULL;
	int i;

	/* Verify that cache is empty (Sanity check) */
	if (host_head != NULL || host_tail != NULL || host_count != 0)
		fatal_error("Bug in read_cache_from_file()");

	/* Try to open cache file */
	if ((fp = fopen(cache_file, "r")) == NULL)
		return;

	/* Process cache info */
	while (fgets(cache_line, MAXCLINE+1, fp) != NULL) {
		len = strlen(cache_line);
		if (cache_line[len-1] != '\n')
			continue;
		if ((id = strtok(cache_line, "\t\n ")) == NULL)
			continue;
		if ((value = strtok(NULL, "\n")) == NULL)
			continue;
		if (strcmp(id, "Host:") == 0) {
			if (host_name)
				XtFree(host_name);
	        host_name = XtNewString(value);
		} else 
			for (i=0; i<NUMCACHES; i++)
				if (strcmp(id, cache_label[i]) == 0) {
					if (host_name)
						add_to_cache(i, host_name, value);
					break;
				}
	}

	fclose(fp);
	if (host_name)
		XtFree(host_name);
}


/*
 * add_to_cache - Add the specified host and value to the cache specified
 *                with cache_id (DIRECTORY, WILDCARD, QUOTE or USER).
 */
add_to_cache(cache_id, host_name, value)
int cache_id;
char *host_name;
char *value;
{
	struct host_link *hptr;
	struct cache_link *cptr;
	struct cache_link *next;
	int i;
	int found;

	/* Search for host name */
	found = False;
	if (host_head) {
		hptr = host_head;
		while (hptr) {
			if (strcmp(hptr->host_name, host_name) == 0) {
				found = True;
				break;
			}
			hptr = hptr->next;
		}
	}

	/* If host name found, move to head of list, else create entry for host */
	if (found) {
		if (hptr != host_head) {
			hptr->prev->next = hptr->next;
			if (hptr == host_tail)
				host_tail = hptr->prev;
			else
				hptr->next->prev = hptr->prev;
			hptr->prev = NULL;
			hptr->next = host_head;
			host_head->prev = hptr;
			host_head = hptr;
		}
	} else {
		hptr = XtNew(struct host_link);
		for (i=0; i<NUMCACHES; i++) {
			hptr->cache[i].head = NULL;
			hptr->cache[i].tail = NULL;
			hptr->cache[i].count = 0;
		}
		hptr->host_name = XtNewString(host_name);
		hptr->next = host_head;
		hptr->prev = NULL;
		if (host_head)
			host_head->prev = hptr;
		host_head = hptr;
		if (host_tail == NULL)
			host_tail = hptr;
		host_count++;
	}

	/* If necessary, reduce number of host entries to maximum allowable */
	while (host_count > max_cache_hosts) {
		hptr = host_tail;
		host_tail = hptr->prev;
		XtFree(hptr->host_name);
		for (i=0; i<NUMCACHES; i++) {
			cptr = hptr->cache[i].head;
			while (cptr) {
				XtFree(cptr->value);
				next = cptr->next;
				XtFree((char *)cptr);
				cptr = next;
			}
		}
		XtFree((char *)hptr);
		host_tail->next = NULL;
		host_count--;
	}

	/* Search the cache for the value */
	found = False;
	if (host_head->cache[cache_id].head) {
		cptr = host_head->cache[cache_id].head;
		while (cptr) {
			if (strcmp(cptr->value, value) == 0) {
				found = True;
				break;
			}
			cptr = cptr->next;
		}
	}

	/* If value found, move to head of list, else create entry for value */
	if (found) {
		if (cptr != host_head->cache[cache_id].head) {
			cptr->prev->next = cptr->next;
			if (cptr == host_head->cache[cache_id].tail)
				host_head->cache[cache_id].tail = cptr->prev;
			else
				cptr->next->prev = cptr->prev;
			cptr->prev = NULL;
			cptr->next = host_head->cache[cache_id].head;
			host_head->cache[cache_id].head->prev = cptr;
			host_head->cache[cache_id].head = cptr;
		}
	} else {
		cptr = XtNew(struct cache_link);
		cptr->value = XtNewString(value);
		cptr->next = host_head->cache[cache_id].head;
		cptr->prev = NULL;
		if (host_head->cache[cache_id].head)
			host_head->cache[cache_id].head->prev = cptr;
		host_head->cache[cache_id].head = cptr;
		if (host_head->cache[cache_id].tail == NULL)
			host_head->cache[cache_id].tail = cptr;
		host_head->cache[cache_id].count++;
	}

	/* If necessary, reduce number of values in cache to maximum allowable */
	while (host_head->cache[cache_id].count > max_cache[cache_id]) {
		cptr = host_head->cache[cache_id].tail;
		host_head->cache[cache_id].tail = cptr->prev;
		XtFree(cptr->value);
		XtFree((char *)cptr);
		host_head->cache[cache_id].tail->next = NULL;
		host_head->cache[cache_id].count--;
	}
}


/*
 * retrieve_cache - Sets "list" to point to a struct containing the values
 *                  cached for "hostname" and cache (cache_id can
 *                  have the values DIRECTORY, WILDCARD, QUOTE or USER).
 *                  Memory pointed to by "list" should be released by
 *                  calling release_array_list().  If the "sort"
 *                  flag is set, then the list is returned sorted,
 *                  otherwise it is returned in order of last use.
 */
retrieve_cache(cache_id, hostname, list, sort)
int cache_id;
char *hostname;
struct sl_struct **list;
int sort;
{
	char **values;
	int indx;
	struct host_link *hptr;
	struct cache_link *cptr;

    /* Search for host name */
    hptr = host_head;
    while (hptr) {
        if (strcmp(hptr->host_name, hostname) == 0)
            break;
		hptr = hptr->next;
	}

	/* If host name not found, return empty list */
	if (hptr == NULL) {
		*list = XtNew(struct sl_struct);
		(*list)->nentries = 0;
		(*list)->entries = (char **)XtMalloc(0);
		return;
	}

	/* Host name found.  Convert linked list of values into array */
	values = (char **)XtMalloc(sizeof(char *)*hptr->cache[cache_id].count);
	cptr = hptr->cache[cache_id].head;
	indx = 0;
	while (cptr) {
		values[indx] = XtNewString(cptr->value);
		indx++;
		cptr = cptr->next;
	}

	/* Sanity check */
	if (indx != hptr->cache[cache_id].count)
		fatal_error("Bug in retrieve_cache()");

	/* Sort entries */
	if (sort)
		quicksort(values, hptr->cache[cache_id].count, strcmp);

	/* Return struct */
	*list = XtNew(struct sl_struct);
	(*list)->nentries = hptr->cache[cache_id].count;
	(*list)->entries = values;
}


/*
 * update_dir_cache - Add the current working directory to the cache of
 *                    recently-referenced directory paths for the specifed 
 *                    host.
 */
update_dir_cache(host)
int host;
{
	if (add_to_cache_ok)
		add_to_cache(DIRECTORY, hinfo[host].hostname, hinfo[host].wd);
}


/*
 * retrieve_hostuser_cache - Sets "list" to point to a struct containing the
 *                           expressions in the hostname/username cache.
 *                           Memory pointed to by "list" should be released by
 *                           calling release_array_list().  If the "sort"
 *                           calling release_array_list().  If the "sort"
 *                           flag is set, then the list is returned sorted,
 *                           otherwise it is returned in order of last use.
 */
retrieve_hostuser_cache(list, sort)
struct sl_struct **list;
int sort;
{
	char **hostusers;
	int hostuser_count;
	struct host_link *hptr;
	struct cache_link *cptr;
	int indx;

	/* Count number of host user pairs */
	hostuser_count = 0;
	hptr = host_head;
	while (hptr) {
		cptr = hptr->cache[USER].head;
		while (cptr) {
			hostuser_count++;
			cptr = cptr->next;
		}
		hptr = hptr->next;
	}

	/* Convert linked list of hostname/username pairs into array */
	hostusers = (char **)XtMalloc(sizeof(char *)*hostuser_count);
	indx = 0;
	hptr = host_head;
	while (hptr) {
		cptr = hptr->cache[USER].head;
		while (cptr) {
			hostusers[indx] = XtMalloc(strlen(hptr->host_name)+
				strlen(cptr->value)+6);
			sprintf(hostusers[indx], "%s   (%s)", hptr->host_name,cptr->value);
			indx++;
			cptr = cptr->next;
		}
		hptr = hptr->next;
	}

	/* Sanity check */
	if (indx != hostuser_count)
		fatal_error("Bug in retrieve_hostuser_cache()");

	/* Sort entries */
	if (sort)
		quicksort(hostusers, hostuser_count, strcmp);

	/* Return struct */
	*list = XtNew(struct sl_struct);
	(*list)->nentries = hostuser_count;
	(*list)->entries = hostusers;
}

