#include "Text.h"

Text::Text(int x, int y, std::string s):x(x), y(y), s(s)  {}

void Text::paint(XInfo &xinfo){
	XDrawString( xinfo.display, xinfo.window, xinfo.gc[2], 
	   this->x, this->y, this->s.c_str(), this->s.length() );
}