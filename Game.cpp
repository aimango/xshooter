/**
 * Elisa Lou e3lou 20372456
 * CS 349 A01 - X Side Scroller
 */

#include <iostream>
#include <deque>
#include <cstring>
#include <sys/time.h>
#include <sstream>
#include <cstdlib>

// X functions
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// my classes
#include "XInfo.h"
#include "Catcher.h"
#include "Bomb.h"
#include "Diamond.h"
#include "Text.h"
#include "Plane.h"
#include "Building.h"

using namespace std;


// TODO:

// flickering on resizing - b.t buildings


// DONE:
// scale diamonds properly - changed to diamonds
// readme for extra features
// outline the graphics?
// grader mode - use diff color
// better splash screen instructions
// drop diamonds for more life!
// memory dealloc - better?
// do grader mode
// need better MVC structure - use header files too
// ****get rid of the remove () functions and use deque erase. 
// create `game over` screen - done
// 'new game' screen
// collision
// shooting from enemy
// 3 lives implementation - done
// catchers on the buildings - done
// keyboard acceleration - done
// game pausing - done
// resizing fix - goooood 

const int Border = 5;
const int BufferSize = 10;
const int FPS = 40;

int bestScore = 0;
int score = 0;
Plane plane(50, 50);
deque<Bomb *> dBombList;
deque<Diamond *> dDiamondList;
deque<Catcher *> dCatcherList;
deque<Building *> dBuildingList;

string convertToString (int i) {
	string s;
	stringstream out;
	out << i;
	return out.str();
}

// delete everything!
void memoryDealloc(){
	for (int i = 0; i < (int)dBombList.size(); i++) {
		delete dBombList[i];
	}
	for (int i = 0; i < (int)dDiamondList.size(); i++) {
		delete dDiamondList[i];
	}
	for (int i = 0; i < (int)dCatcherList.size(); i++) {
		delete dCatcherList[i];
	}
	for (int i = 0; i < (int)dBuildingList.size(); i++) {
		delete dBuildingList[i];
	}

	dBombList.clear();
	dDiamondList.clear();
	dCatcherList.clear();
	dBuildingList.clear();
}

// Function to put out a message on error exits.
void error(string msg){
	cerr << msg << endl;
	memoryDealloc();
	exit(0);
}

// set up GC colors - coloring borrowed from http://slist.lilotux.net/linux/xlib/color-drawing.c
void setGCColors(XInfo &xInfo) {
	Colormap screen_colormap;
	Status rc;
	XColor color;
	string colors[] = {
		"plum",
		"dark slate grey",
		"#AFEEEE",
		"#008080"
	};

	// use black
	XSetForeground(xInfo.display, xInfo.gc[0], BlackPixel(xInfo.display, xInfo.screen));

	// use white
	XSetForeground(xInfo.display, xInfo.gc[1], WhitePixel(xInfo.display, xInfo.screen));

	screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
	for (int i = 2; i < 6; i++) {
		rc = XAllocNamedColor(xInfo.display, screen_colormap, colors[i-2].c_str(), &color, &color);
		if (rc == 0) {
			error("XAllocNamedColor - failed to allocated color.");
		}
		XSetForeground(xInfo.display, xInfo.gc[i], color.pixel);
	}
}

// source http://www.gamedev.net/topic/285005-anyone-knows-how-to-hideshow-mouse-pointer-under-linux-using-opengl/
void makeBlankCursor(XInfo &xInfo) {
	XColor dummy;
	char data[1] = {0};

	// make a blank cursor
	Pixmap blank = XCreateBitmapFromData (xInfo.display, xInfo.window, data, 1, 1);
	if(blank == None) {
		error("error: out of memory. can't create blank pixmap from data");
	}
	Cursor cursor = XCreatePixmapCursor(xInfo.display, blank, blank, &dummy, &dummy, 0, 0);
	XFreePixmap (xInfo.display, blank);

	// set the blank cursor
	XDefineCursor(xInfo.display, xInfo.window, cursor);
}

