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

#define logo64_width 64
#define logo64_height 64

static unsigned char logo64_bits[] = {
 0xff, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x02,
 0x40, 0x00, 0x00, 0x80, 0x9f, 0xff, 0xfc, 0x03, 0xc0, 0x3f, 0xff, 0xf9,
 0x20, 0x00, 0x08, 0x00, 0x00, 0x10, 0x00, 0x04, 0x40, 0x00, 0x10, 0x00,
 0x00, 0x08, 0x00, 0x02, 0x40, 0xfe, 0x13, 0x00, 0x00, 0xc8, 0x7f, 0x02,
 0x80, 0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x01, 0x40, 0x00,
 0x00, 0x02, 0x80, 0x00, 0x00, 0xf9, 0x4f, 0x00, 0x00, 0xf2, 0x4f, 0x00,
 0x00, 0x02, 0x80, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x04, 0x00, 0x01,
 0x80, 0x00, 0x20, 0x00, 0x00, 0xe4, 0x3f, 0x01, 0x80, 0xfc, 0x13, 0x00,
 0x00, 0x08, 0x00, 0x02, 0x40, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x04,
 0x20, 0x00, 0x08, 0x00, 0x00, 0x20, 0xff, 0x09, 0x90, 0xff, 0x04, 0x00,
 0x00, 0x20, 0x00, 0x08, 0x10, 0x00, 0x04, 0x00, 0x00, 0x40, 0x00, 0x10,
 0x08, 0x00, 0x02, 0x00, 0x00, 0x80, 0xfc, 0x27, 0xe4, 0x3f, 0x01, 0x00,
 0x00, 0x80, 0x00, 0x20, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 0xff, 0xff, 0xf8, 0xff, 0x00,
 0xe0, 0xff, 0x1f, 0xff, 0xff, 0xf8, 0xff, 0x01, 0x60, 0x00, 0x18, 0x03,
 0xc0, 0x18, 0x80, 0x01, 0x60, 0x00, 0x18, 0x03, 0xc0, 0x18, 0x00, 0x03,
 0x60, 0xfe, 0x1f, 0x7f, 0xfe, 0x98, 0x3f, 0x06, 0x60, 0xfe, 0x1f, 0x7f,
 0xfe, 0x98, 0x7f, 0x06, 0x60, 0x06, 0x20, 0x60, 0x06, 0x9a, 0x61, 0x06,
 0x60, 0x06, 0x20, 0x60, 0x06, 0x99, 0x61, 0x06, 0x60, 0x06, 0x40, 0x66,
 0x26, 0x99, 0x61, 0x06, 0x60, 0xfe, 0x83, 0x60, 0x86, 0x98, 0x7f, 0x06,
 0x60, 0xfe, 0x83, 0x60, 0x86, 0x98, 0x3f, 0x06, 0x60, 0x00, 0x03, 0x65,
 0x46, 0x18, 0x00, 0x07, 0x60, 0x00, 0x03, 0x61, 0x46, 0x18, 0x80, 0x03,
 0x60, 0xfe, 0x83, 0x60, 0x86, 0x98, 0xff, 0x01, 0x60, 0xfe, 0x83, 0x64,
 0x86, 0x98, 0xff, 0x00, 0x60, 0x06, 0x40, 0x60, 0x06, 0x99, 0x01, 0x00,
 0x60, 0x06, 0x20, 0x60, 0x06, 0x9a, 0x01, 0x00, 0x60, 0x06, 0x20, 0x67,
 0x66, 0x9a, 0x01, 0x00, 0x60, 0x06, 0x10, 0x60, 0x06, 0x98, 0x01, 0x00,
 0x60, 0x06, 0x08, 0x60, 0x06, 0x98, 0x01, 0x00, 0x60, 0x06, 0xc8, 0x67,
 0xe6, 0x99, 0x01, 0x00, 0x60, 0x06, 0x04, 0x60, 0x06, 0x98, 0x01, 0x00,
 0xe0, 0x07, 0x02, 0xe0, 0x07, 0xf8, 0x01, 0x00, 0xe0, 0x07, 0xf2, 0xe7,
 0xe7, 0xf9, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x80, 0x00, 0x20, 0x04, 0x00, 0x01, 0x00, 0x00, 0x80, 0xfc, 0x13,
 0xe4, 0x3f, 0x01, 0x00, 0x00, 0x40, 0x00, 0x08, 0x08, 0x00, 0x02, 0x00,
 0x00, 0x20, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x00, 0x20, 0xff, 0x04,
 0x90, 0xff, 0x04, 0x00, 0x00, 0x10, 0x00, 0x02, 0x20, 0x00, 0x08, 0x00,
 0x00, 0x08, 0x00, 0x01, 0x40, 0x00, 0x10, 0x00, 0x00, 0xe4, 0x3f, 0x01,
 0x80, 0xfc, 0x27, 0x00, 0x00, 0x04, 0x80, 0x00, 0x80, 0x00, 0x20, 0x00,
 0x00, 0x02, 0x40, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0xf9, 0x4f, 0x00,
 0x00, 0xf2, 0x9f, 0x00, 0x00, 0x01, 0x20, 0x00, 0x00, 0x02, 0x80, 0x00,
 0x80, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x01, 0x40, 0xfe, 0x09, 0x00,
 0x00, 0xc8, 0x7f, 0x02, 0x40, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x02,
 0x20, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00, 0x04, 0x9f, 0x7f, 0xfe, 0x01,
 0xc0, 0x3f, 0xff, 0xf9, 0x01, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x80,
 0xff, 0xff, 0xff, 0x01, 0xc0, 0xff, 0xff, 0xff };
