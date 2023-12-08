//
// Created by Han Zhengyang on 2023/10/31.
//
#ifndef COMPLIER_IR_USER_H
#define COMPLIER_IR_USER_H
#include "irPreDeclHeader.h"
#include "Value.h"
#define USERCLASS "UserClass"
class  User : public Value{
protected:
    vector<Value*> operands_;
public:
    User(string name, ValueType* valueType, int id, Value* parent);
    User(string name, ValueType* valueType, int id, Value* parent, Value* value1);
    User(string name, ValueType* valueType, int id, Value* parent, Value* value1, Value* value2);
    User(string name, ValueType* valueType, int id, Value* parent, Value* value1, Value* value2, Value* value3);
    virtual string getClassName();
    void addUsedValues(vector<Value*> values);
    void addUsedValue(Value* value);
    void insertValue(Value* value, int pos);
    int getUsedValueSum();
    Value* getUsedValue(int index);
    vector<Value*> getUsedValues() {
        return operands_;
    }
    void replaceUsedValue(Value* oldValue, Value* newValue);
    void removeAllUsedValue();

};

#endif