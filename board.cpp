#include "board.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXINT 2147483647
using namespace std;

#define BLACK 1
#define WHITE -1
#define SPACE 0
//they can be enum too

wchar_t black[] = L"\u25A0";
wchar_t white[] = L"\u25A1";
wchar_t space[] = L" ";
wchar_t cl[] = L"\n";

void no_solution(char in[]){
  printf("no solution: %s\n", in);
  exit(0);
}

void Board::fill_blank_col(int nowc){
  int size = lim_col[nowc].size();
  for(int i = 0; i < lim_col[nowc][0].fs; i++)
    set_grid(i, nowc, WHITE);
  for(int i = 0; i < size-1; i++){
    for(int j = lim_col[nowc][i].ls + lim_col[nowc][i].l; j < lim_col[nowc][i+1].fs; j++)
      set_grid(j,nowc, WHITE);
  }
  for(int i = lim_col[nowc][size-1].ls+lim_col[nowc][size-1].l; i < c; i++)
    set_grid(i, nowc, WHITE);

  
  int nowb = 0;
  int minl = lim_col[nowc][0].l;
  for(int i = 1; i < lim_col[nowc].size(); i++)
    if(minl > lim_col[nowc][i].l)
      minl = lim_col[nowc][i].l;
  for(int i = 0;i < r; i++)
    if(b[i][nowc] != WHITE){
      nowb++;
      if(i == r-1){//last
	if(nowb < minl)
	for(int j = 0; j < nowb; j++)
	  set_grid(i-j, nowc, WHITE);
      }
    } else{
      if(nowb < minl){
	for(int j = 0; j < nowb; j++)
	  set_grid(i-j, nowc, WHITE);
      }
      nowb = 0;
    }
}

void Board::fill_blank_row(int nowr){
  int size = lim_row[nowr].size();
  for(int i = 0; i < size-1; i++){
    for(int j = lim_row[nowr][i].ls + lim_row[nowr][i].l; j < lim_row[nowr][i+1].fs; j++)
      set_grid(nowr, j, WHITE);
  }
  for(int i = lim_row[nowr][size-1].ls+lim_row[nowr][size-1].l; i < c; i++)
    set_grid(nowr, i, WHITE);

  int nowb = 0;
  int minl = lim_row[nowr][0].l;
  for(int i = 1; i < lim_row[nowr].size(); i++)
    if(minl > lim_row[nowr][i].l)
    minl = lim_row[nowr][i].l;

  for(int i = 0;i < c; i++){
    if(b[nowr][i] != WHITE){
      nowb++;
      if(i == c-1){//last
	if(nowb < minl){
	  for(int j = 0; j < nowb; j++)
	    set_grid(nowr, i-j, WHITE);  
	}
      }
    } else{
      if(nowb < minl){
	for(int j = 0; j < nowb; j++)
	  set_grid(nowr, i-j, WHITE);
      }
      nowb = 0;
    }
  }
}


bool Board::check_all_solved(){
  if(solved_num == r + c)
    return true;
  return false;
}

void Board::set_grid(int r, int c, int v){
  if(b[r][c] != v){
    if(b[r][c] == -v){//impossible
      printf("no solution: ");
      printf("not legal set_grid(%d, %d) from %d to %d\n", r, c, b[r][c], v);
      exit(0);
    }
    printf("set_grid (%d, %d) = %s\n", r, c, (v==1)?"BLACK":"WHITE");
    change_row[r]++;
    change_col[c]++;
    b[r][c] = v;
    set_num++;
  }
}

bool Board::guess(LINE type, int line){//if there are no any hint can solve it
  //Board tb(*this);//testboard //should it reference?
  int set_gridnum = set_num;
  if(type == ROW){//row
    //fill_blank_row(line);
    fillknown_row(line);
    for(int j = 0; j < c; j++)
      if(b[line][j] != SPACE) // update filled grid's col
	update_row(Point(line,j), b[line][j]);
    fill_blank_row(line);
  } else {
    update_line_col(line);
    for(int j = 0; j < r; j++)
      if(b[j][line] != SPACE)
	update_col(Point(j, line), b[j][line]);
    fill_blank_col(line);
  }
  return set_gridnum != set_num;
}

