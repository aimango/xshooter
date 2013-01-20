CXX = g++
XLIBFLAGS = -lX11 -lstdc++ 
LIB_FLAGS = -L/usr/X11R6/lib 
CXXFLAGS =  -g -Wall -MMD 
OBJECTS = xinfo.o displayable.o catcher.o building.o plane.o bomb.o text.o game.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = gamee

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${LIB_FLAGS} ${OBJECTS} ${XLIBFLAGS} -o ${EXEC}

clean :
	rm -rf ${DEPENDS} ${OBJECTS} ${EXEC}

-include ${DEPENDS}
