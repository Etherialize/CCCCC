//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJCALL_H
#define COMPLIER_MIPS_OBJCALL_H

#include "ObjInstruction.h"
#define OBJCALLCLASS "objCallClass"

class ObjCall: public ObjInstruction{
private:
    ObjOperand* targetFunc_;
public:
    ObjCall(ObjOperand* targetFunc):targetFunc_(targetFunc){}
    string getClassName() override { return OBJCALLCLASS;}
    string print() override {
        return "jal " + targetFunc_->print() + "\n";
    }
};


#endif //COMPLIER_MIPS_OBJCALL_H
