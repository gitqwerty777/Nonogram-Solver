#include "board.h"
#include "dfsboard.h"
#include "nonogramWriter.h"
#include <cstdio>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <locale.h>
#define INF 2147483647

using namespace std;

#define saveAndExit(errorcode) {\
  saveResult();\
  exit(errorcode);\
}
 
inline void Board::no_solution(const char in[]){
  if(solveMode == HEURISTIC){
    printf("no solution: %s\n", in);
    printBoard("no solution");
    saveAndExit(1);
  } else if(solveMode == DFS) {
    DEBUG_PRINT("no solution: %s\n", in);
    sprintf(tryFailedReason, "%s\n", in);
    tryFailed = true;
  }
}
inline void Board::no_solution(const char in[], line_type t, int i){//check answer failed
  if(solveMode == HEURISTIC){
    printf("wrong solution: %s %s%d\n", in, (t==ROW)?"ROW":"COL", i);
    saveAndExit(5);
  } else if (solveMode == DFS) {
    DEBUG_PRINT("wrong solution: %s %s%d\n", in, (t==ROW)?"ROW":"COL", i);
    sprintf(tryFailedReason, "%s", in);
    tryFailed = true;
  }
}

void Board::doHeuristic(){
  solveMode = HEURISTIC;

  for(int i = 0; i < c; i++)
    initialFillCol(i);
  printBoard("after fill col");
  
  for(int i = 0 ; i < r; i++)
    initialFillRow(i);
  printBoard("after fill col and row");
  
  isLimitInit = true;

  /*for(int i = 0; i < r; i++)
    for(int j = 0; j < c; j++)
      if(b[i][j] != SPACE)
      fillGrid(i, j, b[i][j]);*/
  
  while(!isAllSolved()){//do heuristic after no limit and grid to update
    if(!doHeuristicByLine())
      break;
  }
  /*if(!isAllSolved()){
    printBoard("beforeheu");
    assert(!doHeuristicByLine());
    printBoard("afterheu");
    }*/
}

