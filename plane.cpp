/*
 * Commands to compile and run
 *
 * g++ -o x3_animation x3_animation.cpp -L/usr/X11R6/lib -lX11 -lstdc++
 * ./x3_animation
 * 
 * Depending on the machine, the -L option and -lstdc++ may
 * not be needed.
 */

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
const int Border = 5;
const int BufferSize = 10;
const int FPS = 30;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}


/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};       


class Plane : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			XFillArc(xinfo.display, xinfo.window, xinfo.gc[1], x, y, width, height, 0, 360*64);
			double cx = x + width/2;
			double cy = y + height/2;
						
			// double alpha;
			// if (look_x > cx ) alpha = atan((look_y - cy) / (look_x - cx) );
			// else alpha = M_PI - atan((look_y - cy) / (cx - look_x) );
			// XFillArc(xinfo.display, xinfo.window, xinfo.gc[0], 
			// 	cx + cos(alpha)*width/4 - width/8,
			// 	cy + sin(alpha)*height/4 - height/8,
			// 	width/4, height/4,
			// 	0,
			// 	360*64);
		}
		
		void lookat(int x, int y) {
			look_x = x;
			look_y = y;
		}
      
      	void moveto(int newx, int newy) {
      		x = newx;
      		y = newy;
      	}
	// constructor
	Plane(int x, int y, int width, int height):x(x), y(y), width(width), height(height)  {
		look_x = x+width/2;
		look_y = y+height/2;
	}
      
	private:
		int x;
		int y;
		int width;
		int height;
		int look_x;
		int look_y;
};

class Ball : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			XFillArc(xinfo.display, xinfo.window, xinfo.gc[1], x, y, diameter, diameter, 0, 360*64);
		}
		
		void move(XInfo &xinfo) {
			x = x + direction;
			if (random() % 100 == 0 || x < -50 || x > 500) {
				direction = -direction;
			}
		}
		
		int getX() {
			return x;
		}
		
		int getY() {
			return y;
		}
		
		Ball(int x, int y, int diameter): x(x), y(y), diameter(diameter) {
			direction = 4;
		}
	
	private:
		int x;
		int y;
		int diameter;
		int direction;
};


list<Displayable *> dList;           // list of Displayables
Plane plane(100, 100, 50, 50);
Ball ball(500, 450, 140);


/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 1200;
	hints.height = 800;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"Plane",		// window's title
		"Plane",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	// Reverse Video
	i = 1;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);


	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | 
		EnterWindowMask | LeaveWindowMask);

	/* vars to make blank cursor */
	Pixmap blank;
	XColor dummy;
	char data[1] = {0};
	Cursor cursor;

	/* make a blank cursor */
	blank = XCreateBitmapFromData (xInfo.display, xInfo.window, data, 1, 1);
	if(blank == None) {
		error("error: out of memory.");
	}
	cursor = XCreatePixmapCursor(xInfo.display, blank, blank, &dummy, &dummy, 0, 0);
	XFreePixmap (xInfo.display, blank);

	// set the blank cursor
	XDefineCursor(xInfo.display, xInfo.window, cursor);

	// above found from http://www.gamedev.net/topic/285005-anyone-knows-how-to-hideshow-mouse-pointer-under-linux-using-opengl/
	//XUndefineCursor(xInfo.display, xInfo.window);


	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	
	XFlush(xInfo.display);
	sleep(2);	// let server get set up before sending drawing commands
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	//XClearWindow( xinfo.display, xinfo.window );
	
	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 0, 0, width, height);
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}
	XFlush( xinfo.display );
}


void handleButtonPress(XInfo &xinfo, XEvent &event) {
	cout << "Got button press!" << endl;
	// dList.push_front(new Text(event.xbutton.x, event.xbutton.y, "Urrp!"));
	// repaint( dList, xinfo );
	
}

void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		}
	}
}

void handleMotion(XInfo &xinfo, XEvent &event, int inside) {
	plane.moveto(event.xbutton.x, event.xbutton.y);	
}

void handleAnimation(XInfo &xinfo, int inside) {
	ball.move(xinfo);
	if (!inside) {
		plane.lookat(ball.getX(), ball.getY());
	}
}

unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&ball);
	dList.push_front(&plane);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			switch( event.type ) {
				case ButtonPress:
					handleButtonPress(xinfo, event);
					break;
				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case MotionNotify:
					handleMotion(xinfo, event, inside);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		}
		
		unsigned long end = now();
		if (end - lastRepaint > 1000000/FPS) {
			handleAnimation(xinfo, inside);
			repaint(xinfo);
			lastRepaint = now();
		}
		if (XPending(xinfo.display) == 0) {
			usleep(1000000/FPS - (end - lastRepaint));
		}
	}
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;

	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
