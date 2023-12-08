//
// Created by Han Zhengyang on 2023/11/3.
//

#ifndef COMPLIER_IR_LOAD_H
#define COMPLIER_IR_LOAD_H
#define LOADCLASS "loadClass"
#include "Instruction.h"
class Load : public Instruction{
public:
    Load(int nameNum, int id, BasicBlock* parent, Value* addr);
    string print() override ;
    string getClassName() override {
        return LOADCLASS;
    }
};


#endif //COMPLIER_IR_LOAD_H
