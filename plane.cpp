#ifndef plane_h
#define plane_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Plane : public Displayable {
	public:
		Plane(int x, int y): x(x), y(y),{
			width = 30;
			heigh = 20;
			velocityX = velocityY = 0;
			lives = 3;
		}

		virtual void paint(XInfo &xInfo) {
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

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		int getLives(){
			return lives;
		}

		void reset(){
			x = 50;
			y = 50;
			lives = 1;
		}

		void setVelocityX(int dir) {
			if (dir) {
				velocityX += 10;
			}
			else {
				velocityX -= 10;
			}
		}

		void setVelocityY(int dir) {
			if (dir) {
				velocityY += 10;
			}
			else {
				velocityY -= 10;
			}
		}

		int getVelocityX() {
			return velocityX;
		}

		int getVelocityY() {
			return velocityY;
		}

		void kill (){
			lives--;
		}

	private:
		double x;
		double y;
		double width;
		double height;
		double velocityX;
		double velocityY;
		int lives;
};

#endif