// initialize X and create window
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long black;

	/*
	 * Display opening uses the DISPLAY	environment variable.
	 * It can go wrong if DISPLAY isn't set, or you don't have permission.
	 */	
	xInfo.display = XOpenDisplay(":0");
	if (!xInfo.display)	{
		error("Can't open display.");
	}
	
	xInfo.width = 800;
	xInfo.height = 600;
	xInfo.screen = DefaultScreen(xInfo.display);

	black = XBlackPixel(xInfo.display, xInfo.screen);

	hints.x = 100;
	hints.y = 100;
	hints.width = xInfo.width;
	hints.height = xInfo.height;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow ( 
		xInfo.display,						// display where window appears
		DefaultRootWindow(xInfo.display), 	// window's parent in window tree
		hints.x, hints.y,					// upper left corner location
		hints.width, hints.height,			// size of the window
		Border,								// width of window's border
		black,								// window border colour
		black);								// window background colour
		
	XSetStandardProperties(
		xInfo.display,					// display containing the window
		xInfo.window,					// window whose properties are set
		"A01 Side Scrolling Game",		// window's title
		"Planes",						// icon's title
		None,							// pixmap for the icon
		argv, argc,						// applications command line args
		&hints);						// size hints for the window

	// set the game speed (varies based on grader mode vs normal mode)
	xInfo.gameSpeed = 5;

	// Create Graphic Contexts
	for (int i = 0; i <= 5; i++){
		xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
		XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
		XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
		XSetLineAttributes(xInfo.display, xInfo.gc[i], 2, LineSolid, CapButt, JoinRound);
	}
	setGCColors(xInfo);
	
	// masks
	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | KeyReleaseMask | ExposureMask);

	// turn off keyboard autorepeat
	XAutoRepeatOff(xInfo.display);

	// seed the random # generator
	srand(time(NULL));

	// hide the mouse cursor
	makeBlankCursor(xInfo);

	// load default font
	string fontName = "fixed";
	xInfo.font = XLoadQueryFont(xInfo.display, fontName.c_str());
	if (!xInfo.font) {
	    error("Error Loading Font");
	}

	// Put the window on the screen
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);

	// let server get set up before sending drawing commands
	sleep(2);
}

void newGame(XInfo &xInfo){
	plane.reset();
	memoryDealloc();
	score = 0;
	xInfo.gameSpeed = 5;
}

/**
 * create new building and catcher when the last building moves far enough.
 * delete building when off screen.
 */
void handleBuildingsAndCatchers(XInfo &xInfo) {
	int lastElt = dBuildingList.size()-1;
	if (dBuildingList.empty() || dBuildingList[lastElt]->getX() < 825 ) {
		double positionX = dBuildingList.empty() ? xInfo.width : dBuildingList[lastElt]->getX() + 50;
		Building *b = new Building(positionX);
		dBuildingList.push_back(b);
	
		// spawn an enemy
		if (rand() % 5 == 1) {
			Catcher *c = new Catcher(positionX + 20, b->getY());
			dCatcherList.push_back(c);
		}
	} else if (!dBuildingList.empty() && dBuildingList[0]->getX() < -50) {
		delete dBuildingList[0];
		dBuildingList.pop_front();
	}
	if (!dCatcherList.empty() && dCatcherList[0]->getX() < -100){
		delete dCatcherList[0];
		dCatcherList.pop_front();
	}
}

// erase bombs and diamonds when off screen.
void handleBombsAndDiamonds(XInfo &xInfo) {
	for (int i = 0; i < (int)dBombList.size(); i++) {
		if (dBombList[i]->getX() < -20 || (dBombList[i]->getSpeedY() > 0 && dBombList[i]->getY() > xInfo.height + 20)
			|| (dBombList[i]->getSpeedY() < 0 && dBombList[i]->getY() < -20)){
			delete dBombList[i];
			dBombList.erase(dBombList.begin() + i);
		}
	}
	for (int i = 0; i < (int)dDiamondList.size(); i++) {
		if (dDiamondList[i]->getX() < -20 || dDiamondList[i]->getY() > xInfo.height){
			delete dDiamondList[i];
			dDiamondList.erase(dDiamondList.begin() + i);
		}
	}
}


