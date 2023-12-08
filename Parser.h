//
// Created by Han Zhengyang on 2023/10/7.
//
#pragma once

#include "Header.h"
#include "Lexer.h"

#define NOT_STMT -1
#define ASSIGN_STMT 0
#define EXP_STMT 1
#define BLOCK_STMT 2
#define IF_STMT 3
#define FOR_STMT 4
#define BREAK_STMT 5
#define CONTINUE_STMT 6
#define RETURN_STMT 7
#define GETINT_STMT 8
#define PRINTF_STMT 9
#define FUNC_BLOCK 1
enum class SynSym {
    CompUnit, Decl, ConstDecl, BType, ConstDef, ConstInitVal, VarDecl, VarDef, InitVal, FuncDef, MainFuncDef,
    FuncType, FuncFParams, FuncFParam, Block, BlockItem, Stmt, ForStmt, Exp, Cond, LVal,
    PrimaryExp, Number, UnaryExp, UnaryOp, FuncRParams, MulExp, AddExp, RelExp, EqExp,
    LAndExp, LOrExp,  ConstExp, Ident, FormatString, IntConst, End
};
class SyntaxTree {
public:
    vector<SyntaxTree *> nodes_;
    SynSym symbol_;
    Word word_;

    int stmt_type_ = -1;
    int block_type_ = 0;  //1 is func
    TableItem* item_;
public:
    SyntaxTree();
    void addChildNode(SyntaxTree *node);
    SynSym getSynSym();
    TableItem* getItem();
    void setVal(int value);
    int getVal();
    int getStmtType();
    int getChildSum();
    SyntaxTree* getChildAt(int index);
    string first_token();
    int line();
};
class Parser {
public:
    SyntaxTree* compUnit_;
    Lexer &lexer_;
    ErrorHandler &errorHandler_;
    ofstream &output_;
    vector<Word> words_;
    int length_;
    int pos_ = 0;
    Word word_;
public:
    Parser(Lexer &lexer, ErrorHandler &errorHandler, ofstream &output);
    void preLexer();
    void next();
    void rollBack();
    void error();
    void print(SyntaxTree* node);
    SyntaxTree* end();
    SyntaxTree* Ident();
    SyntaxTree* FormatString();
    SyntaxTree* IntConst();
    SyntaxTree* CompUnit();
    SyntaxTree* Decl();
    SyntaxTree* ConstDecl();
    SyntaxTree* BType();
    SyntaxTree* ConstDef();
    SyntaxTree* ConstInitVal();
    SyntaxTree* VarDecl();
    SyntaxTree* VarDef();
    SyntaxTree* InitVal();
    SyntaxTree* FuncDef();
    SyntaxTree* MainFuncDef();
    SyntaxTree* FuncType();
    SyntaxTree* FuncFParams();
    SyntaxTree* FuncFParam();
    SyntaxTree* Block();
    SyntaxTree* BlockItem();
    SyntaxTree* Stmt();
    SyntaxTree* ForStmt();
    SyntaxTree* Exp();
    SyntaxTree* Cond();
    SyntaxTree* LVal();
    SyntaxTree* PrimaryExp();
    SyntaxTree* Number();
    SyntaxTree* UnaryExp();
    SyntaxTree* UnaryOp();
    SyntaxTree* FuncRParams();
    SyntaxTree* MulExp();
    SyntaxTree* AddExp();
    SyntaxTree* RelExp();
    SyntaxTree* EqExp();
    SyntaxTree* LAndExp();
    SyntaxTree* LOrExp();
    SyntaxTree* ConstExp();

};


