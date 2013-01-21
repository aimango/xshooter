#ifndef plane_h
#define plane_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Plane : public Displayable {
	public:
		Plane(int x, int y, int width, int height): x(x), y(y), width(width), height(height)  {
			velocityX = velocityY = 0;
			lives = 3;
		}

		virtual void paint(XInfo &xInfo) {
			/* draw a small triangle at the top-left corner of the window. */
			/* the triangle is made of a set of consecutive lines, whose   */
			/* end-point pixels are specified in the 'points' array.       */
			// TODO not sure how to set thickness of lines

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

			XPoint points[] = {
				{x-30, y-15},
				{x+0, y+30},
				{x-30, y-15},
				{x-30, y-15}
			};
			int npoints = sizeof(points)/sizeof(XPoint);
			XDrawLines(xInfo.display, xInfo.window, xInfo.gc[2], points, npoints, CoordModeOrigin);

			XFillArc(xInfo.display, xInfo.window, xInfo.gc[2], x, y, 
				width*xInfo.width/800, height*xInfo.height/600, 0, 360*64);
			XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[2], x-15, y+10, 
				30*xInfo.width/800, 10*xInfo.height/600);
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
			lives = 3;
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

		void kill (){
			x = 50;
			y = 50;
			lives--;
		}

	private:
		int x;
		int y;
		int width;
		int height;
		int velocityX;
		int velocityY;
		int lives;
};

#endif