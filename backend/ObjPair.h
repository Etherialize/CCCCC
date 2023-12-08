//
// Created by Han Zhengyang on 2023/12/5.
//

#ifndef COMPLIER_MIPS_OBJPAIR_H
#define COMPLIER_MIPS_OBJPAIR_H

#include "operand/ObjReg.h"

class ObjPair {
private:
    ObjReg* a;
    ObjReg* b;
public:
    ObjPair(ObjReg* r1, ObjReg*r2):a(r1), b(r2){}
    ObjReg* getA() const{return a;}
    ObjReg* getB() const{return b;}
};

class ObjPair_hash {
public:
    size_t operator()(const ObjPair* rc) const {
        return hash<string>()(rc->getA()->getName())+ hash<string>()(rc->getB()->getName());
    }
};
class ObjPair_cmp {
public:
    bool operator ()(const ObjPair* rc1, const ObjPair* rc2) const {
        return (rc1->getA()->getName() == rc2->getA()->getName()) && (rc1->getB()->getName() == rc2->getB()->getName());
    }
};


#endif //COMPLIER_MIPS_OBJPAIR_H
