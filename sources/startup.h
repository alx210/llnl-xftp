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

#define author_width 84
#define author_height 21
static unsigned char author_bits[] = {
   0x00, 0x00, 0x04, 0x01, 0x89, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x04, 0x01, 0x88, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xa8, 0xb4, 0xdd, 0x99, 0x83, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa8,
   0x0c, 0x89, 0xa4, 0x84, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x04,
   0x89, 0xbc, 0x84, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x04, 0x89,
   0x84, 0x84, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x04, 0x11, 0x99,
   0x84, 0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x67, 0x00, 0x00, 0x0e, 0x00, 0xe0, 0x03, 0x00, 0x0c, 0xc3, 0x00, 0x67,
   0x00, 0x00, 0x0c, 0x00, 0x30, 0x06, 0x00, 0x00, 0xc3, 0x00, 0x6f, 0x3c,
   0x3c, 0x0c, 0x0f, 0x30, 0xe0, 0x3b, 0x8e, 0xc7, 0x07, 0x6f, 0x66, 0x66,
   0x8c, 0x19, 0x30, 0xe0, 0x6e, 0x0c, 0xc3, 0x0d, 0x7b, 0x66, 0x7c, 0x8c,
   0x19, 0xe0, 0x63, 0x66, 0x0c, 0xc3, 0x0c, 0x7b, 0x7e, 0x66, 0x8c, 0x1f,
   0x00, 0x66, 0x66, 0x0c, 0xc3, 0x0c, 0x73, 0x06, 0x66, 0x8c, 0x01, 0x00,
   0x66, 0x66, 0x0c, 0xc3, 0x0c, 0x73, 0x66, 0x66, 0x8c, 0x19, 0x30, 0x66,
   0x66, 0x0c, 0xc3, 0x0c, 0x63, 0x3c, 0x7c, 0x0c, 0x0f, 0xe0, 0x63, 0x66,
   0x0c, 0xce, 0x0c};


#define lllcursor_width 16
#define lllcursor_height 16
#define lllcursor_x_hot 15
#define lllcursor_y_hot 7
static unsigned char lllcursor_bits[] = {
   0x49, 0xfe, 0x49, 0xfe, 0x49, 0xfe, 0x49, 0xfe, 0x49, 0xfe, 0x49, 0xfe,
   0x49, 0xfe, 0x49, 0x00, 0x49, 0x80, 0x89, 0xff, 0x11, 0xe0, 0x33, 0xf0,
   0xc3, 0xff, 0x07, 0xfc, 0x1f, 0xfe, 0xff, 0xff};


#define llnllong_width 117
#define llnllong_height 23
static unsigned char llnllong_bits[] = {
   0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x06, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x33, 0xcc, 0xe7, 0x78, 0x1c,
   0x07, 0x0c, 0x67, 0xe6, 0xf8, 0xfc, 0x71, 0x7c, 0x0e, 0xc3, 0x36, 0xcc,
   0xb1, 0xd9, 0xb6, 0x0d, 0x0c, 0x66, 0xb6, 0x39, 0x6c, 0xdb, 0x1c, 0x1b,
   0x83, 0xe7, 0xc7, 0xf0, 0xd9, 0x86, 0x0f, 0x0c, 0xc6, 0xf3, 0x19, 0x6c,
   0xdb, 0x0c, 0x1f, 0xc3, 0xe6, 0xc7, 0x30, 0xd8, 0x86, 0x01, 0x0c, 0xc6,
   0x33, 0x18, 0x6c, 0xdb, 0x0c, 0x03, 0xc3, 0xc6, 0xc3, 0xb0, 0xd9, 0xb6,
   0x0d, 0x0c, 0x86, 0xb1, 0x19, 0x6c, 0xdb, 0x0c, 0x1b, 0x9f, 0xc7, 0xc3,
   0xe0, 0xd8, 0x1c, 0x07, 0x7c, 0x86, 0xe1, 0x18, 0x6c, 0x73, 0x0c, 0x0e,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xcc, 0x00, 0x63, 0x00, 0x00, 0x70, 0xc0, 0x00, 0x18,
   0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xdc, 0x00, 0x03, 0x00, 0x00, 0x60,
   0xc0, 0x00, 0x18, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xdc, 0x9c, 0x77,
   0x1c, 0x8f, 0x63, 0xc0, 0xe0, 0x78, 0x1c, 0x9f, 0xf3, 0x1c, 0x9f, 0x0d,
   0xfc, 0x36, 0x63, 0x36, 0xdb, 0x66, 0xc0, 0xb0, 0xd9, 0x36, 0xc7, 0x66,
   0x36, 0x87, 0x0d, 0xfc, 0x3c, 0x63, 0x36, 0x9b, 0x67, 0xc0, 0xe0, 0xd9,
   0x36, 0x83, 0x67, 0x36, 0x83, 0x0d, 0xec, 0x36, 0x63, 0x36, 0xdb, 0x66,
   0xc0, 0xb0, 0xd9, 0x36, 0xc3, 0x66, 0x36, 0x83, 0x0d, 0xec, 0x36, 0x63,
   0x36, 0xdb, 0x66, 0xc0, 0xb0, 0xd9, 0x36, 0xc3, 0x66, 0x36, 0x83, 0x0d,
   0xcc, 0x3c, 0x66, 0x1c, 0x9b, 0x67, 0xc0, 0xe7, 0x79, 0x1c, 0x83, 0xc7,
   0x1c, 0x03, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07};


