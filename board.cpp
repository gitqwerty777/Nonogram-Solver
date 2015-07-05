#include "board.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#define MAXINT 2147483647
#define INF 2147483647
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
  fillColByLimit(nowc);
  int minl = 0;//min limit length
  for(int i = 1; i < lim_col[nowc].size(); i++)
    if(!lim_col[nowc][i].isSolved() && minl > lim_col[nowc][i].l)
      minl = lim_col[nowc][i].l;
  if (minl == 0){
    return;
  }
  int nowb = 0;
  for(int i = 0;i < r; i++)//if possible empty grid sequence less than min limit, fill all to white
    if(b[i][nowc] != WHITE){
      nowb++;
      if(i == r-1){//last
	if(nowb < minl)
	  for(int j = 0; j < nowb; j++)
	    fillGrid(i-j, nowc, WHITE);
      }
    } else{
      if(nowb < minl){
	for(int j = 0; j < nowb; j++)
	  fillGrid(i-j, nowc, WHITE);
      }
      nowb = 0;
    }
}

void Board::fill_blank_row(int nowr){
  fillRowByLimit(nowr);

  int minl = INF;
  //  int maxl = INF;
  for(int i = 0; i < lim_row[nowr].size(); i++){
    if(!lim_row[nowr][i].isSolved() && minl > lim_row[nowr][i].l){
      minl = lim_row[nowr][i].l;
    }
    
    //if(!lim_row[nowr][i].isSolved() && maxl < lim_row[nowr][i].l)
    //minl = lim_row[nowr][i].l;
  }
  if(minl == INF){
    return;
  }

  int nowb = 0;
  bool allblack = true;
  for(int i = 0;i < c; i++){
    if(b[nowr][i] == WHITE){
      if(nowb < minl && !allblack){
	printBoard("before fillblank");
	printf("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowr, i, i-nowb+1);
	for(int j = 0; j < nowb; j++){
	  fillGrid(nowr, i-j, WHITE);
	}
      }
      nowb = 0;
      allblack = true;
    } else {
      if(b[nowr][i] == SPACE){
	allblack = false;
      }
      nowb++;
    }
  }
  if(nowb < minl)//the last non-white sequence
    for(int j = 0; j < nowb; j++)
      fillGrid(nowr, c-1-j, WHITE);
}

bool Board::isSolved(){
  if(solved_num == r + c)
    return true;
  return false;
}

void Board::fillGrid(int r, int c, int v){
  if(b[r][c] != v){
    if(b[r][c] == -v){
      printf("no solution:  ");
      printf("illegal fillGrid(%d, %d) from %d to %d\n", r, c, b[r][c], v);
      exit(0);
    }
    printf("fillGrid (%d, %d) = %s\n", r, c, (v==1)?"BLACK":"WHITE");
    change_row[r]++;
    change_col[c]++;
    b[r][c] = v;
    alreadySetGridNumber++;
  }
}

void Board::isLineSolved(line_type type, int line){  //check whole line is solved or not
  if(type == ROW){//row
    if(solved_row[line])//already solved...
      return;
    bool allsolve = true;
    for(int i = 0; i < lim_row[line].size(); i++){
      if(lim_row[line][i].isSolved())
	continue;
      else{
	allsolve = false;
	break;
      }
    }
    if(allsolve){
      printf("%d row all solve\n", line);
      solved_row[line] = true;
      solved_num++;
      int nowi = 0; int nowc = 0;
      while(nowi < lim_row[line].size()){
	if(nowc == lim_row[line][nowi].fs){
	  for(int i = 0; i < lim_row[line][nowi].l; i++)
	    fillGrid(line, nowc++, BLACK);
	  nowi++;
	} else
	  fillGrid(line, nowc++, WHITE);
      }
      for(int i = nowc; i < c; i++)
	fillGrid(line, i, WHITE);
      printBoard("after update ");
    }
  } else if (type == COL) {//col
    if(solved_col[line])
      return;
    bool allsolve = true;
    for(int i = 0; i < lim_col[line].size(); i++){
      if(lim_col[line][i].isSolved())
	continue;
      else{
	allsolve = false;
	break;
      }
    }
    if(allsolve){
      printf("%d col all solve\n", line);
      printBoard("before update ");
      solved_col[line] = true;
      solved_num++;
      int nowi = 0; int nowr = 0;
      while(nowi < lim_col[line].size()){
	printf("nowr %d\n", nowr);
	if(nowr == lim_col[line][nowi].fs){
	  for(int i = 0; i < lim_col[line][nowi].l; i++)
	    fillGrid(nowr++, line, BLACK);
	  nowi++;
	} else
	  fillGrid(nowr++, line, WHITE);
      }
      for(int i = nowr; i < r; i++)
	fillGrid(i, line, WHITE);
      printBoard("after update ");
    }
  }
}

bool Board::heuristic(){ //TODO: change to line_type
  //update_h();
  vector<pair<int, int> > maxChanged;
  int max = -1; 
  for(int i = 0;i < r; i++)
    if(!solved_row[i] && max <= change_row[i]){
      if(max == change_row[i]){
	maxChanged.push_back(pair<int, int>(1, i));
      } else{
	max = change_row[i];
	maxChanged.clear();
	maxChanged.push_back(pair<int, int>(1, i));
      }
    }
  for(int i = 0; i < c; i++)
    if(!solved_col[i] && max <= change_col[i]){
      if(max == change_col[i]){
	maxChanged.push_back(pair<int, int>(2, i));
      } else{
	max = change_col[i];
	maxChanged.clear();
	maxChanged.push_back(pair<int, int>(2, i));
      }
    }
  bool isChange = false;
  for(int i = 0;i < maxChanged.size(); i++){
    if(maxChanged[i].first == 1)
      change_row[maxChanged[i].second] = 0;
    else
      change_col[maxChanged[i].second] = 0;
    if(updateHeuristic(maxChanged[i].first, maxChanged[i].second))
      isChange = true;
  }
  return isChange;
}

bool Board::updateHeuristic(int type, int line){//if there are no any hint can solve it
  int originalSetnum = alreadySetGridNumber;
  if(type == 1){//row
    printf("updateHeuristic: row%d\n", line);
    fillRowByLimit(line);
    for(int j = 0; j < c; j++){
      if(b[line][j] != SPACE)
	updateLimit_row(Point(line,j), b[line][j]);
    }
    fill_blank_row(line);
  } else {
    printf("updateHeuristic: col%d\n", line);
    fillColByLimit(line);
    for(int j = 0; j < r; j++){
      if(b[j][line] != SPACE)
	updateLimit_col(Point(j, line), b[j][line]);
    }
    fill_blank_col(line);
  }
  return originalSetnum != alreadySetGridNumber;//is really updated or not
}

bool Board::in_limit_col(int linei, int limiti, int i){
  return (lim_col[linei][limiti].fs <= i && lim_col[linei][limiti].ls + lim_col[linei][limiti].l-1 >= i);  
}

void Board::fillColByLimit(int c){
  int size = lim_col[c].size();
  for(int i = 0; i < lim_col[c][0].fs; i++)
    fillGrid(i, c, WHITE);
  for(int i = 0; i < size-1; i++)
    for(int j = lim_col[c][i].ls + lim_col[c][i].l; j < lim_col[c][i+1].fs; j++)
      fillGrid(j, c, WHITE);
  for(int i = 0; i < size; i++)
    for(int j = lim_col[c][i].ls; j <= lim_col[c][i].fe; j++)
      fillGrid(j, c, BLACK);
  for(int i = lim_col[c][size-1].ls+lim_col[c][size-1].l; i < r; i++)
    fillGrid(i, c, WHITE);
}

bool Board::updateLimitByGrid_col(int linei, int limiti, int i){//update fs, ls to get correct ans
  int fs = lim_col[linei][limiti].fs;
  int ls = lim_col[linei][limiti].ls;
  int fe = lim_col[linei][limiti].fe;
  int le = lim_col[linei][limiti].le;
  int l = lim_col[linei][limiti].l;
  int v = b[i][linei];
  if(v == BLACK){
    if(only_in_one_limit_col(i, linei, limiti)){
      //printf("update black grid: fs = %d, i-l+1 = %d ls = %d i = %d", fs, i-l+1, ls, i);
      fs = max(fs, i - l+1);//最左解右移
      ls = min(ls, i);//最右解左移
    }
  } else {
    while(fs <= ls){
      bool update = false;
      if(fs <= i && fe >= i){
	fs++; fe++; update = true;}
      if(ls <= i && le >= i){
	ls--; le--; update = true;}
      if(!update)
	break;
    }
  }
  if(fs > ls){//no answer
    char s[1000];
    sprintf(s, "%d col %dth limit (%d, %d) -> fs %d > ls %d\n", linei, limiti, lim_col[linei][limiti].fs, lim_col[linei][limiti].fs, fs, ls);
    no_solution(s);
    return false;
  }
  if(lim_col[linei][limiti].fs != fs || lim_col[linei][limiti].ls != ls){//print some debug message
    printf("(%d, %d)update lim_col[%d][%d]: (%d, %d) ", i, linei, linei, limiti,   lim_col[linei][limiti].fs,   lim_col[linei][limiti].ls);
    printf("-> (%d, %d)\n", fs, ls);
    if(limiti+1 < lim_col[linei].size())
        printf("update lim_col[%d][%d]: (%d, %d) \n", linei, limiti+1,   lim_col[linei][limiti+1].fs,   lim_col[linei][limiti+1].ls);
    //    update_lim_col(linei, limiti, fs, ls);
    }
  lim_col[linei][limiti].set_pos(fs, ls);
  if(fs == ls){//solve this limit successfully!
    setColLimitSolved(linei, limiti);
  }
  return true;
}

bool Board::only_in_one_limit_col(int r, int c, int limiti){
  return (limiti == 0 || !in_limit_col(c, limiti-1, r)) && (in_limit_col(c, limiti, r)) && (limiti == lim_col[c].size()-1 || !in_limit_col(c, limiti+1, r));
}

bool Board::updateLimit_col(struct Point p, int v){//check column limit
  if(v != SPACE){
    for(int j = 0; j < lim_col[p.c].size(); j++)
      if(in_limit_col(p.c, j, p.r))
	updateLimitByGrid_col(p.c, j, p.r);
  }
  //if(v == BLACK)//....
  //先檢查已填的格子可能是哪些constrant的部分
  int limiti = -1;
  for(int j = 0; j < lim_col[p.c].size(); j++)
    if(only_in_one_limit_col(p.r, p.c, j)){
	  limiti = j; break;
    }
  if(limiti != -1){
    if(!updateLimitByGrid_col(p.c, limiti, p.r)){
      printf("no new update\n", p.r, p.c);
    }
  } else {
    return false;//no answer//temp erase
  }
  return true;
}

bool Board::only_in_one_limit_row(int r, int c, int limiti){
  return (limiti == 0 || !in_limit_row(r, limiti-1, c)) && (in_limit_row(r, limiti, c)) && (limiti == lim_row[r].size()-1 || !in_limit_row(r, limiti+1, c));
}

bool Board::in_limit_row(int linei, int limiti, int i){
  return (lim_row[linei][limiti].fs <= i && lim_row[linei][limiti].le >= i);  
}

bool Board::updateLimitByGrid_row(int linei, int limiti, int i){//copy
  int fs = lim_row[linei][limiti].fs;
  int ls = lim_row[linei][limiti].ls;
  int fe = lim_row[linei][limiti].fe;
  int le = lim_row[linei][limiti].le;
  int l = lim_row[linei][limiti].l;
  int v = b[linei][i];
  if(v == BLACK){
    if(only_in_one_limit_row(linei, i, limiti)){
      fs = max(fs, i - l +1);//最左解右移
      ls = min(ls, i);//最右解左移
    }
  } else {
    while(fs <= ls){//white in the possible answer
      bool update = false;
      if(fs <= i && fe >= i){
	fs++; fe++; update = true;}
      if(ls <= i && le >= i){
	ls--; le--; update = true;}
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
  if(lim_row[linei][limiti].fs != fs || lim_row[linei][limiti].ls != ls){
    
    printf("(%d, %d) = %d lim_row[%d][%d]: (%d, %d)", linei, i, b[linei][i], linei, limiti,   lim_row[linei][limiti].fs,   lim_row[linei][limiti].ls);
    printf("-> (%d, %d)\n", fs, ls);
    if(limiti-1)
    printf("lim_row[%d][%d] fs = %d ls = %d\n", linei, limiti-1, lim_row[linei][limiti-1].fs, lim_row[linei][limiti-1].ls);
    }
  lim_row[linei][limiti].set_pos(fs,ls);
  if(fs == ls){
    setRowLimitSolved(linei, limiti);
  } 
  return true;
}

void Board::fillRowByLimit(int r){
  int size = lim_row[r].size();
  for(int i = 0; i < lim_row[r][0].fs; i++)
    fillGrid(r,i, WHITE);
  for(int i = 0; i < size-1; i++)
    for(int j = lim_row[r][i].ls + lim_row[r][i].l; j < lim_row[r][i+1].fs; j++)
      fillGrid( r,j, WHITE);
  for(int i = 0; i < size; i++)
    for(int j = lim_row[r][i].ls; j <= lim_row[r][i].fe; j++)
      fillGrid( r,j, BLACK);
  for(int i = lim_row[r][size-1].ls+lim_row[r][size-1].l; i < r; i++)
    fillGrid(r, i,WHITE);
}

bool Board::updateLimit_row(struct Point p, int v){//copy
  if(v != SPACE){
    for(int j = 0; j < lim_row[p.r].size(); j++)
      if(in_limit_row(p.r, j, p.c))
	updateLimitByGrid_row(p.r, j, p.c);
  }
  //檢查已填的格子可能是哪些limit的一部分，用le, fe等選擇fr -> relimit(after fill)
  int limiti = -1;
  for(int j = 0; j < lim_row[p.r].size(); j++)
    if(only_in_one_limit_row(p.r, p.c, j)){
	  limiti = j; break;
    }
  if(limiti == -1 || !updateLimitByGrid_row(p.r, limiti, p.c)){//is later statement possible?
    //printf("no new update\n", p.r, p.c);
    return false;//no answer//temp erase
  } else {
    
  }
  return true;
}

void Board::printBoard(const char in[]){
  printf("print board: %s\n", in);
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
  /*for(int i = 0; i < r; i++){
    for(int j = 0; j < c; j++){
      if(b[i][j] == 1)
	printf("%ls", black);
      else if(b[i][j] == -1)
	printf("%ls", white);
      else
	printf("%ls", space);
    }
    printf("%ls", cl);
    }*/
  //additional: print all limits
  for(int i = 0; i < r; i++){
    printf("\nrow %d: ", i+1);
    for(int j = 0; j < lim_row[i].size(); j++){
      printf("(%d, %d),  ", lim_row[i][j].fs, lim_row[i][j].ls);
    }
  }
  for(int i = 0; i < c; i++){
    printf("\ncol %d:", i+1);
    for(int j = 0; j < lim_col[i].size(); j++){
      printf("(%d, %d)", lim_col[i][j].fs, lim_col[i][j].ls);
    }
  }
}



void Board::setColLimitSolved(int linei, int limiti){
  int fs = lim_col[linei][limiti].fs;
  int ls = lim_col[linei][limiti].ls;
  int l = lim_col[linei][limiti].l;
  for(int i = 0; i < l; i++)
    fillGrid(fs+i,linei,BLACK);
  if(fs - 1 >= 0)
    fillGrid(fs-1,linei,WHITE);
  if(ls +l < r)
    fillGrid(ls+l,linei,WHITE);
  fill_blank_col(linei);
  isLineSolved(COL, linei);
}
void Board::setRowLimitSolved(int linei, int limiti){
  int fs = lim_row[linei][limiti].fs;
  int ls = lim_row[linei][limiti].ls;
  int l = lim_row[linei][limiti].l;
  if(linei == 5 && limiti == 0){
    printf("\n\n\n53, fs = %d, ls = %d\n", fs, ls);
  }
  for(int i = 0; i < l; i++)
    fillGrid(linei,fs+i,BLACK);
  if(fs - 1 >= 0)
    fillGrid(linei,fs-1,WHITE);
  if(ls + l < c)
    fillGrid(linei,ls+l,WHITE);
  fill_blank_row(linei);
  isLineSolved(ROW, linei);
}

void Board::initialFillRow(int ri){//fill white betweenlast le and now fs
  int remain = c;
  for(int i = 0; i < lim_row[ri].size(); i++)
    remain -= lim_row[ri][i].l;
  remain -= lim_row[ri].size() - 1;
  if(remain == 0){//full filled
    int nowi = 0;
    for(int i = 0; i < lim_row[ri].size(); i++){
      if(i != 0)
	fillGrid(ri, nowi++, WHITE);
      lim_row[ri][i].set_pos(nowi, nowi);
      for(int j = 0; j < lim_row[ri][i].l; j++)
	fillGrid(ri, nowi++, BLACK);
    }
    for(int i = 0; i < lim_row[ri].size(); i++){
      setRowLimitSolved(ri, i);
    }
  } else { // fill partial
    int fs = 0, ls; //first start point, last start point
    int fe, le; // first end
    for(int i = 0; i < lim_row[ri].size(); i++){
      ls = fs + remain;
      fe = fs + lim_row[ri][i].l - 1;
      le = ls + lim_row[ri][i].l - 1;
      for(int j = ls; j <= fe; j++)
	fillGrid(ri, j, BLACK);
      lim_row[ri][i].set_pos(fs, ls);
      fs = fe + 2;//next limit
      for(int j = le+1; j < fs; j++)
	fillGrid(ri, j, WHITE);
    }
  }
}

void Board::initialFillCol(int ci){
  int remain = r;
  for(int i = 0; i < lim_col[ci].size(); i++)
    remain -= lim_col[ci][i].l;
  remain -= lim_col[ci].size() - 1;
  if(remain == 0){//full filled
    int nowi = 0;
    for(int i = 0; i < lim_col[ci].size(); i++){
      lim_col[ci][i].set_pos(nowi, nowi);
      for(int j = 0; j < lim_col[ci][i].l; j++)
	fillGrid(nowi++, ci, BLACK);
      if(i != lim_col[ci].size()-1)
	fillGrid(nowi++, ci, WHITE);
    }
    for(int i = 0; i < lim_col[ci].size(); i++){
      setColLimitSolved(ci, i);
    }
  } else { // fill partial
    int fs = 0, ls; //first start point, last start point
    int fe, le; // first end
    for(int i = 0; i < lim_col[ci].size(); i++){
      ls = fs + remain;
      fe = fs + lim_col[ci][i].l - 1;
      le = ls + lim_col[ci][i].l - 1;
      for(int j = ls; j <= fe; j++){
	fillGrid(j, ci, BLACK);
      }
      lim_col[ci][i].set_pos(fs, ls);
      fs = fe + 2;//next limit
      for(int j = le+1; j < fs; j++)
	fillGrid(j, ci, WHITE);
    }
  }
}

void Board::doHeuristic(){
  for(int i = 0; i < c; i++)
    initialFillCol(i);
  printBoard("after fill col");
  for(int i = 0 ; i < r; i++)
    initialFillRow(i);
  printBoard("after fill col and row");

  //recursively do heuristic after no sure grid to fill in
  while(!isSolved()){
    heuristic();
  }
}

void Board::doDFS(){
  

}

