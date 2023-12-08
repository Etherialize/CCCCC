//
// Created by Han Zhengyang on 2023/12/5.
//

#ifndef COMPLIER_MIPS_GRAPHCOLOR_H
#define COMPLIER_MIPS_GRAPHCOLOR_H

#include "BlockLiveness.h"
#include "ObjPair.h"
class GraphColor {
private:
    ObjModule* module_;
    int const K = ObjPhyReg::allocableRegs_.size();
    unordered_map<ObjBlock*, BlockLiveness*>* livenessMap_ = nullptr;
    /*
     * 图信息
     */
    unordered_map<ObjReg*, unordered_set<ObjMove*>,Reg_hash, Reg_cmp> moveList_;
    /*
     *  邻接表
     */
    unordered_set<ObjPair*, ObjPair_hash, ObjPair_cmp> adjSet_;
    /*
     * 传送指令被合并，src被放到coalesceedNodes中，alias(src) = dst
     */
    unordered_map<ObjReg*, ObjReg*, Reg_hash, Reg_cmp> alias_;

    unordered_map<ObjReg*, unordered_set<ObjReg*,Reg_hash, Reg_cmp>,Reg_hash, Reg_cmp> adjList_;
    unordered_map<ObjReg*, int, Reg_hash, Reg_cmp> degree_;
    /*
     * 存放低度数
     */
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> simplifyWorklist_;
    /*
     * 与传送有关的低度数的点
     */
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> freezeWorklist_;
    /*
     * 存放高度数
     */
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> spillWorklist_;
    /*
     * 本次循环中将要溢出的节点
     */
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> spillNodes_;
    /*
     * 已合并的节点集合
     */
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> coalescedNodes_;
    /*
     * 删除的点
     */
    vector<ObjReg*> selectStack_;
    /*
     * 具备合并可能性的传送指令
     */
    unordered_set<ObjMove*> worklistMoves_;
    /*
     *
     */
    unordered_set<ObjMove*> activeMoves_;
    /*
     * 已合并的传送指令集合
     */
    unordered_set<ObjMove*> coalescedMoves_;
    /*
     * src和dst冲突的传送指令
     */
    unordered_set<ObjMove*> constrainedMoves_;
    /*
     * 不合并的传送指令
     */
    unordered_set<ObjMove*> frozenMoves_;
    ObjVirReg* temp_ = nullptr;
    ObjInstruction* first_ = nullptr;
    ObjInstruction* last_ = nullptr;
public:
    GraphColor(ObjModule* module):module_(module){
        process();
    }

private:
    void init(ObjFunction* function){
        if(livenessMap_ != nullptr) {
            delete(livenessMap_);
        }
        livenessMap_ = BlockLiveness::calBlocksLiveness(function);
        moveList_.clear();
        adjList_.clear();
        for(auto pair:adjSet_) {
            delete(pair);
        }
        adjSet_.clear();
        alias_.clear();
        degree_.clear();
        simplifyWorklist_.clear();
        freezeWorklist_.clear();
        spillWorklist_.clear();
        spillNodes_.clear();
        coalescedNodes_.clear();
        selectStack_.clear();
        worklistMoves_.clear();
        activeMoves_.clear();
        coalescedMoves_.clear();
        frozenMoves_.clear();
        constrainedMoves_.clear();
        for(int i = 0; i < 32; i++) {
            degree_[new ObjPhyReg(i)] =  0x7fffffff;
        }
    }
    void build(ObjFunction* function);
    void addEdge(ObjReg* def, ObjReg* live);
    void makeWorklist(ObjFunction* function);
    void simplify();
    bool moveRelated(ObjReg* node);
    unordered_set<ObjReg*, Reg_hash, Reg_cmp> getFAdjlist(ObjReg* node);
    unordered_set<ObjMove*> nodeMoves(ObjReg* node);
    void enableMoves(ObjReg* node);
    void decreaseDegree(ObjReg* node);
    void addWorklist(ObjReg* node);
    bool adjOK(ObjReg* dst, ObjReg* src);
    bool GeorgeOk(ObjReg* t, ObjReg* r);
    ObjReg* getAlias(ObjReg* node);
    bool conservative(ObjReg* dst, ObjReg* src);
    void combine(ObjReg* dst, ObjReg* src);
    void coalesce();
    void freeze();
    void freezeMoves(ObjReg* node);
    void selectSpill();
    void assignColors(ObjFunction* function);
    void rewriteProgram(ObjFunction* function);
    void  virToPhy(ObjFunction* function, ObjBlock* block);
    void process();
};


#endif //COMPLIER_MIPS_GRAPHCOLOR_H
