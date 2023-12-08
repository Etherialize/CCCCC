//
// Created by Han Zhengyang on 2023/10/31.
//

#include "User.h"
#include "Types/typesHeader.h"

User::User(string name, ValueType* valueType, int id, Value* parent): Value(name, valueType, id, parent){}

User::User(string name, ValueType* valueType, int id, Value* parent, Value* value1):Value(name, valueType, id, parent){
    addUsedValue(value1);
}
User::User(string name, ValueType* valueType, int id, Value* parent, Value* value1, Value* value2):Value(name, valueType, id, parent){
    addUsedValue(value1);
    addUsedValue(value2);
}
User::User(string name, ValueType* valueType, int id, Value* parent, Value* value1, Value* value2, Value* value3):Value(name, valueType, id, parent){
    addUsedValue(value1);
    addUsedValue(value2);
    addUsedValue(value3);
}
string User::getClassName(){
    return USERCLASS;
}
Value* User:: getUsedValue(int index) {
    if(index >= operands_.size()){
        return nullptr;
    } else {
        return operands_.at(index);
    }
}
int User::getUsedValueSum() {
    return operands_.size();
}
void User:: addUsedValues(vector<Value *> values) {
    for(int i = 0; i < values.size(); i++) {
        addUsedValue(values[i]);
    }
}
void User::addUsedValue(Value* value) {
    operands_.push_back(value);
    if(value != nullptr) {
        value->addUser(this);
    }
}
void User::insertValue(Value* value, int pos) {
    operands_.insert(operands_.begin() + pos, value);
    if(value != nullptr) {
        value->addUser(this);
    }
}
void User::replaceUsedValue(Value* oldValue, Value* newValue) {
    for(int i = 0; i < operands_.size(); i++) {
        if(operands_[i] != nullptr && oldValue->getId() == operands_[i]->getId()) {
            operands_[i] = newValue;
            newValue->addUser(this);
        }
    }
}
void User::removeAllUsedValue() {
    for(auto it : operands_) {
        if(it != nullptr) {
            it->removeUser(this);
        }
    }
}