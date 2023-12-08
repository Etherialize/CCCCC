//
// Created by Han Zhengyang on 2023/11/20.
//

#include "ObjModule.h"
#include "ObjGlobalVar.h"
#include "ObjFunction.h"
#include "componentHeader.h"
void ObjModule::addFunction(ObjFunction* function) {
    functions.push_back(function);
}
void ObjModule::addGlobalVar(ObjGlobalVar* globalVar) {
    globalVars.push_back(globalVar);
}
void ObjModule::setMainFunc(ObjFunction *mainFunc) {
    mainFunction_ = mainFunc;
}
vector<ObjGlobalVar*> ObjModule::getGlobalVars(){
    return globalVars;
}
vector<ObjFunction*> ObjModule::getFuncs(){
    return functions;
}
void ObjModule::printModule(ofstream& output) {
    output << ".macro putstr" << endl;
    output << "\tli\t$v0,\t4" << endl;
    output << "\tsyscall" << endl;
    output << ".end_macro" << endl;

    output << ".macro putint" << endl;
    output << "\tli\t$v0,\t1" << endl;
    output << "\tsyscall" << endl;
    output << ".end_macro" << endl;

    output << ".macro getint" << endl;
    output << "\tli\t$v0,\t5" << endl;
    output << "\tsyscall" << endl;
    output << ".end_macro" << endl;

    output << ".data" << endl;

    //.word
    for(int i = 0; i < globalVars.size(); i++) {
        ObjGlobalVar* objGlobalVar = globalVars[i];
        if(!objGlobalVar->isStr() && objGlobalVar->isInit()) {
            objGlobalVar->printGlobalVar(output);
        }
    }

    //.space
    for(int i = 0; i < globalVars.size(); i++) {
        ObjGlobalVar* objGlobalVar = globalVars[i];
        if(!objGlobalVar->isStr() && !objGlobalVar->isInit()) {
            objGlobalVar->printGlobalVar(output);
        }
    }

    //.asciiz

    for(int i = 0; i < globalVars.size(); i++) {
        ObjGlobalVar* objGlobalVar = globalVars[i];
        if(objGlobalVar->isStr() && objGlobalVar->isInit()) {
            objGlobalVar->printGlobalVar(output);
        }
    }

    output << ".text" << endl;

    mainFunction_->printObjFunc(output);
    for(int i = 0; i < functions.size(); i++) {
        if(functions[i]->getName() != "main") {
            output << "\n\n\n\n\n" << endl;
            functions[i]->printObjFunc(output);
        }
    }
}