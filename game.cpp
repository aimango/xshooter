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
// resizing fix - collision detection will need to be updated too.
// memory dealloc - better?
// create `game over` screen - done
// 'new game' screen
// splash screen - need to fig out how to increase font
// need better MVC structure - use header files too

// collision detection 
	//bomb & plane
// shooting from enemy

// 3 lives implementation - done
// catchers on the buildings - done
// keyboard acceleration - done
// game pausing - done

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

int score = 0;
Plane plane(50, 50, 30, 20);
deque<Bomb *> dBombList;
deque<Catcher *> dCatcherList;
deque<Building *> dBuildingList;


void setResizeVars(XInfo &xInfo) {
	XWindowAttributes windowAttr;
	XGetWindowAttributes(xInfo.display, xInfo.window, &windowAttr);
	xInfo.height = windowAttr.height;
	xInfo.width = windowAttr.width;
}

void handleResizing(XInfo &xInfo){
	setResizeVars(xInfo);
	//building.setNewXY(xInfo.width, xInfo.height);
	cout << xInfo.height << " " << xInfo.width << endl;
}

// set up GC colors - coloring borrowed from http://slist.lilotux.net/linux/xlib/color-drawing.c
void setGCColors(XInfo &xInfo) {

	// use midnight blue 
	Colormap screen_colormap;
	Status rc;

	XColor blue;
	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	rc = XAllocNamedColor(xInfo.display, screen_colormap, "midnight blue", &blue, &blue);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'midnight blue' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[0], blue.pixel);

	// use medium purple
	XColor purple;
	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	rc = XAllocNamedColor(xInfo.display, screen_colormap, "medium purple", &purple, &purple);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'medium purple' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[1], purple.pixel);
	
	// use white
	XSetForeground(xInfo.display, xInfo.gc[2], WhitePixel(xInfo.display, xInfo.screen));

	// use pink
	XColor pink;
	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	rc = XAllocNamedColor(xInfo.display, screen_colormap, "pink", &pink, &pink);
	if (rc == 0) {
		error("XAllocNamedColor - failed to allocated 'pink' color.");
	}
	XSetForeground(xInfo.display, xInfo.gc[3], pink.pixel);
}


// hide cursor
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
	// XUndefineCursor(xInfo.display, xInfo.window);
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
		DefaultRootWindow(xInfo.display), 	// window's parent in window tree
		hints.x, hints.y,					// upper left corner location
		hints.width, hints.height,			// size of the window
		Border,								// width of window's border
		black,								// window border colour
		black );							// window background colour
		
	XSetStandardProperties(
		xInfo.display,				// display containing the window
		xInfo.window,				// window whose properties are set
		"Side Scrolling Game",		// window's title
		"Plane",					// icon's title
		None,						// pixmap for the icon
		argv, argc,					// applications command line args
		&hints );					// size hints for the window

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
		ButtonPressMask | KeyPressMask | KeyReleaseMask | ExposureMask);

	// turn off keyboard autorepeat
	XAutoRepeatOff(xInfo.display);

	makeBlankCursor(xInfo);
	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);
	//sleep(2);	// let server get set up before sending drawing commands
}

void handleBuildingsAndCatchers(XInfo &xInfo) {
	int lastElt = dBuildingList.size()-1;
	if (dBuildingList.empty() || dBuildingList[lastElt]->getX() < xInfo.width ) {
		int positionX = dBuildingList.empty() ? xInfo.width : dBuildingList[lastElt]->getX() + 50;
		Building *b = new Building(positionX);
		dBuildingList.push_back(b);
	
		int person = rand()%4;
		if (person == 1) {
			Catcher *c = new Catcher(positionX + 20, b->getY());
			dCatcherList.push_back(c);
		}
	} else if (!dBuildingList.empty() && dBuildingList[0]->getX() < -50) {
		dBuildingList.pop_front();
	}
	if (!dCatcherList.empty() && dCatcherList[0]->getX() < -100){
		dCatcherList.pop_front();
	}
}