#define llnlshort_width 58
#define llnlshort_height 19
static unsigned char llnlshort_bits[] = {
   0x07, 0x80, 0x03, 0xe0, 0x01, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0,
   0x01, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0, 0x03, 0x87, 0x03, 0x00,
   0x07, 0x80, 0x03, 0xe0, 0x03, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0,
   0x07, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0, 0x07, 0x87, 0x03, 0x00,
   0x07, 0x80, 0x03, 0xe0, 0x0e, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0,
   0x1e, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0, 0x1c, 0x87, 0x03, 0x00,
   0x07, 0x80, 0x03, 0xe0, 0x3c, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0,
   0x38, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0, 0x70, 0x87, 0x03, 0x00,
   0x07, 0x80, 0x03, 0xe0, 0x70, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0,
   0xe0, 0x87, 0x03, 0x00, 0x07, 0x80, 0x03, 0xe0, 0xe0, 0x87, 0x03, 0x00,
   0x07, 0x80, 0x03, 0xe0, 0xc0, 0x87, 0x03, 0x00, 0xff, 0x87, 0xff, 0xe3,
   0xc0, 0x87, 0xff, 0x03, 0xff, 0x87, 0xff, 0xe3, 0x80, 0x87, 0xff, 0x03,
   0xff, 0x87, 0xff, 0xe3, 0x80, 0x87, 0xff, 0x03};


