    //
// Created by Han Zhengyang on 2023/11/2.
//

#ifndef COMPLIER_IR_ZEROINITIALIZER_H
#define COMPLIER_IR_ZEROINITIALIZER_H

#include "ConstVal.h"
#define ZEROCLASS "ZeroClass"
class ZeroInitializer: public ConstVal{
private:
    const int length_;
public:
    ZeroInitializer(ArrayType *arrayType, int id);
    string getClassName() override ;
    string print() override ;
    int getEleData(int index1, int index2)override ;
};


#endif //COMPLIER_IR_ZEROINITIALIZER_H
