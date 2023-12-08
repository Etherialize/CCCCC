//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJSTORE_H
#define COMPLIER_MIPS_OBJSTORE_H

#include "ObjInstruction.h"
#define OBJSTORECLASS "objStoreClass"
class ObjStore : public ObjInstruction{
private:
    ObjOperand* src_ = nullptr;
    ObjOperand* addr_ = nullptr;
    ObjOperand* offset_ = nullptr;
public:
    ObjStore(ObjOperand* src, ObjOperand* addr, ObjOperand* offset){
        setSrc(src);
        setAddr(addr);
        setOffset(offset);
    }
    string getClassName() override { return OBJSTORECLASS;}
    void setSrc(ObjOperand* newSrc) {
        addUseReg(src_, newSrc);
        src_ = newSrc;
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
        if(src_ == oldReg) {
            setSrc(newReg);
        }
        if(addr_ == oldReg) {
            setAddr(newReg);
        }
        if(offset_ == oldReg) {
            setOffset(newReg);
        }
    }
    void replaceReg(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(src_ == oldReg) {
            setSrc(newReg);
        }
        if(addr_ == oldReg) {
            setAddr(newReg);
        }
    }
    string print() override {
        return "sw " + src_->print() + ",\t" + offset_->print() + "(" + addr_->print() + ")\n";
    }
};


#endif //COMPLIER_MIPS_OBJSTORE_H
