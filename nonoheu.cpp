#include <stdio.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <unistd.h>
#include "board.h"
#include "nonogramReader.h"
#include "nonogramWriter.h"

using namespace std;

char *problemName = NULL;
int problemNum = 1;

void parseArgument(int argc, char** argv){
  // use getopt to get arg: http://wen00072-blog.logdown.com/posts/171197-using-getopt-parse-command-line-parameter
  while(1){
    int cmd_opt = getopt(argc, argv, "n:f:");
    if (cmd_opt == -1) {    /* End condition always first */
      break;
    }
    if (cmd_opt != '?') {    /* Print option when it is valid */
      fprintf(stderr, "option:-%c\n", cmd_opt);
    }

    switch (cmd_opt) {    /* Lets parse */      /* Error handle: Mainly missing arg or illegal option */
    case 'n':
      if(optarg){
	sscanf(optarg, "%d", &problemNum);
      } else {
	fprintf(stderr, "-n no argument\n");
      }
      break;
    case 'f':
      if (optarg){
	asprintf(&problemName, "%s", optarg);
	fprintf(stderr, "use %s as saved file\n", problemName);
      } else {
	fprintf(stderr, "-f no argument\n");
      }
      break;
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

int main(int argc, char** argv){
  setlocale(LC_ALL, "");
  parseArgument(argc, argv);
  clock_t beginTime = clock();

  struct Board board;
  NonogramInputReader ir(stdin);  //new reader
  for(int i = 0; i < problemNum; i++){
    fprintf(stderr, "To solve problem %d\n", i);
    ir.readInputAndGetBoard(&board, problemName);
    board.doHeuristic();
    board.doDFS();
    board.checkAnswer();
    board.printBoard("after solved");
    board.saveResult();
  }
  clock_t endTime = clock();

  printf("time spent: %lf\n", (double(endTime) - double(beginTime)) / CLOCKS_PER_SEC);
  return 0;
}
