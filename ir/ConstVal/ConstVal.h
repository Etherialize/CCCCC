//
// Created by Han Zhengyang on 2023/11/2.
//

#ifndef COMPLIER_IR_CONSTVAL_H
#define COMPLIER_IR_CONSTVAL_H
#define CONSTVALCLASS "ConstValClass"
#include "../User.h"
class ConstVal : public Value{
public:
    ConstVal(ValueType* valueType, int id);
    virtual string getClassName();
    virtual int getEleData(int index1, int index2){
        return 0;
    }
};


#endif //COMPLIER_IR_CONSTVAL_H
