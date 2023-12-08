//
// Created by Han Zhengyang on 2023/10/31.
//

#include "ArrayType.h"

ArrayType::ArrayType(ValueType* elementType, int elementNum):elementType_(elementType), elementNum_(elementNum), size_(elementNum * elementType->getSize()) {
}
ArrayType::ArrayType(ValueType* elementType, vector<int> dims){
    elementNum_ = dims[0];
    if(dims.size() == 1) {
        elementType_ = elementType;
    } else {
        dims.erase(dims.begin());
        elementType_ = new ArrayType(elementType, dims);
    }
    size_ = elementNum_ * elementType_->getSize();
}
string ArrayType::print(){
    string sb = "";
    sb+="[" + to_string(elementNum_) + " x " + elementType_->print() + "]";
    return sb;
}
ValueType* ArrayType:: getElementType(){
    return elementType_;
}
int ArrayType::getSize() {
    return size_;
}

string ArrayType::toString() {
    return ARRAYTYPE;
}
int ArrayType:: getElementNum() {
    return elementNum_;
}