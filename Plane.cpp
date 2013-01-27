#include "Plane.h"

Plane::Plane(int x, int y): x(x), y(y){
	width = 30;
	height = 20;
	velocityX = velocityY = 0;
	lives = 3;
}

void Plane::paint(XInfo &xInfo) {
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

	XFillArc(xInfo.display, xInfo.window, xInfo.gc[2],
		x * xInfo.width/800, y * xInfo.height/600, 
		width * xInfo.width/800, height * xInfo.height/600, 0, 360*64);
	XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[2], 
		(x-15) * xInfo.width/800, (y+10) * xInfo.height/600, 
		30 * xInfo.width/800, 10 * xInfo.height/600);
}


void Plane::reset(){
	x = 50;
	y = 50;
	lives = 3;
}

void Plane::kill(){
	x = 50;
	y = 50;
	lives--;
}

void Plane::setVelocityX(int dir) {
	if (dir) {
		velocityX += 10;
	}
	else {
		velocityX -= 10;
	}
}

void Plane::setVelocityY(int dir) {
	if (dir) {
		velocityY += 10;
	}
	else {
		velocityY -= 10;
	}
}

int Plane::getVelocityX() {
	return velocityX;
}

int Plane::getVelocityY() {
	return velocityY;
}

int Plane::getX() {
	return x;
}

int Plane::getY() {
	return y;
}

int Plane::getLives(){
	return lives;
}

