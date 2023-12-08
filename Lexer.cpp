//
// Created by Han Zhengyang on 2023/9/23.
//

#include "Lexer.h"
#include <algorithm>
#define SET_CH ch=source.at(index);
#define READ_CH token+=ch;index++;
Word ::Word(string token, LexSym symbol, int line) {
    token_ = token;
    symbol_ = symbol;
    line_ = line;
}

Lexer ::Lexer(string testfile, ErrorHandler &errorHandler) : source(std::move(testfile)), errorHandler_(errorHandler) {
    length = source.length();
}
bool Lexer ::isEnd() {
    return (index >= length);
}
void Lexer ::skipSpace() {
    char ch;
    SET_CH
    while(isspace(ch) && !isEnd()) {
        if(ch == '\n') {
            line++;
        }
        index++;
        if(isEnd()) {
            return;
        }
        SET_CH
    }
}
void Lexer ::error() {
    cout << line << endl;
}
LexSym Lexer ::isKeyWord(string word) {
    auto it = find(keyWords.begin(), keyWords.end(), word);
    if(it == keyWords.end()) {
        return LexSym::EMPTY;
    }
    else{
        if (word == "main") {
            return LexSym::MAINTK;
        } else if (word == "const") {
            return LexSym::CONSTTK;
        } else if (word == "int") {
            return LexSym::INTTK;
        } else if (word == "break") {
            return LexSym::BREAKTK;
        } else if (word == "continue") {
            return LexSym::CONTINUETK;
        } else if (word == "if") {
            return LexSym::IFTK;
        } else if (word == "else") {
            return LexSym::ELSETK;
        } else if (word == "for") {
            return LexSym::FORTK;
        } else if (word == "getint") {
            return LexSym::GETINTTK;
        } else if (word == "printf") {
            return LexSym::PRINTFTK;
        } else if (word == "return") {
            return LexSym::RETURNTK;
        } else if (word == "void") {
            return LexSym::VOIDTK;
        }else {
            return LexSym ::EMPTY;
        }
    }
}
void Lexer ::next() {
    token = "";
    char ch = '\0';
    symbol = LexSym ::EMPTY;
    if(isEnd()) {
        return;
    }
    SET_CH
    //    if(index == 68) {
//        cout << ch << endl;
//    }
    skipSpace();
    if(isEnd()) {
        return;
    }
    SET_CH
    while(ch == '/') {
        if(index + 1 < length) {
            if(source.at(index + 1) == '/') {
                index += 2;
                while(!isEnd() && source.at(index) != '\n') {
                    index++;
                }
                if(isEnd()) {
                    symbol = LexSym ::EMPTY;
                    return;
                }
                else if(source.at(index) == '\n') {
                    line++;
                    index++;
                }
            }
            else if(source.at(index + 1) == '*') {
                index += 2;
                while(index + 1 < length) {
                    if(source.at(index) == '\n') {
                        line++;
                    }
                    if(source.at(index) == '*' && source.at(index + 1) == '/') {
                        break;
                    }
                    index++;
                }
                if(source.at(index) == '*' && source.at(index + 1) == '/') {
                    index += 2;
                } else{
                    symbol = LexSym ::EMPTY;
                    return;
                }
            }
            else{
                break;
            }
            SET_CH
            skipSpace();
            if(isEnd()) {
                symbol = LexSym ::EMPTY;
                return;
            }
            SET_CH
        }
        else{
            break;
        }
    }
    if(ch == '_' || isalpha(ch)) {                        // ident or keyWord
        while(ch == '_' || isalpha(ch) || isdigit(ch)) {
            READ_CH
            SET_CH
        }
        symbol = isKeyWord(token);
        if(symbol == LexSym::EMPTY) {
            symbol = LexSym::IDENFR;
        }
    }
    else if(isdigit(ch)) {
        while(isdigit(ch)) {
            READ_CH
            SET_CH
        }
        symbol = LexSym ::INTCON;
    }
    else if(ch == '\"') {                     //formatString
        READ_CH
        SET_CH
        while(ch != '\"' && !isEnd()) {
            READ_CH
            SET_CH
        }
        if(!isEnd()) {
            READ_CH
        }
        symbol = LexSym ::STRCON;
    }
    else if(ch == '!') {
        READ_CH
        SET_CH
        if(ch == '=') {
            READ_CH
            symbol = LexSym ::NEQ;
        }
        else {
            symbol = LexSym ::NOT;
        }
    }
    else if(ch == '&') {
        READ_CH
        SET_CH
        if(isEnd() || ch != '&') {
            error();
        }
        else {
            READ_CH
            symbol = LexSym ::AND;
        }
    }
    else if(ch == '|') {
        READ_CH
        SET_CH
        if(isEnd() || ch != '|') {
            error();
        }
        else {
            READ_CH
            symbol = LexSym ::OR;
        }
    }
    else if(ch == '+') {
        READ_CH
        symbol = LexSym ::PLUS;
    }
    else if(ch == '-') {
        READ_CH
        symbol = LexSym ::MINU;
    }
    else if(ch == '*') {
        READ_CH
        symbol = LexSym ::MULT;
    }
    else if(ch == '/') {
        READ_CH
        symbol = LexSym ::DIV;
    }
    else if(ch == '%') {
        READ_CH
        symbol = LexSym ::MOD;
    }
    else if(ch == '<') {
        READ_CH
        SET_CH
        if(ch == '=') {
            READ_CH
            symbol = LexSym ::LEQ;
        }
        else {
            symbol = LexSym ::LSS;
        }
    }
    else if(ch == '>') {
        READ_CH
        SET_CH
        if(ch == '=') {
            READ_CH
            symbol = LexSym ::GEQ;
        }
        else {
            symbol = LexSym ::GRE;
        }
    }
    else if(ch == '=') {
        READ_CH
        SET_CH
        if(ch == '=') {
            READ_CH
            symbol = LexSym ::EQL;
        }
        else {
            symbol = LexSym ::ASSIGN;
        }
    }
    else if(ch == ';') {
        READ_CH
        symbol = LexSym ::SEMICN;
    }
    else if(ch == ',') {
        READ_CH
        symbol = LexSym ::COMMA;
    }
    else if(ch == '(') {
        READ_CH
        symbol = LexSym ::LPARENT;
    }
    else if(ch == ')') {
        READ_CH
        symbol = LexSym ::RPARENT;
    }
    else if(ch == '[') {
        READ_CH
        symbol = LexSym ::LBRACK;
    }
    else if(ch == ']') {
        READ_CH
        symbol = LexSym ::RBRACK;
    }
    else if(ch == '{') {
        READ_CH
        symbol = LexSym ::LBRACE;
    }
    else if(ch == '}') {
        READ_CH
        symbol = LexSym ::RBRACE;
    }else if(ch == '\0') {
        symbol = LexSym ::EMPTY;
    }
    else {
        error();
    }
    //cout << token << endl;


}