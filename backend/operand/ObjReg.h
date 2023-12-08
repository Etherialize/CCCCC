//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJREG_H
#define COMPLIER_MIPS_OBJREG_H
#define OBJREGCLASS "objReg"
#include "ObjOperand.h"
class ObjReg: public ObjOperand {
protected:
    string name_;
public:
    string getName() const{
        return name_;
    }
    virtual string getClassName() const override {
        return OBJREGCLASS;
    }
    bool isReg() override {
        return true;
    }
};
class Reg_hash{
public:
    size_t operator()(const ObjReg* obj) const {
        return std::hash<string>()(obj->getName());
    }
};
class Reg_cmp {
public:
    bool operator()(const ObjReg* rc1, const ObjReg* rc2) const       //重载=
    {
        return rc1->getName() == rc2->getName();
    }
};


#endif //COMPLIER_MIPS_OBJREG_H