void Board::initialFillRow(int ri){//TODO: consider no limit in row and col
  int remain = c;
  for(int i = 0; i < lim_row[ri].size(); i++)
    remain -= lim_row[ri][i].l;
  remain -= lim_row[ri].size() - 1;
  if(remain == 0){//limit would fully filled the line
    int nowi = 0;
    for(int i = 0; i < lim_row[ri].size(); i++){
      if(i != 0)
	fillGrid(ri, nowi++, WHITE);
      lim_row[ri][i].set_pos(nowi, nowi);
      for(int j = 0; j < lim_row[ri][i].l; j++)
	fillGrid(ri, nowi++, BLACK);
    }
    for(int i = 0; i < lim_row[ri].size(); i++)//set all limit in row "solved"
      setRowLimitSolved(ri, i);
  } else { // fill partial
    int fs = 0, ls;
    int fe, le;
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
  //changeNumQueue.push(change(ROW, ri));
}
void Board::initialFillCol(int ci){
  int remain = r;
  for(int i = 0; i < lim_col[ci].size(); i++)
    remain -= lim_col[ci][i].l;
  remain -= lim_col[ci].size() - 1;
  if(remain == 0){
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
    int fs = 0, ls;
    int fe, le;
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
  //changeNumQueue.push(change(COL, ci));
}

bool Board::doHeuristicByLine(){//TODO:Optimize, merge queue into class member
  tryFailed = false; // TODO: why this is needed...
  priority_queue<change, vector<change>, change> changeQueue;
  for(int i = 0;i < r; i++)
    if(!solved_row[i])
      changeQueue.push(change(ROW, i, change_row[i]));
  for(int i = 0; i < c; i++)
    if(!solved_col[i])
      changeQueue.push(change(COL, i, change_col[i]));
  
  bool isChange = false;
  int previousChangeNum = 0;
  int previousSetGrid = alreadySetGridNumber;
  int maxChangeNum = changeQueue.top().changeNum;
  while(!changeQueue.empty()){
    change ch = changeQueue.top();
    changeQueue.pop();
    int *mychange;
    if(ch.type == ROW)
      mychange = &change_row[ch.lineNum];
    else
      mychange = &change_col[ch.lineNum];
    *mychange = 0; // reset changed num
    DEBUG_PRINT("update %s %d\n", (ch.type==ROW)?"ROW":"COL", ch.lineNum);
    if(updateByHeuristic(ch.type, ch.lineNum))
      isChange = true;
    if(tryFailed){
      return false;
    }
    if(*mychange != 0)
      changeQueue.push(change(ch.type, ch.lineNum, *mychange));
  }
  if(!isChange && maxChangeNum == 0){
    if(solveMode == HEURISTIC){
      DEBUG_PRINT("NO HEURISTIC ANSWER\n");
      printf("no heursitic answer\n");
      return false;
    }
    else{
      //printf("dfs heursitic complete\n");
      return false;
    }
  }
  return true;
}

bool Board::updateByHeuristic(line_type type, int line){//TODO: improve process
  int originalSetnum = alreadySetGridNumber;
  if(type == ROW){
    DEBUG_PRINT("updateHeuristic: row%d\n", line);
    updateLimitByLimit_row(line);//limit
    fillRowByLimit(line);//limits are updated by others, need to fillgrid //limit
    //changed_row[line] = false;
    for(int j = 0; j < c; j++){//TODO: optimize
      if(b[line][j] != SPACE)
	updateRowLimits(Point(line,j), b[line][j]);//grid, limit
    }
    fill_blank_row(line);

    //if(changed_row[line]){
    //updateLimitByLimit_row(line);
    //      fillRowByLimit(line);
      //changed_row[line] = false;
      //}
  } else {
    DEBUG_PRINT("updateHeuristic: col%d\n", line);
    updateLimitByLimit_col(line);
    fillColByLimit(line);
    //changed_col[line] = false;
    for(int j = 0; j < r; j++){
      if(b[j][line] != SPACE)
	updateColLimits(Point(j, line), b[j][line]);
    }
    fill_blank_col(line);
    //if(changed_col[line]){
    //updateLimitByLimit_col(line);
    //      fillColByLimit(line);
      //changed_col[line] = false;
      //}
  }
  return originalSetnum != alreadySetGridNumber;//is really updated grid or not
}
void Board::fillRowByLimit(int nowr){
  DEBUG_PRINT("fillrowbylimit%d\n", nowr);
  int size = lim_row[nowr].size();
  for(int i = 0; i < lim_row[nowr][0].fs; i++)
    fillGrid(nowr,i, WHITE);
  for(int i = 0; i < size-1; i++){
    for(int j = lim_row[nowr][i].le+1; j < lim_row[nowr][i+1].fs; j++)
      fillGrid( nowr,j, WHITE);
    for(int j = lim_row[nowr][i].ls; j <= lim_row[nowr][i].fe; j++)
      fillGrid( nowr,j, BLACK);
  }
  for(int j = lim_row[nowr][size-1].ls; j <= lim_row[nowr][size-1].fe; j++)
    fillGrid( nowr,j, BLACK);
  for(int i = lim_row[nowr][size-1].le+1; i < c; i++)
    fillGrid(nowr, i,WHITE);
}
void Board::fillColByLimit(int nowc){
  DEBUG_PRINT("fillcolbylimit%d\n", nowc);
  int size = lim_col[nowc].size();
  for(int i = 0; i < lim_col[nowc][0].fs; i++)
    fillGrid(i, nowc, WHITE);
  for(int i = 0; i < size-1; i++)
    for(int j = lim_col[nowc][i].ls + lim_col[nowc][i].l; j < lim_col[nowc][i+1].fs; j++)
      fillGrid(j, nowc, WHITE);
  for(int i = 0; i < size; i++)
    for(int j = lim_col[nowc][i].ls; j <= lim_col[nowc][i].fe; j++)
      fillGrid(j, nowc, BLACK);
  for(int i = lim_col[nowc][size-1].ls+lim_col[nowc][size-1].l; i < r; i++)
    fillGrid(i, nowc, WHITE);
}

bool Board::updateLimitByLimit_row(int nowr){
  int nfs, nls;
  if(!lim_row[nowr][0].isSolved()){//i = 0
    nfs = lim_row[nowr][0].fs;
    if(1 < lim_row[nowr].size()){
      nls = min(lim_row[nowr][1].ls-1-lim_row[nowr][0].l , lim_row[nowr][0].ls);
    } else {
      nls = lim_row[nowr][0].ls;
    }
    setLimit(ROW, nowr, lim_row[nowr][0], nfs, nls);
    if(lim_row[nowr][0].isSolved()){
      setRowLimitSolved(nowr, 0);
    }
  }
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
    setLimit(ROW, nowr, lim_row[nowr][i], nfs, nls);
    if(lim_row[nowr][i].isSolved()){
      setRowLimitSolved(nowr, i);
    }
  }
}
bool Board::updateLimitByLimit_col(int nowc){
  int nfs, nls;
  if(!lim_col[nowc][0].isSolved()){//TODO:reduce duplicate...
    nfs = lim_col[nowc][0].fs;
    if(1 < lim_col[nowc].size()){
      nls = min(lim_col[nowc][1].ls-1-lim_col[nowc][0].l , lim_col[nowc][0].ls);
    } else {
      nls = lim_col[nowc][0].ls;
    }
    setLimit(COL, nowc, lim_col[nowc][0], nfs, nls);
    if(lim_col[nowc][0].isSolved()){
      setColLimitSolved(nowc, 0);
    }
  }
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
    setLimit(COL, nowc, lim_col[nowc][i], nfs, nls);
    if(lim_col[nowc][i].isSolved()){
      setColLimitSolved(nowc, i);
    }
  }
}

