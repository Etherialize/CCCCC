//
// Created by Han Zhengyang on 2023/11/1.
//

#include "Instruction.h"
#define EQUAL "eq"
#define NOTEQUAL "ne"
#define LESS "slt"
#define LESSEQUAL "sle"
#define GRAND "sgt"
#define GRANDEQUAL "sge"
#define CMPCLASS "cmpClass"
class Cmp:public Instruction {
private:
    string condition_;
public:
    Cmp(int nameNum, int id, BasicBlock* parent, string condition, Value* op1, Value* op2);
    string print() override ;
    string getClassName() override {
        return CMPCLASS;
    }
    string getCondition() {
        return condition_;
    }
};