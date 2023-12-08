//
// Created by Han Zhengyang on 2023/11/3.
//

#ifndef COMPLIER_IR_ARITHMETICINS_H
#define COMPLIER_IR_ARITHMETICINS_H

#define ADDINS "ADDINS"
#define SUBINS "SUBINS"
#define MULINS "MULINS"
#define DIVINS "DIVINS"
#define REMINS "REMINS"
#include "Instruction.h"
#define ARITHMETICCLASS "arithmeticClass"
class ArithmeticIns : public Instruction{
private:
    string insType_;
public:
    ArithmeticIns(int nameNum, DataType* dateType, int id, BasicBlock* parent, string insType,Value* value1, Value* value2);
    string print()override ;
    string getClassName() override {
        return ARITHMETICCLASS;
    }
    string getInsType() {
        return insType_;
    }
};


#endif //COMPLIER_IR_ARITHMETICINS_H