void Board::fill_blank_row(int nowr){//fill white to the space sequence that is impossible for any limit to fill in
  DEBUG_PRINT("fill_blank_row %d\n", nowr);
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
	DEBUG_PRINT("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowr, i, i-nowb+1);
	for(int j = 0; j < nowb; j++){
	  fillGrid(nowr, i-j, WHITE);
	}
      }
      nowb = 0;
      allblack = true;
    } else {//black or space
      if(b[nowr][i] == SPACE){
	allblack = false;
      }
      nowb++;//find maximum non-white sequence
    }
  }
  if(nowb < minl && !allblack){//the last non-white sequence
    DEBUG_PRINT("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowr, c-1, c-nowb);
    for(int j = 0; j < nowb; j++){
      fillGrid(nowr, c-1-j, WHITE);
    }
  }
}
void Board::fill_blank_col(int nowc){
  fillColByLimit(nowc);
  DEBUG_PRINT("fillblankcol%d\n", nowc);
  
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
    for(int j = 0; j < nowb; j++){
      fillGrid(r-1-j, nowc, WHITE);
    }
  }
  
}

void Board::updateRowLimits(struct Point p, int v){//if point only in one limit, update limit by it
  //if(isUpdate(p.r, p.c))
  //return;
  for(int j = 0; j < lim_row[p.r].size(); j++)
    if(!lim_row[p.r][j].isSolved() && in_limit_row(p.r, j, p.c))
      updateLimitByGrid_row(p.r, j, p.c);
}
bool Board::updateLimitByGrid_row(int linei, int limiti, int i){
  //printBoard("updateLimitByGrid_row");
  int fs = lim_row[linei][limiti].fs;
  int ls = lim_row[linei][limiti].ls;
  int l = lim_row[linei][limiti].l;
  if(b[linei][i] == BLACK){//find sequences
    if(only_in_one_limit_row(linei, i, limiti)){
      int seqs = i, seqe = i;
      while(seqs-1 >= 0 && b[linei][seqs-1] == BLACK)
	seqs--;
      while(seqe+1 < c && b[linei][seqe+1] == BLACK)
	seqe++;
      fs = max(fs, seqe-l+1);//最左解右移
      ls = min(ls, seqs);//最右解左移
      DEBUG_PRINT("update black grid sequence(%d, %d~%d): limit=(%d, %d)\n", linei, seqs, seqe, fs, ls);
    }
  } else {
    while(fs <= i && fs + l > i)
      fs++;
    while(ls <= i && ls + l > i)
      ls--;
  }
  if(fs > ls){
    //sprintf(tryFailedReason, "fs > ls at row %d limit %d fs %d ls %d, update black grid sequence(%d, %d~%d): limit=(%d, %d)\n", linei, limiti, fs, ls, linei, seqs, seqe, fs, ls);
    sprintf(tryFailedReason, "fs > ls at row %d\n", linei);
    no_solution(tryFailedReason);
    return false;
  }
  //complete, fill the information back
  /*if(lim_row[linei][limiti].fs != fs || lim_row[linei][limiti].ls != ls){
    DEBUG_PRINT("(%d, %d) = %d lim_row[%d][%d]: (%d, %d) -> (%d, %d)", linei, i, b[linei][i], linei, limiti,   lim_row[linei][limiti].fs,   lim_row[linei][limiti].ls, fs, ls);
    }*/
  if(lim_row[linei][limiti].fs != fs || lim_row[linei][limiti].ls != ls)
    setLimit(ROW, linei, lim_row[linei][limiti], fs, ls);
  if(fs == ls){
    setRowLimitSolved(linei, limiti);
  } 
  return true;
}
inline bool Board::in_limit_row(int nowr, int limiti, int nowc){
  return (lim_row[nowr][limiti].fs <= nowc && lim_row[nowr][limiti].le >= nowc);  
}
inline bool Board::only_in_one_limit_row(int nowr, int nowc, int limiti){
  bool inLimiti = false;
  for(int i = 0; i < lim_row[nowr].size(); i++){
    if(in_limit_row(nowr, i, nowc)){
      if(i != limiti)
	return false;
      else
	inLimiti = true;
    }
  }
  return inLimiti;
  //return ((limiti == 0) || !in_limit_row(nowr, limiti-1, nowc)) && (in_limit_row(nowr, limiti, nowc)) && ((limiti == lim_row[nowr].size()-1) || !in_limit_row(nowr, limiti+1, nowc));
}

