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


// TODO:
// object contact
// 'catchers' on the buildings
// keyboard acceleration
// memory dealloc
// game pausing

const int Border = 5;
const int BufferSize = 10;
const int FPS = 30;
const int windowHeight = 600;
const int windowWidth = 800;
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
		virtual void paint(XInfo &xInfo) = 0;
};

class Plane : public Displayable {
	public:
		virtual void paint(XInfo &xInfo) {
			/* draw a small triangle at the top-left corner of the window. */
			/* the triangle is made of a set of consecutive lines, whose   */
			/* end-point pixels are specified in the 'points' array.       */
			/* draw the triangle in a yellow color. */
			// TODO not sure how to set thickness of lines
			x += velocityX;
			y += velocityY;

			XPoint points[] = {
					{x-30, y-15},
					{x+0, y+30},
					{x-30, y-15},
					{x-30, y-15}
				};
			int npoints = sizeof(points)/sizeof(XPoint);
			XDrawLines(xInfo.display, xInfo.window, xInfo.gc[2], points, npoints, CoordModeOrigin);

			XFillArc(xInfo.display, xInfo.window, xInfo.gc[2], x, y, width, height, 0, 360*64);
			XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[2], x-15, y+10, 30, 10);
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		int getVelocityX() {
			return velocityX;
		}

		int getVelocityY() {
			return velocityY;
		}

		void setVelocityX(int dir) {
			if (dir) {
				velocityX += 15;
			}
			else {
				velocityX -= 15;
			}
		}

		void setVelocityY(int dir) {
			if (dir) {
				velocityY += 15;
			}
			else {
				velocityY -= 15;
			}
		}

		// void moveX(int dir) {
		// 	if (dir)
		// 		x+=15;f
		// 	else
		// 		x-=15;
		// }

		// void moveY(int dir){
		// 	if (dir)
		// 		y+=15;
		// 	else 
		// 		y-=15;
		// }


		// constructor
		Plane(int x, int y, int width, int height): x(x), y(y), width(width), height(height)  {
			velocityX = 0;
			velocityY = 0;
		}

	private:
		int x;
		int y;
		int width;
		int height;
		int velocityX;
		int velocityY;
};

class Building : public Displayable {
	public:
		virtual void paint(XInfo &xInfo) {
			for (int i = 0; i < 150; i++) {
				XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x+i*50, y+windowHeight-heights[i], 50, heights[i]);
			}
		}

		void move(XInfo &xInfo) {
			x = x - speed;

		}

		Building(int x, int y): x(x), y(y) {
			speed = 10;
			for (int i = 0 ; i < 100; i++) {
				int person = rand()%4;
				if (person == 1) {

				}
				heights.push_back( rand() % 400 + 50 );
			}
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		vector<int> getHeights() {
			return heights;
		}

	private:
		int x;
		int y;
		int speed;
		vector<int> heights;
};

Plane plane(100, 100, 30, 30);
Building building(windowHeight, 0);


class Bomb : public Displayable {

	public:
		void paint(XInfo &xInfo) {
			//TODO: need to grab initial velocity of the plane
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[2], x, y, 10, 10, 0, 360*64);
		}

		void move(XInfo &xInfo) {
			y = y + speed;
			x = x - speed;
			if (y > building.getY()+windowHeight-building.getHeights().at(0))
				cout << "hit " << windowHeight-building.getHeights().at(0) << endl;
		}

		Bomb(int x, int y): x(x), y(y) {
			speed = 5;
		}

	private:
		int x;
		int y;
		int speed;

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

list<Displayable *> dList;				// list of Displayables
list<Bomb *> dBombList;					// list of Bombs



