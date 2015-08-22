#include "board.h"
#include "dfsboard.h"
#define INF 2147483647

//override
/*
  void DFSBoard::fillGrid(int r, int c, int v){
  puts("dfsboard fillgrid");
  b[r][c] = v;
  }
*/

//Simple DFS
void DFSBoard::DoSimpleDFS(){//without heuristic, only use DFS + backtrace search
  puts("dosimpleDFS");
  int nowr = 0;
  while(nowr <= r){
    if(!FillRow(nowr)){  //try all possibilities to fill the row, will filling next answer after previous called
      //all possibilities in row nowr are failed, recover board to previous row(nowr-1)
      if(nowr == 0)
	break;
      lastfillStart[nowr].clear();
      Rewind(--nowr);
    } else {// fill answer success, continue filling next row
      nowr++;
      if(nowr == r)//complete, check column again
	if(checkDFSAnswer())
	  return;
	else
	  Rewind(--nowr);
    }
  }
}
bool DFSBoard::checkDFSAnswer(){
  for(int i = 0; i < c; i++){
    vector<int> limit = getLimit_col(i);
    //printf("get limit col = %d:", i);
    //for(int k = 0; k < limit.size(); k++)
    //printf("%d ", limit[k]);
    //puts("");
    if(limit.size() != lim_col[i].size())
      return false;
    for(int j = 0; j < lim_col[i].size(); j++)
      if(limit[j] != lim_col[i][j].l)
	return false;
  }
  printBoard("check dfs answer complete!");
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
bool DFSBoard::FillRow(int nowr){//is vector copy by reference?
  vector<int> fillStart;
  printf("fillRow%d\n", nowr);
  if(!getPreviousFillStart(fillStart, nowr))
    return false;

  BackupBoard(original[nowr]);
  bool isSuccess = false;
  do{
    printf("test (");
    for(int i = 0; i < lim_row[nowr].size(); i++)
      printf("%d, ", fillStart[i]);
    printf(") at row %d\n", nowr);
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
  //printf("getnext");
  //for(int j = 0; j < lim_row[nowr].size(); j++){
  //printf("(%d, %d) ", lim_row[nowr][j].fs, lim_row[nowr][j].ls);
  //}
  //puts("");
  //for(int j = 0; j < lim_row[nowr].size(); j++){
  //printf("(%d) ", fillStart[j]);
  //}
  //puts("");
  while(i < lim_row[nowr].size()){
    i = 0;
    while(i < lim_row[nowr].size()){
      if(lim_row[nowr][i].ls > fillStart[i]){//add 1 and return
	fillStart[i]++;
	/*puts("try next: ");
	for(int j = 0; j < lim_row[nowr].size(); j++){
	  printf("(%d) ", fillStart[j]);
	}
	puts("");*/
	printf("%s %s\n", (i != 0)?"yes":"no", (i != lim_row[nowr].size()?"yes":"no"));
	if(((i != 0) && fillStart[i-1] + lim_row[nowr][i].l >= fillStart[i]) || ((i != lim_row[nowr].size()-1) && fillStart[i] + lim_row[nowr][i].l >= fillStart[i+1]))// two blocks cannot be connected, continue finding next ans
	  break;
	/*puts("real next: ");
	for(int j = 0; j < lim_row[nowr].size(); j++){
	  printf("(%d) ", fillStart[j]);
	  }*/
	puts("");
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
  //printBoard("after rewind");
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
    if(!FillRowHeuristic(rowOrder[nowr])){  //try all possibilities to fill the row, will filling next answer after previous called
      //all possibilities in row nowr are failed, recover board to previous row(nowr-1)
      if(nowr == 0){
	puts("rewind to first row: no solution:");
	break;
      }
      lastfillStart[rowOrder[nowr]].clear();
      Rewind(rowOrder[--nowr]);
    } else {// fill answer success, continue filling next row
      rowOrder[++nowr] = getRowWithMinBranch(nowr, rowOrder);
      if(nowr == r)//complete, check column again
	if(checkDFSAnswer())
	  return;
	else
	  Rewind(rowOrder[--nowr]);
    }
  }
  //if no answer, no_solution() and exit
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

bool DFSBoard::FillRowHeuristic(int nowr){//is vector copy by reference?
  vector<int> fillStart;
  printf("fillRow%d\n", nowr);
  if(!getPreviousFillStart(fillStart, nowr))
    return false;

  BackupBoard(original[nowr]);
  bool isSuccess = false;
  do{
    printf("test (");
    for(int i = 0; i < lim_row[nowr].size(); i++)
      printf("%d, ", fillStart[i]);
    printf(") at row %d\n", nowr);
    if(FillRowbyFillStartHeuristic(nowr, fillStart)){//check available
      isSuccess = true;
    } else {
      printf("failed after applying heuristic, try next\n");
    }
  } while(!isSuccess && getNextFillStart(nowr, fillStart));

  lastfillStart[nowr] = fillStart;
  return isSuccess;
}
bool DFSBoard::FillRowbyFillStartHeuristic(int nowr, vector<int>& fillStart){//or directly fill
  int limitNum = lim_row[nowr].size();
  for(int i = 0; i < limitNum; i++)
    for(int j = fillStart[i]; j < fillStart[i]+lim_row[nowr][i].l; j++)
      fillGrid(nowr, j, BLACK);
  for(int i = 0; i < c; i++)
    if(b[nowr][i] == SPACE)
      fillGrid(nowr, i, WHITE);
  //printBoard("beforeFillRowbyFillStartHeuristic");
  while(!isAllSolved() && !tryFailed){
    if(!doHeuristicInOneLine())
      break;
  }
  /*tryFailed = false;
  for(int i = 0; i < c; i++)
    if(!updateByHeuristic(COL, i) || tryFailed){
      tryFailed = true;
      break;
    }
  */
  if(tryFailed){
    printf("tryfailed, rewind %d\n", nowr);
    Rewind(nowr);
    //printBoard("failFillRowbyFillStartHeuristic");
    return false;
  }
  puts("dfs with heursitic success");
  //printBoard("FillRowbyFillStartHeuristic");
  return true;
}
bool DFSBoard::tryUpdateByHeuristic(line_type type, int line){//if there are no any hint can solve it
  int originalSetnum = alreadySetGridNumber;
  if(type == ROW){
    fillRowByLimit(line);
    updateLimitByLimit_row(line);
    for(int j = 0; j < c; j++)
      if(b[line][j] != SPACE)
	updateRowLimits(Point(line,j), b[line][j]);
    fill_blank_row(line);
  } else {
    fillColByLimit(line);
    updateLimitByLimit_col(line);
    for(int j = 0; j < r; j++){
      if(b[j][line] != SPACE)
	updateColLimits(Point(j, line), b[j][line]);
    }
    fill_blank_col(line);
  }
  return originalSetnum != alreadySetGridNumber;//is really updated or not
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
