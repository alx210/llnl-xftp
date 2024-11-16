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
#include "pixmaps.h"

extern Widget w_toplev;
extern Widget w_main;
extern Display *display;
extern int depth;
extern Visual *visual;
extern Window root_window;


/*
 * create_pixmap - Create pixmap based on "info".  This mechanism is not
 *                 efficient for large pixmaps.
 */
Pixmap
create_pixmap(info)
struct image_info *info;
{
    Colormap cmap;
    XColor color;
    char *image_data;
    int i;
    int j;
    int k;
    int indx;
    Pixmap pixmap;
    XImage *image;
    GC gc;
    int class = visual->class;
    int bytewidth;
    int use_color;
	unsigned long foreground;
	unsigned long background;
	XPoint *points;
	int npoints;

    /* Can the display handle colors? */
    if (depth <8 || class == GrayScale || class == StaticGray)
        use_color = False;
 	else {
        use_color = True;
        XtVaGetValues(w_toplev,
            XmNcolormap,    &cmap,
            NULL
        );
        for (i=0; i<info->ncolors; i++) {
			if (info->colors[i].name) {
	            if (!XParseColor(display, cmap, info->colors[i].name, &color)) {
	                use_color = False;
	                break;
	            }
	            if (!XAllocColor(display, cmap, &color)) {
	                use_color = False;
	                break;
	            }
	            info->colors[i].pixel = color.pixel;
			} else
				if (info->colors[i].mono == BG)
					info->colors[i].pixel = background;
				else
					info->colors[i].pixel = foreground;
        }
    }
    if (use_color) {
    	pixmap = XCreatePixmap(display, root_window, info->width, info->height,
			depth);
	    gc = XCreateGC(display, pixmap, (unsigned long)0, NULL);
		points = (XPoint *)XtMalloc(info->width*info->height*sizeof(XPoint));
        for (i=0; i<info->ncolors; i++) {
			npoints = 0;
            for (j=0; j<info->height; j++)
                for (k=0; k<info->width; k++)
                    if (info->pixels[j][k] == info->colors[i].symbol) {
						points[npoints].x = k;
						points[npoints].y = j;
                    	npoints++;
					}
			XSetForeground(display, gc, info->colors[i].pixel);
			XDrawPoints(display, pixmap, gc, points, npoints,
				CoordModeOrigin);
        }
		XtFree((char*)points);
		XFreeGC(display, gc);
    } else {
        bytewidth = (info->width+7)/8;
        image_data = XtCalloc(info->height*bytewidth, 1);
        for (i=0; i<info->ncolors; i++) {
            for (j=0; j<info->height; j++)
                for (k=0; k<info->width; k++)
                    if (info->pixels[j][k] == info->colors[i].symbol) {
                        indx = j*bytewidth+k/8;
                        image_data[indx] |= info->colors[i].mono<<(7-(k&0x7));
                    }
        }
		image = XCreateImage(display, visual, 1, XYBitmap, 0, image_data,
			info->width, info->height, 8, 0);
		pixmap = XCreatePixmap(display, root_window, info->width, info->height,
			depth);
		gc = XCreateGC(display, pixmap, (unsigned long)0, NULL);
		XtVaGetValues(w_main,
			XmNforeground,	&foreground,
			XmNbackground,	&background,
			NULL
		);
		XSetForeground(display, gc, foreground);
		XSetBackground(display, gc, background);
		XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, info->width,
			info->height);
		XtFree(image_data);
		XtFree((char *)image);
		XFreeGC(display, gc);
    }

    return pixmap;
}

