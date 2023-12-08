//
// Created by Han Zhengyang on 2023/11/1.
//

#include "Function.h"
#include "BasicBlock.h"
#include "Argument.h"
#include "Types/typesHeader.h"
#include "Instructions/Br.h"
#include "irBuilder.h"
FunctionIR:: FunctionIR(string name, FunctionType* functionType, int id,vector<Argument*> arguments):Value("@" + name, functionType, id,
                                                                                                                         nullptr), arguments_(arguments) {
}
string FunctionIR:: getClassName() {
    return FUNCTIONCLASS;
}
DataType* FunctionIR::getRetType() {
    return ((FunctionType*)getValType()) ->getReturnType();
}
void FunctionIR::addBasicBlock(BasicBlock* basicBlock){         //唯一在Driver类之外加入指令
    if(basicBlocks_.size() != 0) {
        BasicBlock* cur = basicBlocks_[basicBlocks_.size() - 1];
        if(cur->getInsSum() == 0) {
            Br* br1 = irBuilder::getInstance()->buildBr(cur, basicBlock);
            cur->addInstruction(br1);
        } else {
            Instruction* last = cur->getLastIns();
            Br* br = dynamic_cast<Br*>(last);
            if(br == nullptr) {
                Br* br1 = irBuilder::getInstance()->buildBr(cur, basicBlock);
                cur->addInstruction(br1);
            }
        }
    }
    basicBlocks_.push_back(basicBlock);
}

vector<BasicBlock*>& FunctionIR::getBlocks() {
    return basicBlocks_;
}
void FunctionIR::addArgument(Argument* argument) {
    arguments_.push_back(argument);
}
int FunctionIR::getArgsSum() {
    return arguments_.size();
}
void FunctionIR::printFunctionIR(ofstream & output) {
    string funcDef = "define dso_local " + getValType()->print() + " " + getName() + "(";
    for(int i = 0 ; i <arguments_.size(); i++) {
        funcDef += arguments_[i]->print() + ", ";
    }
    if(arguments_.size() != 0) {
        funcDef.erase(funcDef.length() - 2);
    }
    funcDef += "){";
    output << funcDef << endl;


    for(int i = 0; i < basicBlocks_.size(); i++) {
        basicBlocks_[i]->printBasicBlock(output);
    }
    output << "}" <<endl;
}
void FunctionIR::checkRet() {
    BasicBlock* lastBlock = basicBlocks_[basicBlocks_.size() - 1];
    lastBlock->checkRet();
}
BasicBlock* FunctionIR::getEntry(){
    return basicBlocks_[0];
}
void FunctionIR::removeBlock(BasicBlock* basicBlock) {
    for(int i = 0; i < basicBlocks_.size(); i++) {
        if(basicBlock == basicBlocks_[i]) {
            basicBlocks_.erase(basicBlocks_.begin() + i);
            return;
        }
    }
}