void handleBombs(XInfo &xInfo) {
	for (int i = 0; i < (int)dBombList.size(); i++) {
		if (dBombList[i]->getX() < -20 || dBombList[i]->getY() > xInfo.height || dBombList[i]->getY() < -20)
			dBombList.erase(dBombList.begin() + i);
	}
}

void handleCollisionDetection(XInfo &xInfo) {
	
	for (int j = 0; j< (int)dBombList.size(); j++) {
		int dBombX = dBombList[j]->getX();
		int dBombY = dBombList[j]->getY();

		// collision detection - bombs & catchers
		for (int i = 0; i < (int)dCatcherList.size(); i++) {
			int dCatcherX = dCatcherList[i]->getX();
			int dCatcherY = xInfo.height - dCatcherList[i]->getY();

			if  (dBombY + 10 > dCatcherY && dBombY - 10 < dCatcherY + 30
				&& dBombX + 10 > dCatcherX - 15 && dBombX - 10 < dCatcherX + 15){

				 // cout << dCatcherX << " " << dCatcherY << endl;
				 // cout << dBombX << " " << dBombY << endl;
				cout <<"Hit :)" << endl;
				score ++;
				dBombList[j]->remove();
				dCatcherList[i]->remove();
				break;
			}
		}

		//collision detection - plane and bombs - fix coords
		int dPlaneX = plane.getX();
		int dPlaneY = plane.getY();
		if (dPlaneY > dBombY && dPlaneY < dBombY + 20 &&
			dPlaneX > dBombX && dPlaneX < dBombX + 20) {
				cout << "Plane ran into bomb!" << endl;
				// cout << buildingX + i * 50 << " " << xInfo.height - heights[i] << endl;
				// cout << dPlaneX << " " << dPlaneY << endl;
				plane.kill();
				break;
		}
	}

	//collision detection - plane and buildings
	for (int i = 0; i < (int)dBuildingList.size(); i++){
		int buildingX = dBuildingList[i]->getX();
		int buildingY = dBuildingList[i]->getY();

		int dPlaneX = plane.getX();
		int dPlaneY = plane.getY();
		if (dPlaneY + 20 > xInfo.height - buildingY &&
			dPlaneX + 20 > buildingX && dPlaneX < buildingX + 50) {
				cout << "Plane crashed into building!" << endl;
				// cout << buildingX + i * 50 << " " << xInfo.height - heights[i] << endl;
				// cout << dPlaneX << " " << dPlaneY << endl;
				plane.kill();
				break;
		}
	}

	// //collision detection - bombs and buildings
	// for (int j = 0; j< (int)dBombList.size(); j++) {
	// 	int dBombX = dBombList[j]->getX();
	// 	int dBombY = dBombList[j]->getY();

	// 	for (int i = 0; i < (int)dBuildingList.size(); i++) {
	// 		int buildingX = dBuildingList[i]->getX();
	// 		int buildingY = xInfo.height - dBuildingList[i]->getY();

	// 		if  (buildingX > -15 && buildingX < xInfo.width
	// 			&& dBombY + 10 > buildingY && dBombY - 10 < buildingY + 30
	// 			&& dBombX + 10 > buildingX - 15 && dBombX - 10 < buildingX + 15){

	// 			 // cout << dCatcherX << " " << dCatcherY << endl;
	// 			 // cout << dBombX << " " << dBombY << endl;
	// 			cout <<"Hit :)" << endl;
	// 			score ++;
	// 			dBombList[j]->remove();
	// 			break;
	// 		}
	// 	}
	// }
}


/*
 * Function to repaint a display list
 */