void Board::updateColLimits(struct Point p, int v){
  //if(isUpdate(p.r, p.c))
  //return;
  DEBUG_PRINT("updatelimit_col(%d, %d)\n", p.r, p.c);
  for(int j = 0; j < lim_col[p.c].size(); j++)
    if(!lim_col[p.c][j].isSolved() && in_limit_col(p.c, j, p.r))
      updateLimitByGrid_col(p.c, j, p.r);
}
bool Board::updateLimitByGrid_col(int linei, int limiti, int nowr){//update fs, ls to get correct ans
  //printBoard("updateLimitByGrid_col");
  int fs = lim_col[linei][limiti].fs;
  int ls = lim_col[linei][limiti].ls;
  int l = lim_col[linei][limiti].l;
  if(b[nowr][linei] == BLACK){
    DEBUG_PRINT("updatelimitbygrid_col (%d, %d), limit%d, black\n", nowr, linei, limiti);
    if(only_in_one_limit_col(nowr, linei, limiti)){    //find sequences
      int seqs = nowr, seqe = nowr;
      while(seqs-1 >= 0 && b[seqs-1][linei] == BLACK)
	seqs--;
      while(seqe+1 < r && b[seqe+1][linei] == BLACK)
	seqe++;
      fs = max(fs, seqe-l+1);//最左解右移
      ls = min(ls, seqs);//最右解左移
    }
  } else{ //white
    while(fs <= nowr && nowr < fs + l)
      fs++;
    while(ls <= nowr && nowr < ls + l)
      ls--;
  }
  if(fs > ls){
    sprintf(tryFailedReason, "%d col %dth limit (%d, %d) -> fs %d > ls %d\n", linei, limiti, lim_col[linei][limiti].fs, lim_col[linei][limiti].fs, fs, ls);
    DEBUG_PRINT(tryFailedReason);
    no_solution(tryFailedReason);
    return false;
  }
  /*if(lim_col[linei][limiti].fs != fs || lim_col[linei][limiti].ls != ls){
    DEBUG_PRINT("use grid(%d, %d) to update lim_col[%d][%d]: (%d, %d) ", nowr, linei, linei, limiti,   lim_col[linei][limiti].fs,   lim_col[linei][limiti].ls);
    DEBUG_PRINT("-> (%d, %d)\n", fs, ls);
    }*/
  setLimit(COL, linei, lim_col[linei][limiti], fs, ls);
  if(fs == ls){
    setColLimitSolved(linei, limiti);
  }
  return true;
}
inline bool Board::in_limit_col(int nowc, int limiti, int nowr){
  return (lim_col[nowc][limiti].fs <= nowr && lim_col[nowc][limiti].le >= nowr);  
}
inline bool Board::only_in_one_limit_col(int nowr, int nowc, int limiti){//TODO: move to class limit 
  for(int i = 0; i < lim_col[nowc].size(); i++)
    if(i != limiti && in_limit_col(nowc, i, nowr))
      return false;
  return in_limit_col(nowc, limiti, nowr);
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
  int fe = lim_row[linei][limiti].fe;
  int ls = lim_row[linei][limiti].ls;
  int l = lim_row[linei][limiti].l;
  for(int i = 0; i < l; i++)
    fillGrid(linei,fs+i,BLACK);
  if(fs - 1 >= 0)
    fillGrid(linei,fs-1,WHITE);
  if(fe + 1 < c)
    fillGrid(linei,fe+1,WHITE);
  fill_blank_row(linei);
  isLineSolved(ROW, linei);
}
void Board::setLimit(line_type t, int linei, Limit &l, int fs, int ls){
  if(l.set_pos(fs, ls)){
    DEBUG_PRINT("setlimit %s %d = (%d, %d)\n", (t==ROW)?"row":"col", linei, fs, ls);
    if(t == ROW){
      //changed_row[linei] = true;
      updateLimitByLimit_row(linei);
    } else {
      //changed_col[linei] = true;
      updateLimitByLimit_col(linei);
    }
  }
}

