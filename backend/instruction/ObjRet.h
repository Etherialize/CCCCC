//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJRET_H
#define COMPLIER_MIPS_OBJRET_H
#define OBJRETCLASS "objRetClass"
#include "../component/ObjFunction.h"
#include "ObjInstruction.h"
class ObjRet : public ObjInstruction{
private:
    /*
     * 在函数开头我们提前分配了栈空间， 因此在return的时候需要弹栈并且将所保存的寄存器回复
     */
    ObjFunction* owner_;
public:
    ObjRet(ObjFunction* owner): owner_(owner){
    }
    string getClassName() override { return OBJRETCLASS;}
    string print() {
        string retStr = "";
        int stackSize = owner_->getStackSize();
        if(stackSize != 0) {
            retStr += "addi $sp,\t$sp,\t" + to_string(stackSize) + "\n";
        }
        if(owner_->getName() == "main") {        //主函数直接返回
            retStr += "\tli\t$v0,\t10\n";
            retStr += "\tsyscall\n";
        } else {                                //其他函数需要保存寄存器，现阶段只保存$ra
            vector<ObjReg*>& savedRegs = owner_->getSavedRegs();
            int offset = -4;
            for(auto reg : savedRegs) {
                retStr += "\tlw " + reg->print() + ",\t" + to_string(offset) + "($sp)\n";
                offset -= 4;
            }
            retStr += "\tjr $ra\n";
        }
        return retStr;
    }

};


#endif //COMPLIER_MIPS_OBJRET_H
