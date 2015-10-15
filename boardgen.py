#!/usr/bin/env python
import random
import sys
import re

def usage():
    print 'Usage: boardgen.py n num p1 p2 seed'
    print ''
    print 'Example: boardgen.py 25 1000 0.5 0.35 12345'
    print 'seed could be very very long -- 19937 bits'

def output(n, idx, board):
    fp = open('question.txt', 'a')
    fp.write('$%d\n' % idx)
    rows = [ board[i*n:(i+1)*n] for i in range(n) ]
    cols = [ board[i:n*n:n] for i in range(n) ]
    for col in cols:
	col = map(len, re.findall('X+', ''.join(col)))
	fp.write('\t'.join(map(str, col)) + '\n')
    for row in rows:
	row = map(len, re.findall('X+', ''.join(row)))
	fp.write('\t'.join(map(str, row)) + '\n')
    fp.close()

    fp = open('solution.txt', 'a')
    fp.write('$%d\n' % idx)
    for row in rows:
	fp.write('\t'.join(row).replace('X', '1').replace('.','0') + '\n')
    fp.close()


def main():
    if len(sys.argv) != 6:
	usage()
	return

    n, num, p1, p2, seed = sys.argv[1:]
    n = int(n)
    num = int(num)
    p1 = float(p1)
    p2 = float(p2)
    seed = long(seed)

    random.seed(seed)

    size = n*n
    for i in xrange(num):
	if num == 1:
	    r = int(p1 * size)
	else:
	    r = int((p1 + (p2-p1) * i / (num-1)) * size)
	board = ['X']*r + ['.']*(size-r)
	random.shuffle(board)
	output(n, i+1, board)

if __name__ == '__main__':
    main()

# vim:sw=4:sta:
