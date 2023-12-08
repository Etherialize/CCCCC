//
// Created by Han Zhengyang on 2023/10/7.
//
#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "Header.h"

#define ILLEGAL_STRING 'a'
#define REDEFINE 'b'
#define UNDEFINE 'c'
#define NUMOFPARA 'd'
#define TYPEOFPARA 'e'
#define ERRORRETURN 'f'
#define NORETURN 'g'
#define ASSIGNCONST 'h'
#define MISS_SEMICN 'i'
#define MISS_RPARENT 'j'
#define MISS_RBRACK 'k'
#define NOTMATCHPRINTF 'l'
#define NOTLOOP 'm'
using namespace std;

class ErrorMsg {
public:
    int line_;
    char error_code_;
public:
    ErrorMsg(int line, char error_code);
    string msg();
};
class ErrorHandler {
public:
    ofstream &output_;
    vector<ErrorMsg> errorMsgs_;
    vector<SymTable> symTables_;
    int cur_ = -1;
    int loop_ = 0;
public:
    ErrorHandler(ofstream &output);
    void errorMsg(string msg);
    void addError(int line, char error_code);
    void errorHandle(SyntaxTree* node);
    void checkError(SyntaxTree* node);
    void checkVarDef(SyntaxTree* node);
    void checkFuncDef(SyntaxTree* node);
    void checkMainFuncDef(SyntaxTree* node);
    void checkFuncFParam(SyntaxTree* node);
    void checkBlock(SyntaxTree* node);
    void checkStmt(SyntaxTree* node);
    void checkConstAssign(SyntaxTree* node);
    void checkReturn(SyntaxTree* node);
    void checkPrint(SyntaxTree* node);
    void checkUndefine(SyntaxTree* node);
    int getDim(SyntaxTree* node);
    void newScope();
};




