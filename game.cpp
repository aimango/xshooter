#include <iostream>
#include <vector>
#include <deque>
#include <cstring>
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
// collision detection
// 'catchers' on the buildings
// keyboard acceleration - doneish
// memory dealloc - figure out how to avoid ptrs.. 
// game pausing - done ish
// splash screen - need to fig out how to increase font
// need better MVC structure

const int Border = 5;
const int BufferSize = 10;
const int FPS = 30;
/*
 * Information to draw on the window.
 */
struct XInfo {
	int height;
	int width;
	Display *display;
	int screen;
	Window window;
	GC gc[4];
	XFontStruct *font;
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
			if (x < 0) {
				x = 0;
			} else if (x > xInfo.width) {
				x = xInfo.width;
			}

			y += velocityY;
			if (y < 0) {
				y = 0;
			} else if (y > xInfo.height) {
				y = xInfo.height;
			}

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
				velocityX += 10;
			}
			else {
				velocityX -= 10;
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

class Catcher : public Displayable {

	public:
		void paint(XInfo &xInfo) {
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[3], x+10, xInfo.height-y-15, 30, 30, 0, 360*64);
		}

		void move(XInfo &xInfo) {
			x -= speed;
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		void remove(){
			x= -100;
			y= -100;
			speed = 0;
		}

		Catcher(int x, int y): x(x), y(y){
			speed = 5;
		}

	private:
		int x;
		int y;
		int speed;
};

deque<Catcher *> dCatcherList; 			// list of Catchers
deque<Displayable *> dList;				// list of Displayables

class Building : public Displayable {
	public:
		virtual void paint(XInfo &xInfo) {
			int start = x > 0 ? 0 : (0-x)/ 50;
			//cout << start << endl;
			for (int i = start; i < start + 18; i++) {
				XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x+i*50, y+xInfo.height-heights[i], 50, heights[i]);
			}
		}

		void move(XInfo &xInfo) {
			x -= speed;
		}

		Building(int x, int y): x(x), y(y) {
			speed = 5;
			for (int i = 0 ; i < 150; i++) {
				int height = rand() % 400 + 50;
				heights.push_back( height );
				int person = rand()%4;
				if (person == 1) {
					Catcher *c = new Catcher(x+i*50, height);
					dCatcherList.push_back(c);
					dList.push_front(c);
				}

			}
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		deque<int> getHeights() {
			return heights;
		}

	private:
		int x;
		int y;
		int speed;
		deque<int> heights;
};


class Text : public Displayable {  
	public:
	virtual void paint(XInfo &xinfo)
	{  
		XDrawString( xinfo.display, xinfo.window, xinfo.gc[2], 
	       this->x, this->y, this->s.c_str(), this->s.length() );
	}

	// constructor
	Text(int x, int y, string s):x(x), y(y), s(s)  {}

	private:
	int x;
	int y;
	string s;
};


Plane plane(100, 100, 30, 30);
Building building(600, 0); //xInfo.height


class Bomb : public Displayable {

	public:
		void paint(XInfo &xInfo) {
			//TODO: need to grab initial velocity of the plane
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[2], x, y, 20, 20, 0, 360*64);
		}

		void move(XInfo &xInfo) {
			y = y + speed;
			x = x - speed;
		}


		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		void remove(){
			x= -100;
			y= -100;
			speed = 0;
		}

		Bomb(int x, int y): x(x), y(y) {
			speed = 5;
		}

	private:
		int x;
		int y;
		int speed;
};


deque<Bomb *> dBombList;				// list of Bombs
deque<Building *> dBuildingList;		// list of Buildings

/** 
 * Set up colors
 */ 
void setGCColors(XInfo &xInfo) {

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
	XColor purple;
	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	rc = XAllocNamedColor(xInfo.display, screen_colormap, "medium purple", &purple, &purple);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'medium purple' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[1], purple.pixel);
	
	// use white
	XSetForeground(xInfo.display, xInfo.gc[2], WhitePixel(xInfo.display, xInfo.screen));

	XColor pink;
	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	rc = XAllocNamedColor(xInfo.display, screen_colormap, "pink", &pink, &pink);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'pink' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[3], pink.pixel);
}

void makeBlankCursor(XInfo &xInfo) {
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

}

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
	
	xInfo.width = 800;
	xInfo.height = 600;
	/*
	 * Find out some things about the display you're using.
	 */
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = xInfo.width;
	hints.height = xInfo.height; //!?!?!1

	hints.min_width = 600;
	hints.min_height = 400;
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
	for (int i = 0; i <= 3; i++){
		xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
		XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
		XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
		XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt, JoinRound);
	}

	setGCColors(xInfo);


	// masks
	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | KeyReleaseMask |
		EnterWindowMask | LeaveWindowMask | ExposureMask);

	makeBlankCursor(xInfo);

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	
	XFlush(xInfo.display);
	//sleep(2);	// let server get set up before sending drawing commands
}


/*
 * Function to repaint a display list
 */
