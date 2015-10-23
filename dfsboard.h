#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
#include "assert.h"
class DFSBoard;

class LimitFiller{
public:
  LimitFiller(){fillStart.clear();}
  LimitFiller(vector<Limit> limit):l(limit){ isInit = true;}
  bool isInit;
  vector<Limit> l;
  vector<int> fillStart;
  bool isInited(){return isInit;}
  void setLimit(vector<Limit>& limits){l = (limits); isInit = true;}
  void destroy(){isInit = false; fillStart.clear();}
  bool getNextFillStart();
  bool getNextFillStartbyFillStart();
  bool isLimitLegal();
  //void fillLimitByFillStart(DFSBoard* b, Line&);
};

class DFSBoard: public Board{
public:
  DFSBoard(Board& b): Board(b){//inherit
    printBoard("before DFS");
    /*for(int i = 0; i < r; i++)
      limitFillers.push_back(LimitFiller(lim_row[i]));
    for(int i = 0; i < c; i++)
    limitFillers.push_back(LimitFiller(lim_col[i]));*/
    limitFillers.resize(r+c);
    backupBoards.resize(r+c);
    isFilled.resize(r+c);
  }

  vector<bool> isFilled;
  bool isFilledByDFS(line_type t, int i){if(t == ROW) return isFilled[i]; else return isFilled[r+i];}
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
  
  bool tryFillRowWithHeuristic(Line);
  bool tryFillRowbyFillStartHeuristic(Line&, vector<int>& fillStart);

  bool isRowLegal(vector<int>& fillStart, int nowr);

  void checkSolve();
};

#endif
