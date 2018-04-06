#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <unistd.h>
#include "board.h"
#include "nonogramReader.h"
#include "nonogramWriter.h"

//default options
char *problemName = NULL;
int problemNum = 1;
int boardSize = 25;
bool isTourament = false;

/// @brief use getopt to get arg
///
/// reference: http://wen00072-blog.logdown.com/posts/171197-using-getopt-parse-command-line-parameter
void parseArgument(int argc, char** argv){
  while(true){
    int cmd_opt = getopt(argc, argv, "tn:f:s:");
    if (cmd_opt == -1) {//End condition
      break;
    } else if (cmd_opt != '?') {//Print option when it is valid
      fprintf(stderr, "get option:-%c\n", cmd_opt);
    }
    switch (cmd_opt) { /// Error handle: Mainly missing arg or illegal option
    case 't':
      fprintf(stderr, "enable tourament mode\n");
      isTourament = true;
      break;
    case 'n':
      if(optarg){
        sscanf(optarg, "%d", &problemNum);
        fprintf(stderr, "[OPTION] problem num %d\n", problemNum);
      } else {
        fprintf(stderr, "[OPTION] -n without argument\n");
        exit(1);
      }
      break;
    case 's':
      if(optarg){
        sscanf(optarg, "%d", &boardSize);
        fprintf(stderr, "[OPTION] board size %d\n", boardSize);
      } else {
        fprintf(stderr, "[OPTION] -s without argument\n");
      }
      break;
    case 'f':
      if (optarg){
        problemName = new char[1000];
        int i = sprintf(problemName, "%s", optarg);
        fprintf(stderr, "[OPTION] output file path %s\n", problemName);
      } else {
        fprintf(stderr, "[OPTION] -f no argument\n");
      }
      break;
    case '?':
      fprintf(stderr, "Illegal option\n");
      exit(1);
      break;
    default:
      fprintf(stderr, "Not supported option %s\n", optarg);
      exit(1);
      break;
    }
  }
  /* additional args */
  if (argc > optind) {
    int i = 0;
    for (i = optind; i < argc; i++)
      fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
  }
  //temporaily deleted
  //if(problemName == NULL)
  //int i = asprintf(&problemName, "result");
}

int main(int argc, char** argv){
  setlocale(LC_ALL, "");//for wchar
  parseArgument(argc, argv);

  Board board;
  NonogramInputReader ir(stdin, boardSize);//read question from stdin
  NonogramWriterInterface* writer;
  if(isTourament){
    writer = new NonogramWriter_Tourament;
  } else {
    writer = new NonogramWriter;
  }
  board.setWriter(writer);

  clock_t beginTime = clock();
  for(int i = 0; i < problemNum; i++){
    clock_t start_t = clock();
    fprintf(stderr, "To solve problem %d\n", i);
    ir.readInputAndGetBoard(&board, problemName, isTourament);
    board.doHeuristic();
    board.doDFS();
    if(!board.checkAnswer())
      fprintf(stderr, "check answer failed\n");
    else
      fprintf(stderr, "%d correct\n", i);
    board.printBoard("after solved");
    board.saveResult();
    clock_t end_t = clock();
    printf("time spent: %lf\n", (double(end_t) - double(start_t)) / CLOCKS_PER_SEC);
  }
  clock_t endTime = clock();

  printf("time spent: %lf\n", (double(endTime) - double(beginTime)) / CLOCKS_PER_SEC);
  return 0;
}
