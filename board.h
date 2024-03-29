#ifndef BOARD_H
#define BOARD_H

#include "utility.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
using namespace std;
enum chessType{WHITE=2, BLACK=1, SPACE=0};
enum line_type{ROW, COL};
enum SOLVEMODE{HEURISTIC, DFS};

struct Limit{
  int l;
  int fs, ls, fe, le;
  Limit(int ll){l = ll; fs=0; ls=0;}
  bool set_pos(int a, int b){
    bool isUpdate = (a > fs || b < ls);
    fs = a; ls = b;
    fe = fs+l-1; le = ls+l-1;
    return isUpdate;
  }
  bool isSolved(){
    return (fs == ls);
  }
};

struct RowLimit: public Limit{
  RowLimit(int ll): Limit(ll){}
};

struct ColLimit: public Limit{
  ColLimit(int ll): Limit(ll){}
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
  int* changeNum;
  bool isTried;
  LineChanged(){}
LineChanged(line_type t, int i, int* cn): type(t), lineNum(i), changeNum(cn), isTried(false){
      //print();
  }
  void print(){
      DEBUG_PRINT("line change: line %s, %d, change %d istried %s\n", type==ROW?"ROW":"COL", lineNum, *changeNum, isTried?"True":"False");

  }
  bool operator<(const LineChanged& rhs) const{
      if(isTried != rhs.isTried)
          return !isTried;
      else
          return (*changeNum) > (*rhs.changeNum);
  }
};

typedef LineChanged change;

class NonogramWriterInterface;

struct Board{
  Board(){conflictReason = new char[200];}
  Board(int rr, int cc, vector< vector<struct Limit> > lr, vector< vector<struct Limit> > lc, char* n = NULL, int problemNum = 1){
    conflictReason = new char[200];
    init(rr, cc, lr, lc, n, problemNum);
  }
  void init(int rr, int cc, vector< vector<struct Limit> > lr, vector< vector<struct Limit> > lc, char* n = NULL, int problemNum = 1){
    isLimitInit = false;
    r = rr;  c = cc;
    lim_row = lr; lim_col = lc;

    b.resize(r);
    for(int i = 0; i < r; i++)
      b[i].resize(c);
    for(int i = 0; i < r; i++)
      for(int j = 0; j < c; j++)
          b[i][j] = SPACE;

    isupdate.resize(r);
    for(int i = 0; i < r; i++)
      isupdate[i].resize(c);
    for(int i = 0; i < r; i++)
      for(int j = 0; j < c; j++)
	isupdate[i][j] = false;

    solvedLineNum = 0;
    alreadySetGridNumber = 0;
    change_row.resize(r); change_col.resize(c);
    solved_row.resize(r); solved_col.resize(c);
    changed_row.resize(r); changed_col.resize(c);
    for(int i = 0; i < r; i++){
      change_row[i] = 0;
      solved_row[i] = 0;
      changed_row[i] = true;
    }
    for(int i = 0; i < c; i++){
      change_col[i] = 0;
      solved_col[i] = 0;
      changed_col[i] = true;
    }

    if(name == NULL){
      //name = new char(15);
      //memcpy(name, "nonogram");
      //if(asprintf(&name, "nonogram") != 1)
      //exit(1);
    }
    else
      name = n;
    this->problemNum = problemNum;
    conflict = false;
  }

  void setWriter(NonogramWriterInterface* writer){
    this->writer = writer;
  }

  int problemNum;
  char* name;
  int r, c;
  bool isLimitInit;
  vector< vector<int> > b;//board
  vector< vector<bool> > isupdate;//board
  bool isUpdate(int r, int c){return isupdate[r][c];}
  //TODO: need to use pointer instead of instance
  vector< vector<struct Limit> > lim_row;
  vector< vector<struct Limit> > lim_col;
  vector<int> change_row, change_col;
  //priority_queue<change, vector<change>, change> changeQueue; //TODO: implement change queue in class
  vector<bool> solved_row, solved_col;
  vector<bool> changed_row, changed_col;

  SOLVEMODE solveMode;
  bool conflict;
  char* conflictReason;
  int solvedLineNum;
  int alreadySetGridNumber;
  NonogramWriterInterface* writer;

    bool isAllSolved(){//TODO: ??? use another way to store information
        return (solvedLineNum == r + c);
    }

  void solveGame();

  void doHeuristic();
  bool doHeuristicByLine();
  bool updateByHeuristic(line_type, int);
  void updateRowLimits(struct Point p, int v);//fill a new block, find other block can be updated or not
  bool updateLimitByGrid_row(int linei, int limiti, int i);
  bool updateLimitByLimit_row(int nowr);
  void updateColLimits(struct Point p, int v);
  bool updateLimitByGrid_col(int linei, int limiti, int i);//update possibility of solutions(fs and ls)
  bool updateLimitByLimit_col(int nowc);
  //fill grids based on limit
    void initialFill(line_type lt, std::vector<Limit>& l, int lineIndex);
  void fillRowByLimit(int);
  void fillColByLimit(int);
  void fill_blank_row(int);
  void fill_blank_col(int);
    void fill_blank(line_type lt, std::vector<Limit>& lim, int nowi);
  void fillGrid(int r, int c, int v);//fill single grid
  void fillGrid(line_type lt, int first, int second, int v);
  void fillGrid_Solved(int r, int c, int v);//fill single grid
  //check the relationship between grid and limit
  bool in_limit_col(int linei, int limiti, int i);//whether the block would only be in specified limit
  bool in_limit_row(int linei, int limiti, int i);
  bool only_in_one_limit_col(int r, int c, int limiti);
  bool only_in_one_limit_row(int r, int c, int limiti);

  //high-level functions related to limits
  void setRowLimitSolved(int ri, int limiti);
  void setColLimitSolved(int ri, int limiti);
    void setLimitSolved(line_type lt, std::vector<Limit>& lim, int linei, int limiti);
  void isLineSolved(line_type type, int line);

  bool checkAnswer();//TODO: use getLimitFromBoard
  vector<int> getLimitFromBoard_col(int);
  vector<int> getLimitFromBoard_row(int);

  void printBoard(const char[]);
  void saveResult();
  void saveSimpleResult(FILE *f);
  void saveFullResult();

  void no_solution(const char[]);
  void no_solution(const char[], line_type t, int i);

  void doDFS();

  void setLimit(line_type t, int linei, Limit &l, int fs, int ls);
    void fillByLimit(line_type lt, std::vector<Limit>& lim, int nowi);
    bool checkColor(line_type lt, int nowi, int i, int color);

    void clearBoard();
};


#endif
