//
// Created by Han Zhengyang on 2023/11/13.
//

#include "Cmp.h"
#include "../Types/typesHeader.h"
Cmp::Cmp(int nameNum, int id, BasicBlock* parent, string condition, Value* op1, Value* op2): Instruction("%v" + to_string(nameNum), new IntType(1), id, parent, op1, op2){
    condition_ = condition;
}
string Cmp:: print() {
    string s = getName() + " = icmp " + condition_  + " " + getUsedValue(0)->getValType()->print() + " ";
    s += getUsedValue(0)->getName() + ", " + getUsedValue(1)->getName();
    return s;
}