#define mainlogo_width 100
#define mainlogo_height 100
static unsigned char mainlogo_bits[] = {
   0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0xc0,
   0xff, 0x7f, 0xc0, 0xff, 0x7f, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55,
   0x40, 0x00, 0x40, 0x40, 0x00, 0x40, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0,
   0xaa, 0xc0, 0x73, 0x7e, 0xc0, 0xcf, 0x79, 0xf0, 0x50, 0x55, 0x50, 0x55,
   0x50, 0x55, 0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0xf0, 0xa0, 0xaa, 0xa0,
   0xaa, 0xa0, 0xaa, 0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0xf0, 0x50, 0x55,
   0x50, 0x55, 0x50, 0x55, 0x00, 0xc8, 0x09, 0x00, 0x72, 0x02, 0xf0, 0xa0,
   0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x10, 0x10, 0x00, 0x01, 0x01, 0xf0,
   0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x20, 0x20, 0x80, 0x80, 0x00,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x20, 0x27, 0x80, 0x9c,
   0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x40, 0x40, 0x40,
   0x40, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x80, 0x80,
   0x20, 0x20, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x80,
   0x9c, 0x20, 0x27, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00,
   0x00, 0x01, 0x11, 0x10, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55,
   0x00, 0x00, 0x02, 0x0a, 0x08, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0,
   0xaa, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55,
   0x50, 0x55, 0x00, 0xff, 0xf3, 0xff, 0xf9, 0x07, 0xf0, 0xa0, 0xaa, 0xa0,
   0xaa, 0xa0, 0xaa, 0x00, 0xab, 0x52, 0x55, 0x59, 0x0d, 0xf0, 0x50, 0x55,
   0x50, 0x55, 0x50, 0x55, 0x00, 0xfd, 0xf3, 0xfb, 0xe9, 0x17, 0xf0, 0xa0,
   0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x07, 0x00, 0x0e, 0x38, 0x1c, 0xf0,
   0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x05, 0x20, 0x8a, 0x28, 0x14,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0xff, 0x40, 0x4e, 0xf8,
   0x1f, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0xd5, 0x40, 0x4a,
   0xa8, 0x0a, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0xff, 0x20,
   0x8e, 0xf8, 0x07, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x05,
   0x10, 0x0a, 0x29, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00,
   0x07, 0x10, 0x0e, 0x39, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55,
   0x00, 0x05, 0x08, 0x0a, 0x2a, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0,
   0xaa, 0x00, 0x07, 0x04, 0x0e, 0x38, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55,
   0x50, 0x55, 0x00, 0x05, 0x72, 0xca, 0x29, 0x00, 0xf0, 0xa0, 0xaa, 0xa0,
   0xaa, 0xa0, 0xaa, 0x00, 0x07, 0x02, 0x0e, 0x38, 0x00, 0xf0, 0x50, 0x55,
   0x50, 0x55, 0x50, 0x55, 0x00, 0x00, 0x01, 0x11, 0x00, 0x00, 0xf0, 0xa0,
   0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x80, 0x9c, 0x20, 0x13, 0x00, 0xf0,
   0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x80, 0x80, 0x20, 0x20, 0x00,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x40, 0x40, 0x40, 0x40,
   0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x20, 0x27, 0x80,
   0x9c, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x20, 0x20,
   0x80, 0x80, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x10,
   0x10, 0x00, 0x01, 0x01, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00,
   0xc8, 0x09, 0x00, 0x72, 0x02, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55,
   0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0,
   0xaa, 0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0xf0, 0x50, 0x55, 0x50, 0x55,
   0x50, 0x55, 0xc0, 0x73, 0x7e, 0xc0, 0xcf, 0x79, 0xf0, 0xa0, 0xaa, 0xa0,
   0xaa, 0xa0, 0xaa, 0x40, 0x00, 0x40, 0x40, 0x00, 0x40, 0xf0, 0x50, 0x55,
   0x50, 0x55, 0x50, 0x55, 0xc0, 0xff, 0x7f, 0xc0, 0xff, 0x7f, 0xf0, 0xa0,
   0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x55, 0xf5, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xf2, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xf1, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0,
   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xf0, 0x50, 0x55, 0x50, 0x55,
   0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xf0, 0xa0, 0xaa, 0xa0,
   0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a, 0xf0, 0x50, 0x55,
   0x50, 0x55, 0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x15, 0xf0, 0xa0,
   0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x0a, 0xf0,
   0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
   0x02, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x50, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x01, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa,
   0xaa, 0xaa, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55, 0x01, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0x02,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x50, 0x55, 0x50, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05, 0x00, 0xf0, 0xa0, 0xaa, 0xa0,
   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x00, 0xf0, 0x50, 0x55,
   0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x01, 0x00, 0xf0, 0xa0,
   0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0xf0,
   0x50, 0x55, 0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x00, 0x00,
   0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a, 0x00,
   0x00, 0xf0, 0x50, 0x55, 0x41, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x15,
   0x00, 0x00, 0xf0, 0xa0, 0xaa, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
   0x0a, 0x00, 0x00, 0xf0, 0x50, 0x55, 0x41, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x05, 0x00, 0x00, 0xf0, 0xa0, 0xaa, 0x82, 0xaa, 0xaa, 0xaa, 0xaa,
   0xaa, 0xaa, 0x02, 0x00, 0x00, 0xf0, 0x50, 0x55, 0x01, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x01, 0x00, 0x00, 0xf0, 0xa0, 0xaa, 0x02, 0xa8, 0xaa,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0xf0, 0x40, 0x55, 0x05, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xa0, 0xaa, 0x0a,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x40, 0x55,
   0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x80,
   0xaa, 0xaa, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0x40, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00,
   0xf0, 0x80, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x00, 0x00,
   0x00, 0xf0, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x01, 0x00,
   0x00, 0x00, 0xf0, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x00,
   0x00, 0x00, 0x00, 0xf0, 0x00, 0x54, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0xa8, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
   0x2a, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x50, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x15, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0xa0, 0xaa, 0xaa, 0xaa,
   0xaa, 0xaa, 0x0a, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x40, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xaa,
   0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00,
   0x50, 0x55, 0x55, 0x55, 0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,
   0x00, 0x80, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xf0};


