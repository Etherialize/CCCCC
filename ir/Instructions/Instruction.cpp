//
// Created by Han Zhengyang on 2023/11/1.
//

#include "Instruction.h"
#include "../BasicBlock.h"
#include <string>
Instruction::Instruction(string name, DataType* dateType, int id, BasicBlock* parent):User(name, dateType, id, parent){}
Instruction::Instruction(string name, DataType* dateType, int id, BasicBlock* parent, Value* value1):User(name, dateType, id, parent, value1){}
Instruction::Instruction(string name, DataType* dateType, int id, BasicBlock* parent, Value* value1, Value* value2):User(name, dateType, id, parent, value1, value2){}
Instruction::Instruction(string name, DataType* dateType, int id, BasicBlock* parent, Value* value1, Value* value2, Value* value3):User(name, dateType, id, parent, value1, value2, value3){}
