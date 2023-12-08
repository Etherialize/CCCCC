//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_POINTERTYPE_H
#define COMPLIER_IR_POINTERTYPE_H
#include "ValueType.h"
#include "preDeclTypesHeader.h"

class PointerType: public DataType {
private:
    ValueType* referenceType_;
    const int size_;
public:
    PointerType(ValueType* referenceType);
    int getSize() override ;
    string print() override;
    string toString() override ;
    ValueType* getReferenceType();
};

#endif