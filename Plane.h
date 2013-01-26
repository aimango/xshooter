#ifndef plane_h
#define plane_h

#include "XInfo.h"
#include "Displayable.h"

class Plane : public Displayable {
	public:
		Plane(int x, int y);
		void paint(XInfo &xInfo);
		void setVelocityX(int dir);
		void setVelocityY(int dir);
		void reset();
		void kill();
		int getVelocityX();
		int getVelocityY();
		int getX();
		int getY();
		int getLives();

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