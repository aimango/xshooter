/*
 * Information to draw on the window.
 */

#ifndef xinfo_h
#define xinfo_h

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct XInfo {
	int height;
	int width;
	Display *display;
	int screen;
	Window window;
	GC gc[4];
	XFontStruct *font;
};

#endif