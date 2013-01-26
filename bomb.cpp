#ifndef bomb_h
#define bomb_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Bomb : public Displayable {
	public:
		Bomb(int x, int y, int velocityX, int fromEnemy): x(x), y(y) {
			speedX = (velocityX > 0) ? velocityX - velocityX/2: velocityX + (velocityX/2*-1);
			speedY = (fromEnemy) ? -5 : 5;
			this->fromEnemy = fromEnemy;
		}

		void paint(XInfo &xInfo) {
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[2+fromEnemy], 
				x * xInfo.width/800, y * xInfo.height/600, 
				20 * xInfo.width/800, 20 * xInfo.height/600, 0, 360*64);

		}

		void move(XInfo &xInfo) {
			y = y + speedY;
			x = x + speedX;
		}

		double getX() {
			return x;
		}

		double getY() {
			return y;
		}

	private:
		double x;
		double y;
		double speedX;
		double speedY;
		int fromEnemy;
};

#endif