//
// Created by Han Zhengyang on 2023/11/20.
//

#include "ObjGlobalVar.h"

ObjGlobalVar:: ObjGlobalVar(string name, vector<int> *elements): elements_(*elements) {
    name_ = name.substr(1, name.size() - 1);
    size_ = 4 * elements->size();
    isInit_ = true;
    isStr_ = false;
    content_ = "";
}
ObjGlobalVar:: ObjGlobalVar(string name, int size) {
    name_ = name.substr(1, name.size() - 1);
    size_ = size;
    isInit_ = false;
    isStr_ = false;
    content_ = "";
}
ObjGlobalVar:: ObjGlobalVar(string name, string content){
    int pos;
    while((pos = content.find("\\0a")) != -1) {
        content.replace(pos, 3, "\\n");
    }
    content_ = content;
    name_ = name.substr(1, name.size() - 1);
    size_ = content_.size();
    isInit_ = true;
    isStr_ = true;
}
void ObjGlobalVar:: printGlobalVar(ofstream& output) {
    output << name_ + ":" <<endl;
    if(isInit_) {
        if(isStr_){
            output << ".asciiz\t\"" + content_ + "\"" <<endl;
        } else {
            for(int i = 0; i < elements_.size(); i++) {
                output << ".word\t" + to_string(elements_[i]) <<endl;
            }
        }
    } else {
        output << ".space\t" + to_string(size_) << endl;
    }
}