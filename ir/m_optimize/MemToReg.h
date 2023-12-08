//
// Created by Han Zhengyang on 2023/11/29.
//

#ifndef COMPLIER_MIPS_MEMTOREG_H
#define COMPLIER_MIPS_MEMTOREG_H

#include "../Driver.h"
#include "unordered_set"
class MemToReg{
private:
    int phiNum_ = -1;
    vector<Alloca*> allocas_;
    map<Phi*, Alloca*> phiToAlloca_;
    unordered_set <BasicBlock*> useBlocks_;
    unordered_set<BasicBlock*> defineBlocks_;
    Store* only_;
    FunctionIR* curFunc_;
    BasicBlock* entry_;
public:
    int NumPlus() {return ++phiNum_;}
    void run(Module* module);
    void run(FunctionIR* func);
    void removeBrBlock();
    /*
     * 当进行完removeBrBlock时 可能会出现 br %v1 B1, B1
     */
    void checkBr();
    void removeUselessBlock();
    void buildDominate();
    void buildDF();
    void insertPhi(Alloca* alloca1, unordered_set<BasicBlock*>* CDF);
    void renamePhi();
    unordered_set <BasicBlock*>* calCDF();
    /*
     * 将交集存到s中，并且对s2,s1不做处理
     */
    void intersect(unordered_set<BasicBlock*>& s1, unordered_set<BasicBlock*>& s2);
    void analyzeAlloca(Alloca* alloca1);
};
#endif //COMPLIER_MIPS_MEMTOREG_H