//void Board::make_answer(){
    /*  //make answer
  for(int i = 0; i < lim_row[p.r].size(); i++){
    int fs = lim_row[p.r][i].fs;
    int ls = lim_row[p.r][i].ls;
    for(int j = 0; j < lim_row[p.r][i].l; j++){
      if(fs + j < c)
	la[fs+j] = BLACK;//fill the black by constraint
      if(ls + j < c)
	ra[ls+j] = BLACK;
    }
  }
  for(int j = 0; j < c; j++){//fill with white // r or c?
    if(la[j] == SPACE)
      la[j] = WHITE;
    if(ra[j] == SPACE)
      ra[j] = WHITE;
  }
  printf("(%d, %d) = %d\n", p.r, p.c, v);
  printf("left ans: ");
  for(int i = 0; i < c; i++)
    printf("%d ", la[i]);
  puts("");
  printf("right ans: ");
  for(int i = 0; i < c; i++)
    printf("%d ", ra[i]);
  puts("");*/
//}

void Board::check_solve(LINE type, int line){  //check whole line is solved or not
  if(type == ROW){//row
    if(solved_row[line])//already solve
      return;
    bool allLimitSolved = true;
    for(int i = 0; i < lim_row[line].size(); i++){
      if(lim_row[line][i].fs == lim_row[line][i].ls)
	continue;
      else{
	allLimitSolved = false;
	break;
      }
    }
    if(allLimitSolved){
      printf("%d row all solve\n", line);
      solved_row[line] = true;
      solved_num++;
      int nowi = 0; int nowc = 0;
      while(nowi < lim_row[line].size()){
	if(nowc == lim_row[line][nowi].fs){
	  for(int i = 0; i < lim_row[line][nowi].l; i++)
	    set_grid(line, nowc++, BLACK);
	  nowi++;
	} else
	  set_grid(line, nowc++, WHITE);
      }
      for(int i = nowc; i < c; i++)
	set_grid(line, i, WHITE);
      print_board("after update ");
    }
  } else {//col
    if(solved_col[line])
      return;
    bool allLimitSolved = true;
    for(int i = 0; i < lim_col[line].size(); i++){
      if(lim_col[line][i].fs == lim_col[line][i].ls)
	continue;
      else{
	allLimitSolved = false;
	break;
      }
    }
    if(allLimitSolved){
      printf("%d col all solve\n", line);
      solved_col[line] = true;
      solved_num++;
      int nowi = 0; int nowr = 0;
      while(nowi < lim_col[line].size()){
	if(nowr == lim_col[line][nowi].fs){
	  for(int i = 0; i < lim_col[line][nowi].l; i++)
	    set_grid(nowr++, line, BLACK);
	  nowi++;
	} else{
	  set_grid(nowr++, line, WHITE);
	}
      }
      for(int i = nowr; i < r; i++)
	set_grid(i, line, WHITE);
      print_board("after update ");
    }
  }
}

/*void Board::update_h(){
    int maxunknown = 0;
  int possible_sol = 1;
  for(int i = 0; i < r; i++){
    if(solved_row[i])
      continue;    
    maxunknown = 0;
    possible_sol = 1;
    for(int j = 0; j < lim_row[i].size(); j++){
      int solnum = lim_row[i][j].ls - lim_row[i][j].fs + 1;
      if(maxunknown < solnum)
	maxunknown = solnum;
      possible_sol *= solnum;
    }
    h_row[i] = possible_sol * maxunknown;//maxunknown is important!
    printf("h_row[%d] = %d * %d = %d\n", i, possible_sol, maxunknown, h_row[i]);
  }
  for(int i = 0; i < c; i++){
    if(solved_col[i])
      continue;
    maxunknown = 0;
    possible_sol = 1;
    for(int j = 0; j < lim_col[i].size(); j++){
      int solnum = lim_col[i][j].ls - lim_col[i][j].fs + 1;
      if(maxunknown < solnum)
	maxunknown = solnum;
      possible_sol *= solnum;
    }
    h_col[i] = possible_sol * maxunknown;//maxunknown is important!
    printf("h_col[%d] = %d * %d = %d\n", i, possible_sol, maxunknown, h_col[i]);
  }
  return;
  }*/

