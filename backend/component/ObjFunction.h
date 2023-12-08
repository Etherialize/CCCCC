//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJFUNCTION_H
#define COMPLIER_MIPS_OBJFUNCTION_H

#include "componentPreDecl.h"
#include "../operand/operHeader.h"
#include "../instruction/ObjInstruction.h"
#include <unordered_map>
#include <stack>
#include <algorithm>
class ObjFunction {
private:
    string name_;
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> savedRegs_;
    vector<ObjReg*> savedRegsList_;
    vector<ObjBlock*> blocks_;
    vector<ObjVirReg*> usedVirRegs_;
    vector<ObjImm*> argOffsets_;
    int allocaSize_;
    int stackSize_;
    unordered_map<ObjBlock*, bool> hasSet_;
public:
    ObjFunction(string name);
    void addBlock(ObjBlock* block);
    void addBlockAfter(ObjBlock* block, ObjBlock* index);
    void addVirReg(ObjVirReg* objVirReg);
    vector<ObjVirReg*>& getUsedVirRegs();
    vector<ObjBlock*>& getBlocks();
    vector<ObjReg*>& getSavedRegs();
    void addArgOffsets(ObjImm* objImm);
    void addAllocaSize(int allocaSize);
    int getAllocaSize() {
        return allocaSize_;
    }
    /*
     * stack
     * / -- arg -----------/
     * / -- savedPhyRegs---/
     * / -- virReg --------/
     * / -- alloca --------/
     */
    void setStackSize();
    int getStackSize() {
        return stackSize_;
    }
    void insertPhis(ObjBlock* cur, ObjBlock* pred, vector<ObjInstruction*>* phiLists);
    string getName();
    void printObjFunc(ofstream &output);
};


#endif //COMPLIER_MIPS_OBJFUNCTION_H
