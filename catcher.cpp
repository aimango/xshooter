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
				x*xInfo.width/800-10, xInfo.height-y*xInfo.height/600-15, 
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
			if (attackRate == 100)
				attackRate = 0;
		}

		int getRate(){
			return attackRate;
		}

		int getSpeed(){
			return speed;
		}

		// void setNewXY(int x, int y){
		// 	this->x = x;
		// 	this->y = y;
		// }
		void remove(){
			x= -100;
			y= -100;
			speed = 0;
		}

	private:
		int attackRate;
		int x;
		int y;
		int speed;
};

#endif