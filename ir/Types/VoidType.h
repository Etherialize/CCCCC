//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_VOIDTYPE_H
#define COMPLIER_IR_VOIDTYPE_H
#include"ValueType.h"
class VoidType: public DataType{

public:
    int getSize() override ;
    string toString() override ;
    string print() override ;
};

#endif