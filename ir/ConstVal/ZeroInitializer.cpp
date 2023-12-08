//
// Created by Han Zhengyang on 2023/11/2.
//

#include "ZeroInitializer.h"
#include "../Types/ArrayType.h"
ZeroInitializer::ZeroInitializer(ArrayType* arrayType, int id):ConstVal(arrayType, id), length_(arrayType->getElementNum()){}

string ZeroInitializer::getClassName(){
    return ZEROCLASS;
}
int ZeroInitializer::getEleData(int index1, int index2){
    return 0;
}
string ZeroInitializer::print() {
    return "zeroinitializer";
}