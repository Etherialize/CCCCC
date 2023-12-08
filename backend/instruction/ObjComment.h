//
// Created by Han Zhengyang on 2023/11/21.
//

#ifndef COMPLIER_MIPS_OBJCOMMENT_H
#define COMPLIER_MIPS_OBJCOMMENT_H

#include "ObjInstruction.h"
#define OBJCOMMENTCLASS "objCommentClass"
/*
 * 宏和注释
 */
class ObjComment : public ObjInstruction{
private:
    string content_;
    bool isComment_;
public:
    ObjComment(string content):content_(content){
        isComment_ = true;
    }
    ObjComment(string content, bool isComment):content_(content), isComment_(isComment) {}
    string getClassName() override { return OBJCOMMENTCLASS;}
    string print() override {
        if(isComment_) {
            return "# " + content_ + "\n";
        } else {
            return content_ + "\n";
        }
    }

};


#endif //COMPLIER_MIPS_OBJCOMMENT_H
