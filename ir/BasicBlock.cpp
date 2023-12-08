//
// Created by Han Zhengyang on 2023/11/1.
//

#include "BasicBlock.h"
#include "Function.h"
#include "Types/typesHeader.h"
#include "Instructions/insHeader.h"
BasicBlock::BasicBlock(int nameNum, int id, FunctionIR* parent): Value("%b" + to_string(nameNum), new LabelType(), id, parent){
}
string BasicBlock::getClassName(){
    return BASICBLOCKCLASS;
}
void BasicBlock::addInstruction(Instruction* instruction) {
    if(instructions_.size() != 0) {
        Instruction* last = instructions_[instructions_.size() - 1];
        Br* brIns = dynamic_cast<Br*>(last);
        Return* retIns = dynamic_cast<Return*>(last);
        if(brIns != nullptr || retIns != nullptr) {      //break 和 continue // return
            return;
        }
    }
    instructions_.push_back(instruction);
    if(instruction->getClassName() == BRCLASS) {
        ((Br*) instruction)->setGraph();
    }

}
void BasicBlock::addInstructionToHead(Instruction* instruction) {
    instructions_.insert(instructions_.begin(),instruction);
}
vector<Instruction*> BasicBlock::getInstructions() {
    return instructions_;
}
Instruction* BasicBlock::getIns(int index) {
    return instructions_[index];
}
Instruction* BasicBlock::getLastIns() {
    return instructions_[instructions_.size() - 1];
}
void BasicBlock::printBasicBlock(ofstream & output) {
    output << getName().substr(1, getName().length() - 1) + ":" <<endl;
    int flag = -1;
    for(int i = 0; i < instructions_.size(); i++) {
        Instruction* ins = instructions_[i];
        output << "  " + ins->print() << endl;
    }
    output<<endl;
}
void BasicBlock::checkRet() {
    if(instructions_.size() == 0){
        Return* ret = new Return(new VoidType(), -1, this, NULL);
        instructions_.push_back(ret);
    } else {
        Instruction* last = instructions_[instructions_.size() - 1];
        Br* brLast = dynamic_cast<Br*>(last);
        Return* retLast = dynamic_cast<Return*>(last);
        if(retLast == nullptr && brLast == nullptr) {
            Return* ret = new Return(new VoidType(), -1, this, NULL);
            instructions_.push_back(ret);
        }
    }
}
int BasicBlock::getInsSum() {
    return instructions_.size();
}
void BasicBlock::addPredecessor(BasicBlock* predecessor) {
    predecessors_.push_back(predecessor);
}
void BasicBlock::replacePredecessor(BasicBlock* oldPred) {
    for(int i = 0; i < predecessors_.size(); i++) {
        if(predecessors_[i] == oldPred) {
            predecessors_.erase(predecessors_.begin() + i);
            return;
        }
    }
    throw runtime_error("replacePredecessor error");
}
void BasicBlock::addSuccessor(BasicBlock* successor) {
    successor->addPredecessor(this);
    successors_.push_back(successor);
}
void BasicBlock::replaceSuccessor(BasicBlock* oldSucc, BasicBlock* newSucc) {
    for(int i = 0; i < successors_.size(); i++) {
        if(successors_[i] == oldSucc) {
            successors_.erase(successors_.begin() + i);
            if(newSucc != nullptr) {
                addSuccessor(newSucc);
            }
            return;
        }
    }
    throw runtime_error("replaceSuccessor error");
}
vector<BasicBlock*> BasicBlock::getPredecessors(){return predecessors_;}
vector<BasicBlock*> BasicBlock::getSuccessors(){return successors_;}
void BasicBlock::removePrede(BasicBlock* prede){
    for(int i = 0; i < predecessors_.size(); i++) {
        if(prede == predecessors_[i]){
            predecessors_.erase(predecessors_.begin() + i);
            break;
        }
    }
}
void BasicBlock::removeSucce(BasicBlock* succe) {
    for(int i = 0; i < successors_.size(); i++) {
        if(succe == successors_[i]) {
            successors_.erase(successors_.begin() + i);
            break;
        }
    }
}
BasicBlock* BasicBlock::isBrBlock() {
    Instruction* first = instructions_[0];
    if(first->getClassName() != BRCLASS || instructions_.size() != 1) {
        return nullptr;
    } else {
        Br* br = (Br*) first;
        if(br->hasCondition()) {
            return nullptr;
        }
    }
    return successors_[0];
}
unordered_set<BasicBlock*>& BasicBlock::getDomers(){return domers_;}
unordered_set<BasicBlock*>& BasicBlock::getStrictDomers() {return strictDomers_;}
unordered_set<BasicBlock*>& BasicBlock::getDF() {return DF_;}
BasicBlock* BasicBlock::getImmDomer(){return immDomer_;}
void BasicBlock::addDomer(BasicBlock* domer){domers_.insert(domer);}
int BasicBlock::setDomers(unordered_set<BasicBlock*>& blocks){
    int flag;   // 0 is changed, 1 is no changed
    int size_1 = domers_.size();
    domers_ = unordered_set(blocks);
    flag = size_1 == domers_.size() ? 1 : 0;
    return flag;
}
void BasicBlock::addImmDomee(BasicBlock* immDomee) {
    immDomees_.insert(immDomee);
}
bool BasicBlock::isStrictDominatedBy(BasicBlock* block) {
    auto it = strictDomers_.find(block);
    return it != strictDomers_.end();
}
void BasicBlock::setImmDomer(){
    for(auto it1 : strictDomers_) {
        int flag = 0;
        for(auto it2 : strictDomers_) {
            if(it2->isStrictDominatedBy(it1)) {
                flag = -1;
                break;
            }
        }
        if(flag == 0) {
            immDomer_ = it1;
            it1->addImmDomee(this);
            break;
        }
    }
}
void BasicBlock::setStrictDomers() {
    strictDomers_ = unordered_set(domers_);
    auto ff = strictDomers_.find(this);
    strictDomers_.erase(ff);
}
bool BasicBlock::needPhi(Alloca* alloca1) {
    for(auto it : instructions_) {
        if(it->getClassName() == STORECLASS && it->getUsedValue(1) == alloca1) {
            return false;
        }
        if(it->getClassName() == LOADCLASS && it->getUsedValue(0) == alloca1) {
            return true;
        }
    }
    return true;
}
void BasicBlock::removeIns(Instruction* instruction) {
    int flag = -1;
    for(int i = 0; i < instructions_.size(); i++) {
        if(instructions_[i]->getId() == instruction->getId()) {
            flag = i;
            break;
        }
    }
    instructions_.erase(instructions_.begin() + flag);
}