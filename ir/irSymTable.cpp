//
// Created by Han Zhengyang on 2023/11/3.
//

#include "irSymTable.h"
irSymTable::irSymTable(){
    isEnter = false;
    symTables_.push_back(map<string, Value*> ());
}
void irSymTable:: pushFunc() {
    map<string, Value*> newFuncTable;
    symTables_.push_back(newFuncTable);
    isEnter = true;
}
void irSymTable::pushBlock() {
    if(isEnter) {
        isEnter = false;
        return;
    } else {
        map<string, Value*> newBlockTable;
        symTables_.push_back(newBlockTable);
        return;
    }
}
void irSymTable::popTable() {
    symTables_.pop_back();
}

map<string, Value*> &irSymTable::getNowTable(){
    return symTables_[symTables_.size() - 1];
}

map<string, Value*> &irSymTable:: getGlobalTable() {
    return symTables_[0];
}
void irSymTable::addValue(string ident, Value* value) {
    getNowTable()[ident] = value;
}
Value* irSymTable::find(string ident, bool isFunc) {
    if(isFunc) {
        map<string, Value*> globalTable = getGlobalTable();
        if(globalTable.find(ident) == globalTable.end()) {
            return NULL;
        } else {
            return globalTable[ident];
        }
    } else {
        for(int i = symTables_.size() - 1; i >= 0; i--) {
            if(symTables_[i].find(ident) != symTables_[i].end()) {
                return symTables_[i][ident];
            }
        }
        return NULL;
    }
}
bool irSymTable:: isGlobal() {
    return symTables_.size() == 1;
}