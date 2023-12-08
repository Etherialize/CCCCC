//
// Created by Han Zhengyang on 2023/11/1.
//

#ifndef COMPLIER_IR_GLOBALVAR_H
#define COMPLIER_IR_GLOBALVAR_H
#include "irPreDeclHeader.h"
#include "ConstVal/ConstVal.h"
#define GLOBALVARCLASS "GlobalVarClass"
class GlobalVar : public User{
private:
    bool const isConst_;
public:
    /*
     *  同functionIR,parent = nullptr
     */
    GlobalVar(string name, ConstVal *initVal, int id, bool isConst);
    string getClassName() override ;
    ConstVal* getInitVal();
    bool getIsConst();
    string print() override;
};

#endif

