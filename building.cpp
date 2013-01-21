#ifndef building_h
#define building_h

#include "xinfo.cpp"
#include "catcher.cpp"
#include <deque>
#include <cstdlib>

class Building : public Displayable {
	public:
		Building(int x, int y): x(x), y(y) {
			speed = 5;
			for (int i = 0; i < 150; i++) {
				int height = rand() % 400 + 50;
				heights.push_back( height );
				int person = rand()%4;
				if (person == 1) {
					Catcher *c = new Catcher(x+i*50, height, i);
					dCatcherList.push_back(c);	
				}

			}
		}

		virtual void paint(XInfo &xInfo) {
			int start = x>0 ? 0 : (0-x)/50;
			for (int i = start; i < start + xInfo.width/50 + 2; i++) {
				XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], 
					x + i*50*xInfo.width/800, y + xInfo.height - heights[i]*xInfo.height/600, 
					50*xInfo.width/800, heights[i]*xInfo.height/600);
			}
		}

		void move(XInfo &xInfo) {
			x -= speed * xInfo.width/800;
		}

		int getX() {
			return x;
		}

		void resetX(int x){
			this->x = x;
		}
		std::deque<int> getHeights(){
			return heights;
		}

		std::deque<Catcher *> getCatcherList(){
			return dCatcherList;
		}

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
		std::deque<int> heights;
		std::deque<Catcher *> dCatcherList;
};

#endif