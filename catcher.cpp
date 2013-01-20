#ifndef catcher_h
#define catcher_h

#include "xinfo.cpp"
#include "displayable.cpp"

class Catcher : public Displayable {

	public:
		void paint(XInfo &xInfo) {
			XFillArc(xInfo.display, xInfo.window, xInfo.gc[3], 
				x + 10, xInfo.height - y - 15, 
				30, 30, 0, 360*64);
		}

		void move(XInfo &xInfo) {
			x -= speed * xInfo.width/800;
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		int getIndex() {
			return buildingIndex;
		}

		void setNewXY(int x, int y){
			this->x = x;
			this->y = y;
		}
		void remove(){
			x= -100;
			y= -100;
			speed = 0;
		}

		Catcher(int x, int y, int buildingIndex): x(x), y(y), buildingIndex(buildingIndex) {
			speed = 5;
		}

	private:
		int x;
		int y;
		int speed;
		int buildingIndex;
};

#endif