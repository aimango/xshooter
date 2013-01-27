#ifndef bomb_h
#define bomb_h

#include "XInfo.h"
#include "Displayable.h"

class Bomb : public Displayable {
	public:
		Bomb(int x, int y, int velocityX, int fromEnemy);
		void paint(XInfo &xInfo);
		void move(XInfo &xInfo);
		double getX();
		double getY();
		double getSpeedY();

	private:
		double x;
		double y;
		double speedX;
		double speedY;
		int fromEnemy;
};

#endif