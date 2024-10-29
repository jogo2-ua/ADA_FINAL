#JAVIER ORTS GARCÍA 49370397T

OPTS = -Wall -Wextra -O3 -std=c++0x

all: maze_bb.cc
	g++ ${OPTS} -o maze_bb maze_bb.cc

tar: maze_bb.cc makefile memoria.pdf
	tar cvfz 49370397T.tar.gz maze_bb.cc makefile memoria.pdf