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

#define FALSE    0
#define TRUE     1

#define MAXSTACK 20
#define CUTOFF   3       /* set to 3 when debugged */


/*
 *  quicksort(s, n, compare) - Sort the n strings pointed to by the
 *                             array of character pointers s.  The
 *                             function compare is used to perform the
 *                             comparisons:  compare(s1, s2) must return
 *                             the values -1, 0, and 1 that correspond
 *                             to the relations s1<s2, s1==s2, and 
 *                             s1>s2.
 *                             
 *  This algorithm is a variation of the partition-exchange sort
 *  presented on pp. 116-117 of "Sorting and Searching" by Knuth.
 *  The main difference is here we eliminate the use of artificial
 *  keys (that is, we do not assume that a "minus infinity" precedes
 *  the first term and that a "plus infinity" follows the last term).
 *  
 *  If a partition is no larger than a fixed length, CUTOFF, it is
 *  sorted using a straight insertion sort.  Hence, if CUTOFF is set
 *  to zero, then the algorithm becomes a pure quicksort; if CUTOFF
 *  is set to "plus infinity", the algorithm becomes a pure insertion
 *  sort.  Both of these pure sorts run more slowly than the hybrid
 *  algorithm implemented here.  CUTOFF has been set to a value that
 *  optimally balances use of the two sort algorithms for large
 *  arrays.
 */
quicksort(s, n, compare)
char *s[];
int n;
int (*compare)();
{
    struct {
        int left;
        int right;
    } stack[MAXSTACK+1];

    char *temp;
    char *pivot;
    int stackptr = 0;
    int l = 0;
    int r = n-1;
    int i;
    int j;
    int done = FALSE;
    int flag;
    
    while (!done) 
        if (r-l <= CUTOFF) {
            for (j=l+1; j<=r; j++) {
                i = j-1;
                temp = s[j];
                flag = TRUE;
                while (flag)
                    if (i >= l)
                        if (compare(temp, s[i]) < 0) {
                            s[i+1] = s[i];
                            i--;
                        } else
                            flag = FALSE;
                     else
                        flag = FALSE;
                s[i+1] = temp;
            }
            if (stackptr > 0) {
                l = stack[stackptr].left;
                r = stack[stackptr].right;
                stackptr = stackptr-1;
            } else
                done = TRUE;
        } else {
            i = l;
            j = r;
            pivot = s[l];
lab1:       while (i < j && compare(s[j], pivot) >= 0)
                j--;
            if (i < j) {
                s[i++] = s[j];
                while (i < j && compare(s[i], pivot) <= 0)
                    i++;
                if (i < j) {
                    s[j--] = s[i];
                    goto lab1;
                }
            }
            s[i] = pivot;
            stackptr = stackptr+1;
            if (r-i >= i-l) {
                stack[stackptr].left = i+1;
                stack[stackptr].right = r;
                r = i-1;
            } else {
                stack[stackptr].left = l;
                stack[stackptr].right = i-1;
                l = i+1;
            }
        }
}