void Board::fillGrid(int r, int c, int v){
  //commented because it cannot happen
  /*if(r < 0 || c < 0 || r >= this->r || c >= this->c){
    sprintf(tryFailedReason, "fillgrid out of range");
    no_solution(tryFailedReason);
    return;
    }*/
  if(b[r][c] == SPACE){
    DEBUG_PRINT("fillGrid (%d, %d) = %s\n", r, c, (v==1)?"BLACK":"WHITE");
    b[r][c] = v;
    change_row[r]++;
    change_col[c]++;
    alreadySetGridNumber++;
    if(isLimitInit){//TODO: optimize
      DEBUG_PRINT("update limits(%d, %d)\n", r, c);
      updateRowLimits(Point(r, c), v);
      updateColLimits(Point(r, c), v);
      updateLimitByLimit_row(r);//
      updateLimitByLimit_col(c);//
      fillRowByLimit(r);//limits are updated by others, need to fillgrid
      fillColByLimit(c);//limits are updated by others, need to fillgrid
      DEBUG_PRINT("update limits complete(%d, %d)\n", r, c);
      isupdate[r][c] = true;
    }
  } else if(b[r][c] != v){
    sprintf(tryFailedReason, "illegal fillGrid(%d, %d) from %d to %d\n", r, c, b[r][c], v);
    no_solution(tryFailedReason);
  }
}
void Board::fillGrid_Solved(int r, int c, int v){
  DEBUG_PRINT("fillGrid (%d, %d) = %s\n", r, c, (v==1)?"BLACK":"WHITE");
  b[r][c] = v;
  change_row[r]++;
  change_col[c]++;
  alreadySetGridNumber++;
  isupdate[r][c] = true;
}

void Board::isLineSolved(line_type type, int line){  //check whole line is solved or not
  //DEBUG_PRINT("islinesolved %s-%d\n", type==ROW?"row":"col", line);
  if(type == ROW){//row
    if(solved_row[line])//already solved...
      return;
    for(int i = 0; i < lim_row[line].size(); i++)
      if(!lim_row[line][i].isSolved())//not solved yet, return
	return;

    DEBUG_PRINT("%d row all solve\n", line);
    solved_row[line] = true;
    solvedLineNum++;
    int nowi = 0; int nowc = 0;
    for(int i = 0; i < c; i++)//TODO: fillgrid_solved do not update col limit
      if(b[line][i] == SPACE)
	fillGrid_Solved(line, i, WHITE);
    printBoard("solve row ");
  } else if (type == COL) {//col
    if(solved_col[line])
      return;
    for(int i = 0; i < lim_col[line].size(); i++)
      if(!lim_col[line][i].isSolved())
	return;

    DEBUG_PRINT("%d col all solve\n", line);
    solved_col[line] = true;
    solvedLineNum++;
    int nowi = 0; int nowr = 0;
    for(int i = 0; i < r; i++)
      if(b[i][line] == SPACE)
	fillGrid_Solved(i, line, WHITE);
    printBoard("solve col");
  }
}

vector<int> Board::getLimitFromBoard_row(int nowr){
  vector<int> limit;
  for(int i = 0; i < c; i++){
    if(b[nowr][i] == BLACK){
      int count = 0;
      while(i < c && b[nowr][i] == BLACK){
	count++; i++;
      }
      limit.push_back(count);
    }
  }
  return limit;
}
vector<int> Board::getLimitFromBoard_col(int nowc){
  vector<int> limit;
  for(int i = 0; i < r; i++){
    if(b[i][nowc] == BLACK){
      int count = 0;
      while(i < r && b[i][nowc] == BLACK){
	count++; i++;
      }
      limit.push_back(count);
    }
  }
  return limit;
}

wchar_t boardchar[3][20] = {L" ", L"\u25A0", L"\u25A1"};//space black white
wchar_t cl[] = L"\n";
void Board::printBoard(const char in[]){
  return;
  printf("print board: %s\n", in);
  int maxlr = 0;
  for(int i = 0; i < r; i++)
    if(maxlr < lim_row[i].size())
      maxlr = lim_row[i].size();
  int maxl = maxlr * 3;
  int maxlc = 0;
  for(int i = 0; i < c; i++)
    if(maxlc < lim_col[i].size())
      maxlc = lim_col[i].size();
  
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
      printf("%ls", boardchar[b[i][j]]);
    }
    printf("%ls", cl);
    }*/
  //print all limits
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

