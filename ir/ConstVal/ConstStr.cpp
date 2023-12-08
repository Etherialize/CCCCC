//
// Created by Han Zhengyang on 2023/11/7.
//

#include "ConstStr.h"
#include "../Types/typesHeader.h"
ConstStr::ConstStr(string content, int id):ConstVal(new ArrayType(new IntType(8), content.length()), id), content_(content){
    int length = content.length() + 1;    //1 is the size of \00
    int cur = 0;
    int index;
    while((index = content.find( "\\0a", cur)) != -1) {
        length -= 2;
        cur = index + 3;
    }
    ArrayType* arrayType = new ArrayType(new IntType(8), length);
    setVatType(arrayType);
}
string ConstStr::print() {
    return "c\"" + content_ + "\\00\"";
}
string ConstStr::getClassName() {
    return CONSTSTRCLASS;
}
string ConstStr::getContent() {
    return content_;
}