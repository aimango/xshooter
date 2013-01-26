#ifndef text_h
#define text_h

#include "XInfo.h"
#include "Displayable.h"
#include <string>

class Text : public Displayable {  
	public:
		Text(int x, int y, std::string s);
		void paint(XInfo &xinfo);

	private:
		int x;
		int y;
		std::string s;
};

#endif
