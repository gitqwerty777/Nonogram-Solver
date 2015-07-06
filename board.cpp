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

enum chessType{WHITE=-1, BLACK=1, SPACE=0};

wchar_t black[] = L"\u25A0";
wchar_t white[] = L"\u25A1";
wchar_t space[] = L" ";
wchar_t cl[] = L"\n";

#ifdef __DEBUG__
#define DEBUG_PRINT(fmt, args...)  \
  fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#else
#define DEBUG_PRINT(fmt, args...)    /* Don't do anything in non-debug builds */
#endif

void no_solution(char in[]){
  printf("no solution: %s\n", in);
  exit(1);
}

void Board::fill_blank_col(int nowc){
  fillColByLimit(nowc);
  //DEBUG_PRINT("fillblankcol%d\n", nowc);
  int minl = INF;//min limit length
  for(int i = 0; i < lim_col[nowc].size(); i++)
    if(!lim_col[nowc][i].isSolved() && minl > lim_col[nowc][i].l)
      minl = lim_col[nowc][i].l;
  if (minl == INF){
    return;
  }
  int nowb = 0;
  bool allblack = true;
  //DEBUG_PRINT("minl = %d\n", minl);
  for(int i = 0;i < r; i++){//if possible empty grid sequence less than min limit, fill all to white
    if(b[i][nowc] != WHITE){
      if(b[i][nowc] == SPACE){
	allblack = false;
      }
      nowb++;
    } else{
      if(nowb < minl && !allblack){
	for(int j = 0; j < nowb; j++)
	  fillGrid(i-j, nowc, WHITE);
      }
      nowb = 0;
      allblack = true;
    }
  }
  if(nowb < minl && !allblack){//the last non-white sequence
    printBoard("before fillblank col");
    for(int j = 0; j < nowb; j++){
      fillGrid(r-1-j, nowc, WHITE);
    }
  }

}

void Board::fill_blank_row(int nowr){
  //DEBUG_PRINT("fill_blank_row %d\n", nowr);
  fillRowByLimit(nowr);
  int minl = INF;
  for(int i = 0; i < lim_row[nowr].size(); i++){
    if(!lim_row[nowr][i].isSolved() && minl > lim_row[nowr][i].l){
      minl = lim_row[nowr][i].l;
    }
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
	DEBUG_PRINT("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowr, i, i-nowb+1);
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
  if(nowb < minl && !allblack){//the last non-white sequence
    printBoard("before fillblank");
    //DEBUG_PRINT("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowr, c-1, c-nowb);
    for(int j = 0; j < nowb; j++){
	fillGrid(nowr, c-1-j, WHITE);
    }
  }
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
      exit(2);
    }
    //DEBUG_PRINT("fillGrid (%d, %d) = %s\n", r, c, (v==1)?"BLACK":"WHITE");
    change_row[r]++;
    change_col[c]++;
    b[r][c] = v;
    alreadySetGridNumber++;
  }
}

