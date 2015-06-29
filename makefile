
main: heu
	timeout 2 ./heu < input2  # simpile test to check whether bug exist or not
heu: nonoheu.o board.o
	g++ nonoheu.o board.o -g -o heu; find . | grep ".*\.\(c\|h\|cpp\)" | xargs etags -f tags
nonoheu.o: nonoheu.cpp board.h
	g++ nonoheu.cpp -c
board.o: board.cpp board.h
	g++ board.cpp -c
clean:
	rm -rf *.o