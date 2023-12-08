//
// Created by Han Zhengyang on 2023/11/3.
//

#include "Load.h"
#include "../Types/typesHeader.h"
Load:: Load (int nameNum, int id, BasicBlock* parent, Value* addr): Instruction("%v"+to_string(nameNum), (DataType*)(((PointerType*)addr->getValType())->getReferenceType()), id, parent, addr){}
string Load:: print() {
    return getName() + " = load " + getValType()->print() + ", " +
    getUsedValue(0)->getValType()->print() + " " + getUsedValue(0)->getName();
}