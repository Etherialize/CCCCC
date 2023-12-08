//
// Created by Han Zhengyang on 2023/10/14.
//
#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
#include "Header.h"
#define INTNUM 0
#define INTFUNC 1
#define VOID 2
#define NORMAL_SCOPE 0
#define FUNC_SCOPE 1
#define UNDEFINEDIM -2
#define VOIDDIM -1
class TableItem {
public:
    int id_;
    string ident_;
    int val_;
    int dim_;
    bool error_ = false;
    //int | void
    int type_;

public:
    TableItem(string ident, int type, int dim);
    TableItem(string ident, int type, int dim, bool error);
    void setVal(int value);
    int getVal();
    int getDim();
    string getIdent();

};
class Var : public TableItem {
public:
    int isConst_;
    bool isLoad_ = false;
public:
    Var(string ident, int type, int isConst, int dim);
    Var(Var* old);
    bool getIsLoad();
    void setIsLoad(bool isLoad);
};
class Function : public TableItem {
public:
    int sumOfParas_;
    vector<int> dims_;
    int funcTableId_;
public:
    Function(string ident, int type, vector<int> & dims, int dim, int sum);
    Function(Function* old);
};
class Exps : public TableItem {
public:
    Exps();
};
class SymTable {
public:
    int id_;
    int father_;
    //Function scope
    int type_ = 0;
    bool isVoid_ = true;
    int parasSum_ = 0;

    vector<int> children_;
    vector<TableItem *> items_;
public:
    SymTable(int id, int father);
    bool isRedefine(string ident);
    void addItem(TableItem* tableItem) {
        items_.push_back(tableItem);
    }
    void delItem(string ident);
    bool isContainIdent(string ident, string className);
    int indexOf(string ident, int type);
};
