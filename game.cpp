/**
 * Elisa Lou e3lou 20372456
 * CS 349 A01 - X Side Scroller
 */

#include <iostream>
#include <deque>
#include <cstring>
#include <sys/time.h>
#include <sstream>

// X functions
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// my classes
#include "xinfo.cpp"
#include "catcher.cpp"
#include "bomb.cpp"
#include "text.cpp"
#include "plane.cpp"
#include "building.cpp"

using namespace std;


// TODO:
// resizing fix - easier to use the ratio in the repaint
// collision detection - done
// 3 lives implementation
// 'catchers' on the buildings - done
// keyboard acceleration - done
// memory dealloc - better?
// game pausing - done ish
// splash screen - need to fig out how to increase font
// need better MVC structure ** - YAAAAAA

const int Border = 5;
const int BufferSize = 10;
const int FPS = 30;

/*
 * Function to put out a message on error exits.
 */
void error (string msg) {
	cerr << msg << endl;
	exit(0);
}

string convertToString (int i) {
	string s;
	stringstream out;
	out << i;
	return out.str();
}

struct Game {

};


deque<Bomb *> dBombList;				// list of Bombs
deque<Displayable *> dList;				// list of Displayables


Plane plane(100, 100, 30, 20);
Building building(600, 0); //xInfo.height

void setResizeVars(XInfo &xInfo) {
	XWindowAttributes windowAttr;
	XGetWindowAttributes(xInfo.display, xInfo.window, &windowAttr);
	xInfo.height = windowAttr.height;
	xInfo.width = windowAttr.width;
}

