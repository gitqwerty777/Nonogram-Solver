CXX=g++
CXXFLAGS=-g -pg -fno-omit-frame-pointer -O3

ifeq ($(mode),debug) #if mode variable is empty, setting debug build mode
	mode=debug
	CXXFLAGS+=-D__DEBUG__ # define __DEBUG__
else
	mode=release
endif

all: information main

information:
	@echo "Building on "$(mode)" mode"
	@echo ".........................."

main: heu
heu: nonoheu.o dfsboard.o board.o
	$(CXX) $(CXXFLAGS) nonoheu.o board.o dfsboard.o -o heu; find . | grep ".*\.\(c\|h\|cpp\)" | xargs etags -f TAGS # generate emacs tags
test: heu
	sh runtest.sh # if not passed the test, return error

test100: heu
	./heu -t -n 100 -s 15 < 15question.txt > trash15mul200

test200: heu
	./heu -t -n 200 -s 15 < 15question.txt > trash15mul200

test290: heu
	./heu -t -n 290 -s 15 < 15question.txt > trash15mul200

test25: heu
	./heu -t -s 25 < testdata/tourament1 > test25

t1000: heu
	./heu -t -s 15 < testdata/tour15 

nonoheu.o: nonoheu.cpp dfsboard.h board.h nonogramReader.h nonogramWriter.h
	$(CXX) $(CXXFLAGS) nonoheu.cpp -c
board.o: board.cpp board.h dfsboard.h nonogramWriter.h
	$(CXX) $(CXXFLAGS) board.cpp -c
dfsboard.o: dfsboard.cpp dfsboard.h board.h nonogramWriter.h
	$(CXX) $(CXXFLAGS) dfsboard.cpp -c

clean:
	rm -rf *.o
