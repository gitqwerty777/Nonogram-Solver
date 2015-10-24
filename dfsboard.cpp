#include "board.h"
#include "dfsboard.h"
#include <cstdlib>
#define INF 2147483647

/*void DFSBoard::DoSimpleDFS(){//Simple DFS, without heuristic, not used
  puts("dosimpleDFS");
  int nowr = 0;
  while(nowr <= r){
    if(!tryFillRow(nowr)){//try all possibilities to fill the row, will filling next answer after previous called
      if(nowr == 0)//all possibilities in row nowr are failed, recover board to previous row(nowr-1)
	break;
      lastfillStart[nowr].clear();
      Restore(--nowr);
    } else {// fill answer success, continue filling next row
      nowr++;
      if(nowr == r)//complete, check answer again
	if(isDFSAnswerCorrect())
	  return;
	else
	  Restore(--nowr);
    }
  }
  }
bool DFSBoard::tryFillRow(int nowr){//is vector copy by reference?
  vector<int> fillStart;
  printf("fillRow%d\n", nowr);
  if(!getNextLegalFillStart(fillStart, nowr))
    return false;

  BackupBoard(backupBoards[nowr]);
  bool isSuccess = false;
  do{
    FillRowbyFillStart(nowr, fillStart);
    if(checkColumns()){//check available
      isSuccess = true;
    }
  } while(!isSuccess && getNextFillStart(nowr, fillStart));

  lastfillStart[nowr] = fillStart;
  return isSuccess;
}*/

bool LimitFiller::getNextFillStart(){
  if(fillStart.size() == 0){//this row is not tried yet, return original fs
    fillStart.resize(l.size());
    for(int i = 0; i < l.size(); i++){
      fillStart[i] = l[i].fs;
    }
  } else {//get previous fillstart
    if(!getNextFillStartbyFillStart())
      return false;
  }
  return true;
}
bool LimitFiller::getNextFillStartbyFillStart(){
  int limiti = 0;
  while(limiti < l.size()){
    limiti = 0;
    while(limiti < l.size()){
      if(l[limiti].ls > fillStart[limiti]){//add 1 and return
	fillStart[limiti]++;
	if(!isLimitLegal()){
	  break;//goto i = 0, readding
	}
	/*printf("(");
	  for(int i = 0; i < l.size(); i++)
	  printf("%d ", fillStart[i]);
	  printf(")");*/
	return true;
      } else {//fillstart too big, return to fs, continue adding next bit(limit)
	fillStart[limiti] = l[limiti].fs;
	limiti++;
      }
    }
  }
  //printf("row %d: cannot find next fillstart", nowr);
  return false;
}
bool LimitFiller::isLimitLegal(){
  for(int i = 0; i < l.size()-1; i++)
    if(fillStart[i] + l[i].l >= fillStart[i+1])
      return false;
  return true;
}

//DFS with heuristic
void DFSBoard::DoDFS(){//TODO: choose col
  //choose an answer of the line with minimum possible answer() -> use heuristic to fill board -> check other is legal or not -> if legal, next line; else, refill the current line
  for(int i = 0; i < r+c; i++)
    isFilled[i] = false;
  puts("doDFS");
  lineOrder.clear();
  lineOrder.resize(r+c);
  int dfsLineCount = 0;
  getRowWithMinBranch(dfsLineCount, lineOrder);
  while(!checkAnswer()){
    Line nowLine = lineOrder[dfsLineCount];
    /*printf("now order:\n");
    for(int i = 0; i < dfsLineCount; i++)
      printf("%s%d ", lineOrder[i].t==ROW?"row":"col", lineOrder[i].i);
      puts("");*/
    if(!tryFillRowWithHeuristic(nowLine)){//try all possibilities to fill the row, will filling next answer after previous called
      if(dfsLineCount == 0){
	puts("no solution:cannot fill first row");
	break;
      } else {	//all possibilities in row nowr are failed, recover board to previous row(nowr-1)
	limitFillers[nowLine.index].destroy();
	isFilled[nowLine.index] = false;
	Restore(lineOrder[--dfsLineCount]);
      }
    } else {// fill answer success, continue filling next row
      if(isAllSolved())
	return;
      isFilled[nowLine.index] = true;
      dfsLineCount++;
      getRowWithMinBranch(dfsLineCount, lineOrder);//get next row index
    }
  }
  if(!checkAnswer()){
    fprintf(stderr, "no solution: dfs failed\n");
  }
}

void DFSBoard::checkSolve(){
  for(int i = 0; i < r; i++)
    if(!solved_row[i]){
      bool isSolved = true;
      for(int j = 0; j < c && isSolved; j++)
	if(b[i][j] == SPACE)
	  isSolved = false;
      if(isSolved){
	solved_row[i] = true;
	solvedLineNum++;
      }
    }

  for(int i = 0; i < c; i++)
    if(!solved_col[i]){
      bool isSolved = true;
      for(int j = 0; j < r && isSolved; j++)
	if(b[j][i] == SPACE)
	  isSolved = false;
      if(isSolved){
	solved_col[i] = true;
	solvedLineNum++;
      }
    }
}

