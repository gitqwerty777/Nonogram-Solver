CXX=g++

#if variable mode is empty, setting release build mode
ifeq ($(mode),debug)
	mode=debug
	CXXFLAGS=-std=c++11  -g -pg -fno-omit-frame-pointer -O3
	CXXFLAGS+=-D__DEBUG__ # define __DEBUG__
else
	mode=release
	CXXFLAGS=-std=c++11 -O3 -march=native
endif

all: information main

information:
	@echo "Building on "$(mode)" mode"
	@echo ".........................."

main: heu
heu: nonoheu.o dfsboard.o board.o
	$(CXX) $(CXXFLAGS) nonoheu.o board.o dfsboard.o -o heu; find . | grep ".*\.\(c\|h\|cpp\)" | xargs etags -f TAGS # generate emacs tags

# simple test, including question which has no solution
test: main
	bash runtest.sh
# testN: 15x15 questions
test100: main
	./heu -t -n 100 -s 15 < 15question.txt > output15-100problems
test200: main
	./heu -t -n 200 -s 15 < 15question.txt > output15-200problems
# 18.86 seconds in 2017/10/25
test290: main
	./heu -t -n 290 -s 15 < 15question.txt > output15-290problems
test1000: main
	./heu -t -n 1000 -s 15 < 15question.txt > output15-1000problems
# an hard problem in 15x15 board, 15 seconds in 2018/04/07
test15x15: main
	./heu -t -s 15 < testdata/tour15
# an easy problem in 25x25 board, run very slow
test25x25: main
	./heu -t -s 25 < testdata/tourament1 > output25x25

nonoheu.o: nonoheu.cpp dfsboard.h board.h nonogramReader.h nonogramWriter.h
	$(CXX) $(CXXFLAGS) nonoheu.cpp -c
board.o: board.cpp board.h dfsboard.h nonogramWriter.h
	$(CXX) $(CXXFLAGS) board.cpp -c
dfsboard.o: dfsboard.cpp dfsboard.h board.h nonogramWriter.h
	$(CXX) $(CXXFLAGS) dfsboard.cpp -c

clean:
	rm -f ./heu; rm -rf *.o
