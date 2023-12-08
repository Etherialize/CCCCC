//
// Created by Han Zhengyang on 2023/11/1.
//

#ifndef COMPLIER_IR_ARGUMENT_H
#define COMPLIER_IR_ARGUMENT_H
#define ARGUMENTCLASS "argumentClass"
#include "irPreDeclHeader.h"
#include "Value.h"
class Argument : public Value{
private:
    int rank_;
public:
    Argument(int nameNum, DataType* dataType, int id);
    string print() override ;
    string getClassName() override {
        return ARGUMENTCLASS;
    }
    int getRank() {
        return rank_;
    }
};


#endif //COMPLIER_IR_ARGUMENT_H
