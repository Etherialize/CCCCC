//
// Created by Han Zhengyang on 2023/11/20.
//

#include "irParser.h"
#include "../ir/Types/typesHeader.h"
#include "../ir/ConstVal/conHeader.h"
#include "../ir/Instructions/insHeader.h"
irParser::irParser(Module* irModule) {
    irModule_ = irModule;
    objModule_ = new ObjModule();
    parseModule();
}
ObjModule* irParser::getObjModule()  {
    return objModule_;
}
void irParser:: parseModule() {
    ObjPhyReg::init();
    parseGlobalVars();
    parseFunctions();
}
void irParser:: parseGlobalVars() {
    vector<GlobalVar*> irGlobalVars = irModule_->getGlobalVars();
    for(int i = 0; i < irGlobalVars.size(); i++) {
        ObjGlobalVar* objGlobalVar = parseGlobalVar(irGlobalVars[i]);
        objModule_->addGlobalVar(objGlobalVar);
        globalMap_[irGlobalVars[i]] = objGlobalVar;
    }
}
ObjGlobalVar* irParser:: parseGlobalVar(GlobalVar *globalVar) {
    vector<int>* objElements = new vector<int>();
    ConstVal* initVal = globalVar->getInitVal();
    if(initVal->getClassName() == CONSTARRAYCLASS) {                        //初始值是数组
        objElements = ((ConstArray*)initVal)->getExpandedEles();
        return new ObjGlobalVar(globalVar->getName(), objElements);
    } else if(initVal->getClassName() == ZEROCLASS) {                      //无初始值
        return new ObjGlobalVar(globalVar->getName(), initVal->getValType()->getSize());
    } else if(initVal->getClassName() == CONSTSTRCLASS) {                  //初始值是字符串
        return new ObjGlobalVar(globalVar->getName(), ((ConstStr*)initVal)->getContent());
    } else {                                                               //单变量
        objElements->push_back(((ConstInt*)initVal)->getValue());
        return new ObjGlobalVar(globalVar->getName(), objElements);
    }
}
void irParser::parseFunctions() {
    irToObjMap();
    vector<FunctionIR*> irFunctions = irModule_->getFunctions();
    for(int i = 0; i < irFunctions.size(); i++) {
        parseFunction(irFunctions[i]);
        parsePhis(irFunctions[i]);
    }
}
void irParser::irToObjMap() {
    vector<FunctionIR*> irFunctions = irModule_->getFunctions();
    for(int i = 0; i < irFunctions.size(); i++) {
        // 建立objFunction并建立映射
        FunctionIR* irFunction = irFunctions[i];
        ObjFunction* objFunction = new ObjFunction(irFunction->getName());
        objModule_->addFunction(objFunction);
        functionMap_[irFunction] = objFunction;

        vector<BasicBlock*> basicBlocks = irFunction->getBlocks();
        for(int j = 0; j < basicBlocks.size(); j++) {
            //为每个objFunction建立objBlock，并建立映射
            BasicBlock* irBasicBlock = basicBlocks[j];
            ObjBlock* objBlock = new ObjBlock(irBasicBlock->getName());
            objFunction->addBlock(objBlock);
            blockMap_[irBasicBlock] = objBlock;
        }

//        for(int j = 0; j < basicBlocks.size(); j++) {                     //记录objBlock的前继 后继节点信息
//            BasicBlock* irBlock = basicBlocks[j];
//            vector<BasicBlock*> succes = irBlock->getSuccessors();
//            for(int k = 0; k < succes.size(); k++) {
//                ObjBlock* objBlock = blockMap_[irBlock];
//                ObjBlock* objSucc = blockMap_[succes[k]];
//
//            }
//        }
    }
}
void irParser::parseFunction(FunctionIR* functionIr) {
    objCurFunction_ = functionMap_[functionIr];
    if(objCurFunction_->getName() == "main") {
        objModule_->setMainFunc(objCurFunction_);
    }
    vector<BasicBlock*> irBasicBlocks = functionIr->getBlocks();
    objEntry_ = blockMap_[irBasicBlocks[0]];
    for(int i = 0; i < irBasicBlocks.size(); i++) {
        BasicBlock* irBasicBlock = irBasicBlocks[i];
        parseBasicBlock(irBasicBlock);
    }
}
void irParser::parseBasicBlock(BasicBlock *basicBlock) {
    objCurBlock_ = blockMap_[basicBlock];
    vector<Instruction*> irInstructions = basicBlock->getInstructions();
    for(int i = 0; i < irInstructions.size(); i++) {
        Instruction* irInstruction = irInstructions[i];
        parseInstruction(irInstruction);
    }
}
void irParser::parseInstruction(Instruction* instruction) {
    if (instruction->getClassName() == ALLOCACLASS) {
        parseAlloca((Alloca*)instruction);
    } else if (instruction->getClassName() == ARITHMETICCLASS) {
        parseArithmeticIns((ArithmeticIns*)instruction);
    } else if (instruction->getClassName() == BRCLASS) {
        parseBr((Br*)instruction);
    } else if (instruction->getClassName() == FUNCCALLCLASS) {
        parseFuncCall((FuncCall*)instruction);
    } else if (instruction->getClassName() == GEPCLASS) {
        parseGetElePointer((GetElePointer*)instruction);
    } else if (instruction->getClassName() == LOADCLASS) {
        parseLoad((Load*)instruction);
    } else if (instruction->getClassName() == RETURNCLASS) {
        parseReturn((Return*)instruction);
    } else if (instruction->getClassName() == STORECLASS) {
        parseStore((Store*)instruction);
    } else if (instruction->getClassName() == ZEXTCLASS) {
        parseZext((Zext*)instruction);
    } else if (instruction->getClassName() == PHICLASS || instruction->getClassName() == CMPCLASS) {
        return ;
    }
}
void irParser::parseAlloca(Alloca* alloca1){
    ValueType* ee = ((PointerType*)alloca1->getValType())->getReferenceType();
    ObjOperand* offset = parseConstIntOperand(objCurFunction_->getAllocaSize(), true);
    objCurFunction_->addAllocaSize(ee->getSize());
    objCurBlock_->addInstruction(new ObjComment("alloca a size of " + to_string(ee->getSize()) + " in the offset of " + ((ObjImm*)offset)->print()));
    ObjOperand* dst = parseOperand(alloca1, false);
    ObjArithmetic* addi = new ObjArithmetic(OBJADDU, dst, ObjPhyReg::SP(), offset);
    objCurBlock_->addInstruction(addi);
}
void irParser::parseArithmeticIns(ArithmeticIns* arithmeticIns){
    string irType = arithmeticIns->getInsType();
    if(irType == MULINS || irType == DIVINS || irType == REMINS) {
        parseMulAndDiv(arithmeticIns);
    }else {
        string objType = irType == ADDINS ? OBJADDU : OBJSUBU;
        ObjOperand* dst = parseOperand(arithmeticIns, false);
        ObjOperand* src1;
        src1 = parseOperand(arithmeticIns->getUsedValue(0), irType == ADDINS);
        bool canSrc2Imm = !(src1->getClassName() == OBJIMMCLASS);
        ObjOperand* src2 = parseOperand(arithmeticIns->getUsedValue(1), canSrc2Imm);
        ObjArithmetic* objArithmetic;
        if(!canSrc2Imm){           //src1是常数
            objArithmetic = new ObjArithmetic(objType, dst, src2, src1);
        } else {
            objArithmetic =new ObjArithmetic(objType, dst, src1, src2);
        }
        objCurBlock_->addInstruction(objArithmetic);
    }
}
void irParser::parseMulAndDiv(ArithmeticIns *arithmeticIns) {
    string irType = arithmeticIns->getInsType();
    string objType = irType == MULINS ? OBJMUL :
                     irType == DIVINS ? OBJDIV :
                     ObjREM;
    ObjOperand* dst = parseOperand(arithmeticIns, false);
    ObjOperand* src1 = parseOperand(arithmeticIns->getUsedValue(0), false);
    ObjOperand* src2 = parseOperand(arithmeticIns->getUsedValue(1), false);
    ObjArithmetic* objArithmetic = new ObjArithmetic(objType, dst, src1, src2);
    objCurBlock_->addInstruction(objArithmetic);
}
void irParser::parseBr(Br* br){
    if(br->hasCondition()) { //有条件
        Value* condition = br->getUsedValue(0);
        BasicBlock* trueBlock = (BasicBlock*)br->getUsedValue(1);
        BasicBlock* falseBlock = (BasicBlock*)br->getUsedValue(2);

        if(condition->getClassName() == CONSTINTCLASS) {
            if(((ConstInt*) condition)->getValue() == 1) {                //true
                ObjBranch* j = new ObjBranch(new ObjLabel(blockMap_[trueBlock]->getName()));
                objCurBlock_->addInstruction(j);
                objCurBlock_->setTrueSucc(blockMap_[trueBlock]);
            } else {
                ObjBranch* j = new ObjBranch(new ObjLabel(blockMap_[falseBlock]->getName()));
                objCurBlock_->addInstruction(j);
                objCurBlock_->setTrueSucc(blockMap_[falseBlock]);
            }
        } else {
            Cmp* cmp = (Cmp*)br->getUsedValue(0);
            ObjOperand* src1 = parseOperand(cmp->getUsedValue(0), true);
            bool canSrc2Imm = !(src1->getClassName() == OBJIMMCLASS);
            ObjOperand* src2 = parseOperand(cmp->getUsedValue(1), canSrc2Imm);
            ObjCondType objCondType = parseCondition(cmp);

            ObjBranch* objBranch;
            if(!canSrc2Imm) {    //src1 是常数,需要交换两个源操作数， 并且改变condType
                objCondType = ObjBranch::getSwapCondType(objCondType);
                objBranch = new ObjBranch(objCondType, src2, src1, new ObjLabel(blockMap_[trueBlock]->getName()));
            } else {
                objBranch = new ObjBranch(objCondType, src1, src2, new ObjLabel(blockMap_[trueBlock]->getName()));
            }
            objCurBlock_->addInstruction(objBranch);
            ObjBranch* jFalse = new ObjBranch(new ObjLabel(blockMap_[falseBlock]->getName()));
            objCurBlock_->addInstruction(jFalse);
            objCurBlock_->setTrueSucc(blockMap_[trueBlock]);
            objCurBlock_->setFalseSucc(blockMap_[falseBlock]);
        }
    } else {        //无条件
        ObjBranch* j = new ObjBranch(new ObjLabel(blockMap_[(BasicBlock*)br->getUsedValue(0)]->getName()));
        objCurBlock_->addInstruction(j);
        objCurBlock_->setTrueSucc(blockMap_[(BasicBlock*)br->getUsedValue(0)]);
    }
}
ObjCondType irParser::parseCondition(Cmp* cmp) {
    string irCond = cmp->getCondition();
    ObjCondType objCondType = irCond == EQUAL ? ObjCondType ::EQ:
                              irCond == NOTEQUAL ? ObjCondType ::NE:
                              irCond == LESS ? ObjCondType ::LT:
                              irCond == LESSEQUAL ? ObjCondType ::LE:
                              irCond == GRAND ? ObjCondType ::GT:
                              ObjCondType ::GE;
    return objCondType;
}

