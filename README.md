# Nonogram-Solver
C++ program to solve nonogram by heuristic and DFS

## Build

1. `make mode=release` can build program `heu`(default mode)
2. `make mode=debug` will make program print detail message on each step

## Input Format

normal mode
- line 1: `[row] [column]`
- line 2 ~ r+1: row constraints with space seperated
- line r+2 ~ r+c+1: column constraints with space seperated
- if no constraint in a line, just put "0"

There are some example test input files in `testdata/input*`

tourment mode: used for TCGA or TAAI nonogram input, multiple questions in one file
- line 1: `$[problem index]` (index start from 1)
- line 2 ~ boardSize+1: row constraints with `\t` seperated
- line boardSize+2 ~ boardSize*2+1: column constraints with `\t` seperated
- if no constraint in a line, just put "0"

There are some example test input files `testdata/question.txt`

## Output Format

output answer with number format(1 = black, -1 = white, 0 = space) and graph format(unicode black and white blocks)

## Run Program

normal mode(boardsize is decided by first line of input)
```bash
./heu -f [result file path] < inputfile
```

tourment mode(always square, a.k.a, row = column)
```bash
./heu -t -s [BoardSize] -n [problemNum] < inputfile
```

## Algorithm

- Heuristic
  - Refer to techniques in [wiki](https://en.wikipedia.org/wiki/Nonogram#Solution_techniques)
- DFS
  - fill a line in each step
- Hybrid
  - Use heuristic method first, if it isn't solved, do DFS

## Implementation

- Heuristic
  - update left-most and right-most possible grid of each constraint
    - consider the line which changed the most first
- DFS
  - fill the row or column with minimum possible variations
  - also fill other grids by heuristic as many as possible to decrease further searching space

## Future Improvement

1. Heuristic
  1. optimize the progress of updating constraints and grids
1. DFS
  1. also consider to fill column at each step
    1. is it better to fill only some grids instead of line at each step?
