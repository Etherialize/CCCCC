//
// Created by Han Zhengyang on 2023/11/3.
//

#include "Driver.h"
#include "Module.h"
#include "Argument.h"
#include "Function.h"
#include "BasicBlock.h"
#include "irBuilder.h"
#include "irSymTable.h"
#include "../SymTable.h"
#include "GlobalVar.h"
#include "Types/typesHeader.h"
#include "Instructions/insHeader.h"
Driver::Driver(SyntaxTree* AST, ofstream& output): AST_(AST), output_(output){
    irBuilder_ = irBuilder::getInstance();
    module_ = irBuilder_->buildModule();
}
Module* Driver::getModule() {
    return module_;
}

int Driver::getValue(SyntaxTree *node) {   //getValue仅用在 const， global 初始化求值中。
    if(node->getSynSym() == SynSym::InitVal || node->getSynSym() == SynSym::ConstInitVal) {
        return getValue(node->getChildAt(0));
    }
    else if(node->getSynSym() == SynSym::Exp || node->getSynSym() == SynSym::ConstExp) {
        node->setVal(getValue(node->getChildAt(0)));
    } else if(node->getSynSym() == SynSym::AddExp) {
        if(node->getChildSum() == 1) {
            node->setVal(getValue(node->getChildAt(0)));
        } else {
            int childAdd = getValue(node->getChildAt(0));
            int childMul = getValue(node->getChildAt(2));
            node->setVal(node->getChildAt(1)->first_token() == "+" ? childAdd + childMul : childAdd - childMul);
        }
    } else if(node->getSynSym() == SynSym::MulExp) {
        if(node->getChildSum() == 1) {
            node->setVal(getValue(node->getChildAt(0)));
        } else {
            int childMul = getValue(node->getChildAt(0));
            int childUnary = getValue(node->getChildAt(2));
            string op = node->getChildAt(1)->first_token();
            int result = op == "*" ? childMul * childUnary:
                         op == "/" ? childMul / childUnary:
                         childMul % childUnary;
            node->setVal(result);
        }
    } else if(node->getSynSym() == SynSym::UnaryExp) {
        if(node->getChildSum() == 1) {
            node->setVal(getValue(node->getChildAt(0)));
        } else { // UnaryOp UnaryExp
            int childUnary = getValue(node->getChildAt(1));
            int result = node->getChildAt(0)->first_token() == "-" ? -1 * childUnary : childUnary;
            node->setVal(result);
        }
    } else if(node->getSynSym() == SynSym::PrimaryExp) {
        if(node->getChildAt(0)->first_token() == "(") {
            node->setVal(getValue(node->getChildAt(1)));
        } else if(node->getChildAt(0)->getSynSym() == SynSym::LVal) {
            node->setVal(getConstLValValue(node->getChildAt(0)));
        } else {
            int num = stoi(node->getChildAt(0)->first_token());
            node->setVal(num);
        }
    }
    return node->getVal();
}
int Driver::getConstLValValue(SyntaxTree *lVal) {        //求常量左值
    Var* var = (Var*)lVal->getItem();
    int dim = var->getDim();
    string name = lVal->first_token();
    Value* value = irSymTable_->find(name, false);
    if(lVal->getChildSum() == 1) {           //单常量，则一定是以ConstInt
        return ((ConstInt*)value)->getValue();
    } else {                                 //常数组，GlobalVar或Alloca
        int index1 = getValue(lVal->getChildAt(2));
        int index2 = lVal->getChildSum() == 7 ? getValue(lVal->getChildAt(5)) : 0;
        return ((ConstVal*)((User*)value)->getUsedValue(0))->getEleData(index1, index2);
    }
}
Value* Driver::visitLVal(SyntaxTree *lVal) {             //返回的应该是地址，但如果是常量，则返回ConstInt
    Var* var = (Var*)lVal->getItem();
    int dim = var->getDim();
    string name = lVal->first_token();
    Value* value = irSymTable_->find(name, false);
    if(var->getIsLoad()) {      //说明最终返回的应该是int*类型的地址
        if(dim == 0) {         //单变量
            return value;
        } else {
            Value* baseAddr = value;
            int flag = 0;
            if(((PointerType*)value->getValType())->getReferenceType()->toString() == POINTERTYPE) {         //数组是函数参数
                baseAddr = irBuilder_->buildLoad(curBasicBlock_, value);
                flag = 1;
            }
            Value* index1 = visitExp(lVal->getChildAt(2));
            vector<Value*> index;
            index.push_back(index1);
            if(lVal->getChildSum() == 7) {
                Value* index2 = visitExp(lVal->getChildAt(5));
                index.push_back(index2);
            }
            GetElePointer* addr = irBuilder_->buildGEP(curBasicBlock_, baseAddr,index, flag);
            return addr;
        }
    } else {   //用于数组传参，返回的是int* 或 array*
        if(dim == 0) {
            Value* baseAddr = value;
            int flag = 0;
            if(((PointerType*)value->getValType())->getReferenceType()->toString() == POINTERTYPE) {  //如果是二重指针，将其load则可以直接返回
                baseAddr = irBuilder_->buildLoad(curBasicBlock_, value);
                flag = 1;
                return baseAddr;
            } else {        //则需要getelepointer   i32 0， i32 0 两个index
                vector<Value*> index;
                index.push_back(irBuilder_->buildConstInt(0));
                GetElePointer* addr = irBuilder_->buildGEP(curBasicBlock_, baseAddr, index, flag);
                return addr;
            }
        } else {
            Value* baseAddr = value;
            int flag = 0;
            if(((PointerType*)value->getValType())->getReferenceType()->toString() == POINTERTYPE) {
                baseAddr = irBuilder_->buildLoad(curBasicBlock_, value);
                flag = 1;
            }
            vector<Value*> index;
            Value* index1 = visitExp(lVal->getChildAt(2));
            index.push_back(index1);
            index.push_back(irBuilder_->buildConstInt(0));
            GetElePointer* addr = irBuilder_->buildGEP(curBasicBlock_, baseAddr, index, flag);
            return addr;
        }
    }

}