bool Board::heuristic(){
  //update_h();
  vector<pair<LINE, int> > max_changed;
  int max = -1;
  //find the most changed row/col
  for(int i = 0; i < r; i++)
    if(!solved_row[i] && max <= change_row[i]){
      if(max == change_row[i]){
	max_changed.push_back(pair<LINE, int>(ROW, i));
      } else{
	max = change_row[i];
	max_changed.clear();
	max_changed.push_back(pair<LINE, int>(ROW, i));
      }
    }
  for(int i = 0; i < c; i++)
    if(!solved_col[i] && max <= change_col[i]){
      if(max == change_col[i]){
	max_changed.push_back(pair<LINE, int>(COL, i));
      } else{
	max = change_col[i];
	max_changed.clear();
	max_changed.push_back(pair<LINE, int>(COL, i));
      }
    }
  bool change = false;
  for(int i = 0;i < max_changed.size(); i++){
    if(max_changed[i].first == 1)
      change_row[max_changed[i].second] = 0;
    else
      change_col[max_changed[i].second] = 0;
    if(guess(max_changed[i].first, max_changed[i].second))
      change = true;
  }
  return change;//is changed or not in this function
}

bool Board::pre_process(){
  bool ans;
  int set_gridnum = MAXINT;
  while(set_gridnum != this->set_num){//is update or not
    set_gridnum = this->set_num;
    for(int i = 0; i < r; i++)
      for(int j = 0; j < c; j++)
	if(b[i][j] != SPACE){
	  update_row(Point(i,j), b[i][j]);
	  update_col(Point(i,j), b[i][j]);
	}
    for(int i = 0; i < r; i++)
      fill_blank_row(i);
    for(int i = 0; i < c; i++)
      fill_blank_col(i);
  }
  return true;
}

bool Board::in_limit_col(int linei, int limiti, int i){
  return (lim_col[linei][limiti].fs <= i && lim_col[linei][limiti].ls + lim_col[linei][limiti].l-1 >= i);  
}

void Board::update_line_col(int c){
  int size = lim_col[c].size();
  for(int i = 0; i < lim_col[c][0].fs; i++)
    set_grid(i, c, WHITE);
  for(int i = 0; i < size-1; i++){
    for(int j = lim_col[c][i].ls + lim_col[c][i].l; j < lim_col[c][i+1].fs; j++)
      set_grid(j, c, WHITE);
  }
  for(int i = lim_col[c][size-1].ls+lim_col[c][size-1].l; i < r; i++)
    set_grid(i, c, WHITE);
}

void Board::update_lim_col(int c, int limiti,int fs,int ls){
  lim_col[c][limiti].set_pos(fs, ls);
  int li = limiti-1;
  if(li >= 0){
    lim_col[c][li].set_pos(max(lim_col[c][li].fs, lim_col[c][li+1].fs - 1 - lim_col[c][li].l),
			   min(lim_col[c][li].ls, lim_col[c][li+1].ls +1 + lim_col[c][li+1].l));
  }
  li =limiti + 1;
  if(li < lim_col[c].size()){
    lim_col[c][li].set_pos(max(lim_col[c][li].fs, lim_col[c][li].fs - 1 - lim_col[c][li-1].l), min(lim_col[c][li].ls, lim_col[c][li-1].ls +1 + lim_col[c][li-1].l));
  }/*
  while(li >= 0){
    lim_col[c][li].fs = max(lim_col[c][li].fs, lim_col[c][li+1].fs - 1 - lim_col[c][li].l);
    li--;
  }
  li =limiti + 1;
  while(li < lim_col[c].size()){
    lim_col[c][li].ls = min(lim_col[c][li].ls, lim_col[c][li-1].ls +1 + lim_col[c][li-1].l);
    li++;
    }*/
}

