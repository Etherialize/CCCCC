//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJVIRREG_H
#define COMPLIER_MIPS_OBJVIRREG_H
#define OBJVIRREGCLASS "ObjVirRegClass"
#include "ObjReg.h"
class ObjVirReg :public ObjReg{
private:
    static int index_;
public:
    ObjVirReg() {
        name_ = "vir_" + to_string(++index_);
    }
    string getClassName() const override{
        return OBJVIRREGCLASS;
    }
    string print() override {
        return name_;
    }
    bool isReg() override {
        return true;
    }

    bool needsColor() override {return true;}
};


#endif //COMPLIER_MIPS_OBJVIRREG_H
