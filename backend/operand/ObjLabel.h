//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJLABEL_H
#define COMPLIER_MIPS_OBJLABEL_H
#define OBJLABELCLASS "objLabelClass"
#include "ObjOperand.h"
class ObjLabel : public ObjOperand{
private:
    string name_;
public:
    ObjLabel(string name):name_(name) {}
    string getClassName() const override  {
        return OBJLABELCLASS;
    }
    string print() override {
        return name_;
    }
    bool isReg() override {
        return false;
    }
};


#endif //COMPLIER_MIPS_OBJLABEL_H
