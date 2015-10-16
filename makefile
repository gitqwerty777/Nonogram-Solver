CXX=g++
CXXFLAGS=-g -pg -fno-omit-frame-pointer -O3

ifeq ($(mode),release) #if mode variable is empty, setting debug build mode
	mode=release
else
	mode=debug
	CXXFLAGS+=-D__DEBUG__ # define __DEBUG__
endif

all: information main

information:
	@echo "Building on "$(mode)" mode"
	@echo ".........................."

main: heu
heu: nonoheu.o dfsboard.o board.o
	$(CXX) $(CXXFLAGS) nonoheu.o board.o dfsboard.o -o heu; find . | grep ".*\.\(c\|h\|cpp\)" | xargs etags -f tags # generate emacs tags
test: heu
	sh runtest.sh # if not passed the test, return error

nonoheu.o: nonoheu.cpp dfsboard.h board.h nonogramReader.h
	$(CXX) $(CXXFLAGS) nonoheu.cpp -c
board.o: board.cpp board.h dfsboard.h
	$(CXX) $(CXXFLAGS) board.cpp -c
dfsboard.o: dfsboard.cpp dfsboard.h board.h
	$(CXX) $(CXXFLAGS) dfsboard.cpp -c

clean:
	rm -rf *.o