ConstVal* Driver:: getConInitVal(SyntaxTree* node) {        //只在初始化数组时用到
    vector<ConstVal*> array;
    for(int i = 0; i < node->getChildSum(); i++) {
        SyntaxTree * child = node->getChildAt(i);
        if(child->getSynSym() == SynSym::ConstInitVal || child->getSynSym() == SynSym::InitVal){
            ConstVal* ele = getConInitVal(child);
            array.push_back(ele);
        } else if(child->getSynSym() == SynSym::ConstExp || child->getSynSym() == SynSym::Exp) {   //conExp, 返回一个ConstInt
            ConstVal* ele = irBuilder_->buildConstInt(getValue(node->getChildAt(0)));
            return ele;
        }
    }
    ConstArray* constArray = irBuilder_->buildConstArray(array);
    if(irSymTable_->isGlobal()) {
        int flag = 1;
        for(int i = 0; i < array.size(); i++) {
            if(!((array[i]->getClassName() == CONSTINTCLASS && ((ConstInt *) array[i])->getValue() == 0)|| array[i]->getClassName() == ZEROCLASS)) {
                flag = 0;
            }
        }
        if(flag == 1) {
            ConstVal* zeroInit = irBuilder_->buildZero((ArrayType*)constArray->getValType());
            return zeroInit;
        }
    }
    return constArray;
}
/*这样是为了计算一个元素就初始化一个*/
void Driver::ArrayInitVal(Alloca* base,SyntaxTree* node, vector<int> dims, int *count) {
    for(int i = 0; i < node->getChildSum(); i++) {
        SyntaxTree * child = node->getChildAt(i);
        if(child->getSynSym() == SynSym::InitVal){
            ArrayInitVal(base, child, dims, count);
        } else if(child->getSynSym() == SynSym::Exp) {   //conExp, 返回一个ConstInt
            Value* exp = visitExp(child);
            if(dims.size() == 1) {
                vector<Value*>index;
                index.push_back(irBuilder_->buildConstInt((*count)));
                (*count)++;
                GetElePointer* addr = irBuilder_->buildGEP(curBasicBlock_, base,index,0);
                irBuilder_->buildStore(curBasicBlock_, exp, addr);
            } else {
                int dim_2 = (*count) % dims[1];
                int dim_1 = (*count) / dims[1];
                (*count)++;
                vector<Value*> index;
                index.push_back(irBuilder_->buildConstInt(dim_1));
                index.push_back(irBuilder_->buildConstInt(dim_2));
                GetElePointer* addr = irBuilder_->buildGEP(curBasicBlock_, base,index,0);
                irBuilder_->buildStore(curBasicBlock_, exp, addr);
            }
        }
    }
}
void Driver::run() {
    visit(AST_);
    //module_->printModule(output_);
    MemToReg memToReg = MemToReg();
    memToReg.run(module_);
    module_->printModule(output_);
}
void Driver::visit(SyntaxTree* node){
    if(node->getSynSym() == SynSym::Ident || node->getSynSym() == SynSym::End) {
        return;
    } else {
        switch (node->getSynSym()){
            case SynSym::CompUnit:
                visitCompUnit(node);
                break;
            case SynSym::MainFuncDef:            //visit Block in th visitMainFuncDef, so return
                visitMainFuncDef(node);
                return;
            case SynSym::FuncDef:                //visit Block in th visitFuncDef, so return
                visitFuncDef(node);
                return;
            case SynSym::ConstDef:
                visitConstDef(node);
                return;
            case SynSym::VarDef:
                visitVarDef(node);
                return;
            case SynSym::Block:
                irSymTable_->pushBlock();
                break;
            case SynSym::Stmt:
                visitStmt(node);
                return;
        }
    }
    for(int i = 0; i < node->getChildSum(); i++) {
        visit(node->getChildAt(i));
    }

    if(node->getSynSym() == SynSym::Block) {
        irSymTable_->popTable();
    }

}
void Driver::visitCompUnit(SyntaxTree *node) {
    irSymTable_ = new irSymTable();       //初始化的时候加入全局变量符号表
    return;
}

