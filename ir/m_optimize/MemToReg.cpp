//
// Created by Han Zhengyang on 2023/11/29.
//

#include "MemToReg.h"
#include "../Module.h"
#include "../irBuilder.h"
#include "../Function.h"
#include "../BasicBlock.h"
#include "../Instructions/insHeader.h"
#include <utility>
#include <stack>
void MemToReg::run(Module* module){
    vector<FunctionIR*> funs = module->getFunctions();
    for(int i = 0; i < funs.size(); i++) {
        run(funs[i]);
    }
}
void MemToReg::run(FunctionIR* func) {
    curFunc_ = func;
    entry_ = func->getEntry();
    removeUselessBlock();                        //删去无用块
    removeBrBlock();                          //removeBr块,目前不太确定能不能这样搞，先关闭以后再说
    checkBr();
    buildDominate();                          //计算支配者，计算严格支配者，计算直接支配者
    buildDF();                                //计算支配边界

    BasicBlock* entry = func->getEntry();
    vector<Instruction*> insts = entry->getInstructions();
    for(int i = 0; i < insts.size(); i++) {
        if(insts[i]->getClassName() == ALLOCACLASS && ((Alloca*)insts[i])->canMemToReg()) {
            allocas_.push_back((Alloca*)insts[i]);
        }
    }

    auto alloca1 = allocas_.begin();

    while(alloca1 != allocas_.end()) {
        analyzeAlloca(*alloca1);
        unordered_set <BasicBlock*>* CDF = calCDF();
        unordered_set <BasicBlock*> deletedBlcoks;
        for(auto it : (*CDF)) {
            if(!it->needPhi(*alloca1)) {
                deletedBlcoks.insert(it);
            }
        }
        for(auto it : deletedBlcoks) {
            auto f = CDF->find(it);
            CDF->erase(f);
        }
        insertPhi(*alloca1, CDF);
        alloca1++;
    }
    renamePhi();
    for(auto it : allocas_) {
        it->removeAllUsedValue();
        ((BasicBlock*)(it->getParent()))->removeIns(it);
    }
    allocas_.clear();
    phiToAlloca_.clear();
}
void MemToReg::analyzeAlloca(Alloca* alloca1){
    useBlocks_.clear();
    defineBlocks_.clear();
    only_ = nullptr;

    int storeSum = 0;
    vector<User*>& users = alloca1->getUsers();
    for(int i = 0; i < users.size(); i++) {
        if(users[i]->getClassName() == STORECLASS) {
            defineBlocks_.insert((BasicBlock*)users[i]->getParent());
            if(storeSum == 0) {
                only_ = (Store*)users[i];
            }
            storeSum++;
        } else if(users[i]->getClassName() == LOADCLASS) {
            useBlocks_.insert((BasicBlock*) users[i]->getParent());
        }
    }
    if(storeSum > 1) {
        only_ = nullptr;
    }
}
void MemToReg::removeBrBlock(){
    vector<BasicBlock*>blocks = curFunc_->getBlocks();
    for(int i = 1; i < blocks.size(); i++) {                 //我们要保证entry块没有前驱节点，即使是只有一条无条件我们仍然保留
        BasicBlock* block = blocks[i];
        BasicBlock* successor;
        if((successor = block->isBrBlock()) != nullptr) {
            block->replaceSelf(successor);
            vector<BasicBlock*> preds = block->getPredecessors();
            for(int j = 0; j < preds.size(); j++) {
                preds[j]->replaceSuccessor(block, successor);
            }
            successor->replacePredecessor(block);
            curFunc_->removeBlock(block);
        }
    }
}
void MemToReg::checkBr() {
    vector<BasicBlock*>&blocks = curFunc_->getBlocks();
    for(auto block:blocks) {
        Instruction* last = block->getLastIns();
        if(last->getClassName() == RETURNCLASS) {
            continue;
        }
        Br* br = (Br*)block->getLastIns();
        if(br->hasCondition() && br->getUsedValue(1) == br->getUsedValue(2)) {
            br->handleSameTarget();
            BasicBlock* succ = (BasicBlock*)br->getUsedValue(0);
            block->replaceSuccessor(succ, nullptr);
            succ->replacePredecessor(block);
        }
    }
}
void MemToReg::removeUselessBlock(){
    vector<BasicBlock*>blocks = curFunc_->getBlocks();
    BasicBlock* entry = blocks[0];
    int flag = 1;
    do{
        flag = 1;
        for(int i = 0; i < blocks.size(); i++) {
            BasicBlock* block = blocks[i];
            if(block != entry && block->getPredecessors().size() == 0) {
                flag = 0;                               //说明当前仍有无前继的基本块，把它删了之后还得检查
                blocks.erase(blocks.begin() + i);
                curFunc_->removeBlock(block);
                i--;
                vector<BasicBlock*> succs = block->getSuccessors();
                for(int j = 0; j < succs.size(); j++) {
                    succs[j]->removePrede(block);
                }
            }
        }
    }while(flag == 0);
}
void MemToReg::buildDominate() {
    vector<BasicBlock*>blocks = curFunc_->getBlocks();
    unordered_set<BasicBlock*> initSets;
    for(int i = 0; i < blocks.size(); i++) {
        initSets.insert(blocks[i]);
    }
    for(int i = 0; i < blocks.size(); i++) {
        if(i == 0) {
            blocks[i]->addDomer(blocks[i]);
        } else {
            blocks[i]->setDomers(initSets);
        }
    }
    int flag = 1;
    do{
        flag = 1;
        for(int i = 1; i <blocks.size() ;i++) {
            vector<BasicBlock*> predes = blocks[i]->getPredecessors();
            unordered_set<BasicBlock*> set = unordered_set(predes[0]->getDomers());
            for(int j = 1; j < predes.size(); j++) {
                intersect(set, predes[j]->getDomers());
            }
            set.insert(blocks[i]);
            flag = flag & (blocks[i]->setDomers(set));                         //计算支配者，计算严格支配者
        }
    }while(flag == 0);
    for(int i = 1; i < blocks.size(); i++) {
        blocks[i]->setStrictDomers();
        blocks[i]->setImmDomer();                               //计算直接支配者
    }
}
void MemToReg::buildDF() {
    vector<BasicBlock*> blocks = curFunc_->getBlocks();
    for(int i = 0; i < blocks.size(); i++) {
        BasicBlock* curBlock = blocks[i];
        vector<BasicBlock*> successors = curBlock->getSuccessors();
        for(int j = 0; j < successors.size(); j++) {
            BasicBlock* succ = successors[j];
            BasicBlock* cur = curBlock;
            auto ff = succ->getStrictDomers().end();
            while((ff = succ->getStrictDomers().find(cur)) == succ->getStrictDomers().end()) {
                cur->getDF().insert(succ);
                cur = cur->getImmDomer();
            }
        }
    }
}
unordered_set <BasicBlock*>* MemToReg::calCDF() {
    unordered_set <BasicBlock*> rst;
    for(auto it : defineBlocks_) {
        unordered_set<BasicBlock*> df = it->getDF();
        rst.insert(df.begin(), df.end());
    }

    bool flag = false;
    do{
        flag = false;
        unordered_set <BasicBlock*> newRst = unordered_set(rst);
        for(auto it : rst) {
            unordered_set<BasicBlock*> df = it->getDF();
            newRst.insert(df.begin(), df.end());
        }
        if(newRst.size() != rst.size()) {
            flag = true;
            rst = unordered_set(newRst);
        }
    }while(flag);
    unordered_set <BasicBlock*>* retRst = new unordered_set(rst);
    return retRst;
}
void MemToReg::insertPhi(Alloca* alloca1, unordered_set<BasicBlock*>* CDF) {
    for(auto it : (*CDF)) {
        Phi* phi = irBuilder::getInstance()->buildPhi(it, (DataType*)alloca1->getAllocaType());
        phiToAlloca_[phi] = alloca1;
    }
}
void MemToReg::intersect(unordered_set<BasicBlock*>& s1, unordered_set<BasicBlock*>& s2){
    unordered_set <BasicBlock*> deletedSet;
    for(auto it : s1) {
        auto f = s2.find(it);
        if(f == s2.end()) {          //s2中没有这个元素，需要删除
            deletedSet.insert(it);
        }
    }
    for(auto it : deletedSet) {
        s1.erase(it);
    }
}
void MemToReg::renamePhi() {
    map<BasicBlock*, bool> visitMap;
    map<Alloca*, Value*> varMap;

    vector<BasicBlock*> blocks = curFunc_->getBlocks();
    for(auto it : blocks) {
        visitMap[it] = false;
    }

    for(auto it : allocas_) {
        varMap[it] = irBuilder::getInstance()->buildConstInt(0);
    }
    stack<pair<BasicBlock*, map<Alloca*, Value*>>> dfsStack;
    dfsStack.push(pair<BasicBlock*, map<Alloca*, Value*>>(entry_, varMap));
    while(dfsStack.size() != 0) {
        pair<BasicBlock*, map<Alloca*, Value*>> bm = dfsStack.top();
        dfsStack.pop();
        BasicBlock* curBlock = bm.first;
        varMap = bm.second;
        if(visitMap[curBlock]) {
            continue;
        }
        vector<Instruction*> intrs = curBlock->getInstructions();
        auto it = intrs.begin();
        while((*it)->getClassName() == PHICLASS) {
            varMap[phiToAlloca_[(Phi*)(*it)]] = (*it);
            it++;
        }
        while(it != intrs.end()) {
            Instruction* ins = *it;

            if(ins->getClassName() == LOADCLASS) {
                if(ins->getUsedValue(0)->getClassName() == ALLOCACLASS) {
                    ins->replaceSelf(varMap[(Alloca*)ins->getUsedValue(0)]);
                    ins->removeAllUsedValue();
                    curBlock->removeIns(ins);
                }
            } else if(ins->getClassName() == STORECLASS) {
                if(ins->getUsedValue(1)->getClassName() == ALLOCACLASS) {
                    varMap[(Alloca*)ins->getUsedValue(1)] = ins->getUsedValue(0);
                    ins->removeAllUsedValue();
                    curBlock->removeIns(ins);
                }
            }
            it++;
        }

        vector<BasicBlock*> succes = curBlock->getSuccessors();
        for(int i = 0; i < succes.size(); i++) {
            vector<Instruction*> intrs = succes[i]->getInstructions();
            auto it = intrs.begin();
            while((*it)->getClassName() == PHICLASS) {
                Phi* phi = (Phi*) (*it);
                Alloca* alloca1 = phiToAlloca_[phi];
                phi->addSource(varMap[alloca1], curBlock);
                it++;
            }
            if(!visitMap[succes[i]]) {
                dfsStack.push(pair<BasicBlock*, map<Alloca*, Value*>>(succes[i], map<Alloca*, Value*>(varMap)));
            }
        }

        visitMap[curBlock] = true;
    }

    
}