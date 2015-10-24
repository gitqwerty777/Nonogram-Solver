#include <cstdio>
#include <cstdlib>
#include <string>
#include <new>
#include "board.h"

class NonogramInputReader{
 public:
  int problemNum;
  FILE* f;
  int r, c;
  vector< vector<struct Limit> > lim_row;
  vector< vector<struct Limit> > lim_col;
  int size;

  NonogramInputReader(FILE* p, int size){
    f = p;
    this->size = size;
  }

  void readInput(){
    char in[10000];//I think it's big enough
    char* ptr;
    if(fscanf(f, "%d %d", &r, &c) != 2){
      return;
    }
    lim_row.clear(); lim_col.clear();
    lim_row.resize(r);  lim_col.resize(c);
    ptr = fgets(in, 10000, f);//dumb
    for(int i = 0; i < r; i++){
      ptr = fgets(in, 10000, f);
      char *p;
      p = strtok(in, " ");
      while(p != NULL){
	lim_row[i].push_back(RowLimit(atoi(p)));
	p = strtok(NULL, " ");
      }
    }
    for(int i = 0; i < c; i++){
      ptr = fgets(in, 10000, f);
      char *p;
      p = strtok(in, " ");
      while(p != NULL){
	lim_col[i].push_back(ColLimit(atoi(p)));
	p = strtok(NULL, " ");
      }
    }
  }

  void initBoard(Board *b, char* name){
    b->init(r, c, lim_row, lim_col, name, problemNum);
  }

  void readInput_Tourament(){
    char in[10000];//I think it's big enough
    char *ptr;
    r = size; c = size;
    if(fscanf(f, "$%d", &problemNum) != 1)
      return;
    lim_row.clear(); lim_col.clear();
    lim_row.resize(r);  lim_col.resize(c);
    ptr = fgets(in, 10000, f);//dumb
    for(int i = 0; i < c; i++){
      ptr = fgets(in, 10000, f);
      char *p;
      p = strtok(in, " \t");
      while(p != NULL){
	lim_col[i].push_back(ColLimit(atoi(p)));
	p = strtok(NULL, " \t");
      }
    }
    for(int i = 0; i < r; i++){
      ptr = fgets(in, 10000, f);
      char *p;
      p = strtok(in, " \t");
      while(p != NULL){
	lim_row[i].push_back(RowLimit(atoi(p)));
	p = strtok(NULL, " \t");
      }
    }
  }
  
  void readInputAndGetBoard(Board *b, char* name, bool isTour){
    if(isTour){
      readInput_Tourament();//it can be changed
    } else {
      readInput();
    }
    initBoard(b, name);
  }
  
 private:
};
