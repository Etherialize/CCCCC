//
// Created by Han Zhengyang on 2023/11/7.
//

#include "Return.h"
#include "../Types/typesHeader.h"
Return::Return(DataType* returnType, int id, BasicBlock* parent, Value* rst): Instruction("", returnType, id, parent,rst){}
string Return::print() {
    string sb = "ret";
    if(getValType()->toString() == VOIDTYPE) {
        sb += " void";
        return sb;
    } else {
        sb += " " + getUsedValue(0)->getValType()->print() + " " + getUsedValue(0)->getName();
        return sb;
    }

}