//
// Created by Han Zhengyang on 2023/11/2.
//

#include "ConstVal.h"

ConstVal:: ConstVal(ValueType* valueType, int id): Value("", valueType, id, nullptr){}
string ConstVal::getClassName(){
    return CONSTVALCLASS;
}