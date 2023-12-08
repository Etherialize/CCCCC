//
// Created by Han Zhengyang on 2023/11/29.
//

#ifndef COMPLIER_MIPS_PHI_H
#define COMPLIER_MIPS_PHI_H

#include "Instruction.h"
#define PHICLASS "phiClass"
class Phi : public Instruction {
private:
    int sum_;
public:
    Phi(int name, DataType* dataType, int id,BasicBlock* parent, int predNum);
    void addSource(Value* value, BasicBlock* block);
    string getClassName() override {
        return PHICLASS;
    }
    string print() override ;
    Value* getPhiSrc(BasicBlock* basicBlock);

};


#endif //COMPLIER_MIPS_PHI_H
