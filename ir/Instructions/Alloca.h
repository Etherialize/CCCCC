//
// Created by Han Zhengyang on 2023/11/3.
//

#ifndef COMPLIER_IR_ALLOCA_H
#define COMPLIER_IR_ALLOCA_H

#include "Instruction.h"
#include "../ConstVal/constPreDecl.h"
#define ALLOCACLASS "allocaClass"
class Alloca :public Instruction{
public:
    Alloca(int nameNum, DataType* dataType, int id, BasicBlock* parent);
    Alloca(int nameNum, DataType* dataType, int id, BasicBlock* parent, ConstVal* constVal);
    string print()override ;
    string getClassName() override {
        return ALLOCACLASS;
    }
    bool canMemToReg();
    ValueType* getAllocaType();
};


#endif //COMPLIER_IR_ALLOCA_H
