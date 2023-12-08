//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_VALUE_H
#define COMPLIER_IR_VALUE_H
#include "irPreDeclHeader.h"
#include "Types/ValueType.h"       // why
#include <fstream>
#include <exception>
#define VALUECLASS "ValueClass"
using namespace std;
class  Value{
protected:
    const int id_;
    string name_;
    Value* parent_;
    ValueType* valueType_;
    vector<User*> users_;
    int remainUsage_;
public:
    Value(string name, ValueType* valueType, int id, Value* parent);
    virtual string getClassName();
    int getId() const;
    virtual string getName();
    bool operator==(const Value& other) const;
    bool operator<(const Value& other) const;
    ValueType* getValType();
    void setVatType(ValueType* valueType);
    virtual string print();
    bool isInt32();
    bool isInt1();
    void addUser(User* user);
    void replaceSelf(Value* newValue);
    size_t operator()(const Value& obj) const {
        return std::hash<int>()(obj.id_);
    }
    Value* getParent();
    vector<User*>& getUsers();
    void removeUser(User* user);
};

#endif