bool Board::update_limit_col(int linei, int limiti, int i){//update fs, ls to get correct ans
  int fs = lim_col[linei][limiti].fs;
  int ls = lim_col[linei][limiti].ls;
  int l = lim_col[linei][limiti].l;
  int v = b[i][linei];
  if(v == BLACK){
    fs = max(fs, i - l+1);//最左解右移
    ls = min(ls, i);//最右解左移
  } else {
    while(fs <= ls){
      bool update = false;
      if(fs <= i && fs + l-1 >= i){
	fs++; update = true;}
      if(ls <= i && ls + l-1 >= i){
	ls--; update = true;}
      if(!update)
	break;
    }
    if(fs > ls){//no answer
      char s[1000];
      sprintf(s, "%d col %dth limit (%d, %d) -> fs %d > ls %d\n", linei, limiti, lim_col[linei][limiti].fs, lim_col[linei][limiti].fs, fs, ls);
      no_solution(s);
      return false;
    }
  }
  if(lim_col[linei][limiti].fs != fs || lim_col[linei][limiti].ls != ls){
    printf("(%d, %d)update lim_col[%d][%d]: (%d, %d) ", i, linei, linei, limiti,   lim_col[linei][limiti].fs,   lim_col[linei][limiti].ls);
    printf("-> (%d, %d)\n", fs, ls);
    if(limiti+1 < lim_col[linei].size())
        printf("update lim_col[%d][%d]: (%d, %d) \n", linei, limiti+1,   lim_col[linei][limiti+1].fs,   lim_col[linei][limiti+1].ls);
    //    update_lim_col(linei, limiti, fs, ls);
    }
  lim_col[linei][limiti].set_pos(fs, ls);
  if(fs == ls){
    update_line_col(linei);
    for(int i = 0; i < lim_col[linei][limiti].l; i++)
      set_grid(fs+i,linei,BLACK);
    if(fs - 1 >= 0)
      set_grid(fs-1,linei,WHITE);
    if(ls +l < r)
      set_grid(ls+l,linei,WHITE);
    check_solve(COL, linei);
  }
  return true;
}

bool Board::only_in_one_limit_col(int r, int c, int limiti){
  return (limiti == 0 || !in_limit_col(c, limiti-1, r)) && (in_limit_col(c, limiti, r)) && (limiti == lim_col[c].size()-1 || !in_limit_col(c, limiti+1, r));
}

bool Board::update_col(struct Point p, int v){//check column limit
  if(b[p.r][p.c] != v){
    printf("update_col: ");
    set_grid(p.r, p.c, v);
  }
  vector<int> la(r), ra(r);//left and right possible answer
  for(int i = 0; i < r; i++){
    la[i] = b[i][p.c];
    ra[i] = la[i];
  }
  if(v == WHITE){
    for(int j = 0; j < lim_col[p.c].size(); j++)
      if(in_limit_col(p.c, j, p.r))
	update_limit_col(p.c, j, p.r);
  }
  
  //if(v == BLACK)//....
  //先檢查已填的格子可能是哪些constrant的部分
  int limiti = -1;
  for(int j = 0; j < lim_col[p.c].size(); j++)
    if(only_in_one_limit_col(p.r, p.c, j)){
	  limiti = j; break;
    }
    
  /*    if(in_limit_col(p.c, j, p.r))//only possible at [j]
      if(j == lim_col[p.c].size()-1){
	if( j == 0 || !in_limit_col(p.c, j-1, p.r)){
	  limiti = j; break;
	}
      } else if(!in_limit_col(p.c, j+1, p.r)){
	limiti = j; break;
	}*/
  //if(limiti != -1)
  //  printf("(%d, %d) only one answer at %dth col limit\n",p.r, p.c, limiti+1);
  if(limiti == -1 || !update_limit_col(p.c, limiti, p.r)){
    //printf("no new update\n", p.r, p.c);
    return false;//no answer//temp erase
  } else {/*
    for(int i = lim_col[p.c][limiti].fs; i <= lim_col[p.c][limiti].ls+lim_col[p.c][limiti].l-1; i++){
      if(i == p.r)
	continue;//break;
      if(b[i][p.c] != SPACE && only_in_one_limit_col(i, p.c, limiti)){
	//printf("(%d, %d)only in %d limit col\n", i, p.c, limiti);
	update_limit_col(p.c, limiti, i);
      }
      }*/
  }
  return true;
}

bool Board::only_in_one_limit_row(int r, int c, int limiti){
  return (limiti == 0 || !in_limit_row(r, limiti-1, c)) &&                                                                  (in_limit_row(r, limiti, c)) &&                                                                                            (limiti == lim_row[r].size()-1 || !in_limit_row(r, limiti+1, c));
}

bool Board::in_limit_row(int linei, int limiti, int i){//is this grid possible in specific limit
  return (lim_row[linei][limiti].fs <= i && i <= lim_row[linei][limiti].ls + lim_row[linei][limiti].l - 1);  
}

