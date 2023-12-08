//
// Created by Han Zhengyang on 2023/11/2.
//

#ifndef COMPLIER_IR_CONSTARRAY_H
#define COMPLIER_IR_CONSTARRAY_H

#include "ConstInt.h"
#include "ZeroInitializer.h"
#define CONSTARRAYCLASS "ConstArrayClass"
class ConstArray : public ConstVal{
private:
    vector<ConstVal*> elements_;
public:
    ConstArray(vector<ConstVal*> elements, int id);
    string print()override;
    string getClassName() override ;
    void expandEle(vector<int>* expandedEles);
    vector<int>* getExpandedEles();
    ConstVal* getEle(int index);
    int getEleData(int index1, int index2)override ;

};


#endif //COMPLIER_IR_CONSTARRAY_H
