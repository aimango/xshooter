#include "Catcher.h"

Catcher::Catcher(int x, int y): x(x), y(y) {
	attackRate = 1;
}

void Catcher::paint(XInfo &xInfo) {
	XFillArc(xInfo.display, xInfo.window, xInfo.gc[3], 
		(x-10)*xInfo.width/800, (600-y-15)*xInfo.height/600,
		30*xInfo.height/600, 30*xInfo.width/800, 0, 360*64);
}

void Catcher::move(XInfo &xInfo) {
	x -= xInfo.gameSpeed;
}

void Catcher::incrementRate() {
	attackRate++;
	if (attackRate == 150){
		attackRate = 0;
	}
}


int Catcher::getX() {
	return x;
}

int Catcher::getY() {
	return y;
}

int Catcher::getRate(){
	return attackRate;
}