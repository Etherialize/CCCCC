//
// Created by Han Zhengyang on 2023/11/5.
//

#ifndef COMPLIER_IR_STORE_H
#define COMPLIER_IR_STORE_H
#define STORECLASS "storeClass"
#include "Instruction.h"
#include "../Types/typesHeader.h"
class Store : public Instruction{
public:
    Store(int id, BasicBlock* parent, Value* data,Value* addr);
    string print()override ;
    string getClassName() override {
        return STORECLASS;
    }
};


#endif //COMPLIER_IR_STORE_H
