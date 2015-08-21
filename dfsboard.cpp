#include "board.h"
#include "dfsboard.h"

//override
void DFSBoard::fillGrid(int r, int c, int v){
  b[r][c] = v;
}

vector<int> DFSBoard::getLimit_col(int nowc){
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

bool DFSBoard::checkDFSAnswer(){
  for(int i = 0; i < c; i++){
    vector<int> limit = getLimit_col(i);
    printf("get limit col = %d:", i);
    for(int k = 0; k < limit.size(); k++)
      printf("%d ", limit[k]);
    puts("");
    if(limit.size() != lim_col[i].size())
      return false;
    for(int j = 0; j < lim_col[i].size(); j++)
      if(limit[j] != lim_col[i][j].l)
	return false;
  }
  printBoard("checkdfsanswer complete!");
  return true;
}

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
  
  original[nowr] = b; //backup
  bool isSuccess = false;
  do{
    printf("test (");
    for(int i = 0; i < lim_row[nowr].size(); i++){
      printf("%d, ", fillStart[i]);
    }
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
  while(i < lim_row[nowr].size()){
    i = 0;
    while(i < lim_row[nowr].size()){
      if(lim_row[nowr][i].ls > fillStart[i]){//add 1 and return
	fillStart[i]++;
	if((i != 0 && fillStart[i-1] + lim_row[nowr][i].l >= fillStart[i]) || (i != lim_row[nowr].size() && fillStart[i] + lim_row[nowr][i].l >= fillStart[i+1]))// two blocks cannot be connected, continue finding next ans
	  break;
	return true;
      } else {//fs ... ls ... fs
	fillStart[i] = lim_row[nowr][i].fs;
	i++;
      }
    }
  }
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
  for(int i = 0; i < c; i++)
    b[nowr][i] = original[nowr][nowr][i];
  printBoard("after rewind");
}
void DFSBoard::DoDFS(){
  //TODO: choose an answer of the line with minimum possible answer() -> use heuristic to fill board -> check other lineis legal or not -> if legal, next line
  puts("doDFS");
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
  //if no answer, no_solution() and exit
}
