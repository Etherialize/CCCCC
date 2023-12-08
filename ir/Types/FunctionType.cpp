//
// Created by Han Zhengyang on 2023/10/31.
//

#include "FunctionType.h"
int FunctionType:: getSize() {
    throw runtime_error("getSize of Function");
}
string FunctionType:: toString() {
    return FUNCTIONTYPE;
}
string FunctionType::print(){
    return returnType_->print();
}
FunctionType::FunctionType(vector<DataType*> &formalArgs, DataType *returnType): formalArgs_(formalArgs), returnType_(returnType){
}
DataType* FunctionType:: getReturnType() {
    return returnType_;
}
vector<DataType*> FunctionType::getFormalArgs() {
    return formalArgs_;
}