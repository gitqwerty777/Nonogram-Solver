#include <cstdio>

#ifndef NONOGRAMWRITER_H
#define NONOGRAMWRITER_H

class NonogramWriterInterface{
 public:
  virtual void saveResult(struct Board* b) = 0;//this should be declared to 0
};

class NonogramWriter_Tourament: public NonogramWriterInterface{
public:
  virtual void saveResult(struct Board* b){
    char outputName[100];
    //sprintf(outputName, "%d.out", b->problemNum);
    sprintf(outputName, "solution.txt");
    fprintf(stderr, "saveresult %s\n", outputName);
    
    FILE* f = fopen(outputName, "a");
    if(f == NULL){
      fprintf(stderr, "cannot open file %s\n", outputName);
      return;
    }
    b->saveSimpleResult(f);//BUG: cannot see answer after $200
    fclose(f);
  }
};

class NonogramWriter: public NonogramWriterInterface{
  virtual void saveResult(struct Board* b){
    b->saveFullResult();
  }
};

#endif