void irParser::parseFuncCall(FuncCall* funcCall){
    string objName = funcCall->getTargetName().substr(1, funcCall->getTargetName().size() - 1);
    if(objName == "getint" || objName == "putstr" || objName == "putint") {
        parseSyscall(objName,funcCall);
    } else {
        ObjCall* objCall = new ObjCall(new ObjLabel(objName));

        vector<Value*> args = funcCall->getUsedValues();
        int argSum = args.size();
        for(int i = 0; i < argSum; i++) {
            Value* irArg = args[i];
            ObjOperand* src = parseOperand(irArg, false);
            int offset = -(argSum - i) * 4;
            ObjStore* objStore = new ObjStore(src, ObjPhyReg::SP(), new ObjImm(offset));
            objCurBlock_->addInstruction(objStore);
        }

        if(argSum > 0) {
            ObjOperand* objOffset = parseConstIntOperand(4 * argSum, true);
            ObjArithmetic* subu = new ObjArithmetic(OBJSUBU, ObjPhyReg::SP(), ObjPhyReg::SP(), objOffset);
            objCurBlock_->addInstruction(subu);
        }

        objCurBlock_->addInstruction(objCall);

        if(argSum > 0) {
            ObjOperand* objOffset = parseConstIntOperand(4 * argSum, true);
            ObjArithmetic* addu = new ObjArithmetic(OBJADDU, ObjPhyReg::SP(), ObjPhyReg::SP(), objOffset);
            objCurBlock_->addInstruction(addu);
        }
        objCall->addDefReg(nullptr, ObjPhyReg::RA());
        objCall->addDefReg(nullptr, ObjPhyReg::V0());

        ValueType* retType = funcCall->getTarget()->getValType();
        if(retType->toString() != VOIDTYPE) {
            ObjMove* moveV0 = new ObjMove(parseOperand(funcCall, false), ObjPhyReg::V0());
            objCurBlock_->addInstruction(moveV0);
        }
    }
}
void irParser::parseSyscall(string funcName, FuncCall* funcCall) {
    if(funcName == "getint") {
        ObjOperand* dst = parseOperand(funcCall, false);
        ObjComment* sysCall = new ObjComment(funcName, false);
        ObjMove* move = new ObjMove(dst, ObjPhyReg::V0());
        objCurBlock_->addInstruction(sysCall);
        objCurBlock_->addInstruction(move);
        sysCall->addDefReg(nullptr, ObjPhyReg::V0());
    } else if(funcName == "putint") {
        ObjOperand* arg = parseOperand(funcCall->getUsedValue(0), true);
        ObjMove* move = new ObjMove(ObjPhyReg::A0(), arg);
        ObjComment* sysCall = new ObjComment(funcName, false);
        objCurBlock_->addInstruction(move);
        objCurBlock_->addInstruction(sysCall);
        sysCall->addDefReg(nullptr, ObjPhyReg::V0());
        sysCall->addUseReg(nullptr, ObjPhyReg::A0());
    } else {
        ObjOperand* arg = parseOperand(funcCall->getUsedValue(0), false);
        ObjMove* move = new ObjMove(ObjPhyReg::A0(), arg);
        ObjComment* sysCall = new ObjComment(funcName, false);
        objCurBlock_->addInstruction(move);
        objCurBlock_->addInstruction(sysCall);
        sysCall->addDefReg(nullptr, ObjPhyReg::V0());
        sysCall->addUseReg(nullptr, ObjPhyReg::A0());
    }
}
void irParser::parseGetElePointer(GetElePointer* getElePointer){
    vector<Value*> irValues = getElePointer->getUsedValues();
    ObjOperand* base = parseOperand(irValues[0], false);
    ObjOperand* dst = parseOperand(getElePointer, false);

    PointerType* pointerType = (PointerType*)irValues[0]->getValType();
    ValueType* arrayType = pointerType->getReferenceType();

    for(int i = 1; i < irValues.size(); i++) {
        Value* index = irValues[i];
        ObjOperand* offset;
        if(index->getClassName() == CONSTINTCLASS) {
            offset = parseConstIntOperand(arrayType->getSize() * ((ConstInt*)index)->getValue(), true);
        } else {
            offset = getNewVirReg();
            ObjArithmetic* mul = new ObjArithmetic(OBJMUL, offset, parseOperand(index, false), parseConstIntOperand(arrayType->getSize(), false));
            objCurBlock_->addInstruction(mul);
        }
        ObjOperand* temp = getNewVirReg();
        objCurBlock_->addInstruction(new ObjArithmetic(OBJADDU, temp, base, offset));
        base = temp;
        if(arrayType->toString() == ARRAYTYPE) {
            arrayType = ((ArrayType*) arrayType)->getElementType();
        }
    }
    objCurBlock_->addInstruction(new ObjMove(dst, base));
}
void irParser::parseLoad(Load* load){
    ObjOperand* addr = parseOperand(load->getUsedValue(0), false);
    ObjOperand* dst = parseOperand(load, false);
    ObjOperand* offset = parseConstIntOperand(0, true);
    ObjLoad* objLoad = new ObjLoad(dst, addr, offset);
    objCurBlock_->addInstruction(objLoad);
}
void irParser::parseReturn(Return* return1){
    Value* irRetValue = return1->getUsedValue(0);
    if(irRetValue != nullptr) {
        ObjOperand* objRet = parseOperand(irRetValue, true);
        ObjMove* objMoveV0 = new ObjMove(ObjPhyReg::V0(), objRet);
        objCurBlock_->addInstruction(objMoveV0);
    }

    ObjRet* objRet = new ObjRet(objCurFunction_);
    objRet->addUseReg(nullptr, ObjPhyReg::V0());
    objCurBlock_->addInstruction(objRet);
}
void irParser::parseStore(Store* store){
    ObjOperand* data = parseOperand(store->getUsedValue(0), false);
    ObjOperand* addr = parseOperand(store->getUsedValue(1), false);
    ObjOperand* offset = parseConstIntOperand(0, true);
    ObjStore* objStore = new ObjStore(data, addr, offset);
    objCurBlock_->addInstruction(objStore);
}
void irParser::parseZext(Zext* zext){
    parseCmp((Cmp*)zext->getUsedValue(0));
    operandMap_[zext] = operandMap_[zext->getUsedValue(0)];
}
void irParser::parseCmp(Cmp* cmp) {
    ObjOperand* dst = parseOperand(cmp, false);
    ObjCondType  objCondType = parseCondition(cmp);
    switch (objCondType) {
        case ObjCondType ::EQ:
            parseEq(dst, cmp->getUsedValue(0), cmp->getUsedValue(1));
            break;
        case ObjCondType ::NE:
            parseNe(dst, cmp->getUsedValue(0), cmp->getUsedValue(1));
            break;
        case ObjCondType ::LE:
            parseLe(dst, cmp->getUsedValue(0), cmp->getUsedValue(1));
            break;
        case ObjCondType ::LT:
            parseLt(dst, cmp->getUsedValue(0), cmp->getUsedValue(1));
            break;
        case ObjCondType ::GE:
            parseLe(dst, cmp->getUsedValue(1), cmp->getUsedValue(0));
            break;
        case ObjCondType ::GT:
            parseLt(dst, cmp->getUsedValue(1), cmp->getUsedValue(0));
            break;
    }
}
void irParser::parseEq(ObjOperand* dst, Value* v1, Value* v2) {
    bool canSrc2Imm = !(v1->getClassName() == CONSTINTCLASS);
    ObjOperand* src1 = parseOperand(v1, true);
    ObjOperand* src2 = parseOperand(v2, canSrc2Imm);
    if(!canSrc2Imm){
        ObjOperand* temp = src1;
        src1 = src2;
        src2 = temp;
    }
    ObjPhyReg* tempReg = ObjPhyReg::AT();
    ObjArithmetic* objXor = new ObjArithmetic(OBJXOR, tempReg, src1, src2);
    objCurBlock_->addInstruction(objXor);
    ObjArithmetic* objSltu = new ObjArithmetic(OBJSLTU, dst, ObjPhyReg::AT(), new ObjImm(1));
    objCurBlock_->addInstruction(objSltu);
}
void irParser::parseNe(ObjOperand* dst, Value* v1, Value* v2) {
    bool canSrc2Imm = !(v1->getClassName() == CONSTINTCLASS);
    ObjOperand* src1 = parseOperand(v1, true);
    ObjOperand* src2 = parseOperand(v2, canSrc2Imm);
    if(!canSrc2Imm){
        ObjOperand* temp = src1;
        src1 = src2;
        src2 = temp;
    }
    ObjPhyReg* tempReg = ObjPhyReg::AT();
    ObjArithmetic* objXor = new ObjArithmetic(OBJXOR, tempReg, src1, src2);
    objCurBlock_->addInstruction(objXor);
    ObjArithmetic* objSltu = new ObjArithmetic(OBJSLTU, dst, ObjPhyReg::AT(), ObjPhyReg::ZERO());
    objCurBlock_->addInstruction(objSltu);
}
/*
 *  v1 <= v2 即!(v1 > v2)
 */