void Driver::visitMainFuncDef(SyntaxTree* node){
    irBuilder_->resetNameNum();
    irBuilder_->resetBlockNum();
    string name = "main";
    DataType* returnType = new IntType();
    vector<DataType* >argsType;
    vector<Argument*> arguments;

    curFunc_ = irBuilder_->buildFunction(name, new FunctionType(argsType, returnType), arguments);
    irSymTable_->addValue(name, curFunc_);
    module_->addFunction(curFunc_);
    curBasicBlock_ = irBuilder_->buildBlock(curFunc_);
    irSymTable_->pushFunc();

    visit(node->getChildAt(4));

}

void Driver::visitFuncDef(SyntaxTree* node){
    irBuilder_->resetNameNum();
    irBuilder_->resetBlockNum();
    Function* func = (Function*) node->getItem();
    string name = func->getIdent();

    //returnType
    DataType* returnType;
    if(func->getDim() == VOIDDIM) {
        returnType = new VoidType();
    } else {
        returnType = new IntType();
    }

    //argsType
    vector<DataType*> *argsTypes = new vector<DataType*>();
    vector<Argument*> *arguments = new vector<Argument*>();
    vector<string> *names = new vector<string>();
    if(node->getChildSum() == 6) {       //    有形参
        argsTypes = visitFuncFParams(node->getChildAt(3), names);   // void Func(int a) block
    }

    //arguments
    for(int i = 0; i < argsTypes->size(); i++) {
        Argument* argument = irBuilder_->buildArgument((*argsTypes)[i]);
        arguments->push_back(argument);
    }

    //put new Function to irSymTable
    curFunc_ = irBuilder_->buildFunction(name, new FunctionType(*argsTypes, returnType), (*arguments));
    irSymTable_->addValue(name, curFunc_);
    module_->addFunction(curFunc_);



    //new BasicBlock and new isSymTable
    curBasicBlock_ = irBuilder_->buildBlock(curFunc_);
    irSymTable_->pushFunc();

    //store arguments
    for(int i = 0; i < arguments->size(); i++) {
        Alloca* alloca1 = irBuilder_->buildAlloca(curFunc_->getEntry(), (*arguments)[i]->getValType());
        irBuilder_->buildStore(curBasicBlock_, (*arguments)[i], alloca1);
        irSymTable_->addValue((*names)[i], alloca1);
    }

    visit(node->getChildAt(node->getChildSum() - 1));
    if(returnType->toString() == VOIDTYPE) {
        curFunc_->checkRet();
    }

}
vector<DataType*>* Driver::visitFuncFParams(SyntaxTree* node, vector<string> *names){
    vector<DataType*>* argsTypes = new vector<DataType*>();
    for(int i = 0; i < node->getChildSum(); i++) {
        if(node->getChildAt(i)->getSynSym() == SynSym::FuncFParam) {
            string* name = new string();
            DataType* argType = visitFuncFParam(node->getChildAt(i), name);
            argsTypes->push_back(argType);
            names->push_back(*name);
        }
    }
    return argsTypes;
}

DataType* Driver::visitFuncFParam(SyntaxTree* node, string* name){
    Var* var = (Var*)node->getItem();
    (*name) = var->ident_;
    int dim = var->getDim();
    if(dim == 2) {
        int column = getValue(node->getChildAt(5));
        ArrayType* arrayType = new ArrayType(new IntType(), column);
        DataType* argType = new PointerType(arrayType);
        return argType;
    } else if(dim == 1) {
        return new PointerType(new IntType());
    } else {
        return new IntType();
    }
}


