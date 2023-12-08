//
// Created by Han Zhengyang on 2023/11/2.
//

#include "ConstInt.h"
#include "../Types/IntType.h"
ConstInt:: ConstInt(int bits, int value, int id): ConstVal(new IntType(bits), id), bits_(bits), value_(value){
}

ConstInt:: ConstInt(int value, int id): ConstVal(new IntType(32), id), bits_(32), value_(value){
}
string ConstInt::print(){
    return to_string(value_);
}
string ConstInt::getName() {
    return print();
}
string ConstInt::getClassName() {
    return CONSTINTCLASS;
}
int ConstInt:: getEleData(int index1, int index2){
    return value_;
}
int ConstInt::getValue(){
    return value_;
}