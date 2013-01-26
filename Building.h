#ifndef building_h
#define building_h

#include "XInfo.h"
#include "Displayable.h"

class Building : public Displayable {
	public:
		Building(double startingX);
		void paint(XInfo &xInfo);
		void move(XInfo &xInfo);
		void toggleSpeed();
		int getX();
		int getY();
		
	private:
		double x;
		double y;
		double speed;
		double height;
};

#endif