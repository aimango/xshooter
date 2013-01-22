#ifndef bomb_h
#define bomb_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Bomb : public Displayable {
	public:
		Bomb(int x, int y, int velocityX, int ydir): x(x), y(y) {
			speedX = (velocityX > 0) ? velocityX - 5 : velocityX + 5;
			speedY = (ydir > 0) ? 5 : -5;
		}

		void paint(XInfo &xInfo) {
			//TODO: need to grab initial velocity of the plane
			if (x > - 15 && y < xInfo.height + 15) {
				XFillArc(xInfo.display, xInfo.window, xInfo.gc[2], x, y, 
					20 * xInfo.height/600, 20 * xInfo.height/600, 0, 360*64);
			}
		}

		void move(XInfo &xInfo) {
			y = y + speedY;
			x = x + speedX;
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
			speedX = 0;
			speedY = 0;
		}

	private:
		int x;
		int y;
		int speedX;
		int speedY;
};

#endif