#include "Star.h"
#include <math.h>


Star::Star(int x, int y, int velocityX): x(x), y(y) {
	speedX = velocityX - velocityX/2;
	speedY = 3;
}

void Star::paint(XInfo &xInfo) {
	
	double ratio = xInfo.height/600 > xInfo.width/800 ? xInfo.width/800 : xInfo.height/600;
	
	// creds for coordinates: http://stuff.mit.edu/afs/athena/astaff/source/src-8.4/third/xscreensaver/hacks/deluxe.c Jamie Zawinski
	XPoint points[11];
	x += 0.5;
	int size = int(13 * ratio);
	int size2 = int(5 * ratio);
	double circumference = 3.14159 * 2;
	double o = -3.14159 / 2;

	points[0].x = x + size  * cos(o + 0.0*circumference); 
	points[0].y = y + size  * sin(o + 0.0*circumference);
	points[1].x = x + size2 * cos(o + 0.1*circumference); 
	points[1].y = y + size2 * sin(o + 0.1*circumference);
	points[2].x = x + size  * cos(o + 0.2*circumference); 
	points[2].y = y + size  * sin(o + 0.2*circumference);
	points[3].x = x + size2 * cos(o + 0.3*circumference); 
	points[3].y = y + size2 * sin(o + 0.3*circumference);
	points[4].x = x + size  * cos(o + 0.4*circumference); 
	points[4].y = y + size  * sin(o + 0.4*circumference);
	points[5].x = x + size2 * cos(o + 0.5*circumference); 
	points[5].y = y + size2 * sin(o + 0.5*circumference);
	points[6].x = x + size  * cos(o + 0.6*circumference); 
	points[6].y = y + size  * sin(o + 0.6*circumference);
	points[7].x = x + size2 * cos(o + 0.7*circumference); 
	points[7].y = y + size2 * sin(o + 0.7*circumference);
	points[8].x = x + size  * cos(o + 0.8*circumference); 
	points[8].y = y + size  * sin(o + 0.8*circumference);
	points[9].x = x + size2 * cos(o + 0.9*circumference); 
	points[9].y = y + size2 * sin(o + 0.9*circumference);
	points[10] = points[0];

	XDrawLines (xInfo.display, xInfo.window, xInfo.gc[4], points, sizeof(points)/sizeof(XPoint), CoordModeOrigin);
}

void Star::move(XInfo &xInfo) {
	y = y + speedY;
	x = x + speedX;
}

double Star::getX() {
	return x;
}

double Star::getY() {
	return y;
}
