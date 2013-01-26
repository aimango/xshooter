#ifndef text_h
#define text_h

#include "XInfo.h"
#include <string>

class Text {  
	public:
		Text(int x, int y, std::string s);
		void paint(XInfo &xinfo, bool center);

	private:
		int x;
		int y;
		std::string s;
};

#endif