void Board::saveResult(){//the same as start of printBoard
  writer->saveResult(this);
}
void Board::saveSimpleResult(FILE* f){//TODO: abstract
  fprintf(f, "$%d\n", problemNum);
  for(int i = 0; i < r; i++){
    for(int j = 0; j < c; j++){
      fprintf(f, "%d", (b[i][j] == 1)?1:0);
      if(j != c-1)
	fputs("\t", f);
    }
    fputs("\n", f);
  }
}
void Board::saveFullResult(){
  setlocale(LC_ALL, "");
  char outputName[100];
  sprintf(outputName, "%s.out", name);
  fprintf(stderr, "saveresult %s\n", outputName);
  FILE* f = fopen(outputName, "w");
  if(f == NULL){
    fprintf(stderr, "cannot open file %s\n", outputName);
    //sleep(10);
    saveAndExit(10);
    return;
  }
  int maxlr = 0;
  for(int i = 0; i < r; i++)
    if(maxlr < lim_row[i].size())
      maxlr = lim_row[i].size();
  int maxl = maxlr * 3;
  int maxlc = 0;
  for(int i = 0; i < c; i++)
    if(maxlc < lim_col[i].size())
      maxlc = lim_col[i].size();
  
  for(int i = maxlc-1; i >= 0 ; i--){
    fprintf(f, "%*s ", maxl, "");
    for(int j = 0; j < c; j++)
      if(lim_col[j].size() > maxlc-1-i)
	fprintf(f, "%2d ", lim_col[j][maxlc-1-i].l);
      else
	fprintf(f, "   ");
    fputs("\n", f);
  }
  fprintf(f, "%*s ", maxl, "");
  for(int i = 0; i < c; i++)
    fprintf(f, "---");
  fputs("\n", f);
  
  for(int i = 0; i < r; i++){
    for(int j = 0; j < maxlr; j++){
      if(j < lim_row[i].size())
	fprintf(f, "%2d ", lim_row[i][j].l);
      else
	fprintf(f, "   ");
    }
    fprintf(f, "|");
    for(int j = 0; j < c; j++){
      if(b[i][j] != SPACE)
	fprintf(f, "%2d ", (b[i][j] == 1)?1:0);
      else
	fprintf(f, "   ");
    }
    fputs("\n", f);
  }
  for(int i = 0; i < r; i++){
    for(int j = 0; j < c; j++){
      fprintf(f, "%ls", boardchar[b[i][j]]);
    }
    fprintf(f, "%ls", cl);
  }
}

bool Board::checkAnswer(){
  vector<int> seq;
  for(int i = 0; i < r; i++){
    seq.clear();
    for(int j = 0; j < c; j++){
      if(b[i][j] == BLACK){
	int count = 1;
	while(j+1 < c && b[i][j+1] == BLACK){
	  count++; j++;}
	seq.push_back(count);
      }
    }
    if(lim_row[i].size() != seq.size()){
      no_solution("checkAnswer: wrong answer in", ROW, i);
      return false;
    }
    for(int j = 0; j < lim_row[i].size(); j++)
      if(lim_row[i][j].l != seq[j]){
	no_solution("checkAnswer: wrong answer in", ROW, i);
	return false;
      }
  }
  for(int i = 0; i < c; i++){
    seq.clear();
    for(int j = 0; j < r; j++){
      if(b[j][i] == BLACK){
	int count = 1;
	while(j+1 < r && b[j+1][i] == BLACK){
	  count++; j++;}
	seq.push_back(count);
      }
    }
    if(lim_col[i].size() != seq.size()){
      no_solution("checkAnswer: wrong answer in", COL, i);
      return false;
    }
    for(int j = 0; j < lim_col[i].size(); j++)
      if(lim_col[i][j].l != seq[j]){
	no_solution("checkAnswer: wrong answer in", COL, i);
	return false;
      }
  }
  return true;
}

void Board::doDFS(){
  if(isAllSolved())//if heuristic can't update anymore, do DFS
    return;
  solveMode = DFS;
  DFSBoard dfsboard(*this);
  dfsboard.DoDFS();

  b = dfsboard.b;
}