void Board::isLineSolved(line_type type, int line){  //check whole line is solved or not
  //DEBUG_PRINT("islinesolved %s-%d\n", type==ROW?"row":"col", line);
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
      DEBUG_PRINT("%d row all solve\n", line);
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
      DEBUG_PRINT("%d col all solve\n", line);
      printBoard("before update ");
      solved_col[line] = true;
      solved_num++;
      int nowi = 0; int nowr = 0;
      while(nowi < lim_col[line].size()){
	//DEBUG_PRINT("nowr %d\n", nowr);
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
    DEBUG_PRINT("updateHeuristic: row%d\n", line);
    fillRowByLimit(line);
    updateLimitByLimit_row(line);
    for(int j = 0; j < c; j++){
      if(b[line][j] != SPACE)
	updateLimit_row(Point(line,j), b[line][j]);
    }
    fill_blank_row(line);
  } else {
    DEBUG_PRINT("updateHeuristic: col%d\n", line);
    fillColByLimit(line);
    updateLimitByLimit_col(line);
    for(int j = 0; j < r; j++){
      if(b[j][line] != SPACE)
	updateLimit_col(Point(j, line), b[j][line]);
    }
    fill_blank_col(line);
  }
  return originalSetnum != alreadySetGridNumber;//is really updated or not
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

bool Board::updateLimitByGrid_col(int linei, int limiti, int nowr){//update fs, ls to get correct ans
  int fs = lim_col[linei][limiti].fs;
  int ls = lim_col[linei][limiti].ls;
  int fe = lim_col[linei][limiti].fe;
  int le = lim_col[linei][limiti].le;
  int l = lim_col[linei][limiti].l;
  int v = b[nowr][linei];
  if(v == BLACK){
    //DEBUG_PRINT("updatelimitbygrid_col (%d, %d), limit%d, black\n", nowr, linei, limiti);
    //DEBUG_PRINT("limit%d = (%d, %d)", limiti, fs, ls);
    if(only_in_one_limit_col(nowr, linei, limiti)){    //find sequences
      //DEBUG_PRINT("inlimit\n");
      int seqs = nowr, seqe = nowr;
      while(seqs >= 0 && b[seqs][linei] == BLACK){
	seqs--;
      }
      while(seqe < r && b[seqe][linei] == BLACK){
	seqe++;
      }
      //seqs and seqe are black
      if(seqs < 0 || b[seqs][linei] != BLACK){
	seqs++;
      }
      if(seqe >= r || b[seqe][linei] != BLACK){
	seqe--;
      }
      fs = max(fs, seqe-l+1);//最左解右移
      ls = min(ls, seqs);//最右解左移
      //can move previous and later limit either...
      if(limiti - 1 >= 0){
	//update previous limit
	int li = limiti-1;
	int pfs = lim_col[linei][li].fs;
	int pls = lim_col[linei][li].ls;
	int pl = lim_col[linei][li].l;
	lim_col[linei][li].set_pos(pfs, min(pls, seqs-1-pl));
	if(lim_col[linei][li].isSolved()){
	  setColLimitSolved(linei, li);
	}
      }
      if(limiti + 1 < lim_col[linei].size()){
	int li = limiti+1;
	int pfs = lim_col[linei][li].fs;
	int pls = lim_col[linei][li].ls;
	int pl = lim_col[linei][li].l;
	lim_col[linei][li].set_pos(max(pfs, seqe+2), pls);
      	if(lim_col[linei][li].isSolved()){
	  setColLimitSolved(linei, li);
	}
      }
      DEBUG_PRINT("update black grid sequence(%d~%d, %d): limit=(%d, %d)", seqs, seqe, linei, fs, ls);
    }
  } else {
    while(fs <= ls){
      bool update = false;
      if(fs <= nowr && fe >= nowr){
	fs++; fe++; update = true;}
      if(ls <= nowr && le >= nowr){
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
    DEBUG_PRINT("use grid(%d, %d) to update lim_col[%d][%d]: (%d, %d) ", nowr, linei, linei, limiti,   lim_col[linei][limiti].fs,   lim_col[linei][limiti].ls);
    DEBUG_PRINT("-> (%d, %d)\n", fs, ls);
    //if(limiti+1 < lim_col[linei].size())
    //  DEBUG_PRINT("update lim_col[%d][%d]: (%d, %d) \n", linei, limiti+1,   lim_col[linei][limiti+1].fs,   lim_col[linei][limiti+1].ls);
    //    update_lim_col(linei, limiti, fs, ls);
    }
  lim_col[linei][limiti].set_pos(fs, ls);
  if(fs == ls){//solve this limit successfully!
    setColLimitSolved(linei, limiti);
  }
  return true;
}

bool Board::only_in_one_limit_row(int nowr, int nowc, int limiti){
  return (limiti == 0 || !in_limit_row(nowr, limiti-1, nowc)) && (in_limit_row(nowr, limiti, nowc)) && (limiti == lim_row[nowr].size()-1 || !in_limit_row(nowr, limiti+1, nowc));
}
bool Board::in_limit_row(int nowr, int limiti, int nowc){
  return (lim_row[nowr][limiti].fs <= nowc && lim_row[nowr][limiti].le >= nowc);  
}
bool Board::only_in_one_limit_col(int nowr, int nowc, int limiti){
  return (limiti == 0 || !in_limit_col(nowc, limiti-1, nowr)) && (in_limit_col(nowc, limiti, nowr)) && (limiti == lim_col[nowc].size()-1 || !in_limit_col(nowc, limiti+1, nowr));
}
bool Board::in_limit_col(int nowc, int limiti, int nowr){
  /*bool ans = (lim_col[nowc][limiti].fs <= nowr && lim_col[nowc][limiti].le >= nowr);
  if(ans){
    DEBUG_PRINT("(%d, %d) in col limit%d = (%d, %d)\n", nowr, nowc, limiti, lim_col[nowc][limiti].fs, lim_col[nowc][limiti].le);
  } else {
    DEBUG_PRINT("(%d, %d) not in col limit%d = (%d, %d)\n", nowr, nowc, limiti, lim_col[nowc][limiti].fs, lim_col[nowc][limiti].le);
    }*/
  return (lim_col[nowc][limiti].fs <= nowr && lim_col[nowc][limiti].le >= nowr);  
}

bool Board::updateLimit_col(struct Point p, int v){//check column limit
  //DEBUG_PRINT("updatelimit_col(%d, %d)\n", p.r, p.c);
  if(v != SPACE){
    for(int j = 0; j < lim_col[p.c].size(); j++)
      if(in_limit_col(p.c, j, p.r))
	updateLimitByGrid_col(p.c, j, p.r);
  }
  //if(v == BLACK)//....
  //先檢查已填的格子可能是哪些constrant的部分
  //is this important??
  /*int limiti = -1;
  for(int j = 0; j < lim_col[p.c].size(); j++)
    if(only_in_one_limit_col(p.r, p.c, j)){
	  limiti = j; break;
    }
  if(limiti != -1){
    if(!updateLimitByGrid_col(p.c, limiti, p.r)){
      DEBUG_PRINT("no new update\n", p.r, p.c);
    }
  } else {
    return false;//no answer//temp erase
    }*/
  return true;
}


bool Board::updateLimitByLimit_col(int nowc){
  int nfs, nls;
  for(int i = 1; i < lim_col[nowc].size(); i++){
    if(lim_col[nowc][i].isSolved()){
      continue;
    }
    nfs = max(lim_col[nowc][i-1].fe+2, lim_col[nowc][i].fs);
    if(i+1 < lim_col[nowc].size()){
      nls = min(lim_col[nowc][i+1].ls-1-lim_col[nowc][i].l , lim_col[nowc][i].ls);
    } else {
      nls = lim_col[nowc][i].ls;
    }
    //nls = min(lim_col[nowc][i-1].le+2, lim_col[nowc][i].ls);
    lim_col[nowc][i].set_pos(nfs, nls);
    if(lim_col[nowc][i].isSolved()){
      setColLimitSolved(nowc, i);
    }
  }
  //fillColByLimit(nowc);
}
bool Board::updateLimitByLimit_row(int nowr){
  int nfs, nls;
  for(int i = 1; i < lim_row[nowr].size(); i++){
    if(lim_row[nowr][i].isSolved()){
      continue;
    }
    nfs = max(lim_row[nowr][i-1].fe+2, lim_row[nowr][i].fs);
    if(i+1 < lim_row[nowr].size()){
      nls = min(lim_row[nowr][i+1].ls-1-lim_row[nowr][i].l , lim_row[nowr][i].ls);
    } else {
      nls = lim_row[nowr][i].ls;
    }
    //nls = min(lim_row[nowr][i-1].le+2, lim_row[nowr][i].ls);
    lim_row[nowr][i].set_pos(nfs, nls);
    if(lim_row[nowr][i].isSolved()){
      setRowLimitSolved(nowr, i);
    }
  }
}
bool Board::updateLimitByGrid_row(int linei, int limiti, int i){//copy
  int fs = lim_row[linei][limiti].fs;
  int ls = lim_row[linei][limiti].ls;
  int fe = lim_row[linei][limiti].fe;
  int le = lim_row[linei][limiti].le;
  int l = lim_row[linei][limiti].l;
  int v = b[linei][i];
  if(v == BLACK){
    //find sequences
    if(only_in_one_limit_row(linei, i, limiti)){
      int seqs = i, seqe = i;
      while(seqs >= 0 && b[linei][seqs] == BLACK){
	seqs--;
      }
      while(seqe < c && b[linei][seqe] == BLACK){
	seqe++;
      }
      //seqs and seqe are black
      if(seqs < 0 || b[linei][seqs] != BLACK){
	seqs++;
      }
      if(seqe >= c || b[linei][seqe] != BLACK){
	seqe--;
      }
      fs = max(fs, seqe-l+1);//最左解右移
      ls = min(ls, seqs);//最右解左移
      DEBUG_PRINT("update black grid sequence(%d, %d~%d): limit=(%d, %d)", linei, seqs, seqe, fs, ls);
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
    DEBUG_PRINT("(%d, %d) = %d lim_row[%d][%d]: (%d, %d)", linei, i, b[linei][i], linei, limiti,   lim_row[linei][limiti].fs,   lim_row[linei][limiti].ls);
    DEBUG_PRINT("-> (%d, %d)\n", fs, ls);
  }
  lim_row[linei][limiti].set_pos(fs,ls);
  if(fs == ls){
    setRowLimitSolved(linei, limiti);
  } 
  return true;
}

void Board::fillRowByLimit(int nowr){
  //DEBUG_PRINT("fillrowbylimit%d\n", nowr);
  int size = lim_row[nowr].size();
  for(int i = 0; i < lim_row[nowr][0].fs; i++)
    fillGrid(nowr,i, WHITE);
  for(int i = 0; i < size-1; i++)
    for(int j = lim_row[nowr][i].ls + lim_row[nowr][i].l; j < lim_row[nowr][i+1].fs; j++)
      fillGrid( nowr,j, WHITE);
  for(int i = 0; i < size; i++)
    for(int j = lim_row[nowr][i].ls; j <= lim_row[nowr][i].fe; j++)
      fillGrid( nowr,j, BLACK);
  for(int i = lim_row[nowr][size-1].ls+lim_row[nowr][size-1].l; i < c; i++)
    fillGrid(nowr, i,WHITE);
}

bool Board::updateLimit_row(struct Point p, int v){//copy
  //DEBUG_PRINT("updatelimit_row(%d, %d)\n", p.r, p.c);
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
    //DEBUG_PRINT("no new update\n", p.r, p.c);
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

