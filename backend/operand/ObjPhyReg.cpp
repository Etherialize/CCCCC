//
// Created by Han Zhengyang on 2023/11/24.
//
#include "ObjPhyReg.h"
map <int, string> ObjPhyReg:: indexToName_;
map<string, int> ObjPhyReg:: nameToIndex_;
vector<int> ObjPhyReg:: calleeSavedRegs_;
unordered_set <int> ObjPhyReg::savedRegs_;
vector<int> ObjPhyReg:: allocableRegs_;