void Driver:: visitConstDef(SyntaxTree *node) {     // 1. 全局常单变量   2.局部常单变量    3.全局常数组    4.局部常数组
    Var* var = (Var*) node->getItem();
    int dim = var->getDim();
    //单变量
    if(dim == 0) {    //全局和局部单变量
        int conInitVal = getValue(node->getChildAt(2));
        ConstInt* constInt = irBuilder_->buildConstInt(conInitVal);
        string name = node->first_token();
        irSymTable_->addValue(name, constInt);
    }
    //常量数组
    else {
        string name = node->first_token();
        ConstVal *constVal = getConInitVal(node->getChildAt(node->getChildSum() - 1));
        if(irSymTable_->isGlobal()) {    //全局常数组
            GlobalVar* globalVar = irBuilder_->buildGlobalVar(name, constVal, true);
            module_->addGlobalVar(globalVar);
            irSymTable_->addValue(name, globalVar);
        }
        else {                           //局部常数组
            vector<int> dims;
            dims.push_back(getValue(node->getChildAt(2)));
            if(var->getDim() == 2) {
                dims.push_back(getValue(node->getChildAt(5)));
            }
            ArrayType* arrayType = new ArrayType(new IntType(32), dims);
            Alloca* constArray = irBuilder_->buildConstAlloca(curFunc_->getEntry(),arrayType, constVal);
            string name = node->first_token();
            irSymTable_->addValue(name, constArray);
            if(dims.size() == 1) {
                for(int i = 0; i < dims[0]; i++) {
                    vector<Value*> index;
                    index.push_back(irBuilder_->buildConstInt(i));
                    GetElePointer* pointer = irBuilder_->buildGEP(curBasicBlock_, constArray, index,0);
                    Store* store = irBuilder_->buildStore(curBasicBlock_, irBuilder_->buildConstInt(constVal->getEleData(i,0)),pointer);
                }
            }
            else {
                for(int i = 0; i < dims[0]; i++) {
                    for(int j = 0; j < dims[1]; j++) {
                        vector<Value*> index;
                        index.push_back(irBuilder_->buildConstInt(i));
                        index.push_back(irBuilder_->buildConstInt(j));
                        //获得a[i][j]地址
                        GetElePointer* pointer = irBuilder_->buildGEP(curBasicBlock_, constArray, index, 0);
                        //初始化a[i][j]
                        Store* store = irBuilder_->buildStore(curBasicBlock_, irBuilder_->buildConstInt(constVal->getEleData(i,j)),pointer);
                    }
                }
            }
        }

    }
}

