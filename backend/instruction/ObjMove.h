//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJMOVE_H
#define COMPLIER_MIPS_OBJMOVE_H

#include "ObjInstruction.h"
#define OBJMOVECLASS "objMoveClass"
#define UNDEFINETYPE "undefineType"
/*
 * mfhi mflo move li la
 */
class ObjMove: public ObjInstruction {
private:
    string type_;
    ObjOperand* dst_ = nullptr;
    ObjOperand* src_ = nullptr;
public:
    ObjMove(string type, ObjOperand* dst, ObjOperand* src):type_(type){
        setSrc(src);
        setDst(dst);
    }
    ObjMove(ObjOperand* dst, ObjOperand* src) {
        setSrc(src);
        setDst(dst);
        if(src == nullptr || dst == nullptr) {
            type_ = UNDEFINETYPE;
        }
        else if(src->getClassName() == OBJLABELCLASS) {
            type_ = "la";
        } else if(src->getClassName() == OBJIMMCLASS) {
            type_ = "li";
        } else {
            type_ = "move";
        }
    }
    string getClassName() override { return OBJMOVECLASS;}
    void setDst(ObjOperand* newDst) {
        addDefReg(dst_, newDst);
        dst_ = newDst;
    }
    ObjOperand* getDst(){return dst_;}
    void setSrc(ObjOperand* newSrc) {
        addUseReg(src_, newSrc);
        src_ = newSrc;
    }
    ObjOperand* getSrc(){return src_;}
    void replaceUse(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(src_ == oldReg) {
            setSrc(newReg);
        }
    }
    void replaceReg(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(dst_ == oldReg) {
            setDst(newReg);
        }
        if(src_ == oldReg) {
            setSrc(newReg);
        }
    }
    static ObjMove* mthi(ObjOperand* src) {
        return new ObjMove("mthi", nullptr, src);
    }
    static ObjMove* mfhi(ObjOperand* dst) {
        return new ObjMove("mfhi", dst, nullptr);
    }
    string print() override {
        if(type_ == UNDEFINETYPE) {
            if(src_->getClassName() == OBJLABELCLASS) {
                type_ = "la";
            } else if(src_->getClassName() == OBJIMMCLASS) {
                type_ = "li";
            } else {
                type_ = "move";
            }
        }
        return type_ + " " + dst_->print() + ",\t" + src_->print() + "\n";
    }
};


#endif //COMPLIER_MIPS_OBJMOVE_H
