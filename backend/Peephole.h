//
// Created by Han Zhengyang on 2023/12/7.
//

#ifndef COMPLIER_MIPS_PEEPHOLE_H
#define COMPLIER_MIPS_PEEPHOLE_H

#include "irParser.h"

class Peephole {
private:
    ObjModule* module_;
public:
    Peephole(ObjModule* module):module_(module){
        vector<ObjFunction*> functions = module_->getFuncs();
        for(auto func:functions) {
            process(func);
        }

    }
    void process(ObjFunction* function) {
        handleMove(function);
    }
    void handleMove(ObjFunction* function) {
        vector<ObjBlock*>& blocks = function->getBlocks();
        for(auto curBlock:blocks) {
            vector<ObjInstruction*>& insts = curBlock->getInsts();
            auto inst = insts.begin();
            while(inst != insts.end()) {
                if(isUselessMove(*inst)) {
                    inst = insts.erase(inst);
                } else if(isUselessArith(*inst)){
                    inst = insts.erase(inst);
                } else {
                    *inst = canArithToMove(*inst);
                    inst++;
                }
            }
        }
    }
    bool isUselessMove(ObjInstruction* instruction){
        if(instruction->getClassName() == OBJMOVECLASS) {
            ObjMove* move = (ObjMove*)instruction;
            if(move->getDst()->print() == move->getSrc()->print()) {
                return true;
            }
        }
        return false;
    }
    bool isUselessArith(ObjInstruction* instruction) {
        if(instruction->getClassName() == OBJARITHCLASS) {
            ObjArithmetic* arith = (ObjArithmetic*) instruction;
            if(arith->getDst()->print() == arith->getSrc1()->print() &&
              (arith->getArithType() == OBJADDU || arith->getArithType() == OBJSUBU)) {
                if(arith->getSrc2()->getClassName() == OBJIMMCLASS && arith->getSrc2()->print() == "0") {
                    return true;
                }
            }
        }
        return false;
    }
    ObjInstruction* canArithToMove(ObjInstruction* instruction) {
        if(instruction->getClassName() == OBJARITHCLASS) {
            ObjArithmetic* arith = (ObjArithmetic*) instruction;
            if(arith->getArithType() == OBJADDU || arith->getArithType() == OBJSUBU) {
                if(arith->getSrc2()->getClassName() == OBJIMMCLASS && arith->getSrc2()->print() == "0") {
                    ObjMove* newMove = new ObjMove(arith->getDst(), arith->getSrc1());
                    return newMove;
                }
            }
        }
        return instruction;
    }
};


#endif //COMPLIER_MIPS_PEEPHOLE_H