void Driver::visitVarDef(SyntaxTree* node){   // 1. 全局单变量   2.局部单变量    3.全局数组    4.局部数组
    Var* var = (Var*) node->getItem();
    int dim = var->getDim();
    string name = node->first_token();
    if(dim == 0) {      //单变量
        if(irSymTable_->isGlobal()) {    //全局单变量,其InitVal一定是常量表达式
            ConstVal* initVal;
            if (node->getChildSum() == 3) {  //有初始值
                initVal = irBuilder_->buildConstInt(getValue(node->getChildAt(2)));
            } else {                         //无初始值
                initVal = irBuilder_->buildConstInt(0);
            }
            GlobalVar *globalVar = irBuilder_->buildGlobalVar(name, initVal, false);
            irSymTable_->addValue(name, globalVar);
            module_->addGlobalVar(globalVar);
        } else {                 //局部单变量
            Alloca* localVar = irBuilder_->buildAlloca(curFunc_->getEntry(), new IntType(32));
            if(node->getChildSum() == 3) {   //有初始值
                Value* initVal = visitExp(node->getChildAt(2));
                irBuilder_->buildStore(curBasicBlock_, initVal, localVar);
            }
            irSymTable_->addValue(name, localVar);
        }
    }
    else {              //数组
        if(irSymTable_->isGlobal()) {        //全局数组的初值是ConstInitVal
            ConstVal *constVal;
            vector<int> dims;
            dims.push_back(getValue(node->getChildAt(2)));
            if(var->getDim() == 2) {
                dims.push_back(getValue(node->getChildAt(5)));
            }
            if(node->getChildAt(node->getChildSum() - 1)->getSynSym() == SynSym::InitVal) {
                constVal = getConInitVal(node->getChildAt(node->getChildSum() - 1));
            } else {
                ArrayType* arrayType = new ArrayType(new IntType(), dims);
                constVal = irBuilder_->buildZero(arrayType);
            }
            GlobalVar* globalVar = irBuilder_->buildGlobalVar(name, constVal, false);
            irSymTable_->addValue(name, globalVar);
            module_->addGlobalVar(globalVar);
        } else {
            vector<int> dims;
            dims.push_back(getValue(node->getChildAt(2)));
            if(var->getDim() == 2) {
                dims.push_back(getValue(node->getChildAt(5)));
            }
            ArrayType* arrayType = new ArrayType(new IntType(32), dims);
            Alloca* alloca1 = irBuilder_->buildAlloca(curFunc_->getEntry(), arrayType);
            if(node->getChildAt(node->getChildSum() - 1)->getSynSym() == SynSym::InitVal) {
                int *count = new int();
                ArrayInitVal(alloca1,node->getChildAt(node->getChildSum() - 1),dims, count);
            }
            irSymTable_->addValue(name, alloca1);
        }
    }
}
Value* Driver::visitExp(SyntaxTree* node) {
    SynSym synSym = node->getSynSym();
    if(synSym == SynSym::Exp || synSym == SynSym::InitVal) {
        return visitExp(node->getChildAt(0));
    } else if(synSym == SynSym::AddExp) {
        if(node->getChildSum() == 1) {
            return visitExp(node->getChildAt(0));
        } else {
            Value* childAdd = visitExp(node->getChildAt(0));
            Value* childMul = visitExp(node->getChildAt(2));
            string op = node->getChildAt(1)->first_token() == "+" ? ADDINS : SUBINS;
            if(childAdd->getClassName() == CONSTINTCLASS && childMul->getClassName() == CONSTINTCLASS) {
                int rst = op == ADDINS ? ((ConstInt*)childAdd)->getValue() + ((ConstInt*)childMul)->getValue():
                          ((ConstInt*)childAdd)->getValue() - ((ConstInt*)childMul)->getValue();
                Value* constInt = irBuilder_->buildConstInt(rst);
                return constInt;
            } else {
                Value* addIns = irBuilder_->buildArith(curBasicBlock_, op, childAdd, childMul);
                return addIns;
            }
        }
    } else if(synSym == SynSym::MulExp) {
        if(node->getChildSum() == 1) {
            return visitExp(node->getChildAt(0));
        } else {
            Value* childMul = visitExp(node->getChildAt(0));
            Value* childUnary = visitExp(node->getChildAt(2));
            string opString = node->getChildAt(1)->first_token();
            string op = opString == "*" ? MULINS :
                        opString == "/" ? DIVINS :
                        REMINS;
            if(childMul->getClassName() == CONSTINTCLASS && childUnary->getClassName() == CONSTINTCLASS) {
                int v1 = ((ConstInt*)childMul)->getValue();
                int v2 = ((ConstInt*)childUnary)->getValue();
                int rst = op == MULINS ? (v1 * v2):
                          op == DIVINS ? (v1 / v2):
                          (v1 % v2);
                Value* constInt = irBuilder_->buildConstInt(rst);
                return constInt;
            } else {
                Value* mulIns = irBuilder_->buildArith(curBasicBlock_, op, childMul, childUnary);
                return mulIns;
            }
        }
    } else if(synSym == SynSym::UnaryExp) {
        if(node->getChildSum() == 1) {
            return visitExp(node->getChildAt(0));
        } else if(node->getChildAt(0)->getSynSym() == SynSym::UnaryOp) {
            Value* primary = visitExp(node->getChildAt(1));
            if(node->getChildAt(0)->first_token() == "+"){
                return primary;
            } else if(node->getChildAt(0)->first_token() == "-"){
                if(primary->getClassName() == CONSTINTCLASS) {
                    int rst = -1 * ((ConstInt*)primary)->getValue();
                    Value* constInt = irBuilder_->buildConstInt(rst);
                    return constInt;
                } else {
                    Value* constInt = irBuilder_->buildConstInt(0);
                    Value* subIns = irBuilder_->buildArith(curBasicBlock_, SUBINS, constInt, primary);
                    return subIns;
                }
            } else if(node->getChildAt(0)->first_token() == "!") {
                if(primary->getClassName() == CONSTINTCLASS) {
                    int rst = (((ConstInt*)primary)->getValue() == 0) ? 1 : 0;
                    Value* constInt = irBuilder_->buildConstInt(rst);
                    return constInt;
                } else {
                    Value* cmp = irBuilder_->buildCmp(curBasicBlock_, EQUAL,primary, irBuilder_->buildConstInt(0));
                    Value* zext = irBuilder_->buildZext(curBasicBlock_, cmp);
                    return zext;
                }
            }
        } else {
            Value* funcCall = visitFuncCall(node);
            return funcCall;
        }
    } else if(synSym == SynSym::PrimaryExp) {
        if(node->first_token() == "(") {
            return visitExp(node->getChildAt(1));
        } else if(node->getChildAt(0)->getSynSym() == SynSym::LVal) {
            Var* var = (Var*)node->getChildAt(0)->getItem();
            if(var->getIsLoad()) {
                Value* addr = visitLVal(node->getChildAt(0));
                if(addr->getValType()->toString() == POINTERTYPE ) {     //需要用load将其加载
                    Load* load = irBuilder_->buildLoad(curBasicBlock_, addr);
                    return load;
                } else {            //可能是常量，ConstInt
                    return addr;
                }
            } else {
                return visitLVal(node->getChildAt(0));
            }
        } else {
            return irBuilder_->buildConstInt(stoi(node->first_token()));
        }
    }
    return NULL;
}

Value* Driver::visitFuncCall(SyntaxTree* node){
    string name = node->first_token();
    FunctionIR* functionIr = (FunctionIR*)irSymTable_->find(name, true);
    vector<Value*>* args;
    if(node->getChildSum() == 4) {         //有参数
        args = visitRParams(node->getChildAt(2));     // RParams
    } else {
        args = new vector<Value*>();
    }
    FuncCall* funcCall = irBuilder_->buildFuncCall(curBasicBlock_, functionIr->getRetType(), functionIr->getName(), functionIr, args);
    return funcCall;
}
vector<Value*>* Driver::visitRParams(SyntaxTree* node){
    vector<Value*>* rParams = new vector<Value*> ();
    for(int i = 0; i < node->getChildSum(); i++) {
        if(node->getChildAt(i)->getSynSym() == SynSym::Exp) {
            Value* rParam = visitExp(node->getChildAt(i));
            rParams->push_back(rParam);
        }
    }
    return rParams;
}

