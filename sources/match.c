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


/*
 * match - Returns True if "string" matches "pattern".  The usual C shell
 *         wildcard conventions are followed (including the case in which
 *         the first character of "pattern" must be a period in order for
 *         a match to occur when the first character of "string" is also 
 *         a period.
 */
match(string, pattern)
char *string;
char *pattern;
{
	if ((string[0] == '.') && (pattern[0] != '.'))
		return False;

	return simple_match(string, pattern);
}


/*
 * simple_match - Returns True if "string" matches "pattern".  The usual
 *                C shell wildcard conventions are observed (except for
 *                the rule that if "string" begins with a period, so does
 *                "pattern" if a match is to occur).
 */
simple_match(string, pattern)
char *string;
char *pattern;
{
	char *s = string;
	char *p = pattern;
	int submatch;

	while (*p)
		switch (*p) {
		case '[':
			p++;
			submatch = False;
			while (*p)
				if (*p == ']') {
					p++;
					break;
				} else if (*p == '-')
					p++;
				else if (*(p+1) == '-') {
					if ((*(p+2) == '-') || (*(p+2) == ']') || (*(p+2) == '\0'))
						p +=2;
					else {
						if (((*s >= *p) && (*s <= *(p+2)))
								|| ((*s >= *(p+2)) && (*s <= *p)))
							submatch = True;
						p += 3;
					}
				} else if (*p == *s) {
					submatch = True;
					p++;
				} else
					p++;
			if (!submatch)
				return False;
			s++;
			break;
		case '?':
			if (*s == '\0')
				return False;
			s++;
			p++;
			break;
		case '*':
			while (1) {
				if (simple_match(s, p+1))
					return True;
				if (*s == '\0')
					break;
				s++;
			}
			p++;
			return False;
		default:
			if (*s++ != *p++)
				return False;
		}
	if (*s)
		return False;
	else
		return True;
}