#define name_width 166
#define name_height 46
static unsigned char name_bits[] = {
   0x00, 0x3c, 0x00, 0x00, 0x00, 0x0c, 0x80, 0x0f, 0x00, 0xf0, 0x07, 0xf0,
   0x07, 0x00, 0x00, 0xfc, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x7f, 0x00,
   0x00, 0x80, 0x07, 0xe0, 0xff, 0xff, 0xff, 0x01, 0xf8, 0xff, 0xff, 0xff,
   0x7f, 0x00, 0xff, 0xff, 0x0f, 0x00, 0x80, 0xff, 0x00, 0x00, 0xe0, 0x03,
   0xf0, 0xff, 0xff, 0xff, 0x01, 0xfc, 0xff, 0xff, 0xff, 0x3f, 0xc0, 0xff,
   0xff, 0x3f, 0x00, 0xc0, 0xff, 0x01, 0x00, 0xfc, 0x01, 0xf8, 0xff, 0xff,
   0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0x00,
   0xe0, 0xff, 0x01, 0x80, 0xff, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0x80, 0xff,
   0xff, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0x03, 0xf8, 0xff, 0x03,
   0xc0, 0x7f, 0x00, 0xff, 0xff, 0xff, 0x3f, 0xc0, 0xff, 0xff, 0xff, 0xff,
   0x07, 0xf0, 0xff, 0xff, 0xff, 0x07, 0xc0, 0xff, 0x07, 0xc0, 0x3f, 0x80,
   0xc1, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf0, 0xff,
   0xff, 0xff, 0x07, 0x80, 0xff, 0x07, 0xe0, 0x1f, 0x00, 0x80, 0x7f, 0x00,
   0x00, 0x30, 0xe0, 0xff, 0xff, 0x07, 0x00, 0x18, 0xfe, 0xc7, 0xff, 0x0f,
   0x00, 0xff, 0x0f, 0xe0, 0x1f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0x3f, 0x00, 0x00, 0x00, 0xfe, 0x03, 0xfe, 0x1f, 0x00, 0xfe, 0x1f,
   0xe0, 0x0f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00,
   0x00, 0x00, 0xfc, 0x03, 0xfc, 0x1f, 0x00, 0xfc, 0x1f, 0xf0, 0x07, 0x00,
   0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0xfc,
   0x03, 0xf8, 0x3f, 0x00, 0xf8, 0x3f, 0xf0, 0x03, 0x00, 0x80, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xf8, 0x3f,
   0x00, 0xf8, 0x7f, 0xf8, 0x01, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xf0, 0x3f, 0x00, 0xf0, 0x7f,
   0xfc, 0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00,
   0x00, 0x00, 0xfc, 0x03, 0xe0, 0x3f, 0x00, 0xe0, 0xff, 0xfc, 0x00, 0x00,
   0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc,
   0x03, 0xe0, 0x3f, 0x00, 0xe0, 0xff, 0x7e, 0x00, 0x00, 0x80, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xe0, 0x3f,
   0x00, 0xc0, 0xff, 0x3f, 0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xe0, 0x1f, 0x00, 0x80, 0xff,
   0x1f, 0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00,
   0x00, 0x00, 0xfc, 0x03, 0xe0, 0x1f, 0x00, 0x80, 0xff, 0x0f, 0x00, 0x00,
   0x80, 0xff, 0xff, 0x7f, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc,
   0x03, 0xe0, 0x0f, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xc0, 0xff, 0xff,
   0x1f, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xf0, 0x07,
   0x00, 0x00, 0xfe, 0x0f, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x0f, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xf0, 0x03, 0x00, 0x00, 0xfe,
   0x0f, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xe0, 0x3f, 0x00,
   0x00, 0x00, 0xfc, 0x03, 0xf8, 0x01, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00,
   0xfc, 0xff, 0xff, 0x07, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc,
   0x03, 0x7c, 0x00, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfe, 0xff, 0xff,
   0x03, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x1e, 0x00,
   0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x03, 0x00, 0x00, 0x00, 0xfe,
   0x7f, 0x00, 0x00, 0x80, 0xff, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00,
   0x00, 0x00, 0xfc, 0x87, 0x01, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00,
   0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc,
   0x07, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x00, 0x00, 0x80, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00,
   0x00, 0x80, 0xcf, 0xff, 0x01, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x00, 0xc0, 0x8f,
   0xff, 0x01, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00,
   0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x00, 0xe0, 0x87, 0xff, 0x03, 0x00,
   0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc,
   0x07, 0x00, 0x00, 0x00, 0xf0, 0x03, 0xff, 0x07, 0x00, 0x80, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00,
   0x00, 0xf8, 0x03, 0xfe, 0x07, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x00, 0xf8, 0x01,
   0xfe, 0x0f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00,
   0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x00, 0xfc, 0x01, 0xfc, 0x1f, 0x00,
   0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0xfc,
   0x07, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xfc, 0x3f, 0x00, 0x80, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00,
   0x00, 0xff, 0x00, 0xf8, 0x7f, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0x7f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x80, 0xff, 0x00,
   0xf8, 0xff, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00,
   0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0xf0, 0xff, 0x01,
   0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0xfc,
   0x07, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0xe0, 0xff, 0x03, 0x80, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0xe0, 0xff, 0x07, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0x1f, 0x00, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0xf0, 0x0f, 0x00,
   0xc0, 0xff, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00,
   0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xf8, 0x03, 0x00, 0x80, 0x7f, 0x00,
   0x80, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0x00, 0xfc,
   0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x80, 0x07, 0x00,
   0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
   0x1e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
   0x00, 0x00, 0x0c, 0x00, 0x00, 0x00};


