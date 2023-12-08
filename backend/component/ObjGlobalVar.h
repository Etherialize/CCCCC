//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJGLOBALVAR_H
#define COMPLIER_MIPS_OBJGLOBALVAR_H

#include "componentPreDecl.h"

class ObjGlobalVar {
private:
    string name_;      //其name与源文件中的命名一样
    int size_;
    bool isInit_;
    bool isStr_;
    vector<int> elements_;
    string content_;
public:
    ObjGlobalVar(string name, vector<int> *elements);
    ObjGlobalVar(string name, int size);
    ObjGlobalVar(string name, string content);
    string getName(){
        return name_;
    }
    bool isInit(){
        return isInit_;
    }
    bool isStr() {
        return isStr_;
    }
    void printGlobalVar(ofstream& output);
};


#endif //COMPLIER_MIPS_OBJGLOBALVAR_H
