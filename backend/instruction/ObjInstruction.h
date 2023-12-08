//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJINSTRUCTION_H
#define COMPLIER_MIPS_OBJINSTRUCTION_H
#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include "../operand/operHeader.h"
using namespace std;

class ObjInstruction {
private:
    unordered_set<ObjReg *> regDef_;
    unordered_set<ObjReg *> regUse_;

    void addUse(ObjOperand *newRegUse) {
        if (newRegUse!= nullptr && newRegUse->isReg()) {
            regUse_.insert((ObjReg *) newRegUse);
        }
    }

    void addDef(ObjOperand *newRegDef) {
        if (newRegDef!= nullptr && newRegDef->isReg()) {
            regDef_.insert((ObjReg *) newRegDef);
        }
    }

    void removeDef(ObjOperand *oldRegDef) {
        if (oldRegDef->isReg()) {
            regDef_.erase((ObjReg *) oldRegDef);
        }
    }

    void removeUse(ObjOperand *oldRegUse) {
        if (oldRegUse->isReg()) {
            regUse_.erase((ObjReg *) oldRegUse);
        }
    }

public:
    virtual string getClassName() {return "";}
    void addUseReg(ObjOperand *oldReg, ObjOperand *newReg) {
        if (oldReg != nullptr) {
            removeUse(oldReg);
        }
        addUse(newReg);
    }

    void addDefReg(ObjOperand *oldReg, ObjOperand *newReg) {
        if (oldReg != nullptr) {
            removeDef(oldReg);
        }
        addDef(newReg);
    }

    unordered_set<ObjReg *>& getUse() {
        return regUse_;
    }
    unordered_set<ObjReg *>& getDef() {
        return regDef_;
    }
    virtual string print(){
        return "";
    }
    virtual void replaceUse(ObjOperand *oldReg, ObjOperand *newReg){

    }
    virtual void replaceReg(ObjOperand *oldReg, ObjOperand *newReg){

    }
};

#endif //COMPLIER_MIPS_OBJINSTRUCTION_H
