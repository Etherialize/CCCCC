//
// Created by Han Zhengyang on 2023/11/5.
//

#ifndef COMPLIER_IR_GETELEPOINTER_H
#define COMPLIER_IR_GETELEPOINTER_H

#include "Instruction.h"
#define GEPCLASS "GEPClass"
class GetElePointer : public Instruction{
private:
    // alloca的pointerType所指向的类型
    ValueType* baseType_;
    vector<int> index_;
public:
    /* 强制规定，取址用一行解决
     * 如 a[2][3] 取a[1] ：getelementptr [2 x [3 x i32]], [2 x [3 x i32]]* @a, i32 0, i32 1
     *           取a[1][1]：getelementptr [2 x [3 x i32]], [2 x [3 x i32]]* @a, i32 0, i32 1，i32 1*/
    /*
     * 当有flag的时候，说明这时候的数组是函数参数
     */
    GetElePointer(int nameNum, int id, BasicBlock* parent, Value* value, int flag);
    GetElePointer(int nameNum, int id, BasicBlock* parent, Value* value, Value* index1, int flag);
    GetElePointer(int nameNum, int id, BasicBlock* parent, Value* value, Value* index1, Value* index2, int flag);
    string print()override ;
    string getClassName() override {
        return GEPCLASS;
    }
};


#endif //COMPLIER_IR_GETELEPOINTER_H
