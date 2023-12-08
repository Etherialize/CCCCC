//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_VALUETYPE_H
#define COMPLIER_IR_VALUETYPE_H
#include "preDeclTypesHeader.h"
#include "preDeclTypesHeader.h"
#define INTTYPE "IntType"
#define VOIDTYPE "VoidType"
#define LABELTYPE "LabelType"
#define MODULETYPE "ModuleType"
#define POINTERTYPE "PointerType"
#define ARRAYTYPE "ArrayType"
#define FUNCTIONTYPE "FunctionType"
class ValueType {
public:
    virtual int getSize(){
        return 0;
    }
    virtual string toString(){
        return "???";
    }
    virtual string print(){
        return "???";
    }
};

class DataType: public ValueType {
public:
    virtual int getSize(){
        return 0;
    }
    virtual string toString(){
        return "???";
    }
    virtual string print(){
        return "???";
    }

};

class LabelType: public ValueType {
public:
    int getSize() override ;
    string toString() override ;
    string print(){
        return "label";
    }
};

class ModuleType: public ValueType{
public:
    int getSize() override ;
    string toString() override ;
    string print(){
        return "???";
    }
};

#endif