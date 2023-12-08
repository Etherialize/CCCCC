//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJBLOCK_H
#define COMPLIER_MIPS_OBJBLOCK_H

#include "../instruction/ObjInstruction.h"
#include "../instruction/ObjInsHeader.h"
#include "componentPreDecl.h"

class ObjBlock {
private:
    static int index_;
    string name_;
    vector<ObjInstruction*> instructions_;
    vector<ObjBlock*> predes_;
    ObjBlock* trueSucc_ = nullptr;
    ObjBlock* falseSucc_ = nullptr;
    int brIndex_ = -1;     // br true index
public:
    ObjBlock(string name);
    void addInstruction(ObjInstruction* instruction);
    void addInsToHead(ObjInstruction* instruction);
    void addInstructionIndex(ObjInstruction* f, ObjInstruction* c, int index);
    void addPhi(ObjInstruction* objInstruction);
    vector<ObjInstruction*>& getInsts();
    vector<ObjInstruction*>* getInstsPointer();
    ObjInstruction* getBrIns();
    string getName();
    void printBlock(ofstream& output);
    void addPred(ObjBlock* pred);
    void removePred(ObjBlock* oldPred);
    void setTrueSucc(ObjBlock* succ);
    ObjBlock* getTrueSucc() {
        return trueSucc_;
    }
    ObjBlock* getFalseSucc() {
        return falseSucc_;
    }
    void setFalseSucc(ObjBlock* succ);
    size_t operator()(const ObjBlock& obj) const {
        return std::hash<string>()(obj.name_);
    }
    bool operator==(const ObjBlock& other) const{
        return name_ == other.name_;
    }
    bool operator<(const ObjBlock& other) const{
        return name_ < other.name_;
    }
};



#endif //COMPLIER_MIPS_OBJBLOCK_H
