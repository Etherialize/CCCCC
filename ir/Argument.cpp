//
// Created by Han Zhengyang on 2023/11/1.
//

#include "Argument.h"
#include "Function.h"
Argument::Argument(int nameNum, DataType* dataType, int id):Value("%a" + to_string(nameNum), dataType, id,
                                                                                      nullptr), rank_(nameNum){

}

string Argument::print() {
    string sb = getValType()->print() + " " + getName();
    return sb;
}