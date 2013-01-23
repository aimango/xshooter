#ifndef building_h
#define building_h

#include "xinfo.cpp"
#include "displayable.cpp"
#include <iostream>
#include <deque>
#include <cstdlib>

using namespace std;

class Building : public Displayable {
	public:
		Building(double startingX){
			speed = 5;
			x = startingX;
			y = 40 * (rand() % 9 + 1);
		}

		virtual void paint(XInfo &xInfo) {
			//cout << y << endl;
			double newy = (xInfo.height - y); // why is this fucked up
			//cout << xInfo.height << " " << newy << endl;
			XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], 
				x*xInfo.width/800, newy * xInfo.height / 600,
				50*xInfo.width/800, (xInfo.height - newy)* xInfo.height / 600);
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

	private:
		double x;
		double y;
		double speed;
		double height;
};

#endif