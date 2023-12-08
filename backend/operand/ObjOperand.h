//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJOPERAND_H
#define COMPLIER_MIPS_OBJOPERAND_H

#include <typeinfo>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <string>
using namespace std;
class ObjOperand {
public:
    virtual string getClassName() const {
        return "ObjOperandClass";
    }
    virtual string print() {
        return "ObjOperand.print";
    }
    virtual bool isReg() {
        return false;
    }
    virtual bool isPreColored(){return false;}
    virtual bool needsColor(){return false;}
    virtual bool isAllocated(){return false;}
};


#endif //COMPLIER_MIPS_OBJOPERAND_H
