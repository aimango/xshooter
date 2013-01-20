#ifndef text_h
#define text_h

#include "xinfo.cpp"
#include "displayable.cpp"
#include <string>

class Text : public Displayable {  
	public:
		Text(int x, int y, std::string s):x(x), y(y), s(s)  {}

		virtual void paint(XInfo &xinfo){
			XDrawString( xinfo.display, xinfo.window, xinfo.gc[2], 
			   this->x, this->y, this->s.c_str(), this->s.length() );
		}

	private:
		int x;
		int y;
		std::string s;
};

#endif
