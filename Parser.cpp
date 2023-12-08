//
// Created by Han Zhengyang on 2023/10/7.
//

#include "Parser.h"
#include "ErrorHandler.h"
#include "Parser.h"
#include "SymTable.h"
#include "Lexer.h"
#define ERROR_HANDLER rollBack();errorHandler_.errorMsg(to_string(word_.line_));next();
map<LexSym, std::string> lexSymMap = {
        {LexSym::IDENFR,     "IDENFR" },
        {LexSym::INTCON,     "INTCON" },
        {LexSym::STRCON,     "STRCON" },
        {LexSym::MAINTK,     "MAINTK" },
        {LexSym::CONSTTK,    "CONSTTK" },
        {LexSym::INTTK,      "INTTK" },
        {LexSym::BREAKTK,    "BREAKTK" },
        {LexSym::CONTINUETK, "CONTINUETK" },
        {LexSym::IFTK,       "IFTK" },
        {LexSym::ELSETK,     "ELSETK" },
        {LexSym::NOT,        "NOT" },
        {LexSym::AND,        "AND" },
        {LexSym::OR,         "OR" },
        {LexSym::FORTK,      "FORTK" },
        {LexSym::GETINTTK,   "GETINTTK" },
        {LexSym::PRINTFTK,   "PRINTFTK" },
        {LexSym::RETURNTK,   "RETURNTK" },
        {LexSym::PLUS,       "PLUS" },
        {LexSym::MINU,       "MINU" },
        {LexSym::VOIDTK,     "VOIDTK" },
        {LexSym::MULT,       "MULT" },
        {LexSym::DIV,        "DIV" },
        {LexSym::MOD,        "MOD" },
        {LexSym::LSS,        "LSS" },
        {LexSym::LEQ,        "LEQ" },
        {LexSym::GRE,        "GRE" },
        {LexSym::GEQ,        "GEQ" },
        {LexSym::EQL,        "EQL" },
        {LexSym::NEQ,        "NEQ" },
        {LexSym::ASSIGN,     "ASSIGN" },
        {LexSym::SEMICN,     "SEMICN" },
        {LexSym::COMMA,      "COMMA" },
        {LexSym::LPARENT,    "LPARENT" },
        {LexSym::RPARENT,    "RPARENT" },
        {LexSym::LBRACK,     "LBRACK" },
        {LexSym::RBRACK,     "RBRACK" },
        {LexSym::LBRACE,     "LBRACE" },
        {LexSym::RBRACE,     "RBRACE" },
        {LexSym::EMPTY,      "EMPTY" }
};
map<SynSym, std::string> synSymMap = {
        {SynSym::CompUnit, "CompUnit"},
        {SynSym::Decl, "Decl"},
        {SynSym::ConstDecl, "ConstDecl"},
        {SynSym::BType, "BType"},
        {SynSym::ConstDef, "ConstDef"},
        {SynSym::ConstInitVal, "ConstInitVal"},
        {SynSym::VarDecl, "VarDecl"},
        {SynSym::VarDef, "VarDef"},
        {SynSym::InitVal, "InitVal"},
        {SynSym::FuncDef, "FuncDef"},
        {SynSym::MainFuncDef, "MainFuncDef"},
        {SynSym::FuncType, "FuncType"},
        {SynSym::FuncFParams, "FuncFParams"},
        {SynSym::FuncFParam, "FuncFParam"},
        {SynSym::Block, "Block"},
        {SynSym::BlockItem, "BlockItem"},
        {SynSym::Stmt, "Stmt"},
        {SynSym::ForStmt, "ForStmt"},
        {SynSym::Exp, "Exp"},
        {SynSym::Cond, "Cond"},
        {SynSym::LVal, "LVal"},
        {SynSym::PrimaryExp, "PrimaryExp"},
        {SynSym::Number, "Number"},
        {SynSym::UnaryExp, "UnaryExp"},
        {SynSym::UnaryOp, "UnaryOp"},
        {SynSym::FuncRParams, "FuncRParams"},
        {SynSym::MulExp, "MulExp"},
        {SynSym::AddExp, "AddExp"},
        {SynSym::RelExp, "RelExp"},
        {SynSym::EqExp, "EqExp"},
        {SynSym::LAndExp, "LAndExp"},
        {SynSym::LOrExp, "LOrExp"},
        {SynSym::ConstExp, "ConstExp"},
        {SynSym::Ident, "Ident"},
        {SynSym::FormatString, "FormatString"},
        {SynSym::IntConst, "IntConst"},
        {SynSym::End, "End"}
};
void SyntaxTree::addChildNode(SyntaxTree *node) {
    nodes_.push_back(node);
}
SyntaxTree::SyntaxTree() : word_("", LexSym::EMPTY, 0){
}
SynSym SyntaxTree:: getSynSym(){
    return symbol_;
}
TableItem* SyntaxTree:: getItem() {
    return item_;
}
void SyntaxTree:: setVal(int value){
    item_->setVal(value);
}
int SyntaxTree::getVal(){
    return item_->getVal();
}
int SyntaxTree:: getStmtType(){
    return stmt_type_;
}
int SyntaxTree:: getChildSum() {
    return nodes_.size();
}
SyntaxTree* SyntaxTree:: getChildAt(int index) {
    if(index >= nodes_.size()) {
        return NULL;
    } else {
        return nodes_[index];
    }
}
string SyntaxTree::first_token() {
    if(symbol_ == SynSym::End ||symbol_ == SynSym::Ident) {
        return word_.token_;
    } else {
        SyntaxTree* node = nodes_[0];
        while(node->symbol_ != SynSym::End && node->symbol_ != SynSym::Ident) {
            node = node->nodes_[0];
        }
        return node->word_.token_;
    }
}
int SyntaxTree::line() {
    if(symbol_ == SynSym::End ||symbol_ == SynSym::Ident) {
        return word_.line_;
    } else {
        SyntaxTree* node = nodes_[0];
        while(node->symbol_ != SynSym::End && node->symbol_ != SynSym::Ident) {
            node = node->nodes_[0];
        }
        return node->word_.line_;
    }
}
Parser ::Parser(Lexer &lexer, ErrorHandler &errorHandler, ofstream &output): lexer_(lexer), errorHandler_(errorHandler), output_(output), word_("", LexSym::EMPTY, 0){
}
void Parser::preLexer() {
    lexer_.next();
    while(lexer_.symbol != LexSym ::EMPTY){
        Word word(lexer_.token, lexer_.symbol, lexer_.line);
        words_.push_back(word);
        lexer_.next();
    }
    length_ = words_.size();
}
void Parser::next() {
    if(pos_ < length_) {
        word_ = words_[pos_];
    } else{
        word_ = Word("", LexSym::EMPTY, 0);
    }
    pos_++;
}
void Parser::rollBack() {
    pos_--;
    if(pos_ < length_) {
        word_ = words_[pos_-1];
    } else{
        word_ = Word("", LexSym::EMPTY, 0);
    }
}
void Parser::print(SyntaxTree* node) {
    if(node->symbol_ == SynSym::End || node->symbol_ == SynSym::Ident) {
        output_ << lexSymMap[node->word_.symbol_] + " " + node->word_.token_ << endl;
    }
    else {
        int sum = node->nodes_.size();
        for(int i = 0; i < sum; i++){
            print(node->nodes_[i]);
        }
        if(node->symbol_ == SynSym::BType || node->symbol_ == SynSym::Decl || node->symbol_ == SynSym::BlockItem) {
            return;
        }
        output_<< "<" + synSymMap[node->symbol_] + ">" << endl;
    }
}
SyntaxTree* Parser::end() {
    SyntaxTree *end = new SyntaxTree();
    end->symbol_ = SynSym::End;
    end->word_ = Word(word_.token_, word_.symbol_, word_.line_);
    next();
    return end;
}
SyntaxTree* Parser::Ident() {
    SyntaxTree* idenfr = end();
    idenfr->symbol_ = SynSym::Ident;
    return idenfr;
}
SyntaxTree* Parser::FormatString() {
    SyntaxTree* fString = end();
    string buf = fString->word_.token_;
    for(int i = 0; i < buf.size(); i++) {
        if(i == 0 || i == buf.size() - 1){
            continue;
        }
        if((int)buf[i] == 32 || (int)buf[i] == 33 || ((int)buf[i] >= 40 && (int)buf[i] <= 126 &&(int)buf[i] != 92)) {
            continue;
        }
        else if(buf[i] == 92) {
            if(buf[i+1] != 'n'){
                errorHandler_.addError(fString->word_.line_, ILLEGAL_STRING);
                break;
            }
        }
        else if(buf[i] == '%') {
            if(buf[i+1] != 'd') {
                errorHandler_.addError(fString->word_.line_, ILLEGAL_STRING);
                break;
            }
        } else if(buf[i] == '\\') {
            if(buf[i+1] != 'n') {
                errorHandler_.addError(fString->word_.line_, ILLEGAL_STRING);
                break;
            }
        } else {
            errorHandler_.addError(fString->word_.line_, ILLEGAL_STRING);
            break;
        }
    }
    return fString;
}
SyntaxTree* Parser::IntConst() {
    SyntaxTree* iConst = end();
    return iConst;
}
SyntaxTree* Parser::CompUnit(){
    next();
    SyntaxTree *compUnit = new SyntaxTree();
    compUnit->symbol_ = SynSym::CompUnit;

    // Decl
    while(word_.symbol_ == LexSym::INTTK || word_.symbol_ == LexSym::CONSTTK) {
        //const
        if(word_.symbol_ == LexSym::CONSTTK) {
            SyntaxTree *decl = Decl();
            compUnit->addChildNode(decl);
        } else {
            next();
            if(word_.symbol_ == LexSym::IDENFR) {
                next();
                if(word_.symbol_ == LexSym::COMMA || word_.symbol_ == LexSym::SEMICN ||
                   word_.symbol_ == LexSym::LBRACK || word_.symbol_ == LexSym::ASSIGN) {
                    rollBack();
                    rollBack();
                    SyntaxTree *decl = Decl();
                    compUnit->addChildNode(decl);
                } else {
                    rollBack();
                    rollBack();
                    break;
                }
            } else {
                rollBack();
                break;
            }
        }
    }

    // FuncDef
    while(word_.symbol_ == LexSym::INTTK || word_.symbol_ == LexSym::VOIDTK) {
        if(word_.symbol_ == LexSym::VOIDTK) {
            SyntaxTree* funcDef = FuncDef();
            compUnit->addChildNode(funcDef);
        } else {
            next();
            if(word_.symbol_ == LexSym::IDENFR) {
                rollBack();
                SyntaxTree* funcDef = FuncDef();
                compUnit->addChildNode(funcDef);
            } else {
                rollBack();
                break;
            }
        }
    }

    if(word_.symbol_ == LexSym::INTTK) {
        SyntaxTree* mainDef = MainFuncDef();
        compUnit->addChildNode(mainDef);
    } else {
        errorHandler_.errorMsg("expect a int");
    }
    compUnit_ = compUnit;
    return compUnit;

}
SyntaxTree* Parser::Decl(){
    SyntaxTree *decl = new SyntaxTree();
    decl->symbol_ = SynSym::Decl;
    if(word_.symbol_ == LexSym::CONSTTK) {
        SyntaxTree *constDecl = ConstDecl();
        decl->addChildNode(constDecl);
    } else {
        SyntaxTree *varDecl = VarDecl();
        decl->addChildNode(varDecl);
    }
    return decl;
}
SyntaxTree* Parser::ConstDecl(){
    SyntaxTree *constDecl = new SyntaxTree();
    constDecl->symbol_ = SynSym::ConstDecl;

    SyntaxTree *constTk = end();
    constDecl->addChildNode(constTk);

    SyntaxTree *bType = BType();
    constDecl->addChildNode(bType);

    SyntaxTree *constDef = ConstDef();
    constDecl->addChildNode(constDef);

    while(word_.symbol_ == LexSym::COMMA) {
        SyntaxTree *commaTk = end();
        constDecl->addChildNode(commaTk);

        SyntaxTree *constDef = ConstDef();
        constDecl->addChildNode(constDef);
    }

    if(word_.symbol_ != LexSym::SEMICN) {
        rollBack();
        errorHandler_.addError(word_.line_, MISS_SEMICN);
        next();
//        for(int i = 0; i < constDecl->nodes_.size(); i++) {
//            if(constDecl->nodes_[i]->symbol_ == SynSym::VarDef) {
//                constDecl->nodes_[i]->item_->error_ = true;
//            }
//        }
    } else {
        SyntaxTree *semicn = end();
        constDecl->addChildNode(semicn);
    }

    return constDecl;

}
SyntaxTree* Parser::BType(){
    SyntaxTree* bType = new SyntaxTree();
    bType->symbol_ = SynSym::BType;

    if(word_.symbol_ != LexSym::INTTK) {
        errorHandler_.errorMsg("expect a int");
    }
    SyntaxTree* intTk = end();
    bType->addChildNode(intTk);
    return bType;

}
SyntaxTree* Parser::ConstDef(){
    SyntaxTree* constDef = new SyntaxTree();
    constDef->symbol_ = SynSym::ConstDef;

    SyntaxTree* iden = Ident();
    constDef->addChildNode(iden);
    int dim = 0;
    bool error = false;
    while(word_.symbol_ == LexSym::LBRACK) {
        dim++;
        SyntaxTree* lbrack = end();
        constDef->addChildNode(lbrack);
        SyntaxTree* constExp = ConstExp();
        constDef->addChildNode(constExp);

        if(word_.symbol_ != LexSym::RBRACK ) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RBRACK);
            next();
            error = true;
        } else{
            SyntaxTree* rbrack = end();
            constDef->addChildNode(rbrack);
        }
    }
    constDef->item_ = new Var(iden->first_token(), INTNUM, 1, dim);
    if(word_.symbol_ != LexSym::ASSIGN) {
        errorHandler_.errorMsg("expect a assign");
    }
    constDef->item_->error_ = error;
    SyntaxTree* assign = end();
    constDef->addChildNode(assign);

    SyntaxTree* constInitVal = ConstInitVal();
    constDef->addChildNode(constInitVal);

    return constDef;
}
SyntaxTree* Parser::ConstInitVal(){
    SyntaxTree* constInitVal = new SyntaxTree();
    constInitVal->symbol_ = SynSym::ConstInitVal;

    if(word_.symbol_ == LexSym::LBRACE) {
        SyntaxTree* lbrace = end();
        constInitVal->addChildNode(lbrace);
        if(word_.symbol_ == LexSym::RBRACE) {
            SyntaxTree* rbrace = end();
            constInitVal->addChildNode(rbrace);
        } else {
            SyntaxTree* constInitVal_c1 = ConstInitVal();
            constInitVal->addChildNode(constInitVal_c1);
            while(word_.symbol_ == LexSym::COMMA) {
                SyntaxTree* comma = end();
                constInitVal->addChildNode(comma);
                SyntaxTree* constInitVal_c2 = ConstInitVal();
                constInitVal->addChildNode(constInitVal_c2);
            }

            if(word_.symbol_ != LexSym::RBRACE) {
                errorHandler_.errorMsg("expect a RBRACE");
            }
            SyntaxTree* rbrace = end();
            constInitVal->addChildNode(rbrace);
        }
    } else {
        SyntaxTree* constExp = ConstExp();
        constInitVal->addChildNode(constExp);
    }
    return constInitVal;
}
SyntaxTree* Parser::VarDecl(){
    SyntaxTree* varDecl = new SyntaxTree();
    varDecl->symbol_ = SynSym::VarDecl;

    SyntaxTree* bType = BType();
    varDecl->addChildNode(bType);

    SyntaxTree* varDef = VarDef();
    varDecl->addChildNode(varDef);

    while(word_.symbol_ == LexSym::COMMA) {
        SyntaxTree *commaTk = end();
        varDecl->addChildNode(commaTk);

        SyntaxTree *varDef = VarDef();
        varDecl->addChildNode(varDef);
    }

    if(word_.symbol_ != LexSym::SEMICN) {
        rollBack();
        errorHandler_.addError(word_.line_, MISS_SEMICN);
        next();
//        for(int i = 0; i < varDecl->nodes_.size(); i++) {
//            if(varDecl->nodes_[i]->symbol_ == SynSym::VarDef) {
//                varDecl->nodes_[i]->item_->error_ = true;
//            }
//        }
    } else {
        SyntaxTree *semicn = end();
        varDecl->addChildNode(semicn);
    }
    return varDecl;

}
SyntaxTree* Parser::VarDef(){
    SyntaxTree* varDef = new SyntaxTree();
    varDef->symbol_ = SynSym::VarDef;

    SyntaxTree* iden = Ident();
    varDef->addChildNode(iden);
    int dim = 0;
    bool error = false;
    while(word_.symbol_ == LexSym::LBRACK) {
        dim++;
        SyntaxTree* lbrack = end();
        varDef->addChildNode(lbrack);

        SyntaxTree* constExp = ConstExp();
        varDef->addChildNode(constExp);

        if(word_.symbol_ != LexSym::RBRACK) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RBRACK);
            next();
            error = true;
        } else {
            SyntaxTree* rbrack = end();
            varDef->addChildNode(rbrack);
        }
    }
    varDef->item_ = new Var(iden->first_token(), INTNUM, 0, dim);
    varDef->item_->error_ = error;
    if(word_.symbol_ == LexSym::ASSIGN) {
        SyntaxTree* assign = end();
        varDef->addChildNode(assign);
        SyntaxTree* initVal = InitVal();
        varDef->addChildNode(initVal);
    }
    return varDef;

}
SyntaxTree* Parser::InitVal(){
    SyntaxTree* initVal = new SyntaxTree();
    initVal->symbol_ = SynSym::InitVal;

    if(word_.symbol_ == LexSym::LBRACE) {
        SyntaxTree* lbrace = end();
        initVal->addChildNode(lbrace);
        if(word_.symbol_ == LexSym::RBRACE) {
            SyntaxTree* rbrace = end();
            initVal->addChildNode(rbrace);
        } else {
            SyntaxTree* initVal_c1 = InitVal();
            initVal->addChildNode(initVal_c1);
            while(word_.symbol_ == LexSym::COMMA) {
                SyntaxTree* comma = end();
                initVal->addChildNode(comma);
                SyntaxTree* initVal_c2 = InitVal();
                initVal->addChildNode(initVal_c2);
            }

            if(word_.symbol_ != LexSym::RBRACE) {
                errorHandler_.errorMsg("expect a RBRACE");
            }
            SyntaxTree* rbrace = end();
            initVal->addChildNode(rbrace);
        }
    } else {
        SyntaxTree* exp = Exp();
        initVal->addChildNode(exp);
    }
    return initVal;
}
SyntaxTree* Parser::FuncDef(){
    SyntaxTree* funcDef = new SyntaxTree();
    funcDef->symbol_ = SynSym::FuncDef;
    bool error = false;
    int sum = 0;
    vector<int> dims;
    SyntaxTree* funcType = FuncType();
    funcDef->addChildNode(funcType);
    int dim = funcType->first_token() == "void" ? VOIDDIM : 0;
    SyntaxTree* ident = Ident();
    funcDef->addChildNode(ident);

    SyntaxTree* lparent = end();
    funcDef->addChildNode(lparent);
    int havePara = 0;

    if(word_.symbol_ == LexSym::RPARENT) {
        SyntaxTree* rparent = end();
        funcDef->addChildNode(rparent);
    } else if(word_.symbol_ == LexSym::LBRACE){
        rollBack();
        errorHandler_.addError(word_.line_, MISS_RPARENT);
        next();
        error = true;
    }else {
        havePara = 1;
        SyntaxTree* funcFParams = FuncFParams();
        sum = (funcFParams->nodes_.size() + 1) / 2;
        for(int i = 0; i < sum; i++) {
            dims.push_back(funcFParams->nodes_[2 * i]->item_->dim_);
        }
        funcDef->addChildNode(funcFParams);
        if(word_.symbol_ != LexSym::RPARENT) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RPARENT);
            next();
            error = true;
        } else {
            SyntaxTree* rparent = end();
            funcDef->addChildNode(rparent);
        }
    }
    funcDef->item_ = new Function(ident->first_token(), funcType->first_token() == "void" ? VOID : INTFUNC, dims, dim, sum);
    funcDef->item_->error_ = error;
    SyntaxTree* block = Block();
    block->block_type_ = FUNC_BLOCK;
    funcDef->addChildNode(block);
    return funcDef;
}
SyntaxTree* Parser::MainFuncDef(){
    SyntaxTree* mainFuncDef = new SyntaxTree();
    mainFuncDef->symbol_ = SynSym::MainFuncDef;

    if(word_.symbol_ != LexSym::INTTK) {
        errorHandler_.errorMsg("expect a INTTK");
    } else {
        SyntaxTree* intTk = end();
        mainFuncDef->addChildNode(intTk);
    }

    if(word_.symbol_ != LexSym::MAINTK) {
        errorHandler_.errorMsg("expect a MAINTK");
    } else {
        SyntaxTree* mainTk = end();
        mainFuncDef->addChildNode(mainTk);
    }

    if(word_.symbol_ != LexSym::LPARENT) {
        errorHandler_.errorMsg("expect a LPARENT");
    } else {
        SyntaxTree* lparent = end();
        mainFuncDef->addChildNode(lparent);
    }

    if(word_.symbol_ != LexSym::RPARENT) {
        rollBack();
        errorHandler_.addError(word_.line_, MISS_RPARENT);
        next();
    } else {
        SyntaxTree* rparent = end();
        mainFuncDef->addChildNode(rparent);
    }

    SyntaxTree* block = Block();
    block->block_type_ = FUNC_BLOCK;
    mainFuncDef->addChildNode(block);
    return mainFuncDef;
}
SyntaxTree* Parser::FuncType(){
    SyntaxTree* functype = new SyntaxTree();
    functype->symbol_ = SynSym::FuncType;

    SyntaxTree* type = end();
    functype->addChildNode(type);
    return functype;
}
SyntaxTree* Parser::FuncFParams(){
    SyntaxTree* funcFParams = new SyntaxTree();
    funcFParams->symbol_ = SynSym::FuncFParams;

    SyntaxTree* funcFParam = FuncFParam();
    funcFParams->addChildNode(funcFParam);
    while(word_.symbol_ == LexSym::COMMA) {
        SyntaxTree* comma = end();
        funcFParams->addChildNode(comma);

        SyntaxTree* funcFParam = FuncFParam();
        funcFParams->addChildNode(funcFParam);
    }
    return funcFParams;
}
SyntaxTree* Parser::FuncFParam(){
    SyntaxTree* funcFParam = new SyntaxTree();
    funcFParam->symbol_ = SynSym::FuncFParam;

    SyntaxTree* bType = BType();
    funcFParam->addChildNode(bType);

    SyntaxTree* ident = Ident();
    funcFParam->addChildNode(ident);
    int dim = 0;
    bool error = false;
    if(word_.symbol_ == LexSym::LBRACK) {
        dim++;
        SyntaxTree* lbrack = end();
        funcFParam->addChildNode(lbrack);
        if(word_.symbol_ != LexSym::RBRACK) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RBRACK);
            next();
            error = true;
        } else {
            SyntaxTree* rbrack = end();
            funcFParam->addChildNode(rbrack);
        }
    }

    while(word_.symbol_ == LexSym::LBRACK) {
        dim++;
        SyntaxTree* lbrack = end();
        funcFParam->addChildNode(lbrack);

        SyntaxTree* constExp = ConstExp();
        funcFParam->addChildNode(constExp);

        if(word_.symbol_ != LexSym::RBRACK) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RBRACK);
            next();
            error = true;
        } else {
            SyntaxTree* rbrack = end();
            funcFParam->addChildNode(rbrack);
        }
    }
    funcFParam->item_ = new Var(ident->first_token(), INTNUM, 0, dim);
    funcFParam->item_->error_ = error;
    return funcFParam;
}
SyntaxTree* Parser::Block(){
    SyntaxTree* block = new SyntaxTree();
    block->symbol_ = SynSym::Block;

    if(word_.symbol_ != LexSym::LBRACE) {
        errorHandler_.errorMsg("expect a LBRACE");
    } else {
        SyntaxTree* lbrace = end();
        block->addChildNode(lbrace);
    }

    while(word_.symbol_ != LexSym::RBRACE) {
        SyntaxTree* blockItem = BlockItem();
        block->addChildNode(blockItem);
    }

    SyntaxTree* rbrace = end();
    block->addChildNode(rbrace);

    return block;
}
SyntaxTree* Parser::BlockItem(){
    SyntaxTree* blockItem = new SyntaxTree();
    blockItem->symbol_ = SynSym::BlockItem;

    if(word_.symbol_ == LexSym::CONSTTK || word_.symbol_ == LexSym::INTTK) {
        SyntaxTree* decl = Decl();
        blockItem->addChildNode(decl);
    } else {
        SyntaxTree* stmt = Stmt();
        blockItem->addChildNode(stmt);
    }
    return blockItem;
}
SyntaxTree* Parser::Stmt(){
    SyntaxTree* stmt = new SyntaxTree();
    stmt->symbol_ = SynSym::Stmt;

    if(word_.symbol_ == LexSym::LBRACE) {// Block
        stmt->stmt_type_ = BLOCK_STMT;
        SyntaxTree* block = Block();
        stmt->addChildNode(block);
    }

    else if(word_.symbol_ == LexSym::IFTK) {      // if
        stmt->stmt_type_ = IF_STMT;
        SyntaxTree* ifTk = end();
        stmt->addChildNode(ifTk);
        SyntaxTree* lparent = end();
        stmt->addChildNode(lparent);
        SyntaxTree* cond = Cond();
        stmt->addChildNode(cond);
        if(word_.symbol_ != LexSym::RPARENT) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RPARENT);
            next();
        } else {
            SyntaxTree* rparent = end();
            stmt->addChildNode(rparent);
        }
        SyntaxTree* stmt_c1 = Stmt();
        stmt->addChildNode(stmt_c1);
        if(word_.symbol_ == LexSym::ELSETK) {
            SyntaxTree* elseTk = end();
            stmt->addChildNode(elseTk);
            SyntaxTree* stmt_c2 = Stmt();
            stmt->addChildNode(stmt_c2);
        }
    }

    else if(word_.symbol_ == LexSym::FORTK) {     //for
        stmt->stmt_type_ = FOR_STMT;
        SyntaxTree* forTk = end();
        stmt->addChildNode(forTk);
        SyntaxTree* lparent = end();
        stmt->addChildNode(lparent);

        //first forstmt
        if(word_.symbol_ == LexSym::IDENFR) {
            SyntaxTree* forStmt_1 = ForStmt();
            stmt->addChildNode(forStmt_1);
        }

        if(word_.symbol_ != LexSym::SEMICN) {
            ERROR_HANDLER
        } else {
            SyntaxTree* semicn_1 = end();
            stmt->addChildNode(semicn_1);
        }

        //second cond
        if(word_.symbol_ != LexSym::SEMICN) {
            SyntaxTree* cond = Cond();
            stmt->addChildNode(cond);
        }
        if(word_.symbol_ != LexSym::SEMICN) {
            ERROR_HANDLER
        } else {
            SyntaxTree* semicn_2 = end();
            stmt->addChildNode(semicn_2);
        }

        //third forstmt
        if(word_.symbol_ == LexSym::IDENFR) {
            SyntaxTree* forStmt_2 = ForStmt();
            stmt->addChildNode(forStmt_2);
        }
        if(word_.symbol_ == LexSym::RPARENT) {
            SyntaxTree* rparent = end();
            stmt->addChildNode(rparent);
        } else  {
            ERROR_HANDLER
        }
        SyntaxTree* stmt_c1 = Stmt();
        stmt->addChildNode(stmt_c1);
    }
    //break | continue
    else if(word_.symbol_ == LexSym::BREAKTK || word_.symbol_ == LexSym::CONTINUETK)  {
        stmt->stmt_type_ = word_.symbol_ == LexSym::BREAKTK ? BREAK_STMT : CONTINUE_STMT;
        SyntaxTree* breakOrCon = end();
        stmt->addChildNode(breakOrCon);
        if(word_.symbol_ != LexSym::SEMICN)  {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_SEMICN);
            next();
        } else {
            SyntaxTree* semicn = end();
            stmt->addChildNode(semicn);
        }
    }
    //return
    else if(word_.symbol_ == LexSym::RETURNTK) {
        stmt->stmt_type_ = RETURN_STMT;
        SyntaxTree* returnTk = end();
        stmt->addChildNode(returnTk);
        if(word_.symbol_ != LexSym::SEMICN &&
        ( word_.symbol_ == LexSym::IDENFR || word_.symbol_ == LexSym::PLUS
        || word_.symbol_ == LexSym::MINU || word_.symbol_ == LexSym::INTCON
        || word_.symbol_ == LexSym::LPARENT)) {
            SyntaxTree *exp = Exp();
            stmt->addChildNode(exp);
        }
        if(word_.symbol_ != LexSym::SEMICN) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_SEMICN);
            next();
        } else {
            SyntaxTree* semicn = end();
            stmt->addChildNode(semicn);
        }
    }
    // printf
    else if(word_.symbol_ == LexSym::PRINTFTK)  {
        stmt->stmt_type_ = PRINTF_STMT;
        SyntaxTree* printfTk = end();
        stmt->addChildNode(printfTk);
        SyntaxTree* lparent = end();
        stmt->addChildNode(lparent);
        SyntaxTree* formatString = FormatString();
        stmt->addChildNode(formatString);
        while(word_.symbol_ == LexSym::COMMA) {
            SyntaxTree* comma = end();
            stmt->addChildNode(comma);
            SyntaxTree* exp = Exp();
            stmt->addChildNode(exp);
        }

        if(word_.symbol_ != LexSym::RPARENT) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RPARENT);
            next();
        } else {
            SyntaxTree* rparent = end();
            stmt->addChildNode(rparent);
        }

        if(word_.symbol_ != LexSym::SEMICN) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_SEMICN);
            next();
        } else {
            SyntaxTree* semicn = end();
            stmt->addChildNode(semicn);
        }
    }
    // LVal = EXP | EXP | getint
    else {
        if(word_.symbol_ == LexSym::SEMICN) {     //[exp] ; but no exp
            stmt ->stmt_type_ = EXP_STMT;
            SyntaxTree* semicn = end();
            stmt->addChildNode(semicn);
        }
        else {
            int flag = pos_;
            SyntaxTree* exp = Exp();
            if(word_.symbol_ == LexSym::SEMICN) {
                stmt->stmt_type_ = EXP_STMT;
                SyntaxTree* semicn = end();
                stmt->addChildNode(exp);
                stmt->addChildNode(semicn);
            } else if(word_.symbol_ != LexSym::ASSIGN) {
                stmt->addChildNode(exp);
                rollBack();
                errorHandler_.addError(word_.line_, MISS_SEMICN);
                next();
            }
            else {
                next();
                if(word_.symbol_ == LexSym::GETINTTK) {
                    stmt->stmt_type_ = GETINT_STMT;
                    while(pos_ != flag) {
                        rollBack();
                    }
                    SyntaxTree* lVal = LVal();
                    stmt->addChildNode(lVal);
                    SyntaxTree* assign = end();
                    stmt->addChildNode(assign);
                    SyntaxTree* getIntTk = end();
                    stmt->addChildNode(getIntTk);
                    SyntaxTree* lparent = end();
                    stmt->addChildNode(lparent);
                    if(word_.symbol_ != LexSym::RPARENT) {
                        rollBack();
                        errorHandler_.addError(word_.line_, MISS_RPARENT);
                        next();
                    } else {
                        SyntaxTree* rparent = end();
                        stmt->addChildNode(rparent);
                    }

                    if(word_.symbol_ != LexSym::SEMICN) {
                        rollBack();
                        errorHandler_.addError(word_.line_, MISS_SEMICN);
                        next();
                    } else {
                        SyntaxTree* semicn = end();
                        stmt->addChildNode(semicn);
                    }

                } else {
                    stmt->stmt_type_ = ASSIGN_STMT;
                    while(pos_ != flag) {
                        rollBack();
                    }
                    SyntaxTree* lVal = LVal();
                    stmt->addChildNode(lVal);
                    SyntaxTree* assign = end();
                    stmt->addChildNode(assign);
                    SyntaxTree* exp = Exp();
                    stmt->addChildNode(exp);
                    if(word_.symbol_ != LexSym::SEMICN) {
                        rollBack();
                        errorHandler_.addError(word_.line_, MISS_SEMICN);
                        next();
                    } else {
                        SyntaxTree* semicn = end();
                        stmt->addChildNode(semicn);
                    }
                }
            }
        }
    }

    return stmt;
}
SyntaxTree* Parser::ForStmt(){
    SyntaxTree* forStmt = new SyntaxTree();
    forStmt->symbol_ = SynSym::ForStmt;

    SyntaxTree* lVal = LVal();
    forStmt->addChildNode(lVal);

    SyntaxTree* assign = end();
    forStmt->addChildNode(assign);

    SyntaxTree* exp = Exp();
    forStmt->addChildNode(exp);
    return forStmt;
}
SyntaxTree* Parser::Exp(){
    SyntaxTree* exp = new SyntaxTree();
    exp->item_ = new Exps();
    exp->symbol_ = SynSym::Exp;
    SyntaxTree* addExp = AddExp();
    exp->addChildNode(addExp);
    return exp;
}
SyntaxTree* Parser::Cond(){
    SyntaxTree* cond = new SyntaxTree();
    cond->symbol_ = SynSym::Cond;
    SyntaxTree* lOrExp = LOrExp();
    cond->addChildNode(lOrExp);
    return cond;
}
SyntaxTree* Parser::LVal(){
    SyntaxTree* lVal = new SyntaxTree();
    lVal->symbol_ = SynSym::LVal;
    SyntaxTree* ident = Ident();
    lVal->addChildNode(ident);
    int dim = 0;
    while(word_.symbol_ == LexSym::LBRACK) {
        dim++;
        SyntaxTree* lbrack = end();
        lVal->addChildNode(lbrack);
        SyntaxTree* exp = Exp();
        lVal->addChildNode(exp);
        if(word_.symbol_ != LexSym::RBRACK) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RBRACK);
            next();
        } else{
            SyntaxTree* rbrack = end();
            lVal->addChildNode(rbrack);
        }
    }
    TableItem* var = new Var(ident->first_token(), INTNUM,0, dim);
    lVal->item_ = var;
    return lVal;
}
SyntaxTree* Parser::PrimaryExp(){
    SyntaxTree* primaryExp = new SyntaxTree();
    primaryExp->symbol_ = SynSym::PrimaryExp;
    primaryExp->item_ = new Exps();
    if(word_.symbol_ == LexSym::LPARENT) {
        SyntaxTree* lparent = end();
        primaryExp->addChildNode(lparent);
        SyntaxTree* exp = Exp();
        primaryExp->addChildNode(exp);
        if(word_.symbol_ != LexSym::RPARENT) {
            rollBack();
            errorHandler_.addError(word_.line_, MISS_RPARENT);
            next();
        } else {
            SyntaxTree* rparent = end();
            primaryExp->addChildNode(rparent);
        }
    } else if(word_.symbol_ == LexSym::IDENFR) {
        SyntaxTree* lVal = LVal();
        primaryExp->addChildNode(lVal);
    } else {
        SyntaxTree* number =Number();
        primaryExp->addChildNode(number);
    }
    return primaryExp;
}
SyntaxTree* Parser::Number(){
    SyntaxTree* number = new SyntaxTree();
    number->item_ = new Exps();
    number->symbol_ = SynSym::Number;
    SyntaxTree* intConst = IntConst();
    number->addChildNode(intConst);
    return number;
}
SyntaxTree* Parser::UnaryExp(){
    SyntaxTree* unaryExp = new SyntaxTree();
    unaryExp->item_ = new Exps();
    unaryExp->symbol_ = SynSym::UnaryExp;
    if(word_.symbol_ == LexSym::PLUS || word_.symbol_ == LexSym::MINU || word_.symbol_ == LexSym::NOT) {
        SyntaxTree* unaryOp = UnaryOp();
        unaryExp->addChildNode(unaryOp);
        SyntaxTree* unaryExp_c1 = UnaryExp();
        unaryExp->addChildNode(unaryExp_c1);
        return  unaryExp;
    }
    else if(word_.symbol_ == LexSym::IDENFR) {
        next();
        if(word_.symbol_ == LexSym::LPARENT) {
            rollBack();
            SyntaxTree* ident = Ident();
            unaryExp->addChildNode(ident);
            SyntaxTree* lparent = end();
            unaryExp->addChildNode(lparent);
            if(word_.symbol_ != LexSym::RPARENT && word_.symbol_ != LexSym::SEMICN) {
                SyntaxTree* funcRParams = FuncRParams();
                unaryExp->addChildNode(funcRParams);
            }
            if(word_.symbol_ != LexSym::RPARENT) {
                rollBack();
                errorHandler_.addError(word_.line_, MISS_RPARENT);
                next();
            } else {
                SyntaxTree* rparent = end();
                unaryExp->addChildNode(rparent);
            }
            return unaryExp;
        }
        rollBack();
    }

    SyntaxTree* primaryExp = PrimaryExp();
    unaryExp->addChildNode(primaryExp);
    return unaryExp;
}
SyntaxTree* Parser::UnaryOp(){
    SyntaxTree* unaryOp = new SyntaxTree();
    unaryOp->symbol_ = SynSym::UnaryOp;
    if(!(word_.symbol_ == LexSym::PLUS || word_.symbol_ == LexSym::MINU || word_.symbol_ == LexSym::NOT)) {
        errorHandler_.errorMsg("expect a unaryOp");
    } else {
        SyntaxTree* op = end();
        unaryOp->addChildNode(op);
    }
    return unaryOp;
}
SyntaxTree* Parser::FuncRParams(){
    SyntaxTree* funcRParams = new SyntaxTree();
    funcRParams->symbol_ = SynSym::FuncRParams;
    SyntaxTree* exp = Exp();
    funcRParams->addChildNode(exp);
    while(word_.symbol_ == LexSym::COMMA) {
        SyntaxTree* comma = end();
        funcRParams->addChildNode(comma);
        SyntaxTree* exp = Exp();
        funcRParams->addChildNode(exp);
    }
    return funcRParams;
}
SyntaxTree* Parser::MulExp(){
    SyntaxTree* mulExp = new SyntaxTree();
    mulExp->item_ = new Exps();
    mulExp->symbol_ = SynSym::MulExp;
    SyntaxTree* unaryExp = UnaryExp();
    mulExp->addChildNode(unaryExp);
    while(word_.symbol_ == LexSym::MULT || word_.symbol_ == LexSym::DIV || word_.symbol_ == LexSym::MOD) {
        SyntaxTree* op = end();
        SyntaxTree* unaryExp_c = UnaryExp();
        SyntaxTree* mulExp_c = mulExp;

        mulExp = new SyntaxTree();
        mulExp->item_ = new Exps();

        mulExp->symbol_ = SynSym::MulExp;
        mulExp->addChildNode(mulExp_c);
        mulExp->addChildNode(op);
        mulExp->addChildNode(unaryExp_c);
    }
    return mulExp;
}
SyntaxTree* Parser::AddExp(){
    SyntaxTree* addExp = new SyntaxTree();
    addExp->item_ = new Exps();
    addExp->symbol_ = SynSym::AddExp;
    SyntaxTree* mulExp = MulExp();
    addExp->addChildNode(mulExp);
    while(word_.symbol_ == LexSym::PLUS || word_.symbol_ == LexSym::MINU) {
        SyntaxTree* op = end();
        SyntaxTree* mulExp_c = MulExp();
        SyntaxTree* addExp_c = addExp;

        addExp = new SyntaxTree();
        addExp->item_ = new Exps();

        addExp->symbol_ = SynSym::AddExp;
        addExp->addChildNode(addExp_c);
        addExp->addChildNode(op);
        addExp->addChildNode(mulExp_c);
    }
    return addExp;
}
SyntaxTree* Parser::RelExp(){
    SyntaxTree* relExp = new SyntaxTree();
    relExp->item_ = new Exps();
    relExp->symbol_ = SynSym::RelExp;
    SyntaxTree* addExp = AddExp();
    relExp->addChildNode(addExp);
    while(word_.symbol_ == LexSym::LSS || word_.symbol_ == LexSym::LEQ || word_.symbol_ == LexSym::GRE || word_.symbol_ == LexSym::GEQ) {
        SyntaxTree* op = end();
        SyntaxTree* addExp_c = AddExp();
        SyntaxTree* relExp_c = relExp;
        relExp = new SyntaxTree();
        relExp->item_ = new Exps();
        relExp->symbol_ = SynSym::RelExp;
        relExp->addChildNode(relExp_c);
        relExp->addChildNode(op);
        relExp->addChildNode(addExp_c);
    }
    return relExp;
}
SyntaxTree* Parser::EqExp(){
    SyntaxTree* eqExp = new SyntaxTree();
    eqExp->item_ = new Exps();
    eqExp->symbol_ = SynSym::EqExp;
    SyntaxTree* relExp = RelExp();
    eqExp->addChildNode(relExp);
    while(word_.symbol_ == LexSym::EQL || word_.symbol_ == LexSym::NEQ) {
        SyntaxTree* op = end();
        SyntaxTree* relExp_c = RelExp();
        SyntaxTree* eqExp_c = eqExp;
        eqExp = new SyntaxTree();
        eqExp->item_ = new Exps();
        eqExp->symbol_ = SynSym::EqExp;
        eqExp->addChildNode(eqExp_c);
        eqExp->addChildNode(op);
        eqExp->addChildNode(relExp_c);
    }
    return eqExp;
}
SyntaxTree* Parser::LAndExp(){
    SyntaxTree* lAndExp = new SyntaxTree();
    lAndExp->item_ = new Exps();
    lAndExp->symbol_ = SynSym::LAndExp;
    SyntaxTree* eqExp = EqExp();
    lAndExp->addChildNode(eqExp);
    while(word_.symbol_ == LexSym::AND) {
        SyntaxTree* op = end();
        SyntaxTree* eqExp_c = EqExp();
        lAndExp->addChildNode(eqExp_c);
    }
    return lAndExp;
}
SyntaxTree* Parser::LOrExp(){
    SyntaxTree* lOrExp = new SyntaxTree();
    lOrExp->item_ = new Exps();
    lOrExp->symbol_ = SynSym::LOrExp;
    SyntaxTree* lAndExp = LAndExp();
    lOrExp->addChildNode(lAndExp);
    while(word_.symbol_ == LexSym::OR) {
        SyntaxTree* op = end();
        SyntaxTree* lAndExp_c = LAndExp();
        lOrExp->addChildNode(lAndExp_c);
    }
    return lOrExp;
}
SyntaxTree* Parser::ConstExp(){
    SyntaxTree* constExp = new SyntaxTree();
    constExp->item_ = new Exps();
    constExp->symbol_ = SynSym::ConstExp;
    SyntaxTree* addExp = AddExp();
    constExp->addChildNode(addExp);
    return constExp;
}
