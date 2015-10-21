#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
#include "assert.h"

class LimitFiller{
public:
  LimitFiller(){l.clear(); fillStart.clear();};
  LimitFiller(vector<RowLimit> limits): l(limits){isInit = true;};
  bool isInit;
  vector<RowLimit> l;
  vector<int> fillStart;
  bool isInited(){return isInit;}
  void setLimit(vector<RowLimit>& limits){l = (limits); isInit = true;}
  void destroy(){isInit = false; fillStart.clear();}
  bool getNextFillStart();
  bool getNextFillStartbyFillStart();
  bool isLimitLegal();
};

class DFSBoard: public Board{
public:
  DFSBoard(Board& b): Board(b){//inherit
    printBoard("before DFS");
    limitFillers.resize(r+c);
    backupBoards.resize(r+c);
    isFilled.resize(r+c);
  }

  vector<bool> isFilled;
  bool isFilledByDFS(int nowr){return isFilled[nowr];}
  vector< LimitFiller > limitFillers;
  vector <vector<int> > lastfillStart;
  vector<Board> backupBoards;
  vector<Line> lineOrder;

  void Restore(Line&);
  void Backup(Line&);
  void BackupBoard(Board &b);
  void RestoreBoard(Board &b);

  //simple DFS
  void DoSimpleDFS();
  bool isDFSAnswerCorrect();
  
  bool tryFillRow(int);
  void FillRowbyFillStart(int, vector<int>&);
  
  bool checkColumns();
  bool checkAvailableCol(int);

  //DFS with heuristic
  void DoDFS();
  void getRowWithMinBranch(int nowr, vector<Line>& lineOrder);
  
  bool tryFillRowWithHeuristic(int);
  bool tryFillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart);

  bool isRowLegal(vector<int>& fillStart, int nowr);
};

#endif
