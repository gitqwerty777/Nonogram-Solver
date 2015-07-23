#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
class DFSBoard: public Board{
 public:
 DFSBoard(Board& b): Board(b){
    printf("dfs board print board");
    this->printBoard("dsfsfdfsdsf");
  }

  vector< vector<int> > lastfillStart;
  vector< vector<int> > original;
  
  //simpledfs
  void DoSimpleDFS();
  bool FillRow(int);
  bool getNextFillStart(int, vector<int>&);
  void FillRowbyFillStart(int, vector<int>&);
  bool checkColumns();
  void checkAvailableCol(int);
  void Rewind(int);
  //dfs with heuristic
  void DoDFS();
  
};

#endif