#define smith_width 89
#define smith_height 126
static unsigned char smith_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x20, 0x09, 0x00, 0x28, 0x01, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xd8, 0x2a, 0x00, 0x52, 0x05, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x2a, 0x42, 0x15, 0x54, 0x15, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x55, 0x05, 0x40, 0xa9, 0x15, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x80, 0xde, 0x2a, 0x10, 0x6d, 0x2f, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xc0, 0xb5, 0x92, 0xa4, 0xaa, 0x5d, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xd0, 0x6e, 0x4b, 0x52, 0x75, 0xb7, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xb8, 0xb5, 0x54, 0xd5, 0xd6, 0x7e, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xd4, 0x4a, 0x05, 0x55, 0xbb, 0xed, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x7e, 0x97, 0x5a, 0xea, 0xee, 0x7f, 0x05, 0x00, 0x00,
   0x00, 0x00, 0x40, 0xab, 0x2d, 0xaa, 0xb5, 0xfd, 0xfe, 0x0b, 0x00, 0x00,
   0x00, 0x00, 0xc0, 0xfd, 0xbe, 0x76, 0xdf, 0xaf, 0xfb, 0x12, 0x00, 0x00,
   0x00, 0x00, 0x60, 0x57, 0xef, 0xfa, 0x75, 0xff, 0xfe, 0x27, 0x00, 0x00,
   0x00, 0x00, 0xd0, 0xae, 0xfa, 0xab, 0xff, 0xfd, 0xb7, 0x15, 0x00, 0x00,
   0x00, 0x00, 0xb8, 0xf7, 0xdf, 0x7e, 0xdf, 0xff, 0xff, 0x57, 0x00, 0x00,
   0x00, 0x00, 0xec, 0x5e, 0xfd, 0xff, 0xfd, 0xff, 0xdb, 0x5b, 0x00, 0x00,
   0x00, 0x00, 0xfa, 0xeb, 0xb7, 0xff, 0xf7, 0xdd, 0xfe, 0x56, 0x01, 0x00,
   0x00, 0x00, 0xaf, 0x5f, 0xff, 0xf6, 0xbf, 0xff, 0xff, 0x7f, 0x01, 0x00,
   0x00, 0x80, 0xfa, 0xf5, 0xed, 0xff, 0xff, 0xef, 0xfb, 0xd7, 0x02, 0x00,
   0x00, 0xc0, 0xaf, 0xdf, 0xbf, 0x7b, 0xff, 0xff, 0xff, 0xff, 0x05, 0x00,
   0x00, 0x80, 0xf5, 0x6a, 0xf5, 0xde, 0xff, 0xff, 0xbf, 0xaf, 0x06, 0x00,
   0x00, 0xc0, 0xdf, 0xdf, 0xff, 0xf7, 0xed, 0xff, 0xff, 0xdf, 0x05, 0x00,
   0x00, 0xe0, 0xf6, 0x75, 0xb5, 0x7d, 0xbf, 0xfd, 0xff, 0xbf, 0x0b, 0x00,
   0x00, 0xe0, 0xbf, 0xdf, 0xdf, 0xd7, 0xfd, 0xff, 0xff, 0xff, 0x16, 0x00,
   0x00, 0xf0, 0xed, 0xf6, 0xf5, 0x7e, 0xef, 0xff, 0xff, 0xbf, 0x0f, 0x00,
   0x00, 0x70, 0xff, 0x5f, 0xff, 0xfb, 0xbd, 0xed, 0xff, 0xff, 0x15, 0x00,
   0x00, 0xf8, 0xbf, 0xf5, 0xab, 0xae, 0xef, 0xff, 0x77, 0xb7, 0x0f, 0x00,
   0x00, 0xf8, 0xf7, 0x7f, 0xff, 0xfb, 0xfe, 0xbf, 0xff, 0xff, 0x1a, 0x00,
   0x00, 0x74, 0x7f, 0xfb, 0xb5, 0xbe, 0xdf, 0xee, 0xff, 0xdf, 0x0f, 0x00,
   0x00, 0xfc, 0xff, 0x6f, 0xef, 0xeb, 0xff, 0xff, 0xfe, 0xfd, 0x1d, 0x00,
   0x00, 0xfc, 0x6f, 0xfd, 0xda, 0xfe, 0xf6, 0xff, 0xbf, 0x77, 0x0b, 0x00,
   0x00, 0xfa, 0xfe, 0xbf, 0x77, 0xf7, 0x7f, 0xdb, 0xf7, 0xff, 0x1f, 0x00,
   0x00, 0xfc, 0xdf, 0xfe, 0xdd, 0xbd, 0xfd, 0xff, 0x7e, 0xbb, 0x16, 0x00,
   0x00, 0xdc, 0xfb, 0x77, 0xf7, 0xef, 0xf7, 0xff, 0xf7, 0xef, 0x3b, 0x00,
   0x00, 0xfc, 0xbe, 0xde, 0x6d, 0xfb, 0xbe, 0xda, 0xbd, 0xbd, 0x7e, 0x00,
   0x00, 0xfe, 0xf7, 0xb5, 0xb6, 0xaf, 0xd5, 0x76, 0xef, 0xfe, 0x2b, 0x00,
   0x00, 0xfa, 0x5d, 0x0a, 0xa9, 0x6a, 0x25, 0xa9, 0xfb, 0xb7, 0x7e, 0x00,
   0x00, 0x7e, 0xaf, 0x52, 0x00, 0x00, 0x00, 0x44, 0x56, 0xfd, 0x3b, 0x00,
   0x00, 0xfc, 0x7f, 0x05, 0x04, 0x00, 0x88, 0x10, 0xb5, 0xef, 0x6e, 0x00,
   0x00, 0xfe, 0x56, 0x08, 0x00, 0x40, 0x00, 0x40, 0xd4, 0xfa, 0xbf, 0x00,
   0x00, 0xea, 0xbf, 0x45, 0x01, 0x00, 0x00, 0x00, 0x52, 0xfd, 0x7a, 0x00,
   0x00, 0xfe, 0x6d, 0x12, 0x40, 0x00, 0x00, 0xa4, 0x54, 0xb7, 0x7f, 0x00,
   0x00, 0xfd, 0x5f, 0x09, 0x00, 0x00, 0x00, 0x00, 0xd2, 0xfa, 0x5f, 0x00,
   0x00, 0xfe, 0x3f, 0x21, 0x01, 0x08, 0x00, 0x80, 0x54, 0xfd, 0x76, 0x00,
   0x00, 0xfa, 0xad, 0x14, 0x00, 0x00, 0x80, 0x20, 0xa2, 0xbe, 0x7f, 0x00,
   0x00, 0xfe, 0x5f, 0x21, 0x04, 0x81, 0x08, 0x8a, 0x54, 0xfd, 0x3d, 0x00,
   0x00, 0xbc, 0x5f, 0x4a, 0x51, 0x20, 0x82, 0x20, 0xa9, 0xeb, 0x7f, 0x00,
   0x00, 0xfe, 0x5b, 0x04, 0xaa, 0x16, 0x28, 0x57, 0xaa, 0x7e, 0x3f, 0x00,
   0x00, 0xfc, 0x2f, 0xa9, 0xa4, 0xa9, 0xc2, 0xba, 0xaa, 0xfa, 0x7f, 0x00,
   0x00, 0xfc, 0x5f, 0x25, 0xd9, 0x16, 0x58, 0x6f, 0xd5, 0xde, 0x37, 0x00,
   0x00, 0xf8, 0xad, 0x90, 0xee, 0x57, 0xa0, 0xfb, 0x55, 0xf5, 0x7f, 0x00,
   0x00, 0xf8, 0x2f, 0xa5, 0xbd, 0x16, 0xf4, 0x6e, 0xd7, 0xfe, 0x5f, 0x00,
   0x00, 0xf8, 0x9b, 0xa0, 0xea, 0x2a, 0x48, 0xf9, 0x5a, 0xf5, 0x3d, 0x00,
   0x00, 0xf0, 0x2f, 0x95, 0x2a, 0x09, 0xb0, 0x4a, 0x55, 0xfd, 0x2f, 0x00,
   0x00, 0xb0, 0x57, 0x00, 0xa0, 0x44, 0xaa, 0x2a, 0xa5, 0xd6, 0x3f, 0x00,
   0x00, 0xd0, 0x2e, 0x01, 0x49, 0x29, 0xa8, 0x92, 0x48, 0xfd, 0x17, 0x00,
   0x00, 0xe0, 0x9b, 0x00, 0x20, 0x0a, 0x52, 0x09, 0x22, 0xb5, 0x0e, 0x00,
   0x00, 0x20, 0x56, 0x02, 0x80, 0x50, 0x94, 0x24, 0x90, 0xde, 0x0b, 0x00,
   0x00, 0xa0, 0x9a, 0x08, 0x00, 0x04, 0x50, 0x02, 0x40, 0xb5, 0x06, 0x00,
   0x00, 0x40, 0x2a, 0x00, 0x10, 0x50, 0xaa, 0x48, 0xaa, 0x5a, 0x01, 0x00,
   0x00, 0x40, 0x55, 0x05, 0x00, 0x04, 0xa8, 0x02, 0x80, 0xb6, 0x01, 0x00,
   0x00, 0x60, 0x54, 0x00, 0x00, 0x52, 0x50, 0x08, 0x54, 0x5b, 0x01, 0x00,
   0x00, 0x40, 0xa9, 0x92, 0x00, 0x00, 0xa8, 0x22, 0xa1, 0xd6, 0x00, 0x00,
   0x00, 0x40, 0x55, 0x01, 0x00, 0x95, 0xa4, 0x00, 0xaa, 0xad, 0x00, 0x00,
   0x00, 0x80, 0xa8, 0x14, 0x00, 0x20, 0xa8, 0x8a, 0xa8, 0x5a, 0x00, 0x00,
   0x00, 0x80, 0xaa, 0x02, 0x40, 0x0a, 0x50, 0x21, 0x52, 0x57, 0x00, 0x00,
   0x00, 0x00, 0xa9, 0x92, 0x08, 0x44, 0x50, 0x09, 0xa9, 0x6d, 0x00, 0x00,
   0x00, 0x00, 0x52, 0x0a, 0x80, 0x28, 0x55, 0xa5, 0xaa, 0x2a, 0x00, 0x00,
   0x00, 0x00, 0xac, 0x42, 0x22, 0xb2, 0x68, 0x49, 0x50, 0x17, 0x00, 0x00,
   0x00, 0x00, 0xb4, 0x14, 0x80, 0xac, 0xb6, 0x92, 0xd6, 0x2a, 0x00, 0x00,
   0x00, 0x00, 0x58, 0x85, 0x24, 0x52, 0xdb, 0x2a, 0x69, 0x17, 0x00, 0x00,
   0x00, 0x00, 0x50, 0x29, 0x00, 0x48, 0x55, 0x49, 0xaa, 0x02, 0x00, 0x00,
   0x00, 0x00, 0x68, 0x15, 0x11, 0x11, 0xa8, 0x24, 0xb5, 0x05, 0x00, 0x00,
   0x00, 0x00, 0x50, 0x22, 0x04, 0x44, 0x25, 0x55, 0xd5, 0x02, 0x00, 0x00,
   0x00, 0x00, 0xa0, 0x4a, 0x81, 0x10, 0x50, 0x45, 0x6a, 0x01, 0x00, 0x00,
   0x00, 0x00, 0xa0, 0x14, 0x28, 0x42, 0x49, 0xa9, 0xaa, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x60, 0xa5, 0x82, 0x00, 0xa2, 0x2a, 0xd5, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xa0, 0x92, 0x28, 0x2a, 0x54, 0xa5, 0x5a, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x40, 0x49, 0x44, 0x55, 0x5b, 0x95, 0xea, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x60, 0x95, 0x92, 0xaa, 0xf6, 0xab, 0x5a, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xa0, 0x4a, 0xe9, 0xfd, 0xfb, 0x56, 0xd5, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x40, 0x95, 0x24, 0x97, 0x54, 0xab, 0x36, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xd0, 0x4a, 0x92, 0x68, 0x57, 0x55, 0x6d, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xa8, 0x55, 0x49, 0x82, 0xa8, 0x52, 0x3b, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xa0, 0xaa, 0x24, 0x29, 0x52, 0x55, 0x2d, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x50, 0x2b, 0x45, 0x92, 0xa4, 0xaa, 0x16, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x48, 0xd5, 0x92, 0x4a, 0xb5, 0xaa, 0x0b, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xa4, 0x2e, 0x55, 0xaa, 0xaa, 0x6a, 0x07, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x48, 0x55, 0x29, 0x51, 0x55, 0xd5, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x80, 0x90, 0xba, 0x52, 0x4a, 0xaa, 0xaa, 0x07, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x8a, 0xaa, 0x95, 0x92, 0x24, 0xed, 0x05, 0x00, 0x00, 0x00,
   0x00, 0x40, 0x20, 0x75, 0x2b, 0x44, 0x48, 0xb5, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x20, 0x95, 0x5a, 0x55, 0x01, 0x25, 0xed, 0x06, 0x00, 0x00, 0x00,
   0x00, 0x48, 0xa0, 0x6a, 0xab, 0x14, 0x48, 0xb5, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x20, 0x94, 0xc2, 0x5a, 0x42, 0xa5, 0xfe, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x88, 0xa0, 0xd0, 0x57, 0x95, 0x48, 0xf5, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x25, 0x54, 0x80, 0xba, 0xaa, 0x6a, 0xdf, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x12, 0x22, 0x20, 0x6f, 0x55, 0xd5, 0xfd, 0x03, 0x00, 0x00, 0x00,
   0x80, 0x80, 0x10, 0x04, 0xfd, 0xab, 0x76, 0x6f, 0x0d, 0x00, 0x00, 0x00,
   0x20, 0x00, 0x14, 0x40, 0xba, 0xde, 0xdd, 0xff, 0x0b, 0x00, 0x00, 0x00,
   0x90, 0x00, 0x0a, 0x10, 0xf4, 0x77, 0xff, 0xb6, 0x1e, 0x00, 0x00, 0x00,
   0x24, 0x50, 0x09, 0x40, 0xe9, 0xff, 0xf7, 0xdf, 0x1b, 0x00, 0x00, 0x00,
   0x09, 0xa5, 0x12, 0x00, 0xd0, 0xfd, 0xff, 0xf5, 0x37, 0x00, 0x00, 0x00,
   0x02, 0x10, 0x09, 0x88, 0xa0, 0xd7, 0xae, 0x5f, 0x5d, 0x00, 0x00, 0x00,
   0x00, 0x40, 0x02, 0x00, 0x42, 0xbd, 0xf7, 0xf6, 0xef, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x01, 0x20, 0x80, 0xf6, 0xda, 0xdb, 0x5b, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x01, 0x02, 0x00, 0x5e, 0xbf, 0xfe, 0xb6, 0x03, 0x00, 0x00,
   0x00, 0x48, 0x02, 0x00, 0x80, 0xed, 0xed, 0xeb, 0xdb, 0x06, 0x00, 0x00,
   0x20, 0x00, 0x85, 0x00, 0x40, 0x7a, 0xfb, 0xfe, 0xaf, 0x07, 0x00, 0x00,
   0x00, 0xa0, 0x02, 0x04, 0x90, 0xd6, 0xef, 0xeb, 0xfb, 0x0a, 0x00, 0x00,
   0x00, 0x00, 0x15, 0x00, 0x40, 0xe9, 0xba, 0xde, 0xd7, 0x17, 0x00, 0x00,
   0x00, 0x08, 0x0a, 0x00, 0x54, 0x52, 0xef, 0xfd, 0x7f, 0x2b, 0x00, 0x00,
   0x00, 0x80, 0x14, 0x00, 0x90, 0xd0, 0x7b, 0xd7, 0xef, 0x5b, 0x00, 0x00,
   0x00, 0x11, 0x2a, 0x80, 0x4a, 0x60, 0xad, 0xfd, 0xbf, 0x2a, 0x00, 0x00,
   0x00, 0x00, 0x28, 0x20, 0x24, 0xd0, 0xeb, 0x76, 0xff, 0x55, 0x01, 0x00,
   0x02, 0x90, 0x2a, 0x40, 0x09, 0xe9, 0x5e, 0xfb, 0x5f, 0xa9, 0x00, 0x00,
   0x10, 0x20, 0xd0, 0x90, 0x02, 0x78, 0xd5, 0xfe, 0xbf, 0x72, 0x02, 0x00,
   0x4a, 0x02, 0xa2, 0x24, 0x04, 0xec, 0xb5, 0xeb, 0x7d, 0x14, 0x09, 0x00,
   0x10, 0xa0, 0xd0, 0x52, 0x41, 0xfa, 0xee, 0xfd, 0x2f, 0x7d, 0x04, 0x00,
   0x04, 0x08, 0x40, 0x05, 0x00, 0xae, 0xbd, 0x7b, 0x55, 0xbb, 0x08, 0x00,
   0x28, 0x41, 0x92, 0xab, 0x00, 0xfd, 0xd6, 0xee, 0xf7, 0x5e, 0x26, 0x00,
   0x10, 0x00, 0x40, 0x03, 0x84, 0xbf, 0xbd, 0xfa, 0x2d, 0xbf, 0x4a, 0x00,
   0x20, 0x80, 0x80, 0x0e, 0x20, 0x77, 0xab, 0xfd, 0xdb, 0xba, 0x35, 0x00,
   0x20, 0x00, 0x52, 0x07, 0xc0, 0xff, 0x2d, 0x7a, 0x6d, 0x7d, 0x9b, 0x00};


