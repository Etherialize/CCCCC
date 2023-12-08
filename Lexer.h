//
// Created by Han Zhengyang on 2023/9/23.
//
#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;
#include "Header.h"


enum class LexSym {
    IDENFR, INTCON, STRCON, MAINTK, CONSTTK, INTTK, BREAKTK, CONTINUETK, IFTK, ELSETK,
    NOT, AND, OR, FORTK, GETINTTK, PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK, MULT, DIV, MOD,
    LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK,
    LBRACE, RBRACE, EMPTY
};
class Lexer {
public:
    string source;
    int length;
    int index = 0;
    string token;
    LexSym symbol = LexSym::EMPTY;
    int line = 1;
    int num;
    vector<string> keyWords = {"main", "const", "int", "break", "continue", "if", "else", "for", "getint", "printf", "return" ,"void"};
    ErrorHandler &errorHandler_;

public:
    Lexer(string testfile, ErrorHandler &errorHandler);
    void next();
    bool isEnd();
    void skipSpace();
    LexSym isKeyWord(string word);
    void error();
};
class Word {
public:
    string token_;
    LexSym symbol_;
    int line_;
public:
    Word(string token, LexSym symbol, int line);
};
