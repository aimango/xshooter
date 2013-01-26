#ifndef catcher_h
#define catcher_h

#include "XInfo.h"
#include "Displayable.h"


class Catcher : public Displayable {
	public:
		Catcher(int x, int y);
		void paint(XInfo &xInfo);
		void move(XInfo &xInfo);
		void incrementRate();
		int getX();
		int getY();
		int getRate();

	private:
		int attackRate;
		double x;
		double y;
};

#endif