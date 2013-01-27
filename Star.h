#ifndef star_h
#define star_h

#include "XInfo.h"
#include "Displayable.h"

class Star : public Displayable {
	public:
		Star(int x, int y, int velocityX);
		void paint(XInfo &xInfo);
		void move(XInfo &xInfo);
		double getX();
		double getY();

	private:
		double x;
		double y;
		double speedX;
		double speedY;
};

#endif