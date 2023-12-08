//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJBRANCH_H
#define COMPLIER_MIPS_OBJBRANCH_H

#include "ObjInstruction.h"
#include "../component/ObjBlock.h"
#define OBJBRANCHCLASS "objBranchClass"
enum ObjCondType {
    ANY, EQ, NE, GE, GT, LE, LT
};
class ObjBranch : public ObjInstruction{
private:
    ObjCondType condType_;
    ObjOperand* src1_ = nullptr;
    ObjOperand* src2_ = nullptr;
    ObjOperand* target_ = nullptr;
    bool hasCondition_ = false;
    int srcSum_ = 0;
public:
    ObjBranch(ObjCondType condType, ObjOperand* src1, ObjOperand* src2, ObjOperand* target): condType_(condType){
        hasCondition_ = true;
        srcSum_ = 2;
        setSrc1(src1);
        setSrc2(src2);
        setTarget(target);
    }
    ObjBranch(ObjOperand* target){
        condType_ = ObjCondType ::ANY;
        srcSum_ = 1;
        hasCondition_ = false;
        setTarget(target);
    }
    ObjBranch(ObjCondType condType, ObjOperand* src1, ObjOperand* target):condType_(condType){
        srcSum_ = 1;
        hasCondition_ = true;
        setSrc1(src1);
        setTarget(target);
    }
    string getClassName() override { return OBJBRANCHCLASS;}
    void setSrc1(ObjOperand* newSrc1) {
        addUseReg(src1_, newSrc1);
        src1_ = newSrc1;
    }
    void setSrc2(ObjOperand* newSrc2) {
        addUseReg(src2_, newSrc2);
        src2_ = newSrc2;
    }
    void setTarget(ObjOperand* newTarget) {
        addUseReg(target_, newTarget);
        target_ = newTarget;
    }
    void replaceUse(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(src1_ == oldReg) {
            setSrc1(newReg);
        }
        if(src2_ == oldReg) {
            setSrc2(newReg);
        }
        if(target_ == oldReg) {
            setTarget(newReg);
        }
    }
    void replaceReg(ObjOperand *oldReg, ObjOperand *newReg) override {
        if(src1_ == oldReg) {
            setSrc1(newReg);
        }
        if(src2_ == oldReg) {
            setSrc2(newReg);
        }
        if(target_ == oldReg) {
            setTarget(newReg);
        }
    }
    static ObjCondType getOppCondType(ObjCondType type) {
        if(type == ObjCondType::EQ) {
            return ObjCondType::NE;
        } else if(type == ObjCondType::NE) {
            return ObjCondType ::EQ;
        } else if(type == ObjCondType::LT) {
            return ObjCondType ::GE;
        }else if(type == ObjCondType::GT) {
            return ObjCondType ::LE;
        }else if(type == ObjCondType::LE) {
            return ObjCondType ::GT;
        }else if(type == ObjCondType::GE){
            return ObjCondType ::LT;
        }
        return ObjCondType::EQ;
    }
    static ObjCondType getSwapCondType(ObjCondType type) {
        if(type == ObjCondType::EQ) {
            return ObjCondType::EQ;
        } else if(type == ObjCondType::NE) {
            return ObjCondType ::NE;
        } else if(type == ObjCondType::LT) {
            return ObjCondType ::GT;
        }else if(type == ObjCondType::GT) {
            return ObjCondType ::LT;
        }else if(type == ObjCondType::LE) {
            return ObjCondType ::GE;
        }else if(type == ObjCondType::GE){
            return ObjCondType ::LE;
        }
        return ObjCondType::EQ;
    }
    static string condToStr(ObjCondType type) {
        if(type == ObjCondType::EQ) {
            return "eq";
        } else if(type == ObjCondType::NE) {
            return "ne";
        } else if(type == ObjCondType::LT) {
            return "lt";
        }else if(type == ObjCondType::GT) {
            return "gt";
        }else if(type == ObjCondType::LE) {
            return "le";
        }else if(type == ObjCondType::GE){
            return "ge";
        }
        return "eq";
    }
    string print() override {
        if(!hasCondition_) {
            return "j " + target_->print() + "\n";
        } else if(srcSum_ == 1) {
            return "b" + condToStr(condType_) + "z " + src1_->print() + ",\t" + target_->print() + "\n";
        } else {
            return "b" + condToStr(condType_) + " " + src1_->print() + ",\t" + src2_->print() + ",\t" + target_->print() + "\n";
        }
    }
};

#endif //COMPLIER_MIPS_OBJBRANCH_H
