//
// Created by Han Zhengyang on 2023/11/3.
//

#ifndef COMPLIER_IR_DRIVER_H
#define COMPLIER_IR_DRIVER_H

#include <fstream>
#include "irPreDeclHeader.h"
#include "../Parser.h"
#include "Instructions/insPreDeclHeader.h"
#include "ConstVal/conHeader.h"
#include "m_optimize/MemToReg.h"
class Driver {
private:
    int strNum_ = 0;
private:
    const string strName_ = "_str_";
    ofstream &output_;
    vector<BasicBlock*> f2Blocks_;
    vector<BasicBlock*> falseBlocks_;
    SyntaxTree* AST_;
    BasicBlock* curBasicBlock_;
    FunctionIR* curFunc_;
    Module* module_;
    irSymTable* irSymTable_;
    irBuilder* irBuilder_;
public:
    Driver(SyntaxTree* AST, ofstream& output);
    Module* getModule();
    void run();
    int strNumPlus() {return strNum_++;}
    int getValue(SyntaxTree* node);
    int getConstLValValue(SyntaxTree* lVal);
    Value* visitLVal(SyntaxTree* lVal);
    ConstVal* getConInitVal(SyntaxTree* node);
    void ArrayInitVal(Alloca* addr,SyntaxTree* node, vector<int> dims, int* count);
    void visit(SyntaxTree* node);
    void visitCompUnit(SyntaxTree* node);
    void visitFuncDef(SyntaxTree* node);
    vector<DataType*> *visitFuncFParams(SyntaxTree* node, vector<string> *names);
    DataType* visitFuncFParam(SyntaxTree* node, string* name);
    void visitMainFuncDef(SyntaxTree* node);
    void visitVarDef(SyntaxTree* node);
    void visitConstDef(SyntaxTree* node);
    Value* visitExp(SyntaxTree* node);
    Value* visitFuncCall(SyntaxTree* node);
    void visitStmt(SyntaxTree* node);
    void visitAssignStmt(SyntaxTree* node);         //0
    void visitExpStmt(SyntaxTree* node);            //1
    void visitIfStmt(SyntaxTree* node);             //3
    void visitCond(SyntaxTree* node, BasicBlock* trueBlock, BasicBlock* falseBlock);
    void visitLOrExp(SyntaxTree* node, BasicBlock* trueBlock, BasicBlock* falseBlock);
    void visitLAndExp(SyntaxTree* node, BasicBlock* trueBlock, BasicBlock* falseBlock);
    Value* visitEqExp(SyntaxTree* node);
    Value* visitRelExp(SyntaxTree* node);
    void visitForStmt(SyntaxTree* node);            //4
    void visitBreakStmt(SyntaxTree* node);          //5
    void visitContinueStmt(SyntaxTree* node);       //6
    void visitReturnStmt(SyntaxTree* node);         //7
    void visitGetintStmt(SyntaxTree* node);         //8
    void visitPrintStmt(SyntaxTree* node);          //9
    vector<Value*>* visitFormatString(SyntaxTree* node);
    vector<string>* handleFormatString(string content);
    vector<Value*>* visitRParams(SyntaxTree* node);
};


#endif //COMPLIER_IR_DRIVER_H
