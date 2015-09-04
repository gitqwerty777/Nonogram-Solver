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

int r, c;
char *problemName = NULL;
vector< vector<struct Limit> > lim_row, lim_col;

void parseArgument(int argc, char** argv){
  // use getopt to get arg: http://wen00072-blog.logdown.com/posts/171197-using-getopt-parse-command-line-parameter
  while(1){
    int cmd_opt = getopt(argc, argv, "f:");
    /* End condition always first */
    if (cmd_opt == -1) {
      break;
    }
    /* Print option when it is valid */
    if (cmd_opt != '?') {
      fprintf(stderr, "option:-%c\n", cmd_opt);
    }
    /* Lets parse */
    switch (cmd_opt) {
    case 'f':
      if (optarg) 
	asprintf(&problemName, "%s", optarg);
      break;
      /* Error handle: Mainly missing arg or illegal option */
    case '?':
      fprintf(stderr, "Illegal option\n");
      break;
    default:
      fprintf(stderr, "Not supported option\n");
      break;
    }
  }
  /* additional args */
  if (argc > optind) {
    int i = 0;
    for (i = optind; i < argc; i++)
      fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
  }
  if(problemName == NULL)
    asprintf(&problemName, "result");
}

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

int main(int argc, char** argv){
  setlocale(LC_ALL, "");
  parseArgument(argc, argv);
  clock_t beginTime = clock();
  readInputLimit();
  struct Board board(r, c, lim_row, lim_col, problemName);
  board.doHeuristic();
  board.doDFS();
  board.checkAnswer();
  board.printBoard("after solved");
  board.saveResult();
  clock_t endTime = clock();
  printf("time spent: %lf\n", (double(endTime) - double(beginTime)) / CLOCKS_PER_SEC);
  return 0;
}
