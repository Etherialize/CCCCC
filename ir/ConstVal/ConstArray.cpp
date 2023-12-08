//
// Created by Han Zhengyang on 2023/11/2.
//

#include "ConstArray.h"
#include "../Types/ArrayType.h"
ConstArray::ConstArray(vector<ConstVal*> elements, int id) :ConstVal(new ArrayType(elements[0]->getValType(), elements.size()), id),elements_(elements){
}
string ConstArray::getClassName() {
    return CONSTARRAYCLASS;
}
string ConstArray::print() {
    int sum = elements_.size();
    string sb = "[";
    for(int i = 0; i < sum; i++) {
        if(i == sum - 1) {
            sb += elements_[i]->getValType()->print() + " " + elements_[i]->print();
        } else {
            sb += elements_[i]->getValType()->print() + " " + elements_[i]->print() + ", ";
        }
    }
    sb += "]";
    return sb;
}
int ConstArray::getEleData(int index1, int index2){
    return elements_[index1]->getEleData(index2, 0);
}
vector<int>* ConstArray::getExpandedEles(){
    vector<int>* expandedEles = new vector<int>();
    expandEle(expandedEles);
    return expandedEles;
}
//void ConstArray::expandEle(vector<int>* expandedEles) {
//    if(elements_[0]->getValType()->toString() == INTTYPE) {
//        for(int i = 0; i < elements_.size(); i++) {
//            expandedEles->push_back(((ConstInt*)elements_[i])->getValue());
//        }
//    } else if()
//    else {
//        for(int i = 0; i < elements_.size(); i++) {
//            ((ConstArray*) elements_[i])->expandEle(expandedEles);
//        }
//    }
//}
void ConstArray::expandEle(vector<int> *expandedEles) {
    for(int i = 0; i < elements_.size(); i++) {
        if(elements_[i]->getClassName() == CONSTINTCLASS) {
            expandedEles->push_back(((ConstInt*)elements_[i])->getValue());
        } else if(elements_[i]->getClassName() == ZEROCLASS) {
            ValueType* zeroArray = elements_[i]->getValType();
            int size = zeroArray->getSize();
            for(int i = 0; i < size / 4; i++) {
                expandedEles->push_back(0);
            }
        } else {
            ((ConstArray*)elements_[i])->expandEle(expandedEles);
        }
    }
}
ConstVal* ConstArray:: getEle(int index){
    return elements_[index];
}
