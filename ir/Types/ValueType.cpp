//
// Created by Han Zhengyang on 2023/10/31.
//

#include "ValueType.h"
string LabelType:: toString() {
    return LABELTYPE;
}
int LabelType:: getSize(){
    throw runtime_error("getSize of Label");
}

string ModuleType:: toString(){
    return MODULETYPE;
}
int ModuleType:: getSize() {
    throw runtime_error("getSize of Module");
}