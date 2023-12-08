//
// Created by Han Zhengyang on 2023/11/7.
//

#ifndef COMPLIER_IR_FUNCCALL_H
#define COMPLIER_IR_FUNCCALL_H

#include "Instruction.h"
#define FUNCCALLCLASS "funcCallClass"
class FuncCall :public Instruction{
private:
    string funcName_;
    FunctionIR* target_;
public:
    /*
     * if retrunType is VoidType, then the nameNum is ""
     * */
    FuncCall(int nameNum, DataType* returnType, int id, BasicBlock* parent, string funcName, FunctionIR* target, vector<Value*>* args);
    string print() override ;
    string getClassName() override {
        return FUNCCALLCLASS;
    }
    FunctionIR* getTarget(){
        return target_;
    }
    string getTargetName();
};


#endif //COMPLIER_IR_FUNCCALL_H
