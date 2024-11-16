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

#include <X11/Intrinsic.h>
#include "xftp.h"
#include "str.h"

struct server_st {
	char *pattern;
	int system;
	int server;
};

struct name_st {
	int id;
	char *name;
};

static struct server_st open_info[] = {
	{ "MultiNet FTP Server Process",	SYS_VMS,	SERVER_VMS_MULTINET      },
	{ "VersaTerm FTP Server",			SYS_MAC,    SERVER_MAC_VERSATERM     },
	{ "Macintosh Resident FTP server",	SYS_MAC,	SERVER_MAC_NCSATELNET    },
	{ "Peter's Macintosh FTP daemon",	SYS_MAC,	SERVER_MAC_FTPD          },
	{ "WinQVT",							SYS_DOS,	SERVER_DOS_WINQVT        },
	{ "Windows NT FTP Server",			SYS_NT,		SERVER_NT                },
	{ "NSL UniTree",					SYS_UNIX,	SERVER_UNIX_NSL_UNITREE  },
	{ "NSL UNITREE",					SYS_UNIX,	SERVER_UNIX_NSL_UNITREE  },
	{ "UniTree",						SYS_UNIX,	SERVER_UNIX_UNITREE      },
	{ "UNITREE",						SYS_UNIX,	SERVER_UNIX_UNITREE      }
};

static struct server_st syst_info[] = {
	{ "UniTree",						SYS_UNIX,	SERVER_UNKNOWN       },
	{ "UNITREE",						SYS_UNIX,	SERVER_UNKNOWN       },
	{ "UNIX",							SYS_UNIX,	SERVER_UNKNOWN		 },
	{ "215 VMS MultiNet",				SYS_VMS,	SERVER_VMS_MULTINET  },
	{ "215 VMS",						SYS_VMS,	SERVER_VMS_MULTINET  },
	{ "200 VMS",						SYS_VMS,	SERVER_VMS_3_0       },
	{ "215 MACOS Peter's Server",		SYS_MAC,	SERVER_MAC_FTPD      },
	{ "Windows_NT",						SYS_NT,		SERVER_NT            }
};

static struct name_st systems[] = {
	{ SYS_UNIX,	"UNIX"	},
	{ SYS_VMS,	"VMS"	},
	{ SYS_MAC,	"MAC"	},
	{ SYS_DOS,	"DOS"	},
	{ SYS_NT,	"NT"	}
};

static struct name_st servers[] = {
	{ SERVER_UNKNOWN,			"UNKNOWN"			},
	{ SERVER_VMS_3_0,			"VMS_3_0"			},
	{ SERVER_VMS_MULTINET,		"VMS_MULTINET"		},
	{ SERVER_MAC_VERSATERM,		"MAC_VERSATERM"		},
	{ SERVER_MAC_NCSATELNET,	"MAC_NCSATELNET"	},
	{ SERVER_MAC_FTPD,			"MAC_FTPD"			},
	{ SERVER_DOS_WINQVT,		"DOS_WINQVT"		},
	{ SERVER_NT,				"NT"				},
	{ SERVER_UNIX_UNITREE,		"UNIX_UNITREE"      },
	{ SERVER_UNIX_NSL_UNITREE,	"UNIX_NSL_UNITREE"  }
};

static int nopen_info = XtNumber(open_info);
static int nsyst_info = XtNumber(syst_info);
static int nsystems = XtNumber(systems);
static int nservers = XtNumber(servers);

extern struct st_host_info hinfo[];


/*
 * determine_server_from_open - Try to determine the type of system and server
 *                              the remote host has from the first "reply"
 *                              returned by the FTP server.  If successful,
 *                              hinfo[host].system and hinfo[host].server
 *                              are set; otherwise a no-op.
 */
determine_server_from_open(host, reply)
int host;
char *reply;
{
	int i;

	for (i=0; i<nopen_info; i++)
		if (strstr(reply, open_info[i].pattern)) {
			hinfo[host].system = open_info[i].system;
			hinfo[host].server = open_info[i].server;
			return;
		}
}


/*
 * determine_server_from_syst - Try to determine the type of system and server
 *                              the remote host has from the server's "reply"
 *                              to the SYST command.  If successful,
 *                              hinfo[host].system and hinfo[host].server
 *                              are set; otherwise a no-op.
 */
determine_server_from_syst(host, reply)
int host;
char *reply;
{
	int i;

	for (i=0; i<nsyst_info; i++)
		if (strstr(reply, syst_info[i].pattern)) {
			if ((syst_info[i].system != hinfo[host].system)
					|| (syst_info[i].server != SERVER_UNKNOWN))
				hinfo[host].server = syst_info[i].server;
			hinfo[host].system = syst_info[i].system;
			return;
		}
}


/*
 * system_name - Returns a pointer to a string that contains the name of
 *               an operating system that corresponds to "id", a system
 *               identifier.  The return value points to an internal buffer.
 */
char *
system_name(id)
int id;
{
	int i;

	for (i=0; i<nsystems; i++)
		if (id == systems[i].id)
			return systems[i].name;
	
	/* Sanity check */
	fatal_error("Bug in system_name()");
	return NULL;   /* Needed to keep gcc compiler happy */
}


/*
 * server_name - Returns a pointer to a string that contains the name of
 *               an FTP server that corresponds to "id", a server identifier.
 *               The return value points to an internal buffer.
 */
char *
server_name(id)
int id;
{
	int i;

	for (i=0; i<nservers; i++)
		if (id == servers[i].id)
			return servers[i].name;
	
	/* Sanity check */
	fatal_error("Bug in server_name()");
	return NULL;   /* Needed to keep gcc compiler happy */
}

