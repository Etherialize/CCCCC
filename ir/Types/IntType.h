//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_INTTYPE_H
#define COMPLIER_IR_INTTYPE_H
#include "ValueType.h"
class IntType : public DataType{
    const int bits_;
public:
    IntType(int bits);
    IntType();
    int getSize() override ;
    int getBits();
    string toString() override ;
    string print() override ;
};

#endif