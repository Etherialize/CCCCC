//
// Created by Han Zhengyang on 2023/11/3.
//

#include "Alloca.h"
#include "GetElePointer.h"
#include "../Types/typesHeader.h"
#include "../ConstVal/conHeader.h"
//局部单变量和数组
Alloca:: Alloca (int nameNum, DataType* dataType, int id, BasicBlock* parent): Instruction("%v"+to_string(nameNum), new PointerType(dataType), id, parent){}

//函数参数

//局部常量数组
Alloca::Alloca(int nameNum, DataType* dataType, int id, BasicBlock* parent, ConstVal* constVal): Instruction("%v" + to_string(nameNum), new PointerType(dataType), id, parent, constVal) {}
string Alloca::print(){
    return getName() + " = alloca " + ((PointerType *)getValType())->getReferenceType()->print();
}
bool Alloca::canMemToReg() {
    for(int i = 0; i < users_.size(); i++) {
        if(users_[i]->getClassName() == GEPCLASS) {
            GetElePointer* gep = (GetElePointer*)users_[i];
            if(gep->getUsedValue(0) == this) {
                return false;
            }
        }
    }
    return true;
}
ValueType* Alloca::getAllocaType() {
    return ((PointerType*)valueType_)->getReferenceType();
}