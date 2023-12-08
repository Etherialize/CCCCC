//
// Created by Han Zhengyang on 2023/11/20.
//

#ifndef COMPLIER_MIPS_IRPARSER_H
#define COMPLIER_MIPS_IRPARSER_H

#include "../ir/irHeader.h"
#include "../ir/ConstVal/conHeader.h"
#include "instruction/ObjInsHeader.h"
#include "component/componentHeader.h"
#include "operand/operHeader.h"
#include <unordered_map>
#include <stack>
#include <algorithm>
class irParser {

private:
    Module* irModule_;
    ObjModule* objModule_;
    ObjFunction* objCurFunction_;
    ObjBlock* objEntry_;
    ObjBlock* objCurBlock_;
    map<GlobalVar*, ObjGlobalVar*> globalMap_;
    map<BasicBlock*, ObjBlock*> blockMap_;
    map<FunctionIR*, ObjFunction*> functionMap_;
    //Value 和 VirReg的映射关系
    map<Value*, ObjOperand*> operandMap_;
public:
    irParser(Module* irModule);
    ObjModule* getObjModule();
    void parseModule();
    void parseGlobalVars();
    ObjGlobalVar* parseGlobalVar(GlobalVar* globalVar);
    void parseFunctions();
    void irToObjMap();
    void parseFunction(FunctionIR* function);
    void parseBasicBlock(BasicBlock* basicBlock);
    void parseInstruction(Instruction* instruction);
    void parseAlloca(Alloca* alloca1);
    void parseArithmeticIns(ArithmeticIns* arithmeticIns);
    void parseMulAndDiv(ArithmeticIns* arithmeticIns);
    void parseBr(Br* br);
    ObjCondType parseCondition(Cmp* cmp);
    void parseFuncCall(FuncCall* funcCall);
    void parseSyscall(string funcName, FuncCall* funcCall);
    void parseGetElePointer(GetElePointer* getElePointer);
    void parseLoad(Load* load);
    void parseReturn(Return* return1);
    void parseStore(Store* store);
    void parseZext(Zext* zext);
    void parseCmp(Cmp* cmp);
    void parseEq(ObjOperand* dst, Value* v1, Value* v2);
    void parseNe(ObjOperand* dst, Value* v1, Value* v2);
    void parseLe(ObjOperand* dst, Value* v1, Value* v2);
    void parseLt(ObjOperand* dst, Value* v1, Value* v2);
    void parseGe(ObjOperand* dst, Value* v1, Value* v2);
    void parseGt(ObjOperand* dst, Value* v1, Value* v2);
    void parsePhis(FunctionIR* irFunction);
    vector<ObjInstruction*>* getCopyList(vector<Phi*>& phis, BasicBlock* irPred, BasicBlock* irBlcok);
    void handleCopyList(vector<ObjOperand*>& path, ObjOperand* cur, vector<ObjInstruction*>*copyList, unordered_map<ObjOperand*, ObjOperand*>& phiMap);
    void handCycle(vector<ObjOperand*>& path, ObjOperand* cur, vector<ObjInstruction*>*copyList, unordered_map<ObjOperand*, ObjOperand*>& phiMap);
    ObjOperand* parseOperand(Value* value, bool canImm);
    ObjOperand* getNewVirReg();
    ObjOperand* parseConstIntOperand(int con, bool canImm);
    ObjOperand* parseArgOperand(Argument* argument);
    ObjOperand* parseGlobalOperand(GlobalVar* globalVar);
    ObjOperand* parseBasicBlockOperand(BasicBlock* basicBlock);
    ObjOperand* getDstVirReg(Value* value);
};


#endif //COMPLIER_MIPS_IRPARSER_H