void handleResizing(XInfo &xInfo){
	setResizeVars(xInfo);
	building.setNewXY(xInfo.width, xInfo.height);

	cout << xInfo.height << " " << xInfo.width << endl;
}

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
	unsigned long black;

	// seed the random # generator
	srand (time(NULL));

	/*
	 * Display opening uses the DISPLAY	environment variable.
	 * It can go wrong if DISPLAY isn't set, or you don't have permission.
	 */	
	xInfo.display = XOpenDisplay( ":0" );
	if (!xInfo.display)	{
		error("Can't open display.");
	}
	
	xInfo.width = 800;
	xInfo.height = 600;
	/*
	 * Find out some things about the display you're using.
	 */
	xInfo.screen = DefaultScreen( xInfo.display );

	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = xInfo.width;
	hints.height = xInfo.height;

	hints.min_width = 600; // !??! 
	hints.min_height = 400;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow ( 
		xInfo.display,						// display where window appears
		DefaultRootWindow(xInfo.display), // window's parent in window tree
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
	makeBlankCursor(xInfo);

	// masks
	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | KeyReleaseMask | ExposureMask);

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
void repaint( XInfo &xInfo, int splash, int numBombs) {
	if (!splash) {
		//XClearWindow( xInfo.display, xInfo.window ); // flickers a lot
		
		XWindowAttributes windowInfo;
		XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
		unsigned int height = windowInfo.height;
		unsigned int width = windowInfo.width;

		//re-draws the backgound
		XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[0], 0, 0, width, height);
		for (int i = 0; i < (int)dList.size(); i++) {
			dList[i]->paint(xInfo);
		}

		// indicate # bombs left
		string text = numBombs>0 ? "Number of Bombs: "+convertToString(numBombs) : "No more bombs!";
		Text numBombs(10, 10, text);
		numBombs.paint(xInfo);

		// indicate # lives left
		text = plane.getLives() >0 ? "Number of Lives: "+convertToString(plane.getLives()) : "No more lives! GAME OVER";
		Text numLives(10, 25, text);
		numLives.paint(xInfo);

		// collision detection - bombs & catchers
		for (int j = 0; j< (int)dBombList.size(); j++) {
			int dBombX = dBombList[j]->getX();
			int dBombY = dBombList[j]->getY();
			if (dBombX < -50 || dBombY > xInfo.width + 50 || dBombX < building.getX())
				continue;

			deque<Catcher *> dCatcherList = building.getCatcherList();
			for (int i = 0; i < (int)dCatcherList.size(); i++) {
				int dCatcherX = dCatcherList[i]->getX();
				int dCatcherY = xInfo.height - dCatcherList[i]->getY();

				if  (dCatcherX > -15 && dCatcherX < xInfo.width
					&& dBombY + 20 > dCatcherY && dBombY < dCatcherY + 30
					&& dBombX + 20 > dCatcherX && dBombX < dCatcherX + 30){

					// cout << dCatcherX << " " << dCatcherY << endl;
					// cout << dBombX << " " << dBombY << endl;
					cout <<" HIT " << endl;
					dBombList[j]->remove();
					dCatcherList[i]->remove();
					break;
					
				}
			}
		}

		//collision detected - plane and buildings
		int buildingX = building.getX();
		deque<int> heights = building.getHeights();
		int start = buildingX > 0 ? 0 : (0-buildingX)/ 50;
		int dPlaneX = plane.getX();
		int dPlaneY = plane.getY();
		for (int i = start; i < start + xInfo.width/50 + 2; i++) {

			if (dPlaneY + 20 > xInfo.height - heights[i] &&
				dPlaneX + 20 > buildingX + i * 50 && dPlaneX < buildingX + 50 * (i+1) ) {
					cout << "Plane crashed into building!" << endl;
					cout << buildingX + i * 50 << " " << xInfo.height - heights[i] << endl;
					cout << dPlaneX << " " << dPlaneY << endl;
					plane.kill();
					break;
			}

		}

		XFlush( xInfo.display );

	} else {
		string lineOne = "Elisa Lou 456. Use w-a-s-d keys to move around the helicopter, and m to make bombs.";
		string lineTwo = "Press c to continue. Press q to terminate the game at any time.";

		XClearWindow (xInfo.display, xInfo.window);
		setResizeVars(xInfo);
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
			XNextEvent (xInfo.display, &event);
			isRetriggered = 1;
		}
	}

	if (!isRetriggered) {

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

void handleKeyPress(XInfo &xInfo, XEvent &event, int &splash, int &numBombs) {
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
				numBombs--;
				if (numBombs >= 0){
					Bomb *bomb = new Bomb(plane.getX(), plane.getY());
					dList.push_back(bomb);
					dBombList.push_front(bomb);
				} 
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

void handleAnimation(XInfo &xInfo, int splash) {
	if (!splash) {
		building.move(xInfo);

		deque<Catcher *> dCatcherList = building.getCatcherList();
		for (int i = 0; i< (int)dCatcherList.size(); i++){
			dCatcherList[i]->move(xInfo);
		}
		for (int i = 0; i < (int)dBombList.size(); i++){
			dBombList[i]->move(xInfo);
		}
	}
}

unsigned long now() { // change this timer ...
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xInfo) {	
	XEvent event;
	unsigned long lastRepaint = 0;
	int splash = 1;
	int numBombs = 50;

	// Add stuff to paint to the display list

	dList.push_back(&building);
	dList.push_front(&plane);

	deque<Catcher *> leCatchers = building.getCatcherList();
	for (int i = 0; i < (int)leCatchers.size() ; i++) {
		cout <<"addy" <<endl;
		dList.push_front(leCatchers[i]);
	}

	while(true) {
		if (XPending(xInfo.display) > 0) {
			XNextEvent( xInfo.display, &event );
			switch( event.type ) {
				case ButtonPress:
					handleButtonPress(xInfo, event);
					break;
				case KeyPress:
					handleKeyPress(xInfo, event, splash, numBombs);
					break;
				case KeyRelease:
					handleKeyRelease(xInfo, event);
					break;
				case Expose:
					cout << "resizing" << endl;
					handleResizing(xInfo);
					break;
			}
		}
		
		unsigned long end = now();
		if (end - lastRepaint > 1000000/FPS) {
			handleAnimation(xInfo, splash);
			repaint(xInfo, splash, numBombs);
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


	for (int i = 0; i < (int)dList.size(); i++) {
		delete dList[i];
	}
	for (int i = 0; i < (int)dBombList.size(); i++) {
		delete dBombList[i];
	}

	//	delete xInfo.display; ??!!
	dList.clear();
	dBombList.clear();
}
