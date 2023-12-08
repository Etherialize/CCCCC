//
// Created by Han Zhengyang on 2023/11/25.
//

#ifndef COMPLIER_MIPS_MIPSDRIVER_H
#define COMPLIER_MIPS_MIPSDRIVER_H

#include "RegAlloc.h"
#include "GraphColor.h"
#include "Peephole.h"
class MipsDriver {
private:
    irParser* IRparser_;
    RegAlloc* regAlloc_;
    Module* irModule_;
    ObjModule* objModule_;
    ofstream &output_;
public:
    MipsDriver(Module* irModule, ofstream& output): irModule_(irModule),output_(output){
        run();
    }
    void run() {
        IRparser_ = new irParser(irModule_);
        objModule_ = IRparser_->getObjModule();
        //objModule_->printModule(output_);
        //output_<<"\n\n\n\n\n\n\n";
        //regAlloc_ = new RegAlloc(objModule_);
        GraphColor graphColor = GraphColor(objModule_);
        Peephole peephole = Peephole(objModule_);
        objModule_->printModule(output_);
    }
};


#endif //COMPLIER_MIPS_MIPSDRIVER_H
