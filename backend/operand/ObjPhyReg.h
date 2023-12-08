//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_OBJPHYREG_H
#define COMPLIER_MIPS_OBJPHYREG_H
#define OBJPHYREGCLASS "objPhyRegClass"
#include "ObjReg.h"
#include "operHeader.h"
class ObjPhyReg :public ObjReg{
private:
    int index_;
    bool isAllocated_;
public:
    static map<int, string> indexToName_;
    static map<string, int> nameToIndex_;
    static vector<int> calleeSavedRegs_;
    static unordered_set <int> savedRegs_;
    static vector<int> allocableRegs_;
    static ObjPhyReg* ZERO() {return new ObjPhyReg("zero");}
    static ObjPhyReg* AT() {return new ObjPhyReg("at");};
    static ObjPhyReg* V0() {return new ObjPhyReg("v0");}
    static ObjPhyReg* A0() {return new ObjPhyReg("a0");}
    static ObjPhyReg* A1() {return new ObjPhyReg("a1");}
    static ObjPhyReg* A2() {return new ObjPhyReg("a2");}
    static ObjPhyReg* A3() {return new ObjPhyReg("a3");}
    static ObjPhyReg* SP() {return new ObjPhyReg("sp");}
    static ObjPhyReg* RA() {return new ObjPhyReg("ra");}
    ObjPhyReg(int index){
        index_ = index;
        name_ = indexToName_[index_];
        isAllocated_ = false;
    }
    ObjPhyReg(int index, bool isAllocated){
        index_ = index;
        name_ = indexToName_[index_];
        isAllocated_ = isAllocated;
    }
    string getClassName() const override {
        return OBJPHYREGCLASS;
    }
    ObjPhyReg(string name){
        name_ = name;
        index_ = nameToIndex_[name_];
    }
    int getIndex() {
        return index_;
    }
    string print() override {
        return "$" + name_;
    }
    bool isReg() override {
        return true;
    }
    bool isAllocated() override {return isAllocated_;}
    bool needsColor() override { return !isAllocated_;}
    bool isPreColored() override {return !isAllocated_;}
    static void init(){
        // 只读 不写 不分配
        indexToName_[0] = "zero";
        // 不能使用，只能做临时寄存器
        indexToName_[1] = "at";
        //返回值
        indexToName_[2] = "v0";
        //参与分配和保存
        indexToName_[3] = "v1";
        //传参寄存器，不用保存
        indexToName_[4] = "a0";
        indexToName_[5] = "a1";
        indexToName_[6] = "a2";
        indexToName_[7] = "a3";
        // 8 - 25 被调用者保存，参与分配
        indexToName_[8] = "t0";
        indexToName_[9] = "t1";
        indexToName_[10] = "t2";
        indexToName_[11] = "t3";
        indexToName_[12] = "t4";
        indexToName_[13] = "t5";
        indexToName_[14] = "t6";
        indexToName_[15] = "t7";
        indexToName_[16] = "s0";
        indexToName_[17] = "s1";
        indexToName_[18] = "s2";
        indexToName_[19] = "s3";
        indexToName_[20] = "s4";
        indexToName_[21] = "s5";
        indexToName_[22] = "s6";
        indexToName_[23] = "s7";
        indexToName_[24] = "t8";
        indexToName_[25] = "t9";
        // OS使用，我们将其作为普通寄存器使用
        indexToName_[26] = "k0";
        indexToName_[27] = "k1";
        // fp， gp作为普通寄存器
        indexToName_[28] = "gp";
        indexToName_[29] = "sp";
        indexToName_[30] = "fp";

        //返回地址 调用者保存
        indexToName_[31] = "ra";

        for(auto it = indexToName_.begin(); it != indexToName_.end(); ++it) {
            nameToIndex_[it->second] = it->first;
        }

        //调用方保存
        calleeSavedRegs_.push_back(3);
        savedRegs_.insert(3);
        for(int i = 4; i < 28; i++) {
            calleeSavedRegs_.push_back(i);
            savedRegs_.insert(i);
        }
        calleeSavedRegs_.push_back(30);
        savedRegs_.insert(30);
        calleeSavedRegs_.push_back(31);
        savedRegs_.insert(31);
        //zero, at, v0, sp, gp, ra不参与分配
        for(int i = 3; i < 28; i++) {
            allocableRegs_.push_back(i);
        }
        allocableRegs_.push_back(30);
        allocableRegs_.push_back(31);

        //初始化一些寄存器
    }
};


#endif //COMPLIER_MIPS_OBJPHYREG_H
