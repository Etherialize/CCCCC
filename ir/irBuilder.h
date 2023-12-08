//
// Created by Han Zhengyang on 2023/11/3.
//

#ifndef COMPLIER_IR_IRBUILDER_H
#define COMPLIER_IR_IRBUILDER_H

#include "Instructions/insPreDeclHeader.h"
#include "Types/preDeclTypesHeader.h"
#include "irPreDeclHeader.h"
#include "ConstVal/constPreDecl.h"

class irBuilder {
private:
    // 虚拟寄存器的计数器，进入新的函数会归0
    static int nameNum_;
    // Value id的虚拟寄存器
    static int id_;
    // block counter
    static int blockNum_;
    //phi counter
    static int phiNum_;

public:
    static irBuilder* getInstance() {
        static irBuilder* instance = new irBuilder();
        return instance;
    }
    //初始化局部常量数组
    void resetNameNum(){nameNum_ = 0;}
    void resetBlockNum(){blockNum_ = 0;}
    ConstStr* buildConstStr(string content);
    ConstInt* buildConstInt(int value);
    ConstInt* buildConstInt(int bits, int value);
    ConstArray* buildConstArray(vector<ConstVal*> array);
    ZeroInitializer* buildZero(ArrayType* arrayType);
    GlobalVar* buildGlobalVar(string name, ConstVal* initVal, bool isConst);
    Argument* buildArgument(DataType* dataType);
    FunctionIR* buildFunction(string name, FunctionType* functionType, vector<Argument*> arguments);
    Module* buildModule();
    BasicBlock* buildBlock(FunctionIR* Func);
    BasicBlock* buildBlockNotAdd(FunctionIR* Func);
    Alloca* buildConstAlloca(BasicBlock* basicBlock, ValueType* valueType, ConstVal* constVal);
    Alloca* buildAlloca(BasicBlock* basicBlock, ValueType* valueType);
    Br* buildBr(BasicBlock* basicBlock, BasicBlock* target);
    Br* buildBr(BasicBlock* basicBlock, Value* cond, BasicBlock* target1, BasicBlock* target2);
    Cmp* buildCmp(BasicBlock* basicBlock, string condition, Value* op1, Value* op2);
    Store* buildStore(BasicBlock* basicBlock, Value* data, Value* addr);
    Load* buildLoad(BasicBlock* basicBlock, Value* addr);
    GetElePointer* buildGEP(BasicBlock* basicBlock, Value* base, vector<Value*> index, int flag);
    ArithmeticIns* buildArith(BasicBlock* basicBlock, string op, Value* l, Value* r);
    FuncCall* buildFuncCall(BasicBlock* basicBlock, DataType* returnType, string funcName, FunctionIR* target, vector<Value*>* args);
    Zext* buildZext(BasicBlock* basicBlock, Value* op1);
    void buildReturn(BasicBlock* basicBlock, DataType* returnType, Value* rst);
    Phi* buildPhi(BasicBlock* basicBlock, DataType* dataType);
    Phi* buildPhi(BasicBlock* basicBlock, DataType* dataType, int predNum);
};


#endif //COMPLIER_IR_IRBUILDER_H
