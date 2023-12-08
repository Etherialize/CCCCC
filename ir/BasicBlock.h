//
// Created by Han Zhengyang on 2023/11/1.
//

#ifndef COMPLIER_IR_BASICBLOCK_H
#define COMPLIER_IR_BASICBLOCK_H

#include "irPreDeclHeader.h"
#include "Instructions/insPreDeclHeader.h"
#include "User.h"
#include "unordered_set"
#define BASICBLOCKCLASS "BasicBlockClass"
class BasicBlock : public Value{
private:
    vector<Instruction*> instructions_;
    /*
     * 前驱和后继
     */
    vector<BasicBlock*> predecessors_;
    vector<BasicBlock*> successors_;
    /*
     * 支配者
     */
    unordered_set<BasicBlock*> domers_;
    /*
     * 严格支配者
     */
    unordered_set<BasicBlock*> strictDomers_;
    /*
     * 直接支配该基本块的块
     */
    BasicBlock* immDomer_;
    /*
     * 该基本块直接支配的块
     */
    unordered_set<BasicBlock*> immDomees_;
    /*
     * 支配边际
     */
    unordered_set<BasicBlock*> DF_;

public:
    BasicBlock(int nameNum, int id, FunctionIR* parent);
    string getClassName() override ;
    void printBasicBlock(ofstream & output);
    void addInstruction(Instruction* instruction);
    void addInstructionToHead(Instruction* instruction);
    void addPredecessor(BasicBlock* predecessor);
    void replacePredecessor(BasicBlock* oldPred);
    void addSuccessor(BasicBlock* successor);
    void replaceSuccessor(BasicBlock* oldSucc, BasicBlock* newSucc);
    vector<BasicBlock*> getPredecessors();
    vector<BasicBlock*> getSuccessors();
    void removePrede(BasicBlock* prede);
    void removeSucce(BasicBlock* succe);
    vector<Instruction*> getInstructions();
    Instruction* getIns(int index);
    Instruction* getLastIns();
    void checkRet();
    int getInsSum();
    /*
     * 只有一个无条件br,返回后继
     */
    BasicBlock* isBrBlock();
    unordered_set<BasicBlock*>& getDomers();
    unordered_set<BasicBlock*>& getStrictDomers();
    unordered_set<BasicBlock*>& getDF();
    BasicBlock* getImmDomer();
    void addDomer(BasicBlock* domer);
    int setDomers(unordered_set<BasicBlock*>& blocks);
    void setStrictDomers();
    void setImmDomer();
    void addImmDomee(BasicBlock* immDomee);
    /*
     * 是否被block严格支配
     */
    bool isStrictDominatedBy(BasicBlock* block);
    bool needPhi(Alloca* alloca1);
    void removeIns(Instruction* instruction);
};


#endif //COMPLIER_IR_BASICBLOCK_H
