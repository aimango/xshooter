#include "Text.h"

Text::Text(int x, int y, std::string s):x(x), y(y), s(s)  {}

void Text::paint(XInfo &xInfo, bool center){
	if (center) {
		int string_width = XTextWidth(xInfo.font, this->s.c_str(), this->s.length());
		x = (xInfo.width - string_width) / 2;
	}
	XDrawString(xInfo.display, xInfo.window, xInfo.gc[2], 
	   this->x, this->y, this->s.c_str(), this->s.length());
}