void irParser::parseLe(ObjOperand* dst, Value* v1, Value* v2) {
    ObjOperand* src1 = parseOperand(v1, true);
    ObjOperand* src2 = parseOperand(v2, false);

    ObjPhyReg* tempReg = ObjPhyReg::AT();
    ObjArithmetic* objSlt = new ObjArithmetic(OBJSLT, tempReg, src2, src1);
    objCurBlock_->addInstruction(objSlt);
    ObjArithmetic* objXor = new ObjArithmetic(OBJXOR,dst, tempReg, new ObjImm(1));
    objCurBlock_->addInstruction(objXor);
}
void irParser::parseLt(ObjOperand* dst, Value* v1, Value* v2) {
    ObjOperand* src1 = parseOperand(v1, false);
    ObjOperand* src2 = parseOperand(v2, true);

    ObjArithmetic* objSlt = new ObjArithmetic(OBJSLT, dst, src1, src2);
    objCurBlock_->addInstruction(objSlt);
}
/*
 * v1 >= v2 <---> v2 <= v1
 */
void irParser::parseGe(ObjOperand* dst, Value* v1, Value* v2) {}
/*
 * v1 > v2 <---> v2 < v1
 */
void irParser::parseGt(ObjOperand* dst, Value* v1, Value* v2) {}

