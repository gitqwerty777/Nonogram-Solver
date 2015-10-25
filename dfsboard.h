#ifndef DFSBOARD_H
#define DFSBOARD_H

#include "board.h"
#include "unistd.h"
#include "cassert"
class DFSBoard;

struct Line{
  line_type t;
  int i;
  int index;//including r and c
  Line(){};
  Line(line_type type, int i, int index): t(type), i(i), index(index){}
  bool equals(line_type t, int i){
    return (this->t == t) && (this-> i == i);
  }
};

class LimitFiller{
public:
  LimitFiller(){fillStart.clear();}
  LimitFiller(vector<Limit>& limit):l(limit){ isInit = true;}

  bool isInit;
  vector<Limit> l;
  vector<int> fillStart;
  
  bool isInited(){return isInit;}
  void setLimit(vector<Limit>& limits){l = (limits); isInit = true;}
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
  bool isFilledByDFS(line_type t, int i){if(t == ROW) return isFilled[i]; else return isFilled[r+i];}
  vector< LimitFiller > limitFillers;
  vector <vector<int> > lastfillStart;
  vector<Board> backupBoards;
  vector<Line> lineOrder;

  void Restore(const Line&);
  void Backup(const Line&);
  void BackupBoard(Board &b);
  void RestoreBoard(const Board &b);

  //DFS with heuristic
  void DoDFS();
  void getLineWithMinBranch(int nowr, vector<Line>& lineOrder);
  
  bool tryFillRowWithHeuristic(Line&);
  void checkSolve();
  bool tryFillRowbyFillStartHeuristic(const Line&, const vector<int>& fillStart);
};

#endif
