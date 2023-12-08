//
// Created by Han Zhengyang on 2023/11/1.
//

#ifndef COMPLIER_IR_INSTRUCTION_H
#define COMPLIER_IR_INSTRUCTION_H

#include "../irPreDeclHeader.h"
#include "../User.h"
#define INSTRUCTIONCLASS "instructionClass"
class Instruction :public User{
public:
    Instruction(string name, DataType* dateType, int id, BasicBlock* parent);
    Instruction(string name, DataType* dateType, int id, BasicBlock* parent, Value* value1);
    Instruction(string name, DataType* dateType, int id, BasicBlock* parent, Value* value1, Value* value2);
    Instruction(string name, DataType* dateType, int id, BasicBlock* parent, Value* value1, Value* value2, Value* value3);
    virtual string print(){
        return "instruction, but why this happen";
    }
    string getClassName() override {
        return INSTRUCTIONCLASS;
    }
};


#endif //COMPLIER_IR_INSTRUCTION_H