bool Board::update_limit_row(int linei, int limiti, int i){//update limit fs, ls by a filled grid
  int fs = lim_row[linei][limiti].fs;
  int ls = lim_row[linei][limiti].ls;
  lim_row[linei][limiti].set_pos(fs, ls);
  int fe = lim_row[linei][limiti].fe;
  int le = lim_row[linei][limiti].le;
  int l = lim_row[linei][limiti].l;
  int v = b[linei][i];
  if(v == BLACK){
    fs = max(fs, i - l +1);//最左解右移
    ls = min(ls, i);//最右解左移
  } else {
    while(fs <= ls){//把白格子移出limit之外
      bool update = false;
      if(fs <= i && fe >= i){//white grid in fs~fe
	fs++; update = true;}
      if(ls <= i && le >= i){//white grid in ls~le
	ls--; update = true;}
      if(!update)
	break;
    }
    if(fs > ls){//no answer
      char s[100];
      sprintf(s, "%d row %dth limit (%d %d) -> fs %d > ls %d\n", linei, limiti, lim_row[linei][limiti].fs, lim_row[linei][limiti].fs, fs, ls);
      no_solution(s);
      return false;
    }
  }
  
  if(lim_row[linei][limiti].fs != fs || lim_row[linei][limiti].ls != ls){//print debug messge
    printf("update: (%d, %d) = %d lim_row[%d][%d]: (%d, %d)", linei, i, b[linei][i], linei, limiti,   lim_row[linei][limiti].fs,   lim_row[linei][limiti].ls);
    printf("-> (%d, %d)\n", fs, ls);
    if(limiti-1)
      printf("lim_row[%d][%d] fs = %d ls = %d\n", linei, limiti-1, lim_row[linei][limiti-1].fs, lim_row[linei][limiti-1].ls);
  }
  lim_row[linei][limiti].set_pos(fs, ls);
  if(fs == ls){//a limit complete
    fillknown_row(linei);
    for(int i = 0; i < lim_row[linei][limiti].l; i++)
      set_grid(linei,fs+i,BLACK);
    //check in the table
    if(fs - 1 >= 0)
      set_grid(linei,fs-1,WHITE);
    if(ls + l < c)
      set_grid(linei,ls+l,WHITE);
    check_solve(ROW, linei);
  } 
  return true;
}

void Board::fillknown_row(int r){
  printf("fillknown-row %d\n", r);
  int size = lim_row[r].size();
  for(int i = 0; i < lim_row[r][0].fs; i++)
    set_grid(r, i, WHITE);
  for(int i = 0; i < size-1; i++)
    for(int j = lim_row[r][i].ls + lim_row[r][i].l; j < lim_row[r][i+1].fs; j++)
      set_grid(r, j, WHITE);
  for(int i = lim_row[r][size-1].ls+lim_row[r][size-1].l; i < r; i++)
    set_grid(r, i, WHITE);
}

bool Board::update_row(struct Point p, int v){//copy
  if(b[p.r][p.c] != v){//is it possible??...
    printf("update_row(%d, %d): ", p.r, p.c);
    set_grid(p.r, p.c, v);
  }
  vector<int> la(c), ra(c);//left and right possible answer  
  for(int i = 0; i < c; i++){//init
    la[i] = b[p.r][i];
    ra[i] = la[i];
  }

  if(v == WHITE){
    for(int j = 0; j < lim_row[p.r].size(); j++)
      if(in_limit_row(p.r, j, p.c))
	update_limit_row(p.r, j, p.c);
  }
  else{
    //先檢查已填的格子可能是哪些constrant的部分，並用le, fe等選擇fr -> relimit(after fill)
    int limiti = -1;
    for(int j = 0; j < lim_row[p.r].size(); j++)
      if(only_in_one_limit_row(p.r, p.c, j)){
	limiti = j;
	break;
      }
    if(limiti == -1 || !update_limit_row(p.r, limiti, p.c)){
      return false;//no answer//temp erase
    } else {
      //printf("limiti = %d", limiti);
    }
  }
  return true;
}

