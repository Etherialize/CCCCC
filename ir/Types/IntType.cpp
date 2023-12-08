//
// Created by Han Zhengyang on 2023/10/31.
//

#include "IntType.h"

IntType:: IntType(int bits): bits_(bits){}
IntType:: IntType(): bits_(32){}
int IntType:: getSize() {
    return bits_/8;
}
int IntType:: getBits() {
    return bits_;
}
string IntType::print(){
    return "i" + to_string(bits_);
}
string IntType:: toString(){
    return INTTYPE;
}