//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJARITHMETIC_H
#define COMPLIER_MIPS_OBJARITHMETIC_H
#define OBJADDU "addu"
#define OBJSUBU "subu"
#define OBJXOR "xor"
#define OBJSLTU "sltu"
#define OBJSLT "slt"
#define OBJMUL "mul"
#define OBJDIV "div"
#define ObjREM "rem"
#include "ObjInstruction.h"
#define OBJARITHCLASS "objArithClass"
/*
 * 需要分配寄存器
 */
class ObjArithmetic : public ObjInstruction{
private:
    string type_;
    ObjOperand* dst_ = nullptr;
    ObjOperand* src1_ = nullptr;
    ObjOperand* src2_ = nullptr;
public:
    string getClassName() override { return OBJARITHCLASS;}
    ObjArithmetic(string type, ObjOperand* dst, ObjOperand* src1, ObjOperand* src2):type_(type){
        setSrc1(src1);
        setSrc2(src2);
        setDst(dst);
    }
    string getArithType() {return type_;}
    void setDst(ObjOperand* newDst) {
        addDefReg(dst_, newDst);
        dst_ = newDst;
    }
    ObjOperand* getDst(){return dst_;}
    ObjOperand* getSrc1(){return src1_;}
    ObjOperand* getSrc2(){return src2_;}
    void setSrc1(ObjOperand* newSrc1) {
        addUseReg(src1_, newSrc1);
        src1_ = newSrc1;
    }

    void setSrc2(ObjOperand* newSrc2) {
        addUseReg(src2_, newSrc2);
        src2_ = newSrc2;
    }
    void replaceUse(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(src1_ == oldReg) {
            setSrc1(newReg);
        }
        if(src2_ == oldReg) {
            setSrc2(newReg);
        }
    }
    void replaceReg(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(dst_ == oldReg) {
            setDst(newReg);
        }
        if(src1_ == oldReg) {
            setSrc1(newReg);
        }
        if(src2_ == oldReg) {
            setSrc2(newReg);
        }
    }
    bool isSrc2Imm() {
        return src2_->getClassName() == OBJIMMCLASS;
    }
    string print() override {
        if(isSrc2Imm()) {
            string ins;
            if(type_ == OBJADDU) {
                ins = "addiu";
            } else if(type_ == OBJSUBU) {
                ins = "subiu";
            } else if(type_ == OBJSLTU) {
                ins = "sltiu";
            } else {
                ins = type_ + "i";
            }
            return ins + " " + dst_->print() + ",\t" + src1_->print() + ",\t" + src2_->print() + "\n";
        } else {
            if(type_ == OBJMUL) {
                return "mult " + src1_->print() + ",\t" + src2_->print() + "\n\t" +
                       "mflo " + dst_->print() + "\n";
            } else if(type_ == OBJDIV) {
                return  "div " + src1_->print() + ",\t" + src2_->print() + "\n\t" +
                              "mflo " + dst_->print() + "\n";
            } else if(type_ == ObjREM) {
                return  "div " + src1_->print() + ",\t" + src2_->print() + "\n\t" +
                        "mfhi " + dst_->print() + "\n";
            } else {
                return type_ + " " + dst_->print() + ",\t" + src1_->print() + ",\t" + src2_->print() + "\n";
            }
        }
    }
};


#endif //COMPLIER_MIPS_OBJARITHMETIC_H