void DFSBoard::getRowWithMinBranch(int dfsLineCount, vector<Line>& lineOrder){//TODO: RENAME, lineorder-checkequal, linear search used -> bool memorize
  int mini;
  line_type mint;
  long minv;
  bool first = true;
  for(int i = 0; i < r; i++){
    if(solved_row[i] || isFilledByDFS(ROW, i))
      continue;
    long v = 1;
    for(int j = 0; j < lim_row[i].size(); j++)
      if(!lim_row[i][j].isSolved())
	v *= lim_row[i][j].ls-lim_row[i][j].fs+1;
    if(v < minv || first){
      mint = ROW;
      minv = v;
      mini = i;
      first = false;
    }
  }
  for(int i = 0; i < c; i++){
    if(solved_col[i] || isFilledByDFS(COL, i))
      continue;
    long v = 1;
    for(int j = 0; j < lim_col[i].size(); j++)
      if(!lim_col[i][j].isSolved())
	v *= lim_col[i][j].ls-lim_col[i][j].fs+1;
    if(v < minv || first){
      mint = COL;
      minv = v;
      mini = i;
      first = false;
    }
  }
  if(first == true){//TODO: why first == true...
    //fprintf(stderr, "first = true");
    checkSolve();
    printBoard("first=true");
    return;
  }
  //assert(first != true); //may happened in 15 $202, that solve two lines simultaneously
  //printf("min branch %s%d: %d possibility\n", (mint==ROW)?"row":"col", mini, minv);
  if(mint == ROW)
    lineOrder[dfsLineCount] = Line(mint, mini, mini);
  else
    lineOrder[dfsLineCount] = Line(mint, mini, r+mini);
}
bool DFSBoard::tryFillRowWithHeuristic(Line nowLine){
  if(nowLine.t == ROW){
    limitFillers[nowLine.index].setLimit(lim_row[nowLine.i]);
  } else {
    limitFillers[nowLine.index].setLimit(lim_col[nowLine.i]);
  }
  LimitFiller& filler = limitFillers[nowLine.index];
  if(!filler.getNextFillStart())
    return false;
  
  bool isSuccess = false;
  Backup(nowLine);
  do{
    if(tryFillRowbyFillStartHeuristic(nowLine, filler.fillStart)){//check available
      isSuccess = true;
    } else{
      Restore(nowLine);
    }
  } while(!isSuccess && filler.getNextFillStart());
  
  return isSuccess;
}

bool DFSBoard::tryFillRowbyFillStartHeuristic(Line& nowLine, vector<int>& fillStart){
  tryFailed = false;

  /* fill row */
  //fillLimitByFillStart(this, nowLine);//TODO: fillstart is already in limitfillers TODO: implement fillLimitByFillStart
  if(nowLine.t == ROW){
    int nowr = nowLine.i;
    int limitNum = lim_row[nowr].size();
    for(int i = 0; i < limitNum; i++)
      for(int j = fillStart[i]; j < fillStart[i]+lim_row[nowr][i].l; j++)
	fillGrid(nowr, j, BLACK);
    for(int i = 0; i < c; i++)
      if(b[nowr][i] == SPACE)
	fillGrid(nowr, i, WHITE);
  } else {
    int nowc = nowLine.i;
    int limitNum = lim_col[nowc].size();
    for(int i = 0; i < limitNum; i++)
      for(int j = fillStart[i]; j < fillStart[i]+lim_col[nowc][i].l; j++)
	fillGrid(j,nowc, BLACK);
    for(int i = 0; i < c; i++)
      if(b[i][nowc] == SPACE)
	fillGrid(i, nowc, WHITE);
  }
  
  /* try heuristic */
  while(!isAllSolved() && !tryFailed)//TODO: no need to test tryfailed because it's checked in doheu...
    if(!doHeuristicInOneLine())
      break;
  if(tryFailed){//find contradiction when updating heuristic, restore
    //printf("try fill: failed [%s], restore %d\n", tryFailedReason, nowr);
    return false;
  }

  checkSolve();//TODO: refact
  if(isAllSolved()){
    return checkAnswer();
  }
  //puts("dfs with heursitic success");
  return true;
}

void DFSBoard::Restore(Line& l){
  //printf("restore %d\n", nowr);
  RestoreBoard(backupBoards[l.index]);
}
void DFSBoard::Backup(Line& l){
  BackupBoard(backupBoards[l.index]);
}
void DFSBoard::BackupBoard(Board &b){//TODO: need to add something?
  b.b = this->b;
  b.lim_row = this->lim_row;
  b.lim_col = this->lim_col;
  b.change_row = this->change_row;
  b.change_col = this->change_col;
  b.solved_row = this->solved_row;
  b.solved_col = this->solved_col;
  b.solvedLineNum = this->solvedLineNum;
  b.alreadySetGridNumber = this->alreadySetGridNumber;
}
void DFSBoard::RestoreBoard(Board &b){
  this->b = b.b;
  this->lim_row = b.lim_row;
  this->lim_col = b.lim_col;
  this->change_row = b.change_row;
  this->change_col = b.change_col;
  this->solved_row = b.solved_row;
  this->solved_col = b.solved_col;
  this->solvedLineNum = b.solvedLineNum;
  this->alreadySetGridNumber = b.alreadySetGridNumber;
}
