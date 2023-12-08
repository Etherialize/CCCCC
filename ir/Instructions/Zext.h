//
// Created by Han Zhengyang on 2023/11/13.
//

#ifndef COMPLIER_IR_ZEXT_H
#define COMPLIER_IR_ZEXT_H
#define ZEXTCLASS "zextClass"
#include "Instruction.h"

class Zext: public Instruction {
public:
    Zext(int nameNum, int id, BasicBlock* parent, Value* op1);
    string print() override;
    string getClassName() override {
        return ZEXTCLASS;
    }
};


#endif //COMPLIER_IR_ZEXT_H
