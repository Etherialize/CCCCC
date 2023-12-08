//
// Created by Han Zhengyang on 2023/11/1.
//

#include "GlobalVar.h"
#include "Types/typesHeader.h"
GlobalVar ::GlobalVar(string name, ConstVal *initVal, int id, bool isConst): User("@"+name, new PointerType(initVal->getValType()), id,
                                                                                  nullptr, initVal), isConst_(isConst){}
string GlobalVar::getClassName() {
    return GLOBALVARCLASS;
}
ConstVal* GlobalVar ::getInitVal() {
    return (ConstVal*) getUsedValue(0);
}
bool GlobalVar::getIsConst() {
    return isConst_;
}
string GlobalVar::print() {
    return getName() + " = dso_local " + ((isConst_ )? "constant " : "global ") + (((PointerType*)getValType())->getReferenceType())->print() + " " + getUsedValue(0)->print();
}
