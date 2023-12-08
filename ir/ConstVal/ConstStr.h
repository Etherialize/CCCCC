//
// Created by Han Zhengyang on 2023/11/7.
//

#ifndef COMPLIER_IR_CONSTSTR_H
#define COMPLIER_IR_CONSTSTR_H

#include "ConstVal.h"
#define  CONSTSTRCLASS "ConstStrClass"
class ConstStr : public ConstVal{
private:
    string content_;
public:
    ConstStr(string content, int id);
    string print() override ;
    string getClassName() override ;
    string getContent();
};


#endif //COMPLIER_IR_CONSTSTR_H
