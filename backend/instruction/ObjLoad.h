//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJLOAD_H
#define COMPLIER_MIPS_OBJLOAD_H

#include "ObjInstruction.h"
#define OBJLOADCLASS "objLoadClass"
class ObjLoad : public ObjInstruction{
private:
    ObjOperand* dst_ = nullptr;
    ObjOperand* addr_ = nullptr;
    ObjOperand* offset_ = nullptr;
public:
    ObjLoad(ObjOperand* dst, ObjOperand* addr, ObjOperand* offset){
        setDst(dst);
        setAddr(addr);
        setOffset(offset);
    }string getClassName() override { return OBJLOADCLASS;}
    void setDst(ObjOperand* newDst) {
        addDefReg(dst_, newDst);
        dst_ = newDst;
    }
    void setAddr(ObjOperand* newAddr) {
        addUseReg(addr_, newAddr);
        addr_ = newAddr;
    }
    void setOffset(ObjOperand* newOffset) {
        addUseReg(offset_, newOffset);
        offset_ = newOffset;
    }
    void replaceUse(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(addr_ == oldReg) {
            setAddr(newReg);
        }
        if(offset_ == oldReg) {
            setOffset(newReg);
        }
    }
    void replaceReg(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(dst_ == oldReg) {
            setDst(newReg);
        }
        if(addr_ == oldReg) {
            setAddr(newReg);
        }
    }
    string print() override {
        return "lw " + dst_->print() + ",\t" + offset_->print() + "(" + addr_->print() + ")\n";
    }
};


#endif //COMPLIER_MIPS_OBJLOAD_H
