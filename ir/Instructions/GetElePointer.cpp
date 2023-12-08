//
// Created by Han Zhengyang on 2023/11/5.
//

#include "GetElePointer.h"
#include "../Types/typesHeader.h"
#include "../ConstVal/ConstInt.h"
GetElePointer::GetElePointer(int nameNum, int id, BasicBlock* parent, Value* value, int flag): Instruction("%v" + to_string(nameNum), new PointerType(((ArrayType*)((PointerType*)value->getValType())->getReferenceType())->getElementType()), id, parent, value){
    baseType_ = ((PointerType *) value->getValType())->getReferenceType();
    if(flag == 0) {
        insertValue(new ConstInt(0, -1), 1);
    }
}
GetElePointer::GetElePointer(int nameNum, int id, BasicBlock* parent, Value* value, Value* index1, int flag): Instruction("%v" + to_string(nameNum), new PointerType(((PointerType*)value->getValType())->getReferenceType()), id, parent, value, index1){
    baseType_ = ((PointerType *) value->getValType())->getReferenceType();
    if(flag == 0) {
        setVatType(new PointerType(((ArrayType*)((PointerType*)value->getValType())->getReferenceType())->getElementType()));
        insertValue(new ConstInt(0, -1), 1);
    }
}
GetElePointer::GetElePointer(int nameNum, int id, BasicBlock* parent, Value* value, Value* index1, Value* index2, int flag):Instruction("%v" + to_string(nameNum), new PointerType(new IntType(32)), id, parent, value,index1, index2){
    baseType_ = ((PointerType *) value->getValType())->getReferenceType();
    if(flag == 0) {
        insertValue(new ConstInt(0, -1), 1);
    }
}
string GetElePointer::print() {
    string sb = "";
    sb += getName() + " = getelementptr " + baseType_->print() + ", ";
    sb += getUsedValue(0)->getValType()->print() + " " + getUsedValue(0)->getName() + ", ";
    for(int i = 1; i < getUsedValueSum(); i++) {
        sb += getUsedValue(i)->getValType()->print() + " " + getUsedValue(i)->getName() + ", ";
    }
    sb.erase(sb.length() - 2);
    return sb;
}