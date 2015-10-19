#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
#include "assert.h"
class DFSBoard: public Board{
 public:
 DFSBoard(Board& b): Board(b){//inherit
    this->printBoard("before DFS");
    lastfillStart.resize(r);
    backupBoards.resize(r);
  }

  vector< vector<int> > lastfillStart;
  vector<Board> backupBoards;

  void Restore(int);
  void Backup(int);
  void BackupBoard(Board &b);
  void RestoreBoard(Board &b);

  bool getNextLegalFillStart(vector<int>&, int);
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
  void getRowWithMinBranch(int nowr, vector<int>& rowOrder);
  
  bool tryFillRowWithHeuristic(int);
  bool tryFillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart);

  bool isRowLegal(vector<int>& fillStart, int nowr);
};

#endif
