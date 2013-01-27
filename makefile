CXX = g++
XLIBFLAGS = -lX11 -lstdc++ 
LIB_FLAGS = -L/usr/X11R6/lib 
CXXFLAGS =  -g -Wall -MMD 
OBJECTS = Catcher.o Building.o Plane.o Bomb.o Diamond.o Text.o Game.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = assign1

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${LIB_FLAGS} ${OBJECTS} ${XLIBFLAGS} -o ${EXEC}

clean :
	rm -rf ${DEPENDS} ${OBJECTS} ${EXEC}

-include ${DEPENDS}

run :
	./${EXEC}

all :
	make ${EXEC} && ./${EXEC}