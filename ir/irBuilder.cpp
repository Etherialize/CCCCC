//
// Created by Han Zhengyang on 2023/11/3.
//

#include "irBuilder.h"
#include "ConstVal/ConstArray.h"
#include "ConstVal/ConstInt.h"
#include "ConstVal/ConstStr.h"
#include "ConstVal/ZeroInitializer.h"
#include "Instructions/insHeader.h"
#include "Types/typesHeader.h"
#include "Argument.h"
#include "BasicBlock.h"
#include "Function.h"
#include "GlobalVar.h"
#include "Module.h"
int irBuilder::nameNum_ = 0;
// Value id的虚拟寄存器
int irBuilder::id_ = 0;
// block counter
int irBuilder::blockNum_ = 0;
int irBuilder::phiNum_ = 0;
ConstStr* irBuilder::buildConstStr(string content){
    return new ConstStr(content, id_++);
}
ConstInt* irBuilder::buildConstInt(int value) {
    return new ConstInt(value, id_++);
}
ConstInt* irBuilder::buildConstInt(int bits, int value) {
    return new ConstInt(bits, value, id_++);
}
ConstArray* irBuilder::buildConstArray(vector<ConstVal*> array) {
    return new ConstArray(array, id_++);
}
ZeroInitializer* irBuilder::buildZero(ArrayType* arrayType) {
    return new ZeroInitializer(arrayType, id_++);
}
GlobalVar* irBuilder::buildGlobalVar(string name, ConstVal* initVal, bool isConst) {
    return new GlobalVar(name, initVal, id_++, isConst);
}
Argument* irBuilder::buildArgument(DataType* dataType) {
    return new Argument(nameNum_++, dataType, id_++);
}
FunctionIR* irBuilder::buildFunction(string name, FunctionType* functionType, vector<Argument*> arguments){
    return new FunctionIR(name, functionType, id_++, arguments);
}
Module* irBuilder::buildModule(){return new Module(id_++);}
BasicBlock* irBuilder::buildBlock(FunctionIR* Func){
    BasicBlock* basicBlock = new BasicBlock(blockNum_++, id_++, Func);
    Func->addBasicBlock(basicBlock);
    return basicBlock;
}
BasicBlock*irBuilder:: buildBlockNotAdd(FunctionIR* Func) {
    BasicBlock* basicBlock = new BasicBlock(blockNum_++, id_++, Func);
    return basicBlock;
}
Alloca * irBuilder::buildConstAlloca(BasicBlock* basicBlock, ValueType* valueType, ConstVal* constVal){
    Alloca* alloca1 = new Alloca(nameNum_++, (DataType*)valueType, id_++, basicBlock, constVal);
    basicBlock->addInstructionToHead(alloca1);
    return alloca1;
}
Alloca * irBuilder::buildAlloca(BasicBlock* basicBlock, ValueType* valueType){
    Alloca* alloca1 = new Alloca(nameNum_++, (DataType*)valueType, id_++, basicBlock);
    basicBlock->addInstructionToHead(alloca1);
    return alloca1;
}

Br* irBuilder::buildBr(BasicBlock* basicBlock, BasicBlock* target) {
    Br* br = new Br(id_++, basicBlock, target);
    basicBlock->addInstruction(br);
    return br;
}
Br* irBuilder::buildBr(BasicBlock* basicBlock, Value* cond, BasicBlock* target1, BasicBlock* target2) {
    Br* br = new Br(id_++, basicBlock, cond, target1, target2);
    basicBlock->addInstruction(br);
    return br;
}
Cmp* irBuilder:: buildCmp(BasicBlock* basicBlock, string condition, Value* op1, Value* op2) {
    Cmp* cmp = new Cmp(nameNum_++, id_++, basicBlock, condition, op1, op2);
    basicBlock->addInstruction(cmp);
    return cmp;
}

Load* irBuilder:: buildLoad(BasicBlock* basicBlock, Value* addr){
    Load* load = new Load(nameNum_++, id_++, basicBlock, addr);
    basicBlock->addInstruction(load);
    return load;
}
GetElePointer* irBuilder::buildGEP(BasicBlock* basicBlock, Value* base, vector<Value*> index, int flag){
    GetElePointer* getElePointer;
    if(index.size() == 0) {
        getElePointer = new GetElePointer(nameNum_++, id_++, basicBlock, base, flag);
    }
    else if(index.size() == 1) {
        getElePointer = new GetElePointer(nameNum_++, id_++, basicBlock, base, index[0], flag);
    } else {
        getElePointer = new GetElePointer(nameNum_++, id_++, basicBlock, base, index[0], index[1], flag);
    }
    basicBlock->addInstruction(getElePointer);
    return getElePointer;
}
Store* irBuilder::buildStore(BasicBlock* basicBlock, Value* data, Value* addr){
    Store* store = new Store(id_++, basicBlock, data, addr);
    basicBlock->addInstruction(store);
    return store;
}

ArithmeticIns* irBuilder::buildArith(BasicBlock* basicBlock, string op, Value* l, Value* r) {
    ArithmeticIns* arithmeticIns = new ArithmeticIns(nameNum_++, new IntType(), id_++, basicBlock, op, l, r);
    basicBlock->addInstruction(arithmeticIns);
    return arithmeticIns;
}
FuncCall* irBuilder::buildFuncCall(BasicBlock* basicBlock, DataType* returnType, string funcName, FunctionIR* target, vector<Value*>* args) {
    FuncCall* funcCall = new FuncCall(nameNum_++, returnType, id_++, basicBlock, funcName, target, args);
    basicBlock->addInstruction(funcCall);
    return funcCall;
}
void irBuilder::buildReturn(BasicBlock* basicBlock, DataType* returnType, Value* rst) {
    Return* ret = new Return(returnType, id_++, basicBlock, rst);
    basicBlock->addInstruction(ret);
}
Zext* irBuilder:: buildZext(BasicBlock* basicBlock, Value* op1) {
    Zext* zext = new Zext(nameNum_++, id_++, basicBlock, op1);
    basicBlock->addInstruction(zext);
    return zext;
}
Phi* irBuilder::buildPhi(BasicBlock* basicBlock, DataType* dataType){
    Phi* phi = new Phi(phiNum_++, dataType, id_++, basicBlock, basicBlock->getPredecessors().size());
    basicBlock->addInstructionToHead(phi);
    return phi;
}
Phi* irBuilder::buildPhi(BasicBlock* basicBlock, DataType* dataType, int predNum){
    Phi* phi = new Phi(phiNum_++, dataType, id_++, basicBlock, predNum);
    basicBlock->addInstructionToHead(phi);
    return phi;
}