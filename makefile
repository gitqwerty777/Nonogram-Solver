CXX=g++
CXXFLAGS=-g

ifeq ($(mode),release) #if mode variable is empty, setting debug build mode
	mode=release
else
	mode=debug
	CXXFLAGS+="-D __DEBUG__" # define __DEBUG__
endif

all: information main

information:
ifneq ($(mode),release)
ifneq ($(mode),debug)
	@echo "Invalid build mode." 
	@echo "Please use 'make mode=release' or 'make mode=debug'"
	@exit 1
endif
endif
	@echo "Building on "$(mode)" mode"
	@echo ".........................."
main: heu
	timeout 5 ./heu < input3  # simpile test to check whether bug exist or not
heu: nonoheu.o board.o dfsboard.o
	$(CXX) nonoheu.o board.o dfsboard.o -o heu; find . | grep ".*\.\(c\|h\|cpp\)" | xargs etags -f tags # generate emacs tags
nonoheu.o: nonoheu.cpp board.h dfsboard.h
	$(CXX) $(CXXFLAGS) nonoheu.cpp -c
board.o: board.cpp board.h 
	$(CXX) $(CXXFLAGS) board.cpp -c
dfsboard.o: dfsboard.cpp dfsboard.h
	$(CXX) $(CXXFLAGS) dfsboard.cpp -c
clean:
	rm -rf *.o
