//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_ARRAYTYPE_H
#define COMPLIER_IR_ARRAYTYPE_H
#include "preDeclTypesHeader.h"
#include "ValueType.h"

class ArrayType : public ValueType {
private:
    ValueType* elementType_;
    int elementNum_;
    int size_;
public:
    ArrayType(ValueType* elementType, int elementNum);
    ArrayType(ValueType* elementType, vector<int> dims);
    int getSize() override ;
    string toString() override ;
    string print() override ;
    int getElementNum();
    ValueType* getElementType();

};

#endif