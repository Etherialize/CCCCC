//
// Created by Han Zhengyang on 2023/11/2.
//

#ifndef COMPLIER_IR_CONSTINT_H
#define COMPLIER_IR_CONSTINT_H

#include "ConstVal.h"
#define CONSTINTCLASS "ConstIntCLASS"
class ConstInt : public ConstVal{
private:
    const int value_;
    const int bits_;
public:
    ConstInt(int bits, int value, int id);
    ConstInt(int value, int id);
    string getClassName() override ;
    string getName();
    string print()override ;
    int getEleData(int index1, int index2)override ;
    int getValue();
};


#endif //COMPLIER_IR_CONSTINT_H
