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
  vector<Board> original;
  
  //simpledfs
  //void fillGrid(int r, int c, int v);
  bool checkDFSAnswer();
  void DoSimpleDFS();
  
  bool FillRow(int);
  bool getPreviousFillStart(vector<int>&, int);
  bool getNextFillStart(int, vector<int>&);
  void FillRowbyFillStart(int, vector<int>&);
  
  bool checkColumns();
  bool checkAvailableCol(int);
  void Rewind(int);
  //dfs with heuristic
  void DoDFS();
  bool FillRowHeuristic(int);
  bool FillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart);
  bool tryUpdateByHeuristic(line_type type, int line);
  void RewindBoard(Board &b);
};

#endif
