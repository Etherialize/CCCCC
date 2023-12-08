//
// Created by Han Zhengyang on 2023/11/1.
//

#include "Module.h"
#include "Function.h"
#include "GlobalVar.h"
#include "Types/typesHeader.h"
Module:: Module(int id): Value("Module", new ModuleType(), id, nullptr){

}
void Module:: addFunction(FunctionIR* function){
    functions_.push_back(function);
}
void Module:: addGlobalVar(GlobalVar* globalVar) {
    globalVars_.push_back(globalVar);
}

void Module::printModule(ofstream& output) {
    for(int i = 0; i < declares.size(); i++) {
        output << declares[i] << endl;
    }
    for(int i = 0; i < globalVars_.size(); i++) {
        output << globalVars_[i]->print() << endl;
    }

    for(int i = 0; i < functions_.size(); i++) {
        functions_[i]->printFunctionIR(output);
    }

}
vector<GlobalVar*> Module::getGlobalVars() {
    return globalVars_;
}
vector<FunctionIR*> Module::getFunctions() {
    return functions_;
}