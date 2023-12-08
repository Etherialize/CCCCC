//
// Created by Han Zhengyang on 2023/10/31.
//

#include "Value.h"
#include "User.h"
#include "Types/typesHeader.h"
Value:: Value(string name, ValueType* valueType, int id, Value* parent): name_(name), valueType_(valueType), id_(id), parent_(parent){
    remainUsage_ = 0;
}
int Value:: getId() const {
    return id_;
}
string Value::print(){
    return "value, but why this happen";
}
string Value:: getClassName(){
    return VALUECLASS;
}
string Value:: getName(){
    return name_;
}
bool Value::operator==(const Value& other) const{
    return id_ == other.getId();
}
bool Value::operator<(const Value& other) const{
    return id_ < other.getId();
}
ValueType* Value:: getValType() {
    return valueType_;
}
void Value::setVatType(ValueType* valueType) {
    valueType_ = valueType;
}
bool Value::isInt32() {
    return valueType_->toString() == INTTYPE && ((IntType*) valueType_)->getBits() == 32;
}
bool Value::isInt1() {
    return valueType_->toString() == INTTYPE && ((IntType*) valueType_)->getBits() == 1;
}
void Value::addUser(User* user) {
    users_.push_back(user);
    remainUsage_++;
}
void Value::replaceSelf(Value* newValue) {
    for(int i = 0; i < users_.size(); i++) {
        users_[i]->replaceUsedValue(this, newValue);
    }
}
Value* Value::getParent() {return parent_;}
vector<User*>& Value::getUsers(){return users_;}
void Value::removeUser(User* user) {
    int flag = -1;
    for(int i = 0; i < users_.size(); i++) {
        if(users_[i]->getId() == user->getId()) {
            flag = i;
            break;
        }
    }
    auto it = users_.begin() + flag;
    users_.erase(it);
}