#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
class DFSBoard: public Board{
 public:
 DFSBoard(Board& b): Board(b){//inherit
    this->printBoard("before DFS");
    lastfillStart.resize(r);
    original.resize(r);
  }

  vector< vector<int> > lastfillStart;
  vector<Board> original;

  void Rewind(int);
  void BackupBoard(Board &b);
  void RewindBoard(Board &b);

  bool getPreviousFillStart(vector<int>&, int);
  bool getNextFillStart(int, vector<int>&);
  
  //simple DFS
  void DoSimpleDFS();
  bool isDFSAnswerCorrect();
  
  bool tryFillRow(int);
  void FillRowbyFillStart(int, vector<int>&);
  
  bool checkColumns();
  bool checkAvailableCol(int);

  //DFS with heuristic
  void DoDFS();
  int getRowWithMinBranch(int nowr, vector<int>& rowOrder);
  
  bool tryFillRowHeuristic(int);
  bool tryFillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart);
};

#endif
