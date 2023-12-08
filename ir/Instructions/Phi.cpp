//
// Created by Han Zhengyang on 2023/11/29.
//

#include "Phi.h"
#include "../BasicBlock.h"
Phi::Phi(int name, DataType* dataType, int id,BasicBlock* parent, int predNum):Instruction("%p" + to_string(name), dataType, id, parent), sum_(predNum){
}
void Phi::addSource(Value *value, BasicBlock *block) {
    addUsedValue(value);
    addUsedValue(block);
}
Value* Phi::getPhiSrc(BasicBlock* basicBlock) {
    for(int i = 0; i < operands_.size(); i+=2) {
        if(operands_[i+1] == basicBlock) {
            return operands_[i];
        }
    }
}
string Phi::print() {
    string s = getName() + " = phi " + valueType_->print();
    for(int i = 0; i < 2*sum_; i=i+2) {
        if(getUsedValue(i) == nullptr) {break;}
        s += " [ " + getUsedValue(i)->getName() + ", " + getUsedValue(i+1)->getName() + "], ";
    }
    s.erase(s.length() - 2);
    return s;
}