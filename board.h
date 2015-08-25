#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <vector>
#include <string.h>
#include <queue>
using namespace std;

enum chessType{WHITE=2, BLACK=1, SPACE=0};
enum line_type{ROW, COL};
enum SOLVEMODE{HEURISTIC, DFS};

struct Limit{
  int l;
  int fs, ls, fe, le;
  Limit(int ll){l = ll; fs=0; ls=0;}
  void set_pos(int a, int b){
    fs = a; ls = b;
    fe = fs+l-1; le = ls+l-1;
  }
  bool isSolved(){
    return (fs == ls);
  }
};

struct Point{
  int r, c;
  Point(int rr, int cc){
    r = rr; c = cc;
  }
};

class LineChanged{//TODO:
 public:
  line_type type;
  int lineNum;
  int changeNum;
  LineChanged(){}
  LineChanged(line_type t, int i, int cn){
    type = t;
    lineNum = i;
    changeNum = cn;
  }
  bool operator()(const LineChanged& lc, const LineChanged& rc) const {
    return (lc.changeNum < rc.changeNum);
  }
};

typedef struct LineChanged change;

struct Board{
  Board(){}
  Board(int rr, int cc, vector< vector<struct Limit> >& lr, vector< vector<struct Limit> >& lc){
    r = rr;  c = cc;
    lim_row = lr; lim_col = lc;
    b.resize(r);
    for(int i = 0; i < r; i++)
      b[i].resize(c);
    solvedLineNum = 0;
    alreadySetGridNumber = 0;
    change_row.resize(r); change_col.resize(c);
    solved_row.resize(r); solved_col.resize(c);
  }

  int r, c;
  vector< vector<int> > b;//board
  vector< vector<struct Limit> > lim_row, lim_col;
  vector<int> change_row, change_col;
  //priority_queue<change, vector<change>, change> changeQueue; //TODO: implement
  vector<bool> solved_row, solved_col;

  SOLVEMODE solveMode;
  bool tryFailed;
  int solvedLineNum;
  int alreadySetGridNumber;

  void solveGame();
  
  void doHeuristic();
  bool doHeuristicInOneLine();
  bool updateByHeuristic(line_type, int);  
  void updateRowLimits(struct Point p, int v);//fill a new block, find other block can be updated or not
  void updateColLimits(struct Point p, int v);
  bool updateLimitByGrid_col(int linei, int limiti, int i);//update possibility of solutions(fs and ls)
  bool updateLimitByGrid_row(int linei, int limiti, int i);
  bool updateLimitByLimit_row(int nowr);
  bool updateLimitByLimit_col(int nowc);
  //fill grids based on limit
  void initialFillRow(int i);
  void initialFillCol(int i);
  void fillColByLimit(int);
  void fillRowByLimit(int);
  void fill_blank_row(int);
  void fill_blank_col(int);
  void fillGrid(int r, int c, int v);//fill single grid
  //check the relationship between grid and limit
  bool in_limit_col(int linei, int limiti, int i);//whether the block would only be in specified limit
  bool in_limit_row(int linei, int limiti, int i);
  bool only_in_one_limit_col(int r, int c, int limiti);
  bool only_in_one_limit_row(int r, int c, int limiti);
  
  //high-level functions related to limits
  void setRowLimitSolved(int ri, int limiti);
  void setColLimitSolved(int ri, int limiti);
  void isLineSolved(line_type type, int line);
  bool isAllSolved(){
    return (solvedLineNum == r + c);
  }
  bool checkAnswer();//TODO: use getLimitFromBoard 
  vector<int> getLimitFromBoard_col(int);
  vector<int> getLimitFromBoard_row(int);
  
  void printBoard(const char[]);//for debug usage
  void no_solution(const char[]);
  void no_solution(const char[], line_type t, int i);

  void doDFS();
};

#endif