/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

	srand ( time(NULL) );

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
	hints.width = windowWidth;
	hints.height = windowHeight;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,						// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,					// upper left corner location
		hints.width, hints.height,			// size of the window
		Border,								// width of window's border
		black,								// window border colour
		black );							// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"Plane",			// window's title
		"Plane",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	for (int i = 0; i <= 2; i++){
		xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
		XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
		XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
		XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);
	}

	/*
	 * COLORS!
	 */

	// use midnight blue 
	// coloring borrowed from http://slist.lilotux.net/linux/xlib/color-drawing.c
	XColor blue;
	Colormap screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	Status rc = XAllocNamedColor(xInfo.display, screen_colormap, "midnight blue", &blue, &blue);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'midnight blue' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[0], blue.pixel);

	// use hot pink 
	// coloring borrowed from http://slist.lilotux.net/linux/xlib/color-drawing.c
	XColor pink;
	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	rc = XAllocNamedColor(xInfo.display, screen_colormap, "medium purple", &pink, &pink);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'medium purple' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[1], pink.pixel);
	
	// use white
	XSetForeground(xInfo.display, xInfo.gc[2], WhitePixel(xInfo.display, xInfo.screen));



	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | KeyReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);

	/* vars to make blank cursor */
	XColor dummy;
	char data[1] = {0};

	/* make a blank cursor */
	Pixmap blank = XCreateBitmapFromData (xInfo.display, xInfo.window, data, 1, 1);
	if(blank == None) {
		error("error: out of memory. can't create blank pixmap from data");
	}
	Cursor cursor = XCreatePixmapCursor(xInfo.display, blank, blank, &dummy, &dummy, 0, 0);
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
void repaint( XInfo &xInfo, int inside) {
	if (inside) {
		list<Displayable *>::const_iterator begin = dList.begin();
		list<Displayable *>::const_iterator end = dList.end();

		//XClearWindow( xInfo.display, xInfo.window ); // flickers a lot
		
		XWindowAttributes windowInfo;
		XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
		unsigned int height = windowInfo.height;
		unsigned int width = windowInfo.width;

		//re-draws the backgound
		XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[0], 0, 0, width, height);
		while( begin != end ) {
			Displayable *d = *begin;
			d->paint(xInfo);
			begin++;
		}
		XFlush( xInfo.display );
	} else { 

	}
}


void handleButtonPress(XInfo &xInfo, XEvent &event) {
	cout << "Got button press!" << endl;
	//dList.push_front(new Text(event.xbutton.x, event.xbutton.y, "Urrp!"));
	//repaint( xInfo );
}

void handleKeyRelease(XInfo &xInfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];

	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key release -- %c\n", text[0]);
		switch (text[0]){
			case 'w': {
				plane.setVelocityY(1);
				break;
			}
			case 'a': {
				plane.setVelocityX(1);
				break;
			}
			case 's': {
				plane.setVelocityY(0);
				break;
			}
			case 'd': {
				plane.setVelocityX(0);
				break;
			}
		}
	}
}

void handleKeyPress(XInfo &xInfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		switch (text[0]){
			case 'q':
				error("Terminating normally.");
				break;
			case 'm': {
				Bomb *bomb = new Bomb(plane.getX(), plane.getY());
				dList.push_front(bomb);
				dBombList.push_front(bomb);
				break;
			}
			case 'w': {
				plane.setVelocityY(0);
				break;
			}
			case 'a': {
				plane.setVelocityX(0);
				break;
			}
			case 's': {
				plane.setVelocityY(1);
				break;
			}
			case 'd': {
				plane.setVelocityX(1);
				break;
			}
		}
	}
}

void handleMotion(XInfo &xInfo, XEvent &event, int inside) {
	//plane.moveto(event.xbutton.x, event.xbutton.y);	
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
		// pause the game?
	}
}

void handleResizing(XInfo &xInfo){
	XWindowAttributes windowAttr;
	XGetWindowAttributes(xInfo.display, xInfo.window, &windowAttr);
	int newHeight = windowAttr.height;
	int newWidth = windowAttr.width;
	cout << newHeight << " " << newWidth << endl;


}

unsigned long now() { // change this timer ...
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
				case KeyRelease:

					handleKeyRelease(xInfo, event);
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
				case Expose:
					cout << "resizing" << endl;
					handleResizing(xInfo);
					break;
			}
		}
		
		unsigned long end = now();
		if (end - lastRepaint > 1000000/FPS) {
			handleAnimation(xInfo, inside);
			repaint(xInfo, inside);
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

	// list<Displayable *>::const_iterator begin = dList.begin();
	// list<Displayable *>::const_iterator end = dList.end();

	// while (begin != end) {
	// 	Displayable *d = *begin;
	// 	begin++;
	// 	delete d;
	// }

	// list<Bomb *>::const_iterator beginn = dBombList.begin();
	// list<Bomb *>::const_iterator endd = dBombList.end();
	// while (beginn != endd) {
	// 	Bomb *dq = *beginn;
	// 	beginn++;
	// 	delete dq;
	// }

	dList.clear();
	dBombList.clear();
}