void handleCollisionDetection(XInfo &xInfo) {
	for (int j = 0; j< (int)dBombList.size(); j++) {
		double dBombX = dBombList[j]->getX();
		double dBombY = dBombList[j]->getY();

		// bombs & catchers
		for (int i = 0; i < (int)dCatcherList.size(); i++) {
			double dCatcherX = dCatcherList[i]->getX();
			double dCatcherY = 600 - dCatcherList[i]->getY();

			if  (dBombY + 10 > dCatcherY && dBombY - 10 < dCatcherY + 30
				&& dBombX + 10 > dCatcherX - 20 && dBombX - 10 < dCatcherX + 20){
					cout <<"Hit :)" << endl;
					score ++;
					delete dBombList[j];
					delete dCatcherList[i];
					dBombList.erase(dBombList.begin() + j);
					dCatcherList.erase(dCatcherList.begin() + i);
					break;
			}
		}

		// plane and bombs
		double dPlaneX = plane.getX();
		double dPlaneY = plane.getY();
		if (dBombY > dPlaneY - 20 && dBombY < dPlaneY + 20 &&
			dBombX > dPlaneX - 20 && dBombX < dPlaneX + 20) {
				cout << "Plane ran into enemy bomb! Life--" << endl;
				plane.kill();
				break;
		}
	}

	// plane and diamonds
	for (int j = 0; j< (int)dDiamondList.size(); j++) {
		double dDiamondX = dDiamondList[j]->getX();
		double dDiamondY = dDiamondList[j]->getY();

		double dPlaneX = plane.getX();
		double dPlaneY = plane.getY();
		if (dDiamondY > dPlaneY - 20 && dDiamondY < dPlaneY + 30 &&
			dDiamondX > dPlaneX - 20 && dDiamondX < dPlaneX + 30) {
				cout << "Plane caught a diamond! Life++" << endl;
				plane.incrementLives();
				delete dDiamondList[j];
				dDiamondList.erase(dDiamondList.begin() + j);
		}
	}

	// plane and buildings
	for (int i = 0; i < (int)dBuildingList.size(); i++){
		double buildingX = dBuildingList[i]->getX();
		double buildingY = dBuildingList[i]->getY();

		double dPlaneX = plane.getX();
		double dPlaneY = plane.getY();

		if (dPlaneY + 20 > (600 - buildingY) &&
			dPlaneX + 20 > buildingX && dPlaneX < buildingX + 50) {
				cout << "Plane crashed into building!" << endl;
				plane.kill();
				break;
		}
	}

	// bombs and buildings
	for (int j = 0; j< (int)dBombList.size(); j++) {
		double dBombX = dBombList[j]->getX();
		double dBombY = dBombList[j]->getY();

		// don't bother if they are enemy bombs.
		if (dBombList[j]->getSpeedY() < 0){ 
			continue;
		}

		for (int i = 0; i < (int)dBuildingList.size(); i++) {
			double buildingX = dBuildingList[i]->getX();
			double buildingY = 600 - dBuildingList[i]->getY();

			if  (buildingX > -15 
				&& buildingX < xInfo.width && dBombY + 10 > buildingY
				&& dBombX + 10 > buildingX - 15 && dBombX - 10 < buildingX + 15){
					delete dBombList[j];
					dBombList.erase(dBombList.begin() + j);
					break;
			}
		}
	}
}


