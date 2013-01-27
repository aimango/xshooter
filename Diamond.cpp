#include "Diamond.h"

Diamond::Diamond(int x, int y, int velocityX): x(x), y(y) {
	speedX = velocityX - velocityX/2;
	speedY = 3;
}

void Diamond::paint(XInfo &xInfo) {
	XDrawLine(xInfo.display, xInfo.window, xInfo.gc[4], x*xInfo.width/800, y*xInfo.height/600, (x+10)*xInfo.width/800, (y-10)*xInfo.height/600);
	XDrawLine(xInfo.display, xInfo.window, xInfo.gc[4], (x+10)*xInfo.width/800, (y-10)*xInfo.height/600, (x+20)*xInfo.width/800, y*xInfo.height/600);
	XDrawLine(xInfo.display, xInfo.window, xInfo.gc[4], (x+20)*xInfo.width/800, y*xInfo.height/600, (x+10)*xInfo.width/800, (y+10)*xInfo.height/600);
	XDrawLine(xInfo.display, xInfo.window, xInfo.gc[4], (x+10)*xInfo.width/800, (y+10)*xInfo.height/600, x*xInfo.width/800, y*xInfo.height/600);
}

void Diamond::move(XInfo &xInfo) {
	y = y + speedY;
	x = x + speedX;
}

double Diamond::getX() {
	return x;
}

double Diamond::getY() {
	return y;
}
