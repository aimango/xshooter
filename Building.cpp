#include "Building.h"
#include <cstdlib>

Building::Building(double startingX){
	x = startingX;
	y = 40 * (rand() % 9 + 1);
}

void Building::paint(XInfo &xInfo) {

	XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[2], 
	 	x * xInfo.width/800, (600 - y) * xInfo.height / 600,
	 	50 * xInfo.width/800, y * xInfo.height / 600);

	for (int i = 0; i < y/40; i++) {
		XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[0], 
			x * xInfo.width/800, (600 - y+i*40) * xInfo.height / 600,
			50 * xInfo.width/800, (40 * xInfo.height / 600));
	}

}

void Building::move(XInfo &xInfo) {
	x -= xInfo.gameSpeed;
}

void Building::toggleSpeed() {
	if (speed == 5)
		speed = 1;
	else
		speed = 5;
}

int Building::getX() {
	return x;
}

int Building::getY() {
	return y;
}
