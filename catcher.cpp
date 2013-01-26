#ifndef catcher_h
#define catcher_h

#include "xinfo.cpp"
#include "displayable.cpp"


class Catcher : public Displayable {
	public:
		Catcher(int x, int y): x(x), y(y) {
			attackRate = 1;
		}

		void paint(XInfo &xInfo) {
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[3], 
				(x-10)*xInfo.width/800, (600-y-15)*xInfo.height/600,
				30*xInfo.height/600, 30*xInfo.width/800, 0, 360*64);
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

		void incrementRate() {
			attackRate++;
			if (attackRate == 150)
				attackRate = 0;
		}

		int getRate(){
			return attackRate;
		}

	private:
		int attackRate;
		double x;
		double y;
};

#endif