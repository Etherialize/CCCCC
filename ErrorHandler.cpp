//
// Created by Han Zhengyang on 2023/10/7.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include "ErrorHandler.h"
#include "Parser.h"
#include "SymTable.h"
#include "Lexer.h"
using namespace std;
ErrorMsg::ErrorMsg(int line, char error_code): line_(line), error_code_(error_code) {}
bool compareLine(const ErrorMsg &e1, const ErrorMsg &e2){
    return e1.line_ < e2.line_;
}
void ErrorHandler::errorHandle(SyntaxTree *node) {
    checkError(node);
    sort(errorMsgs_.begin(), errorMsgs_.end(), compareLine);
    for(int i = 0; i < errorMsgs_.size(); i++){
        output_ << to_string(errorMsgs_[i].line_) + " " + errorMsgs_[i].error_code_ << endl;
    }
}
string ErrorMsg::msg() {
    return to_string(line_) + " " + to_string(error_code_);
}
ErrorHandler::ErrorHandler(ofstream &output): output_(output){

}
void ErrorHandler::errorMsg(string msg) {
     output_ << msg << endl;
}
void ErrorHandler::addError(int line, char error_code) {
    for(int i = 0; i < errorMsgs_.size(); i++){
        if(errorMsgs_[i].line_ == line && errorMsgs_[i].error_code_ == error_code){
            return;
        }
    }
    errorMsgs_.push_back(ErrorMsg(line, error_code));
}
void ErrorHandler::newScope() {
    SymTable symTable = SymTable(symTables_.size(), cur_);
    cur_ = symTables_.size();
    symTables_.push_back(symTable);
}
void ErrorHandler:: checkError(SyntaxTree *node) {
    bool isLoop = node->stmt_type_ == FOR_STMT;
    if(isLoop){
        loop_++;
    }
    if(node->symbol_ == SynSym::End || node->symbol_ == SynSym::Ident) {
        return;
    } else {
        switch (node->symbol_) {
            case SynSym::CompUnit:
                newScope();
                break;
            case SynSym::MainFuncDef:
                checkMainFuncDef(node);
                break;
            case SynSym::ConstDef:
            case SynSym::VarDef:
                checkVarDef(node);
                break;
            case SynSym::FuncDef:
                checkFuncDef(node);
                break;
            case SynSym::FuncFParam:
                checkFuncFParam(node);
                break;
            case SynSym::LVal:
                checkUndefine(node);
                break;
            case SynSym::UnaryExp:
                if(node->nodes_[0]->symbol_ == SynSym::Ident)
                    checkUndefine(node);
                break;
            case SynSym::Stmt:
                checkStmt(node);
                break;
            case SynSym::Block:
                checkBlock(node);
                break;
            case SynSym::ForStmt:
                checkConstAssign(node->nodes_[0]);
                break;

        }

        //loop check
        for(int i = 0; i < node->nodes_.size(); i++){
            checkError(node->nodes_[i]);
        }
        //over
        if(node->symbol_ == SynSym::Block)
            cur_ = symTables_[cur_].father_;
        if(isLoop)
            loop_--;
    }
}
void ErrorHandler:: checkVarDef(SyntaxTree *node) {
    TableItem* var = node->item_;
    if(symTables_[cur_].isRedefine(var->ident_)){
        addError(node->line(), REDEFINE);
    } else {
        TableItem* item = new Var((Var*)var);
        symTables_[cur_].addItem(item);
    }
}
void ErrorHandler:: checkFuncDef(SyntaxTree *node) {
    TableItem* func = node->item_;
    TableItem *item = new Function((Function*) func);
    if(symTables_[cur_].isRedefine(func->ident_)) {
        addError(node->line(), REDEFINE);
    } else{
        symTables_[cur_].addItem(item);
        ((Function* )item)->funcTableId_ = symTables_.size();
    }
    newScope();
    symTables_[cur_].type_ = FUNC_SCOPE;
    symTables_[cur_].isVoid_ = item->type_ == VOID;
    if(func->type_ == INTFUNC) {
        SyntaxTree* block = node->nodes_[node->nodes_.size()-1];
        SyntaxTree* lastBlockItem = block->nodes_[block->nodes_.size() - 2];
        if(lastBlockItem->symbol_ == SynSym::BlockItem) {
            SyntaxTree* stmt = lastBlockItem->nodes_[0];
            if(stmt->stmt_type_ == RETURN_STMT){
                return;
            }
        }
        SyntaxTree* rbarce = block->nodes_[block->nodes_.size() - 1];
        addError(rbarce->line(), NORETURN);
    }
}
void ErrorHandler:: checkMainFuncDef(SyntaxTree *node) {
    SyntaxTree* block;
    block = node->nodes_[node->nodes_.size() - 1];
    newScope();
    symTables_[cur_].type_ = FUNC_SCOPE;
    symTables_[cur_].isVoid_ = false;
    SyntaxTree* lastBlockItem = block->nodes_[block->nodes_.size() - 2];
    if(lastBlockItem->symbol_ == SynSym::BlockItem) {
        SyntaxTree* stmt = lastBlockItem->nodes_[0];
        if(stmt->stmt_type_ == RETURN_STMT){
            return;
        }
    }
    SyntaxTree* rbarce = block->nodes_[block->nodes_.size() - 1];
    addError(rbarce->line(), NORETURN);
}
void ErrorHandler:: checkFuncFParam(SyntaxTree *node) {
    TableItem* var = node->item_;
    if(symTables_[cur_].isRedefine(var->ident_)) {
        addError(node->line(), REDEFINE);
    } else {
        TableItem* item = new Var((Var*) var);
        symTables_[cur_].addItem(var);
    }
}
void ErrorHandler:: checkUndefine(SyntaxTree *node) {
    SymTable curTable = symTables_[cur_];
    int index;
    while((index = curTable.indexOf(node->first_token(), node->symbol_ == SynSym::LVal ? INTNUM : INTFUNC)) == -1) {
        if(curTable.father_ == -1) {
            addError(node->line(), UNDEFINE);
            return;
        }
        curTable = symTables_[curTable.father_];
    }
    if(node->symbol_ == SynSym::LVal) {
        Var* varSym = (Var*)curTable.items_[index];
        Var* var = (Var*)node->getItem();
        var->setIsLoad((varSym->getDim() - var->getDim()) == 0);
        return;
    } else {
        TableItem* func = curTable.items_[index];
        SyntaxTree* RParams = new SyntaxTree();
        int rSumOfPara = 0;
        if(node->nodes_.size() >= 3) {
            RParams = node->nodes_[2];
        }
        int fSumOfPara = ((Function*) func)->sumOfParas_;
        if(node->nodes_.size() >= 3 && RParams->symbol_ == SynSym::FuncRParams) {
            rSumOfPara = ((int)RParams->nodes_.size() + 1) / 2;
        }
        if(fSumOfPara == rSumOfPara && fSumOfPara == 0){  //函数定义错误或形参个数为0，不会检查函数实参类型
            return;
        }else if(fSumOfPara != rSumOfPara){
            addError(node->line(), NUMOFPARA);
        }else{
            vector<int> rDims;
            for(int i = 0; i < rSumOfPara; i++) {
                rDims.push_back(getDim(RParams->nodes_[2 * i]));
            }
            vector<int> fDims = ((Function*) func)->dims_;
            for(int i = 0; i<rSumOfPara; i++) {
                if(rDims[i] != UNDEFINEDIM && rDims[i] != fDims[i]) {
                    addError(node->line(), TYPEOFPARA);
                    return;
                }
            }
        }
    }
}
void ErrorHandler::checkStmt(SyntaxTree *node) {
    switch (node->stmt_type_) {
        case ASSIGN_STMT:
        case GETINT_STMT:
            checkConstAssign(node->nodes_[0]);
            break;

        case BREAK_STMT:
        case CONTINUE_STMT:
            if(loop_ == 0) {
                addError(node->line(), NOTLOOP);
            }
            break;

        case RETURN_STMT:
            checkReturn(node);
            break;

        case PRINTF_STMT:
            checkPrint(node);
            break;
    }
}
void ErrorHandler:: checkConstAssign(SyntaxTree *node) {
    SymTable curTable = symTables_[cur_];
    TableItem* item = node->item_;
    int index;
    while((index = curTable.indexOf(item->ident_, INTNUM)) == -1) {
        if(curTable.father_ == -1) {
            return;
        }
        curTable = symTables_[curTable.father_];
    }
    if(((Var*)curTable.items_[index])->isConst_ == 1) {
        addError(node->line(), ASSIGNCONST);
    }
}
void ErrorHandler::checkReturn(SyntaxTree *node) {
    //"return " return
    if(node->nodes_.size() > 1 && node->nodes_[1]->symbol_ == SynSym::Exp) {
        SymTable curTable = symTables_[cur_];
        while(curTable.type_ != FUNC_SCOPE)
            curTable = symTables_[curTable.father_];
        if(curTable.isVoid_){
            addError(node->line(), ERRORRETURN);
        }
    }
}
void ErrorHandler::checkPrint(SyntaxTree *node) {
    SyntaxTree* formatString = node->nodes_[2];
    string fs = formatString->first_token();
    int fsum = 0;
    int rsum = 0;
    for(int  i = 0; i < fs.size(); i++) {
        if(fs[i] == '%' &&(i +1 ) < fs.size() && fs[i+1] == 'd'){
            fsum++;
            i++;
        }
    }
    for(int i = 0; i < node->nodes_.size(); i++) {
        if(node->nodes_[i]->symbol_ == SynSym::Exp)
            rsum++;
    }
    if(rsum != fsum) {
        addError(node->line(), NOTMATCHPRINTF);
    }
}
void ErrorHandler:: checkBlock(SyntaxTree *node) {
    if(node->block_type_ != FUNC_BLOCK) {
        newScope();
    }
}
int ErrorHandler:: getDim(SyntaxTree *node) {
    if (node->symbol_ == SynSym::Exp) {
        node->item_->dim_ = getDim(node->nodes_[0]);
    } else if (node->symbol_ == SynSym::AddExp || node->symbol_ == SynSym::MulExp) {
        int min = 100;
        int temp = UNDEFINEDIM;
        for (int i = 0; i < node->nodes_.size(); i++) {
            temp = getDim(node->nodes_[i]);
            if (temp < min) {
                min = temp;
            }
        }
        node->item_->dim_ = min;
    } else if (node->symbol_ == SynSym::End || node->symbol_ == SynSym::UnaryOp) {
        return 100;
    } else if (node->symbol_ == SynSym::UnaryExp) {
        if (node->nodes_[0]->symbol_ == SynSym::PrimaryExp) {
            node->item_->dim_ = getDim(node->nodes_[0]);
        } else if (node->nodes_[0]->symbol_== SynSym::Ident) {
            SymTable curTable = symTables_[cur_];
            int index;
            while ((index = curTable.indexOf(node->first_token(), INTFUNC)) ==-1) {
                if (curTable.father_ == -1) {
                    node->item_->dim_ = UNDEFINEDIM;
                    return UNDEFINEDIM;
                }
                curTable = symTables_[curTable.father_];
            }
            Function *func = (Function *) curTable.items_[index];
            node->item_->dim_ = func->dim_;
        } else {
            node->item_->dim_ = getDim(node->nodes_[1]);
        }
    } else if(node->symbol_ == SynSym::PrimaryExp) {
        if(node->nodes_[0]->symbol_ == SynSym::End) {
            node->item_->dim_ = getDim(node->nodes_[1]);
        } else if(node->nodes_[0]->symbol_ == SynSym::Number) {
            node->item_->dim_ = 0;
        }else{
            SyntaxTree* lVal = node->nodes_[0];
            SymTable curTable = symTables_[cur_];
            int index;
            while ((index = curTable.indexOf(node->first_token(),INTNUM)) ==-1) {
                if (curTable.father_ == -1) {
                    node->item_->dim_ = UNDEFINEDIM;
                    return UNDEFINEDIM;
                }
                curTable = symTables_[curTable.father_];
            }
            Var* var = (Var*) curTable.items_[index];
            int fdim = var->dim_;
            int rdim = lVal->item_->dim_;
            node->item_->dim_ = fdim - rdim;
        }
    } else {
        return UNDEFINEDIM;
    }
    return node->item_->dim_;
}
