#include "Bomb.h"

Bomb::Bomb(int x, int y, int velocityX, int fromEnemy): x(x), y(y) {
	speedX = (velocityX > 0) ? (velocityX - velocityX/2) : (velocityX - velocityX/2);
	speedY = (fromEnemy) ? -5 : 5;
	this->fromEnemy = fromEnemy;
}

void Bomb::paint(XInfo &xInfo) {
	int width = fromEnemy ? 20 : 10;
	XFillArc(xInfo.display, xInfo.window, xInfo.gc[1+fromEnemy], 
		x * xInfo.width/800, y * xInfo.height/600, 
		width * xInfo.width/800, 20 * xInfo.height/600, 0, 360*64);
	XDrawArc(xInfo.display, xInfo.window, xInfo.gc[0], 
		x * xInfo.width/800, y * xInfo.height/600, 
		width * xInfo.width/800, 20 * xInfo.height/600, 0, 360*64);
}

void Bomb::move(XInfo &xInfo) {
	y = y + speedY;
	x = x + speedX;
}

double Bomb::getX() {
	return x;
}

double Bomb::getY() {
	return y;
}

double Bomb::getSpeedY() {
	return speedY;
}
