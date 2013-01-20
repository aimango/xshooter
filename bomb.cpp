#ifndef bomb_h
#define bomb_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Bomb : public Displayable {
	public:
		void paint(XInfo &xInfo) {
			//TODO: need to grab initial velocity of the plane
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[2], x, y, 
				20 * xInfo.height/600, 20 * xInfo.height/600, 0, 360*64);
		}

		void move(XInfo &xInfo) {
			y = y + speed;
			x = x - speed;
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		void remove(){
			x= -100;
			y= -100;
			speed = 0;
		}

		Bomb(int x, int y): x(x), y(y) {
			speed = 5;
		}

	private:
		int x;
		int y;
		int speed;
};

#endif