CXX=g++
CXXFLAGS=-g -pg -fno-omit-frame-pointer

ifeq ($(mode),release) #if mode variable is empty, setting debug build mode
	mode=release
else
	mode=debug
	CXXFLAGS+=-D__DEBUG__ # define __DEBUG__
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

main: heu test
heu: nonoheu.o dfsboard.o board.o 
	$(CXX) $(CXXFLAGS) nonoheu.o board.o dfsboard.o -o heu; find . | grep ".*\.\(c\|h\|cpp\)" | xargs etags -f tags # generate emacs tags
test: nonoheu.o dfsboard.o board.o 
	sh testInput.sh # if not passed the test, return error

# can these deleted?
#nonoheu.o: nonoheu.cpp dfsboard.h board.h 
#	$(CXX) $(CXXFLAGS) nonoheu.cpp -c
#board.o: board.cpp board.h dfsboard.h
#	$(CXX) $(CXXFLAGS) board.cpp -c
#dfsboard.o: dfsboard.cpp dfsboard.h board.h
#	$(CXX) $(CXXFLAGS) dfsboard.cpp -c

clean:
	rm -rf *.o