void repaint( XInfo &xInfo, int splash, int numBombs, int &paused) {
	if (plane.getLives() <= 0) {
		paused = 0;
		string lineOne = "GAME OVER.";
		string lineTwo = "SCORE: "+convertToString(score);
		string lineThree = "Press c to play again or q to quit the game.";

		setResizeVars(xInfo);
		XClearWindow (xInfo.display, xInfo.window);
		Text line(xInfo.width/2-40, xInfo.height/2 - 20, lineOne);
		Text line2(xInfo.width/2-40, xInfo.height/2, lineTwo);
		Text line3(xInfo.width/2-135, xInfo.height/2 + 20, lineThree);
		line.paint(xInfo);
		line2.paint(xInfo);
		line3.paint(xInfo);
	}
	else if (splash){
		XClearWindow (xInfo.display, xInfo.window);
		if (paused) {
			string lineZero = "Press c to continue gameplay. Press q to quit the game.";
			Text line0(xInfo.width/2-180, xInfo.height/2-20, lineZero);
			line0.paint(xInfo);
		} else {
			string lineZero = "Elisa Lou 456.";
			string lineOne = "Use w-a-s-d keys to move around the helicopter, and m to make bombs.";
			string lineTwo = "Press c to start gameplay. Press q to terminate the game at any time.";

			Text line0(xInfo.width/2-40, xInfo.height/2-20, lineZero);
			Text line(xInfo.width/2-200, xInfo.height/2, lineOne);
			Text line2(xInfo.width/2-180, xInfo.height/2 + 20, lineTwo);
			line.paint(xInfo);
			line2.paint(xInfo);
			line0.paint(xInfo);
		}
		setResizeVars(xInfo);
	}
	else {
		//XClearWindow( xInfo.display, xInfo.window ); // flickers a lot
		paused = 1;
		XWindowAttributes windowInfo;
		XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
		unsigned int height = windowInfo.height;
		unsigned int width = windowInfo.width;

		//re-draws the backgound
		XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[0], 0, 0, width, height);

		/*
		 * Repaint everything
		 */
		plane.paint(xInfo);
		for (int i = 0; i < (int)dCatcherList.size(); i++) {
			dCatcherList[i]->paint(xInfo);
		}
		for (int i = 0; i < (int)dBombList.size(); i++) {
			dBombList[i]->paint(xInfo);
		}
		for (int i = 0; i < (int)dBuildingList.size(); i++) {
			dBuildingList[i]->paint(xInfo);
		}

		// indicate # bombs left
		string text = numBombs>0 ? "Number of Bombs: "+convertToString(numBombs) : "No more bombs!";
		Text numBombs(10, 10, text);
		numBombs.paint(xInfo);

		// indicate # lives left
		text = plane.getLives() >0 ? "Number of Lives: "+convertToString(plane.getLives()) : "No more lives! GAME OVER";
		Text numLives(10, 25, text);
		numLives.paint(xInfo);

		// indicate current score
		text = "Score: "+convertToString(score);
		Text scoring(10, 40, text);
		scoring.paint(xInfo);

		handleBuildingsAndCatchers(xInfo);
		handleCollisionDetection(xInfo);
		handleBombs(xInfo);
		XFlush( xInfo.display );
	}
}

void handleButtonPress(XInfo &xInfo, XEvent &event) {
	cout << "Got button press!" << endl;
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
					Bomb *bomb = new Bomb(plane.getX(), plane.getY(), plane.getVelocityX(), 0);
					dBombList.push_back(bomb);
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
				if (plane.getLives() <= 0){ // faack, have to reset everything
					plane.reset();
					numBombs = 50;
					score = 0;
					//building.resetX(600);
				}
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
		for (int i = 0; i < (int)dBuildingList.size(); i++) {
			dBuildingList[i]->move(xInfo);
		}
		for (int i = 0; i< (int)dCatcherList.size(); i++){
			dCatcherList[i]->move(xInfo);
			dCatcherList[i]->incrementRate();
			int rate = dCatcherList[i]->getRate();
			if (rate % 50 == 0) {
				Bomb *bomb = new Bomb(
					dCatcherList[i]->getX(), xInfo.height - dCatcherList[i]->getY() - 30, -10, 1);
				dBombList.push_back(bomb);
			}
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
	int paused = 0;

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
			repaint(xInfo, splash, numBombs, paused);
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

	for (int i = 0; i < (int)dBombList.size(); i++) {
		delete dBombList[i];
	}
	for (int i = 0; i < (int)dCatcherList.size(); i++) {
		delete dCatcherList[i];
	}
	for (int i = 0; i < (int)dBuildingList.size(); i++) {
		delete dBuildingList[i];
	}

	//delete xInfo.display; 
	dBombList.clear();
	dCatcherList.clear();
	dBuildingList.clear();
}
