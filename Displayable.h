/*
 * An abstract class representing displayable things. 
 */

#ifndef displayable_h
#define displayable_h

#include "XInfo.h"


class Displayable {
	public:
		virtual void paint(XInfo &xInfo) = 0;
};

#endif