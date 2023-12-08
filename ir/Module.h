//
// Created by Han Zhengyang on 2023/11/1.
//
#ifndef COMPLIER_IR_MODULE_H
#define COMPLIER_IR_MODULE_H
#include "irPreDeclHeader.h"
#include "User.h"
#include "irPreDeclHeader.h"
#define MODULECLASS "ModuleClass"
class Module : public Value{
private:
    vector<string> declares = {"declare i32 @getint()","declare void @putint(i32)"
                               ,"declare void @putch(i32)","declare void @putstr(i8*)"};
public:
    vector<FunctionIR*> functions_;
    vector<GlobalVar*> globalVars_;
public:
    Module(int id);
    string getClassName(){
        return MODULECLASS;
    }
    void addFunction(FunctionIR* function);
    void addGlobalVar(GlobalVar* globalVar);
    void printModule(ofstream & output);
    vector<GlobalVar*> getGlobalVars();
    vector<FunctionIR*> getFunctions();
};

#endif