//
// Created by Han Zhengyang on 2023/10/31.
//

#include "VoidType.h"
int VoidType::getSize(){
    throw runtime_error("getSize of Void");
}
string VoidType::toString(){
    return VOIDTYPE;
}
string VoidType::print() {
    return "void";
}