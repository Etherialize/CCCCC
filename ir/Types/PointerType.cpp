//
// Created by Han Zhengyang on 2023/10/31.
//

#include "PointerType.h"
PointerType:: PointerType(ValueType* referenceType): referenceType_(referenceType), size_(4){

}
string PointerType:: print() {
    return referenceType_->print() + "*";
}
int PointerType:: getSize(){
    return size_;
}
string PointerType::toString() {
    return POINTERTYPE;
}
ValueType* PointerType:: getReferenceType(){
    return referenceType_;
}