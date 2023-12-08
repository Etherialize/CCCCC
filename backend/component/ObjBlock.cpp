//
// Created by Han Zhengyang on 2023/11/20.
//

#include "ObjBlock.h"
int ObjBlock:: index_ = -1;
ObjBlock::ObjBlock(string name) {
    if(name.length() >= 6 && name.substr(0,6) == "basic_") {
        name_ = name + "_succ_" + to_string(++index_);
    }else {
        name_ = "basic_" + name.substr(1, name.size() - 1) + "_" + to_string(++index_);
    }
}
void ObjBlock::addInstruction(ObjInstruction* instruction) {
    instructions_.push_back(instruction);
}
void ObjBlock::addInsToHead(ObjInstruction* instruction) {
    instructions_.insert(instructions_.begin(), instruction);
}
void ObjBlock::addInstructionIndex(ObjInstruction* f, ObjInstruction* c, int index) {
    int flag = 0;
    for(int i = 0; i < instructions_.size(); i++) {
        if(instructions_[i] == f) {
            flag = i;
            break;
        }
    }
    instructions_.insert(instructions_.begin() + flag + index, c);
}
void ObjBlock::addPhi(ObjInstruction* objInstruction) {
    instructions_.insert(instructions_.end() - 1, objInstruction);
}
vector<ObjInstruction*>& ObjBlock::getInsts(){
    return instructions_;
}
vector<ObjInstruction*>* ObjBlock::getInstsPointer() {return &instructions_;}
ObjInstruction* ObjBlock::getBrIns() {
    if(brIndex_  == -1) {
        for(int i = 0; i < instructions_.size(); i++) {
            if(instructions_[i]->getClassName() == OBJBRANCHCLASS){
                brIndex_ = i;
                break;
            }
        }
    }
    return instructions_[brIndex_];
}
string ObjBlock::getName() { return name_;}
void ObjBlock::printBlock(ofstream& output) {
    output << name_ + ":" << endl;
    for(int i = 0; i < instructions_.size(); i++) {
        output << "\t" + instructions_[i]->print();
    }
}
void ObjBlock::addPred(ObjBlock* pred){predes_.push_back(pred);}
void ObjBlock::removePred(ObjBlock* oldPred) {
    for(int i = 0; i < predes_.size(); i++) {
        if(predes_[i] == oldPred) {
            predes_.erase(predes_.begin() + i);
            return;
        }
    }
}
void ObjBlock::setTrueSucc(ObjBlock* succ){
    trueSucc_ = succ;
    succ->addPred(this);
}
void ObjBlock::setFalseSucc(ObjBlock* succ){
    falseSucc_ = succ;
    succ->addPred(this);
}