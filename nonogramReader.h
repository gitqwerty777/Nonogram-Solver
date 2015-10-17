#include <cstdio>
#include <cstdlib>
#include <string>
#include <new>
#include "board.h"

class NonogramInputReader{
 public:
  bool isLegal;
  FILE* f;
  int r, c;
  vector< vector<struct Limit> > lim_row, lim_col;
  
  NonogramInputReader(FILE* p){
    if(p == NULL){
      isLegal = false;
      return;
    } else{
      f = p;
    }
  }

  void readInput(){
    char in[10000];//I think it's big enough
    fscanf(f, "%d %d", &r, &c);
    lim_row.clear(); lim_col.clear();
    lim_row.resize(r);  lim_col.resize(c);
    fgets(in, 10000, f);//dumb
    for(int i = 0; i < r; i++){
      fgets(in, 10000, f);
      char *p;
      p = strtok(in, " ");
      while(p != NULL){
	lim_row[i].push_back(Limit(atoi(p)));
	p = strtok(NULL, " ");
      }
    }
    for(int i = 0; i < c; i++){
      fgets(in, 10000, f);
      char *p;
      p = strtok(in, " ");
      while(p != NULL){
	lim_col[i].push_back(Limit(atoi(p)));
	p = strtok(NULL, " ");
      }
    }
  }

  void getBoard(Board *b, char* name){
    b->init(r, c, lim_row, lim_col, name);
  }

  void readInput_Tourament(){
    char in[10000];//I think it's big enough
    r = 15; c = 15;
    int questionNum;
    fscanf(f, "$%d", &questionNum);
    lim_row.clear(); lim_col.clear();
    lim_row.resize(r);  lim_col.resize(c);
    fgets(in, 10000, f);//dumb
    for(int i = 0; i < c; i++){
      fgets(in, 10000, f);
      char *p;
      p = strtok(in, " \t");
      while(p != NULL){
	lim_col[i].push_back(Limit(atoi(p)));
	p = strtok(NULL, " \t");
      }
    }
    for(int i = 0; i < r; i++){
      fgets(in, 10000, f);
      char *p;
      p = strtok(in, " \t");
      while(p != NULL){
	lim_row[i].push_back(Limit(atoi(p)));
	p = strtok(NULL, " \t");
      }
    }
  }
  
  void readInputAndGetBoard(Board *b, char* name){
    readInput_Tourament();//it can be changed
    getBoard(b, name);
  }
  
 private:
};
