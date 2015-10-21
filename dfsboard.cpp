#include "board.h"
#include "dfsboard.h"
#define INF 2147483647
/*
void DFSBoard::DoSimpleDFS(){//Simple DFS, without heuristic, not used
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

bool DFSBoard::checkColumns(){
  return true;//assume is correct
  bool isLegal = true;
  for(int i = 0; i < c && isLegal; i++)
    isLegal = isLegal && checkAvailableCol(i);
  return isLegal;
}
bool DFSBoard::checkAvailableCol(int nowc){//not implement yet
  for(int i = 0; i < r; i++){
    
  }
}

//DFS with heuristic
void DFSBoard::DoDFS(){//TODO: choose col
  //choose an answer of the line with minimum possible answer() -> use heuristic to fill board -> check other is legal or not -> if legal, next line; else, refill the current line
  for(int i = 0; i < r; i++)
    isFilled[i] = false;
  puts("doDFS");
  vector<int> rowOrder(r);
  int rowCount = 0;
  getRowWithMinBranch(rowCount, rowOrder);
  while(!checkAnswer()){
    int nowrow = rowOrder[rowCount];
    if(!tryFillRowWithHeuristic(nowrow)){//try all possibilities to fill the row, will filling next answer after previous called
      if(rowCount == 0){
	puts("no solution:cannot fill first row");
	break;
      } else {	//all possibilities in row nowr are failed, recover board to previous row(nowr-1)
	limitFillers[nowrow].destroy();
	isFilled[nowrow] = false;
	Restore(rowOrder[--rowCount]);
      }
    } else {// fill answer success, continue filling next row
      if(isAllSolved())
	return;
      isFilled[nowrow] = true;
      rowCount++;
      getRowWithMinBranch(rowCount, rowOrder);//get next row index
    }
  }
  if(!checkAnswer()){
    fprintf(stderr, "no solution: dfs failed\n");
  }
}


void DFSBoard::getRowWithMinBranch(int rowCount, vector<int>& rowOrder){
  int mini;
  long minv = INF;
  bool first = true;
  for(int i = 0; i < r; i++){
    if(solved_row[i] || isFilledByDFS(i))
      continue;
    long v = 1;
    for(int j = 0; j < lim_row[i].size(); j++)
      if(!lim_row[i][j].isSolved())
	v *= lim_row[i][j].ls-lim_row[i][j].fs+1;
    if(v < minv || first){
      minv = v;
      mini = i;
      first = false;
    }
  }
  rowOrder[rowCount] = mini;
}
bool DFSBoard::tryFillRowWithHeuristic(int nowr){
  limitFillers[nowr].setLimit(lim_row[nowr]);
  LimitFiller& filler = limitFillers[nowr];
  printf("fillRow%d\n", nowr);
  if(!filler.getNextFillStart())
    return false;

  bool isSuccess = false;
  Backup(nowr);
  do{
    if(tryFillRowbyFillStartHeuristic(nowr, filler.fillStart)){//check available
      isSuccess = true;
    } else{
      Restore(nowr);
    }
  } while(!isSuccess && filler.getNextFillStart());
  
  /*if(isSuccess){
    printf("fill row %d success, fillstart: ", nowr);
    for(int i = 0; i < lim_row[nowr].size(); i++){
      printf("%d ", fillStart[i]);
      }
    puts("");
  }*/
  return isSuccess;
}
bool DFSBoard::tryFillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart){//TODO: check bug
  tryFailed = false;

  /* fill row */
  int limitNum = lim_row[nowr].size();
  for(int i = 0; i < limitNum; i++)
    for(int j = fillStart[i]; j < fillStart[i]+lim_row[nowr][i].l; j++)
      fillGrid(nowr, j, BLACK);
  for(int i = 0; i < c; i++)
    if(b[nowr][i] == SPACE)
      fillGrid(nowr, i, WHITE);

  /* try heuristic */
  while(!isAllSolved() && !tryFailed)//TODO: no need to test tryfailed
    if(!doHeuristicInOneLine())
      break;
  if(tryFailed){//find contradiction when updating heuristic, restore
    printf("try fill: failed [%s], restore %d\n", tryFailedReason, nowr);
    return false;
  }
  //puts("dfs with heursitic success");
  return true;
}

bool DFSBoard::isDFSAnswerCorrect(){
  for(int i = 0; i < c; i++){
    vector<int> limitc = getLimitFromBoard_col(i);
    if(limitc.size() != lim_col[i].size())
      return false;
    for(int j = 0; j < lim_col[i].size(); j++)
      if(limitc[j] != lim_col[i][j].l)
	return false;
  }
  printBoard("find dfs answer!");
  return true;
}

void DFSBoard::Restore(int nowr){
  //printf("restore %d\n", nowr);
  RestoreBoard(backupBoards[nowr]);
}
void DFSBoard::Backup(int nowr){
  BackupBoard(backupBoards[nowr]);
}
void DFSBoard::BackupBoard(Board &b){
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
