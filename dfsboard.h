#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
class DFSBoard: public Board{
 public:
 DFSBoard(Board& b): Board(b){
    printf("dfs board print board");
    this->printBoard("dsfsfdfsdsf");
    sleep(10);
  }
  void DoDFS();
};

#endif
