//
// Created by Han Zhengyang on 2023/11/23.
//

#include "RegAlloc.h"
RegAlloc::RegAlloc(ObjModule* objModule):objModule_(objModule){
    virToPhy();
}

void RegAlloc::virToPhy() {
    vector<ObjFunction*> objFuncs = objModule_->getFuncs();
    for(int i = 0; i < objFuncs.size(); i++) {
        virToPhy(objFuncs[i]);
    }
}
void RegAlloc::virToPhy(ObjFunction *curFunc) {
    objCurFunction_ = curFunc;
    virToMem_.clear();
    vector<ObjVirReg*> curFuncUsedVirs = objCurFunction_->getUsedVirRegs();
    for(int i = 0; i < curFuncUsedVirs.size(); i++) {
        ObjVirReg* virReg = curFuncUsedVirs[i];
        ObjImm* objImm = new ObjImm(objCurFunction_->getAllocaSize());
        objCurFunction_->addAllocaSize(4);
        virToMem_[virReg] = objImm;
    }

    vector<ObjBlock*> objBlocks = objCurFunction_->getBlocks();
    for(int i = 0; i < objBlocks.size(); i++) {
        objCurBlock_ = objBlocks[i];
        curInts_ = objCurBlock_->getInstsPointer();
        int index = 0;
        while(index < curInts_->size()) {
            int regSum = virToPhy((*curInts_)[index], index);
            index += regSum + 1;
        }
    }
}
int RegAlloc::virToPhy(ObjInstruction* objInst, int index) {
    int t_s = 8;
    int useCnt = 0;
    int defCnt = 0;
    unordered_set<ObjReg*> useMap = objInst->getUse();
    unordered_set<ObjReg*> defMap = objInst->getDef();
    for(auto it : useMap) {
        if(it->getClassName() == OBJVIRREGCLASS) {
            ObjImm* objImm = virToMem_[(ObjVirReg*)it];
            ObjPhyReg* objPhyReg = new ObjPhyReg(t_s);
            ObjLoad* load = new ObjLoad(objPhyReg, ObjPhyReg::SP(), objImm);
            objInst->replaceUse(it, objPhyReg);
            curInts_->insert(curInts_->begin() + index,load);
            t_s++;
            index++;
            useCnt++;
        }
    }
    for(auto it : defMap) {
        if(it->getClassName() == OBJVIRREGCLASS) {
            ObjImm* objImm = virToMem_[(ObjVirReg*)it];
            ObjPhyReg* objPhyReg = new ObjPhyReg(t_s);
            ObjStore* objStore = new ObjStore(objPhyReg, ObjPhyReg::SP(), objImm);
            objInst->replaceReg(it, objPhyReg);
            curInts_->insert(curInts_->begin() + index + 1 ,objStore);
            t_s++;
            index++;
            defCnt++;
        }
    }
    return defCnt + useCnt;
}