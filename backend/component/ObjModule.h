//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJMODULE_H
#define COMPLIER_MIPS_OBJMODULE_H

#include "componentPreDecl.h"

class ObjModule {
private:
    vector<ObjFunction*> functions;
    vector<ObjGlobalVar*> globalVars;
    ObjFunction* mainFunction_;
public:
    void addFunction(ObjFunction* function);
    void addGlobalVar(ObjGlobalVar* globalVar);
    vector<ObjGlobalVar*> getGlobalVars();
    vector<ObjFunction*> getFuncs();
    void setMainFunc(ObjFunction* mainFunc);
    void printModule(ofstream& output);
};


#endif //COMPLIER_MIPS_OBJMODULE_H
