//
// Created by Han Zhengyang on 2023/11/7.
//

#ifndef COMPLIER_IR_RETURN_H
#define COMPLIER_IR_RETURN_H

#include "Instruction.h"
#define RETURNCLASS "returnClass"
class Return : public Instruction{
public:
    Return(DataType* returnType, int id, BasicBlock* parent, Value* rst);
    string print()override ;
    string getClassName() override {
        return RETURNCLASS;
    }
};


#endif //COMPLIER_IR_RETURN_H
