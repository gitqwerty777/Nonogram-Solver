#include <stdio.h>
#include <vector>
#include <string.h>
using namespace std;

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

enum line_type{ROW, COL};

struct Board{
  Board(int rr, int cc, vector< vector<struct Limit> >& lr, vector< vector<struct Limit> >& lc){
    r = rr;  c = cc;
    lim_row = lr; lim_col = lc;
    b.resize(r);
    for(int i = 0; i < r; i++)
      b[i].resize(c);
    solved_num = 0;
    alreadySetGridNumber = 0;
    h_row.resize(r); h_col.resize(c);
    change_row.resize(r); change_col.resize(c);
    solved_row.resize(r); solved_col.resize(c);
  }

  int r, c;
  vector< vector<int> > b;//board
  vector< vector<struct Limit> > lim_row, lim_col;
  vector<int> h_row, h_col;
  vector<int> change_row, change_col;
  vector<bool> solved_row, solved_col;
  int solved_num;
  int alreadySetGridNumber;

  void doHeuristic();
  bool doHeuristicInOneLine();
  bool updateHeuristic(int, int);
  //update limit by heuristic
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
  
  void doDFS();

  //high-level functions related to limits
  void setRowLimitSolved(int ri, int limiti);
  void setColLimitSolved(int ri, int limiti);
  void isLineSolved(line_type type, int line);
  bool isSolved();//all

  void printBoard(const char[]);//for debug usage
};


