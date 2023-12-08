//
// Created by Han Zhengyang on 2023/12/5.
//

#ifndef COMPLIER_MIPS_BLOCKLIVENESS_H
#define COMPLIER_MIPS_BLOCKLIVENESS_H

#include "irParser.h"

class BlockLiveness {
private:
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> uses_;
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> defs_;
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> liveIn_;
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> liveOut_;
public:
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> getLiveOut(){return liveOut_;}
    static unordered_map<ObjBlock*, BlockLiveness*>* calBlocksLiveness(ObjFunction* function) {
        unordered_map<ObjBlock*, BlockLiveness*>* rst = new unordered_map<ObjBlock*, BlockLiveness*>();
        vector<ObjBlock*>& blocks = function->getBlocks();
        for(auto block : blocks) {
            BlockLiveness* curLiveness = new BlockLiveness();
            (*rst)[block] = curLiveness;

            vector<ObjInstruction*> insts = block->getInsts();
            for(auto inst :insts) {
                unordered_set <ObjReg*>& useRegs = inst->getUse();
                for(auto useReg: useRegs) {
                    //use before def
                    if( useReg->needsColor()&&curLiveness->defs_.find(useReg) == curLiveness->defs_.end()) {
                        curLiveness->uses_.insert(useReg);
                    }
                }
                unordered_set <ObjReg*>& defRegs = inst->getDef();
                for(auto defReg: defRegs) {
                    //def before use
                    if(defReg->needsColor()&&curLiveness->uses_.find(defReg) == curLiveness->uses_.end()) {
                        curLiveness->defs_.insert(defReg);
                    }
                }
            }
            curLiveness->liveIn_ = unordered_set(curLiveness->uses_);
        }

        bool changed = false;
        do{
            changed = false;
            for(int i = blocks.size() - 1; i > -1; i--) {
                unordered_set<ObjReg*, Reg_hash, Reg_cmp> newLiveOut;
                ObjBlock* curBlock = blocks[i];
                BlockLiveness* curLiveness = (*rst)[curBlock];
                if(curBlock->getTrueSucc() != nullptr) {
                    newLiveOut.insert((*rst)[curBlock->getTrueSucc()]->liveIn_.begin(), (*rst)[curBlock->getTrueSucc()]->liveIn_.end());
                }
                if(curBlock->getFalseSucc() != nullptr) {
                    newLiveOut.insert((*rst)[curBlock->getFalseSucc()]->liveIn_.begin(), (*rst)[curBlock->getFalseSucc()]->liveIn_.end());
                }
                if(newLiveOut.size() != curLiveness->liveOut_.size()) {
                    changed = true;
                    curLiveness->liveOut_ = newLiveOut;
                    curLiveness->calLiveIn();
                }
            }

        }while(changed);
        return rst;
    }
    void calLiveIn () {
        liveIn_ = unordered_set(liveOut_);
        for(auto defReg:defs_) {
            liveIn_.erase(defReg);
        }
        for(auto useReg:uses_) {
            liveIn_.insert(useReg);
        }
    }
};


#endif //COMPLIER_MIPS_BLOCKLIVENESS_H
