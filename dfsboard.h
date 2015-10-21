#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
#include "assert.h"

class LimitFiller{
public:
  LimitFiller(){l.clear(); fillStart.clear();};
  LimitFiller(vector<Limit> limits): l(limits){isInit = true;};
  bool isInit;
  vector<Limit> l;
  vector<int> fillStart;
  bool isInited(){return isInit;}
  void setLimit(vector<Limit>& limits){l = limits; isInit = true;}
  void destroy(){isInit = false; fillStart.clear();}
  bool getNextFillStart();
  bool getNextFillStartbyFillStart();
  bool isLimitLegal();
};

class DFSBoard: public Board{
public:
  DFSBoard(Board& b): Board(b){//inherit
    printBoard("before DFS");
    limitFillers.resize(r);
    backupBoards.resize(r);
  }

  vector< LimitFiller > limitFillers;
  vector <vector<int> > lastfillStart;
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