void repaint( XInfo &xInfo, int splash) {
	if (!splash) {
		//XClearWindow( xInfo.display, xInfo.window ); // flickers a lot
		
		XWindowAttributes windowInfo;
		XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
		unsigned int height = windowInfo.height;
		unsigned int width = windowInfo.width;

		//re-draws the backgound
		XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[0], 0, 0, width, height);
		for (int i = 0; i < dList.size(); i++) {
			dList[i]->paint(xInfo);
		}


		// collision detection > <

		deque<int> heights = building.getHeights();

		for (int j = 0; j< dBombList.size(); j++) {
			int dBombX = dBombList[j]->getX();
			int dBombY = dBombList[j]->getY();
			if (dBombX < 0 || dBombY > xInfo.width || dBombX < building.getX())
				continue;

			for (int i = 0; i < dCatcherList.size(); i++) {
				int dCatcherX = dCatcherList[i]->getX();
				int dCatcherY = dCatcherList[i]->getY();

				if  (dCatcherX > 0 && dCatcherX < xInfo.width){
					if ( dBombY + 10 > dCatcherY - 15 
						&& dBombX + 20 > dCatcherX - 30 && dBombX - 20 < dCatcherX + 30){

						cout << dCatcherX << " " << dCatcherY << endl;
						cout << dBombX << " " << dBombY << endl;
						cout <<" HITTT " << endl;
						dBombList[j]->remove();
						dCatcherList[i]->remove();
						break;
					}
				}
			}

			// int index = dBombList[i]->getX()/50 + start ; //!?!??!
			

			// if (dBombList[i]->getY() >= xInfo.height - heights[index]) {
			// 	cout << "index " << index << endl;
			// 	cout << "HITTT" <<endl;
			// 	dBombList[i]->remove();
			// }

		}

		XFlush( xInfo.display );

	} else {
		XClearWindow (xInfo.display, xInfo.window);
		string lineOne = "Elisa Lou 456. Use w-a-s-d keys to move around the helicopter, and m to make bombs.";
		string lineTwo = "Press c to continue. Press q to terminate the game at any time.";
		Text line(10, xInfo.height/2, lineOne);
		Text line2(10, xInfo.height/2 + 10, lineTwo);
		line.paint(xInfo);
		line2.paint(xInfo);
	}
}

void handleButtonPress(XInfo &xInfo, XEvent &event) {
	cout << "Got button press!" << endl;
}

void handleKeyRelease(XInfo &xInfo, XEvent &event) {
	
	// deleting auto-repeated keypress events:
	// from http://stackoverflow.com/questions/2100654/ignore-auto-repeat-in-x11-applications
	int isRetriggered = 0;

	if (XEventsQueued(xInfo.display, QueuedAfterReading)) {
		XEvent peekEvent;
		XPeekEvent(xInfo.display, &peekEvent);

		if (peekEvent.type == KeyPress && peekEvent.xkey.time == event.xkey.time &&
			peekEvent.xkey.keycode == event.xkey.keycode){ 

			// remove retriggered KeyPress event
			// cout << "key was retriggered" << endl;
			XNextEvent (xInfo.display, &event);
			isRetriggered = 1;
		}
	}

	if (!isRetriggered) {
		// cout << "Key was released" << endl;

		KeySym key;
		char text[BufferSize];

		int i = XLookupString( 
			(XKeyEvent *)&event, 	// the keyboard event
			text, 					// buffer when text will be written
			BufferSize, 			// size of the text buffer
			&key, 					// workstation-independent key symbol
			NULL );					// pointer to a composeStatus structure (unused)
		if ( i == 1) {
			// printf("Got key release -- %c\n", text[0]);
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
}

void handleKeyPress(XInfo &xInfo, XEvent &event, int &splash) {
	KeySym key;
	char text[BufferSize];
	
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		// printf("Got key press -- %c\n", text[0]);
		switch (text[0]){
			case 'q':
				error("Terminating normally.");
				break;
			case 'm': {
				Bomb *bomb = new Bomb(plane.getX(), plane.getY());
				dList.push_back(bomb);
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
			case 'c': {
				splash = 0;
				break;
			}
			case 'f': case 'F': {
				splash = 1;
				break;
			}
		}
	}
}

void handleMotion(XInfo &xInfo, XEvent &event, int inside) {
	//plane.moveto(event.xbutton.x, event.xbutton.y);	
}

void handleAnimation(XInfo &xInfo, int inside, int splash) {
	if (!splash) {
		building.move(xInfo);
		for (int i = 0; i< dCatcherList.size(); i++){
			dCatcherList[i]->move(xInfo);
		}
		for (int i = 0; i < dBombList.size(); i++){
			dBombList[i]->move(xInfo);
		}
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
	dList.push_back(&building);
	dList.push_front(&plane);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;
	int splash = 1;
	while(true) {
		if (XPending(xInfo.display) > 0) {
			XNextEvent( xInfo.display, &event );
			switch( event.type ) {
				case ButtonPress:
					handleButtonPress(xInfo, event);
					break;
				case KeyPress:
					handleKeyPress(xInfo, event, splash);
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
			handleAnimation(xInfo, inside, splash);
			repaint(xInfo, splash);
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


	for (int i = 0; i < dList.size(); i++) {
		delete dList[i];
	}
	for (int i = 0; i < dBombList.size(); i++) {
	 	delete dBombList[i];
	}

	//	delete xInfo.display;
	dList.clear();
	dBombList.clear();
}
