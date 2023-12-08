//
// Created by Han Zhengyang on 2023/11/5.
//

#include "Store.h"

Store::Store(int id, BasicBlock* parent, Value* data,Value* addr):Instruction("", new VoidType(), id, parent, data, addr){}
string Store::print() {
    return "store " + getUsedValue(0)->getValType()->print() + " " + getUsedValue(0)->getName()
    + ", " + getUsedValue(1)->getValType()->print() + " " + getUsedValue(1)->getName();
}
