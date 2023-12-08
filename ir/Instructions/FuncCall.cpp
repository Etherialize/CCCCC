//
// Created by Han Zhengyang on 2023/11/7.
//

#include "FuncCall.h"
#include "../Function.h"
FuncCall::FuncCall(int nameNum, DataType* returnType, int id, BasicBlock* parent, string funcName, FunctionIR* target, vector<Value*>* args):Instruction("%v" + to_string(nameNum), returnType, id, parent), funcName_(funcName), target_(target){
    addUsedValues(*args);
}
string FuncCall::print() {
    string sb = "";
    if(getValType()->toString() == INTTYPE) {
        sb += getName() + " = ";
    }
    sb += "call "+ getValType()->print() + " " + funcName_ + "(";
    for(int i = 0; i < getUsedValueSum(); i++) {
        sb += getUsedValue(i)->getValType()->print() + " " + getUsedValue(i)->getName() + ", ";
    }
    if(getUsedValueSum() > 0) {
        sb.erase(sb.length() - 2);
    }
    sb += ")";
    return sb;
}
string FuncCall::getTargetName() {
    return funcName_;
}