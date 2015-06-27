#include <stdio.h>
#include <vector>
#include <string.h>
using namespace std;

struct Limit{
  int l;
  int fs, ls, fe, le;
  Limit(int ll){
    l = ll;
  }
  void set_pos(int a, int b){
    fs = a; ls = b;
    fe = a + l -1; le = ls + l -1;
  }
};

struct Point{
  int r, c;
  Point(int rr, int cc){
    r = rr; c = cc;
  }
};

enum LINE{ROW, COL};

struct Board{
  Board(int rr, int cc, vector< vector<struct Limit> >& lr, vector< vector<struct Limit> >& lc){
    r = rr;
    c = cc;
    b.resize(r);
    for(int i = 0; i < r; i++)
      b[i].resize(c);
    lim_row = lr; lim_col = lc;
    solved_num = 0;
    set_num = 0;
    h_row.resize(r); h_col.resize(c);
    change_row.resize(r); change_col.resize(c);
    solved_row.resize(r); solved_col.resize(c);
  }

  int r, c;
  vector< vector<int> > b;//board
  vector< vector<struct Limit> > lim_row;
  vector< vector<struct Limit> > lim_col;
  vector<int> h_row, h_col;
  vector<int> change_row, change_col;
  vector<bool> solved_row, solved_col;
  int solved_num;
  int set_num;

  //logic reasoning
  bool pre_process();
  void fill_row(int i);
  void fill_col(int i);
  int  is_full_filled_col(int);
  int  is_full_filled_row(int);
  bool update_row(struct Point p, int v);//fill a new block, find other block can be updated or not
  bool update_col(struct Point p, int v);
  bool update_limit_col(int linei, int limiti, int i);//update possibility of solutions(fs and ls)
  bool update_limit_row(int linei, int limiti, int i);
  bool in_limit_col(int linei, int limiti, int i);//whether the block would only be in specified limit
  bool in_limit_row(int linei, int limiti, int i);
  bool only_in_one_limit_col(int r, int c, int limiti);
  bool only_in_one_limit_row(int r, int c, int limiti);
  void update_lim_col(int, int,int,int);
  void update_lim_row(int, int,int,int);
  void update_line_col(int);
  void fillknown_row(int);
  //board
  void set_grid(int r, int c, int v);
  void print_board(const char[]);
  void fill_blank_row(int);
  void fill_blank_col(int);

  //heuristic
  void do_heuristic();
  
  void update_h();
  void check_solve(LINE, int line);
  bool guess(LINE, int);
  bool heuristic();

  bool check_all_solved();

  //dfs
  void dfs();
};


