#include "board.h"
#include "dfsboard.h"
#include "nonogramWriter.h"
#include <cstdio>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <locale.h>
#define INF 2147483647

/// @brief use heuristic to solve in the beginning
void Board::doHeuristic(){
  solveMode = HEURISTIC;
  printBoard("init fill row");
  for(int i = 0 ; i < r; i++)
    initialFill(ROW, lim_row[i] ,i);
  printBoard("init fill col");
  for(int i = 0; i < c; i++)
    initialFill(COL, lim_col[i], i);
  printBoard("after fill col and row");
  isLimitInit = true;//used to update after fillgrid, TODO: delete to test

  while(!isAllSolved()){//do heuristic after no limit and grid to update
    if(!doHeuristicByLine()){
      break;
    }
    fprintf(stderr, "do heuristic by line  twice!!\n");
    printBoard("do heuristic by line use twice!!");

    fprintf(stderr, "change row:\n");
    for(int i = 0; i < c; i++){
      fprintf(stderr, "%d ", change_row[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "change col:\n");
    for(int i = 0; i < r; i++){
      fprintf(stderr, "%d ", change_col[i]);
    }
    fprintf(stderr, "\n");
  }
}

void Board::initialFill(line_type lt, std::vector<Limit>& l, int lineIndex){
  int remain = (lt==ROW)?c:r;//one row has c elements, one column has r elements
  for(int i = 0; i < l.size(); i++)
    remain -= l[i].l;
  remain -= l.size() - 1;
  if(remain == 0){//can fill all
    int nowi = 0;
    for(size_t i = 0; i < l.size(); i++){
      l[i].set_pos(nowi, nowi);
      for(int j = 0; j < l[i].l; j++)
        fillGrid(lt, nowi++, lineIndex, BLACK);
      if(i != l.size()-1)
        fillGrid(lt, nowi++, lineIndex, WHITE);
    }
    for(size_t i = 0; i < l.size(); i++){
      setLimitSolved(lt, l, lineIndex, i);
    }
  } else { // fill partial
    int fs = 0, ls;
    int fe, le;
    for(size_t i = 0; i < l.size(); i++){
      ls = fs + remain;
      fe = fs + l[i].l - 1;
      le = ls + l[i].l - 1;
      for(int j = ls; j <= fe; j++){
        fillGrid(lt, j, lineIndex, BLACK);
      }
      l[i].set_pos(fs, ls);
      fs = fe + 2;//next limit, should have a white to seperate
      for(int j = le+1; j < fs; j++)
        fillGrid(lt, j, lineIndex, WHITE);
    }
  }
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

///@brief select and solve one line by heuristics
bool Board::doHeuristicByLine(){//TODO:Optimize
  //TODO:remove change queue
  std::vector<change> changeQueue;
  changeQueue.clear();
  //initial: push unsolved row and col
  for(int i = 0;i < r; i++)
    if(!solved_row[i])
      changeQueue.push_back(change(ROW, i, (&change_row[i])));
  for(int i = 0; i < c; i++)
    if(!solved_col[i])
      changeQueue.push_back(change(COL, i, (&change_col[i])));

  bool isChanged = false;
  int previousChangeNum = 0;
  int previousSetGrid = alreadySetGridNumber;
  std::sort(changeQueue.begin(), changeQueue.end());
  int maxChangeNum = *(changeQueue[0].changeNum);
  while(changeQueue.size() != 0){
    std::sort(changeQueue.begin(), changeQueue.end());
    epr("change queue size %lu\n", changeQueue.size());
    for(int i = 0; i < changeQueue.size(); i++){
      changeQueue[i].print();
    }
    change& ch = changeQueue[0];//choose the line that update the most grids
    if(ch.isTried)//all are tried
      break;
    ch.isTried = true;
    //changeQueue.assign(changeQueue.begin()+1, changeQueue.end());
    *ch.changeNum = 0; //reset change num of the best line
    DEBUG_PRINT("update %s %d\n", (ch.type==ROW)?"ROW":"COL", ch.lineNum);
    isChanged = updateByHeuristic(ch.type, ch.lineNum);
    if(conflict){//limit conflict, no answer
      return false;
    }
    if((*ch.changeNum) != 0){//if not solve this line yet, push it back
      epr("set all line change to false\n");
      if(solveMode == HEURISTIC){
        for(int i = 0; i < changeQueue.size(); i++){
          changeQueue[i].isTried = false;
        }
      }
    }
  }
  if(isChanged || maxChangeNum != 0){
    return true;
  } else {//nothing can be updated
    if(solveMode == HEURISTIC){
      DEBUG_PRINT("NO HEURISTIC ANSWER\n");
      epr("no heursitic answer\n");
    }
    //if it's not heuristic mode, it's in DFS, directly finish heursitic
    return false;
  }
}

///@brief main method to do heuristic instructions
/// line type = row or col
///@TODO: improve process
bool Board::updateByHeuristic(line_type type, int line){
  int originalSetnum = alreadySetGridNumber;
  if(type == ROW){
    DEBUG_PRINT("updateHeuristic: row%d\n", line);
    updateLimitByLimit_row(line);
    fillRowByLimit(line);//limits are updated by others, need to fillgrid
    for(int j = 0; j < c; j++){//TODO: optimize
      if(b[line][j] != SPACE)
        updateRowLimits(Point(line,j), b[line][j]);//grid, limit
    }
    //fill_blank(type, lim_row[line], line);
    fill_blank_row(line);
  } else {
    DEBUG_PRINT("updateHeuristic: col%d\n", line);
    updateLimitByLimit_col(line);
    fillColByLimit(line);
    for(int j = 0; j < r; j++){
      if(b[j][line] != SPACE)
        updateColLimits(Point(j, line), b[j][line]);
    }
    //fill_blank(type, lim_col[line], line);
    fill_blank_col(line);
  }
  return originalSetnum != alreadySetGridNumber;//is really updated grid or not
}

inline void Board::no_solution(const char in[]){
  if(solveMode == HEURISTIC){
    fprintf(stderr, "no solution: %s\n", in);
    printBoard("no solution");
    clearBoard();
    saveAndExit(1);
  } else if(solveMode == DFS) {
    DEBUG_PRINT("no solution: %s\n", in);
    sprintf(conflictReason, "%s\n", in);
    conflict = true;
  }
}
void Board::clearBoard(){
  for(int i = 0; i < r; i++)
    for(int j = 0; j < c; j++){
      b[i][j] = SPACE;
    }
}
inline void Board::no_solution(const char in[], line_type t, int i){//check answer failed
  if(solveMode == HEURISTIC){
    printf("wrong solution: %s %s%d\n", in, (t==ROW)?"ROW":"COL", i);
    clearBoard();
    saveAndExit(5);
  } else if (solveMode == DFS) {
    DEBUG_PRINT("wrong solution: %s %s%d\n", in, (t==ROW)?"ROW":"COL", i);
    sprintf(conflictReason, "%s", in);
    conflict = true;
  }
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
void Board::fillByLimit(line_type lt, std::vector<Limit>& lim, int nowi){
  DEBUG_PRINT("fillbylimit%d\n", nowi);
  int size = lim.size();
  for(int i = 0; i < lim[0].fs; i++)
    fillGrid(lt, i, nowi, WHITE);
  for(int i = 0; i < size-1; i++){
    for(int j = lim[i].le+1; j < lim[i+1].fs; j++)
      fillGrid(lt, j, nowi,WHITE);
    for(int j = lim[i].ls; j <= lim[i].fe; j++)
      fillGrid(lt, j, nowi,BLACK);
  }
  for(int j = lim[size-1].ls; j <= lim[size-1].fe; j++)
    fillGrid(lt, j, nowi,BLACK);
  for(int i = lim[size-1].le+1; i < c; i++)
    fillGrid(lt, i,nowi, WHITE);
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

inline bool Board::checkColor(line_type lt, int nowi, int i, int color){
  if(lt == ROW){
    return b[nowi][i] == color;
  } else {
    return b[i][nowi] == color;
  }
}
void Board::fill_blank(line_type lt, std::vector<Limit>& lim, int nowi){//fill white to the space sequence that is impossible for any limit to fill in
  DEBUG_PRINT("fill_blank %d\n", nowi);
  fillByLimit(lt, lim, nowi);
  int minl = INF;
  for(int i = 0; i < lim.size(); i++){
    if(!lim[i].isSolved() && minl > lim[i].l){
      minl = lim[i].l;
    }
  }
  if(minl == INF){
    return;
  }

  int nowb = 0;
  bool allblack = true;
  int linelength = (lt==ROW)?c:r;
  for(int i = 0;i < linelength; i++){
    if(checkColor(lt, nowi, i, WHITE)){
      if(nowb < minl && !allblack){
        DEBUG_PRINT("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowi, i, i-nowb+1);
        for(int j = 0; j < nowb; j++){
          fillGrid(lt, i-j, nowi, WHITE);
        }
      }
      nowb = 0;
      allblack = true;
    } else {//black or space
      if(checkColor(lt, nowi, i, SPACE)){
        allblack = false;
      }
      nowb++;//find maximum non-white sequence
    }
  }
  if(nowb < minl && !allblack){//the last non-white sequence
    DEBUG_PRINT("fillblank: nowb = %d, minl = %d, (%d, %d~%d), white\n", nowb, minl, nowi, c-1, c-nowb);
    for(int j = 0; j < nowb; j++){
      fillGrid(lt, c-1-j, nowi, WHITE);
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
    //sprintf(conflictReason, "fs > ls at row %d limit %d fs %d ls %d, update black grid sequence(%d, %d~%d): limit=(%d, %d)\n", linei, limiti, fs, ls, linei, seqs, seqe, fs, ls);
    sprintf(conflictReason, "fs > ls at row %d\n", linei);
    no_solution(conflictReason);
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
    sprintf(conflictReason, "%d col %dth limit (%d, %d) -> fs %d > ls %d\n", linei, limiti, lim_col[linei][limiti].fs, lim_col[linei][limiti].fs, fs, ls);
    //DEBUG_PRINT(conflictReason);
    no_solution(conflictReason);
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
void Board::setLimitSolved(line_type lt, std::vector<Limit>& lim, int linei, int limiti){
  int fs = lim[limiti].fs;
  int fe = lim[limiti].fe;
  int ls = lim[limiti].ls;
  int l = lim[limiti].l;
  for(int i = 0; i < l; i++)
    fillGrid(lt, fs+i, linei, BLACK);
  if(fs - 1 >= 0)
    fillGrid(lt, fs-1, linei, WHITE);
  if(fe + 1 < c)
    fillGrid(lt, fe+1, linei, WHITE);
  fill_blank(lt, lim, linei);
  isLineSolved(lt, linei);
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
    sprintf(conflictReason, "fillgrid out of range");
    no_solution(conflictReason);
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
    sprintf(conflictReason, "illegal fillGrid(%d, %d) from %d to %d\n", r, c, b[r][c], v);
    no_solution(conflictReason);
  }
}
void Board::fillGrid(line_type lt, int second, int first, int v){
  int r, c;
  if(lt == ROW){
    r = first;
    c = second;
  } else {
    r = second;
    c = first;
  }
  //commented because it cannot happen
  /*if(r < 0 || c < 0 || r >= this->r || c >= this->c){
    sprintf(conflictReason, "fillgrid out of range");
    no_solution(conflictReason);
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
    sprintf(conflictReason, "illegal fillGrid(%d, %d) from %d to %d\n", r, c, b[r][c], v);
    no_solution(conflictReason);
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

std::vector<int> Board::getLimitFromBoard_row(int nowr){
  std::vector<int> limit;
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
std::vector<int> Board::getLimitFromBoard_col(int nowc){
  std::vector<int> limit;
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
#ifdef __DEBUG__
  fprintf(stderr,"print board: %s\n", in);
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
#else
  //fprintf(stderr, "no printboard() in release mode!\n");
#endif
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
  std::vector<int> seq;
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

///@brief start DFS, go to DFSBoard
void Board::doDFS(){
  if(isAllSolved())//if heuristic can't update anymore, do DFS
    return;
  solveMode = DFS;
  DFSBoard dfsboard(*this);
  dfsboard.DoDFS();

  b = dfsboard.b;
}
