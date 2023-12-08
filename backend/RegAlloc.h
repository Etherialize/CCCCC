//
// Created by Han Zhengyang on 2023/11/23.
//

#ifndef COMPLIER_MIPS_REGALLOC_H
#define COMPLIER_MIPS_REGALLOC_H

#include "irParser.h"
class RegAlloc {
private:
    ObjModule* objModule_;
    ObjFunction* objCurFunction_;
    ObjBlock* objCurBlock_;
    vector<ObjInstruction*>* curInts_;
    /*
     * 每个函数一个映射关系
     */
    map<ObjVirReg*, ObjImm*> virToMem_;
    map<ObjVirReg*, int> virToPhy_;
    map<int, ObjVirReg*> phyToVir_;
    vector<int> canAllocaPhy_ = ObjPhyReg::allocableRegs_;
public:
    RegAlloc(ObjModule* objModule);
    void virToPhy();
    void virToPhy(ObjFunction* curFunc);
    int virToPhy(ObjInstruction* objInst, int index);
};


#endif //COMPLIER_MIPS_REGALLOC_H
