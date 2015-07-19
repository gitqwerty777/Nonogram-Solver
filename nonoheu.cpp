#include <stdio.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <unistd.h>
#include "board.h"
using namespace std;

/*
input format: 
line 1: row column(with space seperated)
line 2 ~ r+1: row constraints(with space seperated)
line r+2 ~ r+c+1: column constraint(with space seperated)
if no constraint, just put "0"
output:
with number (1 = black, -1 = white, 0 = space) and graph and some debug information
*/

int r, c;
vector< vector<struct Limit> > lim_row, lim_col;
void readInputLimit(){
  char in[10000];//big enough
  scanf("%d %d", &r, &c);
  lim_row.resize(r);  lim_col.resize(c);
  gets(in);//dumb
  for(int i = 0; i < r; i++){
    gets(in);
    char *p;
    p = strtok(in, " ");
    while(p != NULL){
      lim_row[i].push_back(Limit(atoi(p)));
      p = strtok(NULL, " ");
    }
  }
  for(int i = 0; i < c; i++){
    gets(in);
    char *p;
    p = strtok(in, " ");
    while(p != NULL){
      lim_col[i].push_back(Limit(atoi(p)));
      p = strtok(NULL, " ");
    }
  }
}

int main(){
  setlocale(LC_ALL, "");
  clock_t beginTime = clock();
  readInputLimit();
  struct Board board(r, c, lim_row, lim_col);
  board.doHeuristic();
  if(!board.isAllSolved()){//if heuristic can't update anymore, do DFS
    board.doDFS();
  }
  board.checkAnswer();
  board.printBoard("after solved");
  clock_t endTime = clock();
  printf("time spent: %lf\n", (double(endTime) - double(beginTime)) / CLOCKS_PER_SEC);
  return 0;
}

