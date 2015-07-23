#include "board.h"
#include "dfsboard.h"

void DFSBoard::DoSimpleDFS(){//no use heuristic, fs, ls...
  int i = 0;
  while(i < r){
    if(!FillRow(i)){//try all possibilities to fill the row, will filling next answer after previous called
      if(i == 0)
	break;
      Rewind(--i);//recover answer in line i
    }
  }
  if(i == 0)//no answer
    return;
}
bool DFSBoard::FillRow(int nowr){//is vector copy by reference?
  vector<int> fillStart;
  printf("fillRow%d\n", nowr);
  if(lastfillStart[nowr].size() == 0){//new
    for(int i = 0; i < lim_row[nowr].size(); i++){
      fillStart[i] = lim_row[nowr][i].fs;
    }
  } else {//next fillstart
    fillStart = lastfillStart[nowr];
  }

  bool isSuccess = false;
  while(getNextFillStart(nowr, fillStart)){
    printf("test (");
    for(int i = 0; i < lim_row[nowr].size(); i++){
      printf("%d, ", fillStart[i]);
    }
    printf(")\n");
    FillRowbyFillStart(nowr, fillStart);
    if(checkColumns()){//check available
      printf("success!!\n");
      isSuccess = true;
      break;
    }
  }

  lastfillStart[nowr] = fillStart;
  return isSuccess;
}
bool DFSBoard::getNextFillStart(int nowr, vector<int>& fillStart){
  int i = 0;
  while(i < lim_row[nowr].size()){
    if(lim_row[nowr][i].ls > fillStart[i]){
      fillStart[i]++;
      return true;
    } else {
      fillStart[i] = lim_row[nowr][i].fs;
      i++;
    }
  }
  return false;
}
void DFSBoard::FillRowbyFillStart(int nowr, vector<int>& fillStart){//or directly fill
  int limitNum = lim_row[nowr].size();
  for(int i = 0; i < limitNum; i++)
    for(int j = fillStart[i]; j < fillStart[i]+lim_row[nowr][i].l; j++)
      fillGrid(nowr, i, BLACK);
  for(int i = 0; i < c; i++)
    if(b[nowr][i] == SPACE)
      fillGrid(r, i, WHITE);
}
bool DFSBoard::checkColumns(){
  for(int i = 0; i < c; i++)
    checkAvailableCol(i);
}
void DFSBoard::checkAvailableCol(int nowc){
  //newboard and check?
}
void DFSBoard::Rewind(int nowr){
  for(int i = 0; i < c; i++){
    b[nowr][i] = original[nowr][i];
  }
}
void DFSBoard::DoDFS(){
  //find line with minimum possible answer 
  //try by line order first
  /*int rewindindex = 0;
  while(rewindindex != r){
    int errorcode = 0;
    while(FillRowAndUpdate(i) != 0){//if fill this cause other line error, refill, if no answer, return -2
      if(errorCode == -2){
	Rewind(--rewindindex);
      }
      Rewind(i);//recover snapshot
      
    }
    if(errorcode == -2){

    }
  }
  for(int j = 0; j < c; j++){
    FillCol(j);
    }*/
  
  //try an answer and search, can use heuristic either
  
  //if find answer, return

  //if no answer, no_solution()
}
