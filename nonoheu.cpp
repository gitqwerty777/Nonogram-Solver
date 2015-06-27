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

output:
with number (1 = black, -1 = white, 0 = space) and graph and some debug information
*/

int main(){
  int r, c;
  char in[10000];//big enough
  int cons;
  vector< vector<struct Limit> > lim_row;
vector< vector<struct Limit> > lim_col;
  setlocale(LC_ALL, "");

  clock_t begin, end;  double time_spent;
  begin = clock();

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
  struct Board board(r, c, lim_row, lim_col);
  //pre process
  for(int i = 0; i < c; i++)
    board.fill_col(i);
  board.print_board("after col");
  for(int i = 0 ; i < r; i++)
    board.fill_row(i);
  board.print_board("after col and row");
  
  puts("start heuristic");
  while(!board.check_all_solved()){// if find every block solved, exit(check by solved_line_num)
    if(!board.heuristic()){//no update
    }
  }
  board.print_board("after solved");
  end = clock();
  time_spent = (double(end) - double(begin)) / CLOCKS_PER_SEC;
  printf("time spent: %lf\n", time_spent);
  return 0;
}
