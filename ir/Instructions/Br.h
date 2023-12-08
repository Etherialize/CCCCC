//
// Created by Han Zhengyang on 2023/11/13.
//

#ifndef COMPLIER_IR_BR_H
#define COMPLIER_IR_BR_H

#include "Instruction.h"
#define BRCLASS "brClass"
class Br :public Instruction{
private:
    bool hasCondition_;
public:
    Br(int id, BasicBlock* parent, BasicBlock* target);
    Br(int id, BasicBlock* parent, Value* cond, BasicBlock* target1, BasicBlock* target2);
    bool hasCondition();
    void setGraph();
    void handleSameTarget();
    string print() override ;
    string getClassName() override {
        return BRCLASS;
    }
};


#endif //COMPLIER_IR_BR_H
