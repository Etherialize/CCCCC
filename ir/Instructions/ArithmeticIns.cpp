//
// Created by Han Zhengyang on 2023/11/3.
//

#include "ArithmeticIns.h"

ArithmeticIns::ArithmeticIns(int nameNum, DataType* dateType, int id, BasicBlock* parent, string insType, Value* value1, Value* value2): Instruction("%v" + to_string(nameNum), dateType, id, parent, value1, value2){
    insType_ = insType;
}
string ArithmeticIns:: print() {
    string op = insType_ == ADDINS ? "add":
                insType_ == SUBINS ? "sub":
                insType_ == MULINS ? "mul":
                insType_ == DIVINS ? "sdiv":
                "srem";
    return getName() + " = " + op + " " + getValType()->print() + " " + getUsedValue(0)->getName() + ", " + getUsedValue(1)->getName();
}