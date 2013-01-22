#ifndef building_h
#define building_h

#include "xinfo.cpp"
#include "displayable.cpp"
#include <deque>
#include <cstdlib>

class Building : public Displayable {
	public:
		Building(int startingX){
			speed = 5;
			x = startingX;
			y = random() % 400 + 50;
		}

		virtual void paint(XInfo &xInfo) {
			// int start = x>0 ? 0 : (0-x)/50;
			// for (int i = start; i < start + xInfo.width/50 + 2; i++) {
			XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], 
				x *xInfo.width/800, (xInfo.height - y)*xInfo.height/600, 
				50*xInfo.width/800, y*xInfo.height/600);
			// }
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

		void resetX(int x){
			this->x = x;
		}
		// std::deque<int> getHeights(){
		// 	return heights;
		// }

		// std::deque<Catcher *> getCatcherList(){
		// 	return dCatcherList;
		// }

		// void setNewXY(int newWidth, int newHeight){
		// 	for (int i = 0; i < (int) heights.size(); i++) {
		// 		heights[i] = heights[i] * newHeight / 600;
		// 	}
		// 	//x = x * newWidth / 800;
		// 	for (int i = 0; i < (int) dCatcherList.size(); i++) {
		// 		dCatcherList[i]->setNewXY( (x+dCatcherList[i]->getIndex() *50) * newWidth/800, dCatcherList[i]->getY() * newHeight/600);
		// 	}
		// }

	private:
		int x;
		int y;
		int speed;
		int height;
		// std::deque<int> heights;
};

#endif