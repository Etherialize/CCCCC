//
// Created by Han Zhengyang on 2023/11/1.
//
#ifndef COMPLIER_IR_FUNCTION_H
#define COMPLIER_IR_FUNCTION_H
#include "irPreDeclHeader.h"
#include "User.h"
#include "Instructions/insPreDeclHeader.h"
#define FUNCTIONCLASS "FunctionClass"
class FunctionIR: public Value{
private:
    vector<Argument*> arguments_;
    vector<BasicBlock*> basicBlocks_;
public:
    /*
     * 懒了，犯不着给Function加parent，不管了不管了
     */
    FunctionIR(string name, FunctionType* functionType, int id, vector<Argument*> arguments);
    string getClassName() override ;
    DataType* getRetType();
    void printFunctionIR(ofstream & output);
    void addBasicBlock(BasicBlock* basicBlock);
    vector<BasicBlock*>& getBlocks();                           //改成引用的时候别忘记看一下MemToReg中removeUselessBlock
    void addArgument(Argument* argument);
    int getArgsSum();
    void checkRet();
    BasicBlock* getEntry();
    void removeBlock(BasicBlock* basicBlock);
};

#endif