void handleResizing(XInfo &xInfo){
	XWindowAttributes windowAttr;
	XGetWindowAttributes(xInfo.display, xInfo.window, &windowAttr);

	// force minimum 600 x 400 window size
	if (windowAttr.height < 400) {
		XResizeWindow(xInfo.display, xInfo.window, windowAttr.width, 400);
	} 
	if (windowAttr.width < 600) {
		XResizeWindow(xInfo.display, xInfo.window, 600, windowAttr.height);
	}
	xInfo.height = windowAttr.height;
	xInfo.width = windowAttr.width;
}

// repaint all the objects
void repaint( XInfo &xInfo, int splash, int &paused) {

	handleResizing(xInfo);

	if (plane.getLives() <= 0) {
		if (score > bestScore){
			bestScore = score;
		}
		paused = 0;
		string lineOne = "GAME OVER.";
		string lineTwo = convertToString(score)+ " PTS";
		if (score == 0) {
			lineTwo = "TRY AGAIN .. YOU GOT " + lineTwo;
		}
		else {
			lineTwo = (score == bestScore) ? 
				("YOU CURRENTLY HOLD THE HIGHEST SCORE OF " + lineTwo): ("HIGH SCORE: " + convertToString(bestScore) + " YOUR SCORE: " + lineTwo);
		}
		string lineThree = "Press c to play again or q to quit the game.";

		XClearWindow (xInfo.display, xInfo.window);
		Text line1(1, xInfo.height/2 - 20, lineOne);
		Text line2(1, xInfo.height/2, lineTwo);
		Text line3(1, xInfo.height/2 + 20, lineThree);
		line1.paint(xInfo, 1);
		line2.paint(xInfo, 1);
		line3.paint(xInfo, 1);
	}
	else if (splash){

		XClearWindow (xInfo.display, xInfo.window);
		if (paused) {
			string lineZero = "Press c to continue gameplay. Press q to quit the game.";
			Text line0(1, xInfo.height/2-20, lineZero);
			line0.paint(xInfo, 1);
		} 
		else {
			string name = "Elisa Lou 456.";
			string lineZero = "INSTRUCTIONS:";
			string lineOne = "Use w-a-s-d keys to move around the helicopter, and m to drop bombs at enemies.";
			string lineTwo = "Avoid getting attacked by enemy missiles. Pick up diamonds to gain life.";
			string lineThree = "Press c to diamondt gameplay. Press q to terminate the game at any time.";
			string lineFour = "GRADER MODE: Press g during gameplay to toggle.";

			Text lineName(1, xInfo.height/2-50, name);
			Text line0(1, xInfo.height/2-20, lineZero);
			Text line1(1, xInfo.height/2, lineOne);
			Text line2(1, xInfo.height/2 + 20, lineTwo);
			Text line3(1, xInfo.height/2 + 40, lineThree);
			Text line4(1, xInfo.height/2 + 70, lineFour);
			
			lineName.paint(xInfo, 1);
			line0.paint(xInfo, 1);
			line1.paint(xInfo, 1);
			line2.paint(xInfo, 1);
			line3.paint(xInfo, 1);
			line4.paint(xInfo, 1);
		}
	}
	else {
		paused = 1;

		XWindowAttributes windowInfo;
		XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
		unsigned int height = windowInfo.height;
		unsigned int width = windowInfo.width;

		//re-draws the backgound
		int colorIndex = xInfo.gameSpeed == 2 ? 3 : 5;
		XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[colorIndex], 0, 0, width, height);

		// repaint everything!
		plane.paint(xInfo);
		for (int i = 0; i < (int)dCatcherList.size(); i++) {
			dCatcherList[i]->paint(xInfo);
		}
		for (int i = 0; i < (int)dBombList.size(); i++) {
			dBombList[i]->paint(xInfo);
		}
		for (int i = 0; i < (int)dDiamondList.size(); i++) {
			dDiamondList[i]->paint(xInfo);
			//cout << i << "*__*" << endl;
		}
		for (int i = 0; i < (int)dBuildingList.size(); i++) {
			dBuildingList[i]->paint(xInfo);
		}

		// indicate # lives left
		string text = "Number of Lives: " + convertToString(plane.getLives());
		Text numLives(10, 20, text);
		numLives.paint(xInfo, 0);

		// indicate current score
		text = "Score: " + convertToString(score);
		Text scoring(10, 40, text);
		scoring.paint(xInfo, 0);

		handleBuildingsAndCatchers(xInfo);
		handleCollisionDetection(xInfo);
		handleBombsAndDiamonds(xInfo);
		XFlush(xInfo.display);
	}
}

