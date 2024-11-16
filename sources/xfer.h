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

#ifndef __xfer_h
#define __xfer_h

#include <stdio.h>
#include "list.h"

#ifdef HIPPI
#define XFER_SYS_TCPBUFLEN   (378*1024)
#define XFER_USER_TCPBUFLEN  (336*1024)
#define XFER_TCPWINSHIFT     4
#else
#define XFER_SYS_TCPBUFLEN   (32*1024)
#define XFER_USER_TCPBUFLEN  (32*1024)
#endif
#define XFER_FILEBUFLEN      XFER_USER_TCPBUFLEN

#define MAXLEVELS  20

#define MAXSTAGES    5
#define STAGE_DELTA  60    /* Adjust after testing */

#define DELTA 5

#define COPY   0
#define RCOPY  1
#define VIEW   2

#define TYPE_UNKNOWN  0
#define TYPE_DIR      1
#define TYPE_FILE     2

#define NOT_PROCESSED     0
#define CREATED           1
#define UNABLE_TO_CREATE  2
#define ALREADY_EXISTS    3

#define SOURCE  0
#define SINK    1

#define ABOVE_START_NODE  0
#define AT_START_NODE     1
#define BELOW_START_NODE  2

#define STATUS_PASS1  "Examining selected entries on source host"
#define STATUS_PASS2  "Examining sink host for possible conflicts"
#define STATUS_PASS3  "Creating directories on sink host, as needed"
#define STATUS_XFER   "File being transferred"
#define STATUS_WAIT   "Waiting for Unitree stage to complete"
#define STATUS_STAGE  "Unitree file being staged"

struct staged_file_st {
	struct forest_node *node;
	long time;
};

struct xfer_ctrl_block {
	int operation;                        /* Requires initialization */
	int mode;							  /* Requires initialization */
	int level;							  /* Requires initialization */
	int state;							  /* Requires initialization */
	int nretries;                         /* Requires initialization */
	int src_host;						  /* Requires initialization */
	int snk_host;						  /* Requires initialization */
	int src_host_type;                    /* Requires initialization */
	int snk_host_type;                    /* Requires initialization */
	int src_host_system;                  /* Requires initialization */
	int snk_host_system;                  /* Requires initialization */
	int src_host_server;				  /* Requires initialization */
	char *src_host_wd;                    /* Requires initialization */
	char *snk_host_wd;                    /* Requires initialization */
	int ignore_create_dir_errors;         /* Requires initialization */
	struct forest_node *forest;           /* Requires initialization */
	struct forest_node *s0ptr;            /* Requires initialization */
	struct forest_node *node[MAXLEVELS];  /* Requires initialization */
	int nstages;                          /* Requires initialization */
	int more_files;                       /* Requires initialization */
	struct forest_node *current_node;
	struct staged_file_st staged_file[MAXSTAGES];
	int data_fd;
	int src_file_fd;
	int snk_file_fd;
	FILE *snk_fp;
	long file_len;
	long file_index;
	int substate;
};

struct del_ctrl_block {
	int recursive;						/* Requires initialization */
	int level;							/* Requires initialization */
	int host;							/* Requires initialization */
	struct entry_link *head[MAXLEVELS]; /* Requires initialization */
	char *rel_path;
	char *full_path;
};

struct forest_node {
	char *entry;
	struct forest_node *next;
	struct forest_node *parent;
	struct forest_node *first_child;
	int type;
	int status;
	int position;
};

#endif  /* __xfer_h */

