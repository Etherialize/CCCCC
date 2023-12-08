//
// Created by Han Zhengyang on 2023/11/13.
//

#include "Br.h"
#include "../BasicBlock.h"
#include "../Types/typesHeader.h"
Br::Br(int id, BasicBlock* parent, BasicBlock* target): Instruction("", new VoidType(), id, parent, target) {
    hasCondition_ = false;
}
Br::Br(int id, BasicBlock* parent, Value* cond, BasicBlock* target1, BasicBlock* target2): Instruction("", new VoidType(), id, parent, cond, target1, target2) {
    hasCondition_ = true;
}
bool Br:: hasCondition() {
    return hasCondition_;
}
void Br::setGraph() {
    if(hasCondition_) {
        ((BasicBlock*)parent_)->addSuccessor((BasicBlock*)getUsedValue(1));
        ((BasicBlock*)parent_)->addSuccessor((BasicBlock*)getUsedValue(2));
    } else {
        ((BasicBlock*)parent_)->addSuccessor((BasicBlock*)getUsedValue(0));
    }
}
void Br::handleSameTarget() {
    hasCondition_ = false;
    Value* cond = operands_[0];
    operands_.erase(operands_.begin());
    cond->removeUser(this);
    Value* target = operands_[0];
    operands_.erase(operands_.begin());
    target->removeUser(this);

}
string Br::print() {
    if(hasCondition_) {
        string s = "br ";
        s += getUsedValue(0)->getValType()->print() + " " + getUsedValue(0)->getName() + ", ";
        s += getUsedValue(1)->getValType()->print() + " " + getUsedValue(1)->getName() + ", ";
        s += getUsedValue(2)->getValType()->print() + " " + getUsedValue(2)->getName();
        return s;
    } else {
        return "br " + getUsedValue(0)->getValType()->print() + " " + getUsedValue(0)->getName();
    }
}