//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJIMM_H
#define COMPLIER_MIPS_OBJIMM_H

#include "ObjOperand.h"
#define OBJIMMCLASS "objImmClass"
class ObjImm: public ObjOperand {
private:
    int immediate_;
public:
    ObjImm(int immediate):immediate_(immediate) {
    }
    string getClassName() const override {
        return OBJIMMCLASS;
    }
    void setImmediate(int immediate) {
        immediate_ = immediate;
    }
    int getImmediate() {
        return immediate_;
    }
    string print() override {
        return to_string(immediate_);
    }
    bool isReg() override {
        return false;
    }
    bool operator<(const ObjImm& other) const {
        return immediate_ < other.immediate_;
    }
    bool operator==(const ObjImm& other) const {
        return immediate_ == other.immediate_;
    }
};



#endif //COMPLIER_MIPS_OBJIMM_H