void handleAnimation(XInfo &xInfo, int splash) {
	if (!splash) {
		for (int i = 0; i < (int)dBuildingList.size(); i++) {
			dBuildingList[i]->move(xInfo);
		}
		for (int i = 0; i < (int)dBombList.size(); i++){
			dBombList[i]->move(xInfo);
		}
		for (int i = 0; i < (int)dDiamondList.size(); i++){
			dDiamondList[i]->move(xInfo);
		}
		for (int i = 0; i< (int)dCatcherList.size(); i++){
			dCatcherList[i]->move(xInfo);
			dCatcherList[i]->incrementRate();

			int rate = dCatcherList[i]->getRate();
			if (rate % 100 == 0) {

				// shoot bombs upwards 95% of the time
				if ((rand() % 20 + 1) > 1) {
					Bomb *bomb = new Bomb(dCatcherList[i]->getX(), 600 - dCatcherList[i]->getY() - 40, -2*xInfo.gameSpeed, 1);
					dBombList.push_back(bomb);
				}
				// drop a life diamond 5% of the time, at random x locations.
				else {
					Diamond *diamond = new Diamond(100*(rand() % 6 + 2), 50, -2*xInfo.gameSpeed);
					dDiamondList.push_back(diamond);
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
		NULL);					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		switch (text[0]){
			case 'q':
				error("Terminating normally.");
				break;
			case 'w':
				plane.setVelocityY(0);
				break;
			case 'a':
				plane.setVelocityX(0);
				break;
			case 's':
				plane.setVelocityY(1);
				break;
			case 'd':
				plane.setVelocityX(1);
				break;
			case 'c':
				if (plane.getLives() <= 0){
					newGame(xInfo);
				}
				splash = 0;
				break;
			case 'f': case 'F':
				splash = 1;
				break;
			case 'g':
				xInfo.gameSpeed = xInfo.gameSpeed == 5 ? 2 : 5;
				break;
			case 'm': {
				Bomb *bomb = new Bomb(plane.getX(), plane.getY()+20, plane.getVelocityX(), 0);
				dBombList.push_back(bomb);
				break;
			}
		}
	}
}

void handleKeyRelease(XInfo &xInfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];

	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL);					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		switch (text[0]){
			case 'w':
				plane.setVelocityY(1);
				break;
			case 'a':
				plane.setVelocityX(1);
				break;
			case 's':
				plane.setVelocityY(0);
				break;
			case 'd':
				plane.setVelocityX(0);
				break;
		}
	}
}

unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xInfo) {	
	XEvent event;
	unsigned long lastRepaint = 0;
	int splash = 1;
	int paused = 0;

	while(true) {
		if (XPending(xInfo.display) > 0) {
			XNextEvent(xInfo.display, &event);
			switch(event.type) {
				case KeyPress:
					handleKeyPress(xInfo, event, splash);
					break;
				case KeyRelease:
					handleKeyRelease(xInfo, event);
					break;
				case Expose:
					handleResizing(xInfo);
					break;
			}
		}
		
		unsigned long end = now();
		if (end - lastRepaint > 1000000/FPS) {
			handleAnimation(xInfo, splash);
			repaint(xInfo, splash, paused);
			lastRepaint = now();
		}
		if (XPending(xInfo.display) == 0) {
			usleep(1000000/FPS - (end - lastRepaint));
		}
	}
}

/*
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up.
 *   Deallocate memory.
 */
int main (int argc, char *argv[]) {
	XInfo xInfo;
	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
	memoryDealloc();
}