void Board::print_board(const char in[]){
  printf("print board %s\n", in);
  int maxlr = 0;
  for(int i = 0; i < r; i++)
    if(maxlr < lim_row[i].size())
      maxlr = lim_row[i].size();
  int maxl = maxlr * 3;
  int maxlc = 0;
  for(int i = 0; i < c; i++){
    if(maxlc < lim_col[i].size())
      maxlc = lim_col[i].size();
  }
  for(int i = maxlc-1; i >= 0 ; i--){
    printf("%*s ", maxl, "");
    for(int j = 0; j < c; j++)
      if(lim_col[j].size() > maxlc-1-i)
	printf("%2d ", lim_col[j][maxlc-1-i].l);
      else
	printf("   ");
    puts("");
  }
  printf("%*s ", maxl, "");
  for(int i = 0; i < c; i++)
    printf("---");
  puts("");
  
  for(int i = 0; i < r; i++){
    for(int j = 0; j < maxlr; j++){
      if(j < lim_row[i].size())
	printf("%2d ", lim_row[i][j].l);
      else
	printf("   ");
    }
    printf("|");
    for(int j = 0; j < c; j++){
      if(b[i][j] != SPACE)
	printf("%2d ", (b[i][j] == 1)?1:0);
      else
	printf("   ");
    }
    
    puts("");
  }
  for(int i = 0; i < r; i++){
    for(int j = 0; j < c; j++){
      if(b[i][j] == 1)
	printf("%ls", black);
      else if(b[i][j] == -1)
	printf("%ls", white);
      else
	printf("%ls", space);
    }
    printf("%ls", cl);
  }
}

void Board::dfs(){
  
}

int Board::is_full_filled_row(int ri){
  int remain = r;
  for(int i = 0; i < lim_row[ri].size(); i++)
    remain -= lim_row[ri][i].l;
  remain -= lim_row[ri].size() - 1;
  return remain;
}

void Board::fill_row(int ri){//fill white betweenlast le and now fs
  int remain = is_full_filled_row(ri);
  if(remain == 0){//full filled
    int nowi = 0;
    for(int i = 0; i < lim_row[ri].size(); i++){
      if(i != 0)
	set_grid(ri, nowi++, WHITE);
      lim_row[ri][i].set_pos(nowi, nowi);
      for(int j = 0; j < lim_row[ri][i].l; j++)
	set_grid(ri, nowi++, BLACK);
    }
  } else { // fill partial
    int fs = 0, ls; //first start point, last start point
    int fe, le; // first end
    for(int i = 0; i < lim_row[ri].size(); i++){
      ls = fs + remain;
      fe = fs + lim_row[ri][i].l - 1;
      le = ls + lim_row[ri][i].l - 1;
      lim_row[ri][i].set_pos(fs, ls);
      for(int j = ls; j <= fe; j++)
	set_grid(ri, j, BLACK);
      fs = fe + 2;//next limit
      for(int j = le+1; j < fs; j++)
	set_grid(ri, j, WHITE);
    }
  }
}

int Board::is_full_filled_col(int ci){
  int remain = r;
  for(int i = 0; i < lim_col[ci].size(); i++)
    remain -= lim_col[ci][i].l;
  remain -= lim_col[ci].size() - 1;
  return remain;
}

void Board::fill_col(int ci){
  int remain = is_full_filled_col(ci);
  if(remain == 0){//full filled
    int nowi = 0;
    for(int i = 0; i < lim_col[ci].size(); i++){
      if(i != 0)
	set_grid(nowi++, ci, WHITE);
      // only one answer (first start = last start), fe and le?
      lim_col[ci][i].set_pos(nowi, nowi);
      for(int j = 0; j < lim_col[ci][i].l; j++)
	set_grid(nowi++, ci, BLACK);
    }
  } else { // partial fill
    int fs = 0, ls, fe, le; //first start point, last start point, first end, last end
    for(int i = 0; i < lim_col[ci].size(); i++){
      ls = fs + remain;
      fe = fs + lim_col[ci][i].l - 1;
      le = ls + lim_col[ci][i].l - 1;
      lim_col[ci][i].set_pos(fs, ls);
      for(int j = ls; j <= fe; j++) // they are sure to be black  between ls and fe
	set_grid(j, ci, BLACK);
      fs = fe + 2;//next limit's first start point
      for(int j = le+1; j < fs; j++) // from my le to next limit's fs is white
	set_grid(j, ci, WHITE);
    }
  }
}

void Board::do_heuristic(){
  for(int i = 0; i < c; i++)
    fill_col(i);
  print_board("after col");
  
  for(int i = 0 ; i < r; i++)
    fill_row(i);
  print_board("after col and row");
}