void irParser::parsePhis(FunctionIR* irFunction) {
    vector<BasicBlock*> irBlocks = irFunction->getBlocks();
    for(int i = 0; i < irBlocks.size(); i++) {
        BasicBlock* irBlock = irBlocks[i];
        ObjBlock* objBlock = blockMap_[irBlock];
        /*
         * kekkekekekekekeke
         */
        objCurBlock_ = objBlock;
        vector<BasicBlock*> irPreds = irBlock->getPredecessors();

        if(irPreds.size() == 1) {           // on phi to handle
            continue;
        }

        vector<Phi*> phis;
        vector<Instruction*> irInsts = irBlock->getInstructions();
        for(int j = 0; j < irInsts.size(); j++) {
            if(irInsts[j]->getClassName() == PHICLASS) {
                phis.push_back((Phi*)irInsts[j]);
            } else {
                break;
            }
        }

        for(int j = 0; j < irPreds.size(); j++) {
            vector<ObjInstruction*>* copyList =  getCopyList(phis, irPreds[j], irBlock);
            objCurFunction_->insertPhis(objBlock, blockMap_[irPreds[j]], copyList);
        }
    }
}
vector<ObjInstruction*>* irParser::getCopyList(vector<Phi*>& phis, BasicBlock* irPred, BasicBlock* irBlcok){
    unordered_map<ObjOperand*, ObjOperand*> phiMap;
    vector<ObjInstruction*>* copyList = new vector<ObjInstruction*>();

    for(auto it : phis) {
        ObjOperand* phiDst = parseOperand(it, false);

        Value* srcValue = it->getPhiSrc(irPred);
        ObjOperand* phiSrc;
        if(srcValue->getClassName() == CONSTINTCLASS) {
            phiSrc = new ObjImm(((ConstInt*)srcValue)->getValue());

        } else {
            phiSrc = parseOperand(srcValue, false);
        }
        phiMap[phiDst] = phiSrc;
    }

    while(phiMap.size() != 0) {
        vector<ObjOperand*> path;
        auto cur = phiMap.begin()->first;
        bool hasCycle = false;
        for(; phiMap.find(cur) != phiMap.end(); cur = phiMap[cur]) {
            auto ff = path.end();
            if((ff = find(path.begin(), path.end(), cur)) == path.end()) {         //无环
                path.push_back(cur);
            }else {
                hasCycle = true;
                break;
            }
        }

        if(hasCycle) {
            handCycle(path, cur, copyList, phiMap);
            handleCopyList(path, cur, copyList, phiMap);
        } else {
            handleCopyList(path, cur, copyList, phiMap);
        }
    }
    return copyList;
}
void irParser::handleCopyList(vector<ObjOperand*>& path, ObjOperand* cur, vector<ObjInstruction*>*copyList, unordered_map<ObjOperand*, ObjOperand*>& phiMap){
    ObjOperand* phiSrc = cur;
    while(path.size() != 0) {
        ObjOperand* phiDst = path.back();
        path.pop_back();

        ObjInstruction* objMove = new ObjMove(phiDst, phiSrc);
        copyList->insert(copyList->begin(), objMove);
        phiSrc = phiDst;
        phiMap.erase(phiDst);
    }
}
void irParser::handCycle(vector<ObjOperand*>& path, ObjOperand* cur, vector<ObjInstruction*>*copyList, unordered_map<ObjOperand*, ObjOperand*>& phiMap){
    ObjOperand* temp = getNewVirReg();
    ObjMove* objMove = new ObjMove(nullptr, nullptr);
    objMove->setDst(temp);

    while(find(path.begin(), path.end(), cur) != path.end()) {
        ObjOperand* src = path.back();
        path.pop_back();
        objMove->setSrc(src);

        copyList->push_back(objMove);
        objMove = new ObjMove(nullptr, nullptr);
        objMove->setDst(src);
        phiMap.erase(src);
    }

    objMove->setSrc(temp);
}
ObjOperand* irParser::parseOperand(Value* value, bool canImm){
    auto it = operandMap_.find(value);
    // 已经被解析
    if(it != operandMap_.end()) {
        ObjOperand* objOperand = it->second;
        //如果是立即数，但不允许返回立即数
        if(!canImm && objOperand->getClassName() == OBJIMMCLASS){
            if(((ObjImm*)objOperand)->getImmediate() == 0) {
                return ObjPhyReg::ZERO();
            } else {
                ObjOperand* virReg = getNewVirReg();
                ObjMove* objMove = new ObjMove(virReg, objOperand);
                objCurBlock_->addInstruction(objMove);
                return virReg;
            }
        } else {
            return it->second;
        }
    } else if(value->getClassName() == ARGUMENTCLASS){
        return parseArgOperand((Argument*) value);
    } else if(value->getClassName() == GLOBALVARCLASS) {
        return parseGlobalOperand((GlobalVar*) value);
    } else if(value->getClassName() == CONSTINTCLASS) {
        return parseConstIntOperand(((ConstInt* )value)->getValue(), canImm);
    } else if(value->getClassName() == BASICBLOCKCLASS) {
        return parseBasicBlockOperand((BasicBlock*)value);
    } else {
        return getDstVirReg(value);
    }
}
ObjOperand* irParser::getNewVirReg(){
    ObjVirReg* virReg = new ObjVirReg();
    objCurFunction_->addVirReg(virReg);
    return virReg;
}
ObjOperand* irParser::parseConstIntOperand(int con, bool canImm){
    if(canImm) {
        ObjImm* objImm = new ObjImm(con);
        return objImm;
    } else {
        if(con == 0) {
            return ObjPhyReg::ZERO();
        } else {
            ObjOperand* dst = getNewVirReg();
            ObjImm* objImm = new ObjImm(con);
            ObjMove* objMove = new ObjMove(dst, objImm);
            objCurBlock_->addInstruction(objMove);
            return dst;
        }
    }
}
ObjOperand* irParser::parseArgOperand(Argument* argument){
    int rank = argument->getRank();            //rank是从0开始的
    ObjImm* offset = new ObjImm(rank * 4);
    ObjOperand* virReg = getNewVirReg();
    operandMap_[argument] = virReg;
    objCurFunction_->addArgOffsets(offset);
    ObjLoad* load = new ObjLoad(virReg, ObjPhyReg::SP(), offset);
    objEntry_->addInsToHead(load);
    return virReg;
}
ObjOperand* irParser::parseGlobalOperand(GlobalVar* globalVar){
    ObjGlobalVar* objGlobalVar = globalMap_[globalVar];
    ObjOperand* virReg = getNewVirReg();
    string objName = objGlobalVar->getName();
    ObjMove* objMove = new ObjMove(virReg, new ObjLabel(objName));
    objCurBlock_->addInstruction(objMove);
    return virReg;
}
ObjOperand* irParser::parseBasicBlockOperand(BasicBlock* basicBlock) {
    ObjBlock* objBlock = blockMap_[basicBlock];
    ObjLabel* objLabel = new ObjLabel(objBlock->getName());
    return objLabel;
}
ObjOperand* irParser::getDstVirReg(Value* value){
    ObjOperand* virReg = getNewVirReg();
    operandMap_[value] = virReg;
    return virReg;
}
