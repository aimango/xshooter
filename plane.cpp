#include <iostream>
#include <list>
#include <vector>
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
	GC		 gc[2];
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
		virtual void paint(XInfo &xInfo) = 0;
};

class Plane : public Displayable {
	public:
		virtual void paint(XInfo &xInfo) {
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[1], x, y, width, height, 0, 360*64);
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}
	  
		void moveto(int newX, int newY) {
			x = newX;
			y = newY;
		}
	// constructor
	Plane(int x, int y, int width, int height):x(x), y(y), width(width), height(height)  {}
	  
	private:
		int x;
		int y;
		int width;
		int height;
};

class Bomb : public Displayable {

	public:
		void paint(XInfo &xInfo) {
			/* draw a rectangle whose top-left corner is at 'x+120,y+50', its width is */
			/* 50 pixels, and height is 60 pixels.                                  */
			XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x+20, y+10, 10, 40);
			//XFillArc(xInfo.display, xInfo.window, xInfo.gc[1], x, y, 10, 10, 0, 360*64);
			//XDrawLine(xInfo.display, xInfo.window, xInfo.gc[0], x, y, x+100, y+100);
		}

		void move(XInfo &xInfo) {
			y = y + speed;
			//if ()
		}

		Bomb(int x, int y): x(x), y(y) {
			speed = 5;
		}

	private:
		int x;
		int y;
		int speed;

};

class Building : public Displayable {
	public:
		virtual void paint(XInfo &xInfo) {
			for (int i = 0; i < maxHoriz; i++) {
				XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x+i*100, y+800-heights[i], 100, heights[i]);
			}
		}
		
		void move(XInfo &xInfo) {
			cout << x << endl;
			x = x - speed;
			if (x < 0) {
				for (int i = 0 ; i < 11; i++)
					heights.push_back( rand() % 600 + 50 );
				maxHoriz+=12;
			}
		}
		
		Building(int x, int y): x(x), y(y), maxHoriz(0) {
			speed = 10;
			srand ( time(NULL) );
			for (int i = 0 ; i < 11; i++)
				heights.push_back( rand() % 600 + 50 );
		}
	
	private:
		int x;
		int y;
		int speed;
		int maxHoriz;
		vector<int> heights;
};

/*
 * Display some text where the user clicked the mouse.
 */
class Text : public Displayable
{  public:
      virtual void paint(XInfo &xInfo)
      {  XDrawImageString( xInfo.display, xInfo.window, xInfo.gc[1], 
               this->x, this->y, this->s.c_str(), this->s.length() );
      }
      
      // constructor
      Text(int x, int y, string s):x(x), y(y), s(s)  {}
      
   private:
      int x;
      int y;
      string s;
};

list<Displayable *> dList;           	// list of Displayables
list<Bomb *> dBombList;					// list of Bombs
Plane plane(100, 100, 50, 50);
Building building(900, 0);


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
		xInfo.display,						// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,					// upper left corner location
		hints.width, hints.height,			// size of the window
		Border,								// width of window's border
		black,								// window border colour
		white );							// window background colour
		
	XSetStandardProperties(
		xInfo.display,				// display containing the window
		xInfo.window,				// window whose properties are set
		"Plane Attack >w<",			// window's title
		"Plane",					// icon's title
		None,						// pixmap for the icon
		argv, argc,					// applications command line args
		&hints );					// size hints for the window

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
void repaint( XInfo &xInfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	// XClearWindow( xInfo.display, xInfo.window );
	
	XWindowAttributes windowInfo;
	XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[0], 0, 0, width, height);
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xInfo);
		begin++;
	}
	XFlush( xInfo.display );
}


void handleButtonPress(XInfo &xInfo, XEvent &event) {
	cout << "Got button press!" << endl;
	//dList.push_front(new Text(event.xbutton.x, event.xbutton.y, "Urrp!"));
	//repaint( xInfo );
	
}

void handleKeyPress(XInfo &xInfo, XEvent &event) {
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
		else if (text[0] == 'm') {
			Bomb *bomb = new Bomb(plane.getX(), plane.getY());
			dList.push_front(bomb);
			dBombList.push_front(bomb);
		}
	}
}

void handleMotion(XInfo &xInfo, XEvent &event, int inside) {
	plane.moveto(event.xbutton.x, event.xbutton.y);	
}

void handleAnimation(XInfo &xInfo, int inside) {
	building.move(xInfo);

	list<Bomb *>::const_iterator begin = dBombList.begin();
	list<Bomb *>::const_iterator end = dBombList.end();
	while( begin != end ) {
		Bomb *d = *begin;
		d->move(xInfo);
		begin++;
	}

	if (!inside) {
		// plane.lookat(ball.getX(), ball.getY());
	}
}

unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xInfo) {
	// Add stuff to paint to the display list
	dList.push_front(&building);
	dList.push_front(&plane);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		if (XPending(xInfo.display) > 0) {
			XNextEvent( xInfo.display, &event );
			switch( event.type ) {
				case ButtonPress:
					handleButtonPress(xInfo, event);
					break;
				case KeyPress:
					handleKeyPress(xInfo, event);
					break;
				case MotionNotify:
					handleMotion(xInfo, event, inside);
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
			handleAnimation(xInfo, inside);
			repaint(xInfo);
			lastRepaint = now();
		}
		if (XPending(xInfo.display) == 0) {
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

	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	while (begin != end) {
		Displayable *d = *begin;
		begin++;
		delete d;
	}

	list<Bomb *>::const_iterator beginn = dBombList.begin();
	list<Bomb *>::const_iterator endd = dBombList.end();
	while (beginn != endd) {
		Bomb *dq = *beginn;
		beginn++;
		delete dq;
	}

	dList.clear();
	dBombList.clear();
}
