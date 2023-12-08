//
// Created by Han Zhengyang on 2023/11/13.
//

#include "Zext.h"
#include "../Types/typesHeader.h"
Zext::Zext(int nameNum, int id, BasicBlock* parent, Value* op1): Instruction("%v" + to_string(nameNum), new IntType(32), id, parent, op1){}
string Zext::print() {
    return getName() + " = zext i1 " + getUsedValue(0)->getName() + " to i32";
}