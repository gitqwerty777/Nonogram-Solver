#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
class DFSBoard: public Board{
 public:
 DFSBoard(Board& b): Board(b){//inherit
    printf("dfs board print board");
    this->printBoard("dsfsfdfsdsf");
    lastfillStart.resize(r);
    original.resize(r);
  }

  vector< vector<int> > lastfillStart;
  vector< vector<vector<int> > > original;
  
  //simpledfs
  void fillGrid(int r, int c, int v);
  vector<int> getLimit_col(int nowc);
  bool checkDFSAnswer();
  void DoSimpleDFS();
  bool FillRow(int);
  bool getNextFillStart(int, vector<int>&);
  void FillRowbyFillStart(int, vector<int>&);
  bool checkColumns();
  bool checkAvailableCol(int);
  void Rewind(int);
  //dfs with heuristic
  void DoDFS();
  
};

#endif
