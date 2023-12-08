//
// Created by Han Zhengyang on 2023/11/3.
//

#ifndef COMPLIER_IR_IRSYMTABLE_H
#define COMPLIER_IR_IRSYMTABLE_H

#include "irPreDeclHeader.h"
class irSymTable {
private:
    vector<map<string, Value*>> symTables_;
    bool isEnter;
public:
    irSymTable();
    void pushFunc();
    void popTable();
    void pushBlock();
    map<string, Value*> &getNowTable();
    map<string, Value*> &getGlobalTable();
    void addValue(string ident, Value* value);
    Value* find(string ident, bool isFunc);
    bool isGlobal();

};


#endif //COMPLIER_IR_IRSYMTABLE_H