void Driver::visitStmt(SyntaxTree* node) {
    switch (node->stmt_type_) {
        case ASSIGN_STMT:
            visitAssignStmt(node);
            break;
        case EXP_STMT:
            visitExpStmt(node);
            break;
        case BLOCK_STMT:
            visit(node->getChildAt(0));
            break;
        case IF_STMT:
            visitIfStmt(node);
            break;
        case FOR_STMT:
            visitForStmt(node);
            break;
        case BREAK_STMT:
            visitBreakStmt(node);
            break;
        case CONTINUE_STMT:
            visitContinueStmt(node);
            break;
        case RETURN_STMT:
            visitReturnStmt(node);
            break;
        case GETINT_STMT:
            visitGetintStmt(node);
            break;
        case PRINTF_STMT:
            visitPrintStmt(node);
            break;
    }
}
void Driver:: visitAssignStmt(SyntaxTree* node){
    Value* addr = visitLVal(node->getChildAt(0));   //get the addr of lVal
    Value* exp = visitExp(node->getChildAt(2));
    irBuilder_->buildStore(curBasicBlock_, exp, addr);
}
void Driver::visitExpStmt(SyntaxTree* node){
    if(node->getChildAt(0)->getSynSym() == SynSym::Exp) {
        visitExp(node->getChildAt(0));
    }
}
void Driver::visitIfStmt(SyntaxTree* node){
    int size = node->getChildSum();
    BasicBlock* trueBlock = irBuilder_->buildBlockNotAdd(curFunc_);
    BasicBlock* falseBlock;
    BasicBlock* nextBlock;
    if(size == 5){  // means no else
        nextBlock = irBuilder_->buildBlockNotAdd(curFunc_);
        falseBlock = nextBlock;
    } else {
        falseBlock = irBuilder_->buildBlockNotAdd(curFunc_);
        nextBlock = irBuilder_->buildBlockNotAdd(curFunc_);
    }
    visitCond(node->getChildAt(2), trueBlock, falseBlock);
    curFunc_->addBasicBlock(trueBlock);
    curBasicBlock_ = trueBlock;
    visitStmt(node->getChildAt(4));
    irBuilder_->buildBr(curBasicBlock_, nextBlock);

    if(node->getChildSum() == 7) {
        curFunc_->addBasicBlock(falseBlock);
        curBasicBlock_ = falseBlock;
        visitStmt(node->getChildAt(6));
        irBuilder_->buildBr(curBasicBlock_, nextBlock);
    }
    curFunc_->addBasicBlock(nextBlock);
    curBasicBlock_ = nextBlock;

}
void Driver::visitCond(SyntaxTree* node, BasicBlock* trueBlock, BasicBlock* falseBlock){
    SyntaxTree* lOr = node->getChildAt(0);
    visitLOrExp(lOr, trueBlock, falseBlock);
}
void Driver::visitLOrExp(SyntaxTree* node, BasicBlock* trueBlock, BasicBlock* falseBlock){
    for(int i = 0; i < node->getChildSum() - 1; i++) {
        SyntaxTree* lAnd = node->getChildAt(i);
        BasicBlock* nextBlock = irBuilder_->buildBlockNotAdd(curFunc_);
        visitLAndExp(lAnd, trueBlock, nextBlock);
        curFunc_->addBasicBlock(nextBlock);
        curBasicBlock_ = nextBlock;
    }
    SyntaxTree* lAnd = node->getChildAt(node->getChildSum() - 1);
    visitLAndExp(lAnd, trueBlock, falseBlock);
}
void Driver::visitLAndExp(SyntaxTree* node, BasicBlock* trueBlock, BasicBlock* falseBlock){
    for(int i = 0; i < node->getChildSum(); i++) {
        SyntaxTree* eqExp = node->getChildAt(i);
        BasicBlock* nextBlock;
        if(node->getChildSum() == 1) {
            nextBlock = trueBlock;
        } else {
            nextBlock = irBuilder_->buildBlockNotAdd(curFunc_);
        }
        Value* rst = visitEqExp(eqExp);
        if(rst->isInt32()) {        //仍需要改
            if(rst->getClassName() == CONSTINTCLASS) {
                int con = ((ConstInt*) rst)->getValue() == 0 ? 0 : 1;
                rst = irBuilder_->buildConstInt(1,con);
            }else {
                rst = irBuilder_->buildCmp(curBasicBlock_, NOTEQUAL, rst, irBuilder_->buildConstInt(0));
            }
        }
        irBuilder_->buildBr(curBasicBlock_, rst, nextBlock, falseBlock);
        if(node->getChildSum() != 1) {
            curFunc_->addBasicBlock(nextBlock);
            curBasicBlock_ = nextBlock;
        }
    }
    if(node->getChildSum() != 1) {
        irBuilder_->buildBr(curBasicBlock_, trueBlock);
    }
}
Value* Driver::visitEqExp(SyntaxTree* node) {
    Value* rst;
    if(node->getChildSum() == 1) {
        rst = visitRelExp(node->getChildAt(0));
    } else {
        rst = visitEqExp(node->getChildAt(0));
        string op = node->getChildAt(1)->first_token();
        Value* rel = visitRelExp(node->getChildAt(2));
        if(rst->isInt1()) {
            rst = irBuilder_->buildZext(curBasicBlock_, rst);
        }
        if(rel->isInt1()) {
            rel = irBuilder_->buildZext(curBasicBlock_, rel);
        }

        if(op == "==") {
            if(rst->getClassName() == CONSTINTCLASS && rel->getClassName() == CONSTINTCLASS) {
                int con = (((ConstInt*)rst)->getValue() == ((ConstInt*)rel)->getValue()) ? 1 : 0;
                rst = irBuilder_->buildConstInt(con);
            } else {
                rst = irBuilder_->buildCmp(curBasicBlock_, EQUAL, rst, rel);
            }
        } else {
            if(rst->getClassName() == CONSTINTCLASS && rel->getClassName() == CONSTINTCLASS) {
                int con = (((ConstInt*)rst)->getValue() != ((ConstInt*)rel)->getValue()) ? 1 : 0;
                rst = irBuilder_->buildConstInt(con);
            } else {
                rst = irBuilder_->buildCmp(curBasicBlock_, NOTEQUAL, rst, rel);
            }
        }
    }

    return rst;
}
Value* Driver::visitRelExp(SyntaxTree* node) {
    Value* rst;
    if(node->getChildSum() == 1) {
        rst = visitExp(node->getChildAt(0));
    } else {
        rst = visitRelExp(node->getChildAt(0));
        string op = node->getChildAt(1)->first_token();
        Value* rel = visitExp(node->getChildAt(2));
        if(rst->isInt1()) {
            rst = irBuilder_->buildZext(curBasicBlock_, rst);
        }
        if(rel->isInt1()) {
            rel = irBuilder_->buildZext(curBasicBlock_, rel);
        }

        if(op == ">") {
            if(rst->getClassName() == CONSTINTCLASS && rel->getClassName() == CONSTINTCLASS) {
                int con = (((ConstInt*)rst)->getValue() > ((ConstInt*)rel)->getValue()) ? 1 : 0;
                rst = irBuilder_->buildConstInt(con);
            } else {
                rst = irBuilder_->buildCmp(curBasicBlock_, GRAND, rst, rel);
            }
        } else if(op == ">="){
            if(rst->getClassName() == CONSTINTCLASS && rel->getClassName() == CONSTINTCLASS) {
                int con = (((ConstInt*)rst)->getValue() >= ((ConstInt*)rel)->getValue()) ? 1 : 0;
                rst = irBuilder_->buildConstInt(con);
            } else {
                rst = irBuilder_->buildCmp(curBasicBlock_, GRANDEQUAL, rst, rel);
            }
        } else if(op == "<"){
            if(rst->getClassName() == CONSTINTCLASS && rel->getClassName() == CONSTINTCLASS) {
                int con = (((ConstInt*)rst)->getValue() < ((ConstInt*)rel)->getValue()) ? 1 : 0;
                rst = irBuilder_->buildConstInt(con);
            } else {
                rst = irBuilder_->buildCmp(curBasicBlock_, LESS, rst, rel);
            }
        } else if(op == "<="){
            if(rst->getClassName() == CONSTINTCLASS && rel->getClassName() == CONSTINTCLASS) {
                int con = (((ConstInt*)rst)->getValue() <= ((ConstInt*)rel)->getValue()) ? 1 : 0;
                rst = irBuilder_->buildConstInt(con);
            } else {
                rst = irBuilder_->buildCmp(curBasicBlock_, LESSEQUAL, rst, rel);
            }
        }
    }
    return rst;
}
void Driver::visitForStmt(SyntaxTree* node){
    SyntaxTree* fStmt1 = NULL;
    SyntaxTree* cond = NULL;
    SyntaxTree* fStmt2 = NULL;
    SyntaxTree* stmt = node->getChildAt(node->getChildSum() - 1);
    for(int i = 1; i < node->getChildSum(); i++) {
        if(node->getChildAt(i)->getSynSym() == SynSym::ForStmt) {
            if(i == 2) {
                fStmt1 = node->getChildAt(i);
            } else {
                fStmt2 = node->getChildAt(i);
            }
        }
        else if(node->getChildAt(i)->getSynSym() == SynSym::Cond) {
            cond = node->getChildAt(i);
        }
    }


    if(fStmt1) {
        visitAssignStmt(fStmt1);
    }

    BasicBlock* condBlock = irBuilder_->buildBlock(curFunc_);
    BasicBlock* trueBlock = irBuilder_->buildBlockNotAdd(curFunc_);
    BasicBlock* f2Block = irBuilder_->buildBlockNotAdd(curFunc_);
    BasicBlock* falseBlock = irBuilder_->buildBlockNotAdd(curFunc_);

    curBasicBlock_ = condBlock;
    if(cond) {
        visitCond(cond, trueBlock, falseBlock);
    }
    curBasicBlock_ = trueBlock;
    curFunc_->addBasicBlock(trueBlock);
    f2Blocks_.push_back(f2Block);
    falseBlocks_.push_back(falseBlock);
    visitStmt(stmt);
    f2Blocks_.pop_back();
    falseBlocks_.pop_back();

    curBasicBlock_ = f2Block;
    curFunc_->addBasicBlock(f2Block);
    if(fStmt2) {
        visitAssignStmt(fStmt2);
    }
    irBuilder_->buildBr(curBasicBlock_, condBlock);

    curBasicBlock_ = falseBlock;
    curFunc_->addBasicBlock(falseBlock);
}
void Driver::visitBreakStmt(SyntaxTree* node){
    irBuilder_->buildBr(curBasicBlock_, falseBlocks_[falseBlocks_.size() - 1]);
}
void Driver::visitContinueStmt(SyntaxTree* node){
    irBuilder_->buildBr(curBasicBlock_, f2Blocks_[f2Blocks_.size() - 1]);
}
void Driver::visitReturnStmt(SyntaxTree* node){
    Value* value = NULL;
    DataType* retType;
    if(node->getChildAt(1)->getSynSym() == SynSym::Exp) {
        value = visitExp(node->getChildAt(1));
        retType = new IntType();
    } else {
        retType = new VoidType();
    }
    irBuilder_->buildReturn(curBasicBlock_, retType, value);
}
void Driver::visitGetintStmt(SyntaxTree* node){
    Value* addr = visitLVal(node->getChildAt(0));
    FuncCall* callGetInt = irBuilder_->buildFuncCall(curBasicBlock_, new IntType(),"@getint", nullptr,new vector<Value*>());
    irBuilder_->buildStore(curBasicBlock_, callGetInt, addr);
}
void Driver::visitPrintStmt(SyntaxTree* node){
    SyntaxTree* formatString = node->getChildAt(2);
    vector<SyntaxTree*> exps;
    for(int i = 0; i < node->getChildSum(); i++) {
        if(node->getChildAt(i)->getSynSym() == SynSym::Exp) {
            exps.push_back(node->getChildAt(i));
        }
    }
    int expIndex = 0;
    vector<Value*>* values = visitFormatString(formatString);
    for(int i = 0; i < values->size(); i++) {
        Value* value = (*values)[i];
        if(value->getClassName() == GLOBALVARCLASS) {         //str
            vector<Value*>* args = new vector<Value*>();
            vector<Value*> index;
            index.push_back(irBuilder_->buildConstInt(0));
            GetElePointer* getElePointer = irBuilder_->buildGEP(curBasicBlock_, value,  index, 0);
            args->push_back(getElePointer);
            FuncCall* funcCall = irBuilder_->buildFuncCall(curBasicBlock_, new VoidType(), "@putstr",
                    nullptr, args);
        } else {                                                 //%d
            Value* rst = visitExp(exps[expIndex++]);
            vector<Value*>* args = new vector<Value*>();
            args->push_back(rst);
            FuncCall* funcCall = irBuilder_->buildFuncCall(curBasicBlock_, new VoidType(), "@putint",
                                                           nullptr, args);
        }
    }
}
vector<string>* Driver::handleFormatString(string content) {
    int pos;
    //将换行符替换为\0a
    while((pos = content.find("\\n")) != -1) {
        content.replace(pos, 2, "\\0a");
    }
    vector<string>* strs = new vector<string>();
    int start = 0;
    int end = 0;
    while(end < content.size()) {
        if(content[end] != '%') {
            end++;
        } else {                  //读到了%
            if(start != end) {
                strs->push_back(content.substr(start, end - start));
            }
            strs->emplace_back("%d");
            end += 2;
            start = end;
        }
    }
    if(start != end) {
        strs->push_back(content.substr(start, end - start));
    }
    return strs;
}
vector<Value*>* Driver::visitFormatString(SyntaxTree* node) {

    // 将字符串按%d分隔
    string content = node->first_token();
    content = content.substr(1, content.size() - 2);
    vector<string>* strs = handleFormatString(content);

    //建立values
    vector<Value*>* values = new vector<Value*>();
    for(int i = 0;i < strs->size(); i++) {
        if((*strs)[i] != "%d") {
            ConstStr *constStr = irBuilder_->buildConstStr((*strs)[i]);
            string name = strName_ + to_string(strNumPlus());
            while(irSymTable_->find(name, false) != NULL) {
                name = strName_ + to_string(strNumPlus());
            }
            GlobalVar* globalVar = irBuilder_->buildGlobalVar(name, constStr, true);
            irSymTable_->addValue(name, globalVar);
            module_->addGlobalVar(globalVar);
            values->push_back(globalVar);
        } else {
            values->push_back(irBuilder_->buildConstStr("%d"));
        }
    }
    return values;
}