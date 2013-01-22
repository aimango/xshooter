#ifndef bomb_h
#define bomb_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Bomb : public Displayable {
	public:
		Bomb(int x, int y, int velocityX, int fromEnemy): x(x), y(y) {
			speedX = (velocityX > 0) ? velocityX - 5 : velocityX + 5;
			speedY = (fromEnemy) ? -5 : 5;
			this->fromEnemy = fromEnemy;
		}

		void paint(XInfo &xInfo) {
			if (x > - 15 && y < xInfo.height + 15) {
				XFillArc(xInfo.display, xInfo.window, xInfo.gc[2+fromEnemy], x, y, 
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
		int fromEnemy;
};

#endif