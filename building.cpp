#ifndef building_h
#define building_h

#include "xinfo.cpp"
#include "displayable.cpp"
#include <deque>
#include <cstdlib>


class Building : public Displayable {
	public:
		Building(double startingX){
			x = startingX;
			y = 40 * (rand() % 9 + 1);
		}

		virtual void paint(XInfo &xInfo) {
			//cout << y << endl;
			double newy = (600 - y); // why is this fucked up
			//cout << xInfo.height << " " << newy << endl;
			XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], 
				x * xInfo.width/800, newy * xInfo.height / 600,
				50 * xInfo.width/800, y * xInfo.height / 600);
		}

		void move(XInfo &xInfo) {
			x -= xInfo.gameSpeed;
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		void toggleSpeed() {
			if (speed == 5)
				speed = 1;
			else
				speed = 5;
		}

	private:
		double x;
		double y;
		double speed;
		double height;
};

#endif