#define uc_width 120
#define uc_height 10
static unsigned char uc_bits[] = {
   0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x42, 0x40, 0x04, 0x04, 0x00, 0x00, 0x44, 0x00, 0x00,
   0xc3, 0x01, 0x13, 0x03, 0x00, 0x04, 0x99, 0x40, 0x04, 0x00, 0x00, 0x00,
   0x40, 0x00, 0x80, 0x20, 0x02, 0x82, 0x00, 0x00, 0x00, 0x85, 0x40, 0x74,
   0x16, 0x99, 0xe6, 0xf6, 0x09, 0xcc, 0x21, 0x30, 0xda, 0x99, 0x76, 0x66,
   0x85, 0x20, 0x92, 0xa4, 0xa4, 0x11, 0x24, 0x09, 0x92, 0x10, 0x40, 0x92,
   0xa4, 0x91, 0x84, 0x99, 0x20, 0x92, 0x64, 0x5e, 0x20, 0x94, 0x04, 0x51,
   0x10, 0x38, 0x51, 0x62, 0x90, 0x74, 0x42, 0x20, 0x4a, 0x22, 0x42, 0x40,
   0x92, 0x04, 0x49, 0x10, 0x25, 0x49, 0x52, 0x48, 0x4a, 0x3c, 0xc0, 0x49,
   0x22, 0x4c, 0x38, 0x22, 0x07, 0x46, 0xe0, 0x38, 0x49, 0x4c, 0x48, 0x72,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
