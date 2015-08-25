#include "board.h"
#include "dfsboard.h"
#define INF 2147483647

//Simple DFS, without heuristic
void DFSBoard::DoSimpleDFS(){
  puts("dosimpleDFS");
  int nowr = 0;
  while(nowr <= r){
    if(!tryFillRow(nowr)){//try all possibilities to fill the row, will filling next answer after previous called
      if(nowr == 0)//all possibilities in row nowr are failed, recover board to previous row(nowr-1)
	break;
      lastfillStart[nowr].clear();
      Rewind(--nowr);
    } else {// fill answer success, continue filling next row
      nowr++;
      if(nowr == r)//complete, check answer again
	if(isDFSAnswerCorrect())
	  return;
	else
	  Rewind(--nowr);
    }
  }
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

bool DFSBoard::getPreviousFillStart(vector<int>& fillStart, int nowr){
  if(lastfillStart[nowr].size() == 0){//this row is not tried yet
    fillStart.resize(lim_row[nowr].size());
    for(int i = 0; i < lim_row[nowr].size(); i++){
      fillStart[i] = lim_row[nowr][i].fs;
    }
  } else {//get previous fillstart
    fillStart = lastfillStart[nowr];
    if(!getNextFillStart(nowr, fillStart))
      return false;
  }
  return true;
}
bool DFSBoard::tryFillRow(int nowr){//is vector copy by reference?
  vector<int> fillStart;
  printf("fillRow%d\n", nowr);
  if(!getPreviousFillStart(fillStart, nowr))
    return false;

  BackupBoard(original[nowr]);
  bool isSuccess = false;
  do{
    FillRowbyFillStart(nowr, fillStart);
    if(checkColumns()){//check available
      isSuccess = true;
    }
  } while(!isSuccess && getNextFillStart(nowr, fillStart));

  lastfillStart[nowr] = fillStart;
  return isSuccess;
}
bool DFSBoard::getNextFillStart(int nowr, vector<int>& fillStart){
  puts("getNextFillStart");
  int i = 0;
  //(0, 2), (3, 4), (5, 6)
  //0, 3, 5 -> 1, 3, 5 -> 2, 3, 5 -> 0, 4, 5 -> 1, 4, 5 -> 2, 4, 5 -> 0, 3, 6 -> 1, 3, 6 ... -> 2, 4, 6 -> false
  int preblack = 0;
  while(i < lim_row[nowr].size()){
    i = 0;
    while(i < lim_row[nowr].size()){
      if(lim_row[nowr][i].ls > fillStart[i]){//add 1 and return
	fillStart[i]++;
	if(((i != 0) && fillStart[i-1] + lim_row[nowr][i].l >= fillStart[i]) || ((i != lim_row[nowr].size()-1) && fillStart[i] + lim_row[nowr][i].l >= fillStart[i+1])){// two blocks cannot be connected, continue finding next ans
	  puts("failed"); 
	  break;
	}
	return true;
      } else {//fs ... ls ... fs
	fillStart[i] = lim_row[nowr][i].fs;
	i++;
      }
    }
  }
  puts("cannot find next fillstart");
  return false;
}
void DFSBoard::FillRowbyFillStart(int nowr, vector<int>& fillStart){//or directly fill
  int limitNum = lim_row[nowr].size();
  for(int i = 0; i < limitNum; i++)
    for(int j = fillStart[i]; j < fillStart[i]+lim_row[nowr][i].l; j++)
      fillGrid(nowr, j, BLACK);
  for(int i = 0; i < c; i++)
    if(b[nowr][i] == SPACE)
      fillGrid(nowr, i, WHITE);
  printBoard("FillRowbyFillStart");
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
void DFSBoard::Rewind(int nowr){
  printf("rewind %d\n", nowr);
  RewindBoard(original[nowr]);
}

//DFS with heuristic
void DFSBoard::DoDFS(){
  //TODO: choose an answer of the line with minimum possible answer() -> use heuristic to fill board -> check other lineis legal or not -> if legal, next line
  puts("doDFS");
  vector<int> rowOrder(r);
  int nowr = 0;
  rowOrder[nowr] = getRowWithMinBranch(nowr, rowOrder);
  while(nowr <= r){
    puts("nowroworder");
    for(int i = 0; i <= nowr; i++)
      printf("%d ", rowOrder[i]);
    puts("");
    if(isAllSolved())
      return;
    if(!tryFillRowHeuristic(rowOrder[nowr])){//try all possibilities to fill the row, will filling next answer after previous called
      if(nowr == 0){
	puts("rewind to the first row: no solution:");
	break;
      }
      //all possibilities in row nowr are failed, recover board to previous row(nowr-1)
      lastfillStart[rowOrder[nowr]].clear();
      Rewind(rowOrder[--nowr]);
    } else {// fill answer success, continue filling next row
      rowOrder[++nowr] = getRowWithMinBranch(nowr, rowOrder);
      if(nowr == r)//complete, check column again
	if(isDFSAnswerCorrect())
	  return;
	else
	  Rewind(rowOrder[--nowr]);
    }
  }
}
int DFSBoard::getRowWithMinBranch(int nowr, vector<int>& rowOrder){
  int mini, minv = INF;
  for(int i = 0; i < r; i++){
    bool used = false;
    for(int j = 0; j < nowr; j++)
      if(i == rowOrder[j])
	used = true;
    if(used)
      continue;
    int v = 1;
    for(int j = 0; j < lim_row[i].size(); j++)
      v *= lim_row[i][j].ls-lim_row[i][j].fs;
    if(v < minv){
      minv = v;
      mini = i;
    }
  }
  return mini;
}
bool DFSBoard::tryFillRowHeuristic(int nowr){
  vector<int> fillStart;
  printf("fillRow%d\n", nowr);
  if(!getPreviousFillStart(fillStart, nowr))
    return false;

  BackupBoard(original[nowr]);
  bool isSuccess = false;
  do{
    if(tryFillRowbyFillStartHeuristic(nowr, fillStart)){//check available
      isSuccess = true;
    } else {
      printf("failed after applying heuristic, try next\n");
    }
  } while(!isSuccess && getNextFillStart(nowr, fillStart));
  
  lastfillStart[nowr] = fillStart;
  return isSuccess;
}
bool DFSBoard::tryFillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart){//or directly fill
  int limitNum = lim_row[nowr].size();
  for(int i = 0; i < limitNum; i++)
    for(int j = fillStart[i]; j < fillStart[i]+lim_row[nowr][i].l; j++)
      fillGrid(nowr, j, BLACK);
  for(int i = 0; i < c; i++)
    if(b[nowr][i] == SPACE)
      fillGrid(nowr, i, WHITE);

  tryFailed = false;
  while(!isAllSolved() && !tryFailed)
    if(!doHeuristicInOneLine())
      break;
  if(tryFailed){//find contradiction when updating heuristic
    printf("try fill: failed, rewind %d\n", nowr);
    Rewind(nowr);
    return false;
  }
  puts("dfs with heursitic success");
  return true;
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
void DFSBoard::RewindBoard(Board &b){
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
