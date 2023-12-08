//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_FUNCTIONTYPE_H
#define COMPLIER_IR_FUNCTIONTYPE_H
#include "preDeclTypesHeader.h"
#include "ValueType.h"
class FunctionType : public ValueType{
private:
    vector<DataType*> formalArgs_;
    DataType* returnType_;
public:
    FunctionType(vector<DataType*> &formalArgs, DataType *returnType);
    int getSize() override ;
    string print() override ;
    string toString() override ;
    DataType* getReturnType();
    vector<DataType*> getFormalArgs();
};

#endif