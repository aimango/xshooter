#ifndef catcher_h
#define catcher_h

#include <iostream>
#include "xinfo.cpp"
#include "displayable.cpp"
#include "bomb.cpp"

class Catcher : public Displayable {
	public:
		Catcher(int x, int y): x(x), y(y) {
			speed = 5;
			attackRate = 1;
		}

		void paint(XInfo &xInfo) {
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[3], 
				(x-10)*xInfo.width/800, (xInfo.height-y-15)*xInfo.height/600, 
				30*xInfo.height/600, 30*xInfo.width/800, 0, 360*64);
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

		void incrementRate() {
			attackRate++;
			if (attackRate == 150)
				attackRate = 0;
		}

		int getRate(){
			return attackRate;
		}

		int getSpeed(){
			return speed;
		}

		void remove(){
			x= -100;
			y= -100;
			speed = 0;
		}

	private:
		int attackRate;
		double x;
		double y;
		int speed;
};

#endif