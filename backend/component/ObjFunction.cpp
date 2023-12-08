//
// Created by Han Zhengyang on 2023/11/20.
//

#include "ObjFunction.h"
#include "ObjBlock.h"
ObjFunction::ObjFunction(string name) {
    name_ = name.substr(1, name.size() - 1);
    allocaSize_ = 0;
}

void ObjFunction::addBlock(ObjBlock *block) {
    blocks_.push_back(block);
}
void ObjFunction::addBlockAfter(ObjBlock* block, ObjBlock* index) {
    for(int i = 0; i < blocks_.size(); i++) {
        if(blocks_[i] == index) {
            blocks_.insert(blocks_.begin() + i + 1, block);
        }
    }
}
void ObjFunction::addVirReg(ObjVirReg* objVirReg) {
    usedVirRegs_.push_back(objVirReg);
}
vector<ObjVirReg*>& ObjFunction::getUsedVirRegs() {
    return usedVirRegs_;
}
vector<ObjBlock*>& ObjFunction:: getBlocks() { return blocks_;}
void ObjFunction::addArgOffsets(ObjImm* objImm) {
    argOffsets_.push_back(objImm);
}
vector<ObjReg*>& ObjFunction::getSavedRegs(){ return savedRegsList_;}
void ObjFunction::addAllocaSize(int allocaSize) {
    allocaSize_ += allocaSize;
}
void ObjFunction::insertPhis(ObjBlock* cur, ObjBlock* pred, vector<ObjInstruction*>* phiLists){
    if(phiLists->size() == 0) {
        return;
    }
    if(pred->getFalseSucc() == nullptr || pred->getFalseSucc() == cur) {
        for(auto it : (*phiLists)) {
            pred->addPhi(it);
        }
    } else {
        ObjInstruction* brIns = pred->getBrIns();
        ObjBlock* newSucc = new ObjBlock(pred->getName());
        ((ObjBranch*)brIns)->setTarget(new ObjLabel(newSucc->getName()));
        pred->setTrueSucc(newSucc);
        newSucc->addPred(pred);
        newSucc->setTrueSucc(cur);
        cur->removePred(pred);
        for(auto it :(*phiLists)){
            newSucc->addInstruction(it);
        }
        ObjBranch *branch = new ObjBranch(new ObjLabel(cur->getName()));
        newSucc->addInstruction(branch);
        addBlockAfter(newSucc, pred);
    }
}
string ObjFunction::getName() {
    return name_;
}
void ObjFunction::setStackSize() {
    //baocun
    for(auto block : blocks_) {
        vector<ObjInstruction*>& insts = block->getInsts();
        for(auto inst : insts) {
            unordered_set<ObjReg*> defRegs = inst->getDef();
            for(auto def: defRegs) {
                if(ObjPhyReg::savedRegs_.find(((ObjPhyReg*)def)->getIndex()) != ObjPhyReg::savedRegs_.end()){
                    savedRegs_.insert(def);
                }
            }
        }
    }
    stackSize_ = savedRegs_.size()*4 + allocaSize_;             //现阶段只保存了ra寄存器
    for(int i = 0; i < argOffsets_.size(); i++) {
        ObjImm* offset = argOffsets_[i];
        offset->setImmediate(offset->getImmediate() + stackSize_);
    }
}
void ObjFunction::printObjFunc(ofstream& output) {
    setStackSize();
    output << name_ + ":" << endl;
    if(name_ != "main") {
        int offset = -4;
        for(auto reg:savedRegs_) {
            output << "\tsw " + reg->print() +",\t" + to_string(offset) + "($sp)"<<endl;
            savedRegsList_.push_back(reg);
            offset -= 4;
        }
    }
    if(stackSize_ != 0) {
        output << "\taddi $sp,\t$sp,\t-" + to_string(stackSize_) << endl;
    }

    for(int i = 0; i < blocks_.size(); i++) {
        blocks_[i]->printBlock(output);
    }
}