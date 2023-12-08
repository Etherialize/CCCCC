//
// Created by Han Zhengyang on 2023/12/5.
//

#include "GraphColor.h"
#include <iostream>
void GraphColor::build(ObjFunction* function) {
    vector<ObjBlock*>& blocks = function->getBlocks();

    for(int i = blocks.size() - 1; i > -1 ; i--) {
        ObjBlock* curBlock = blocks[i];
        unordered_set <ObjReg*, Reg_hash, Reg_cmp> liveness = (*livenessMap_)[curBlock]->getLiveOut();
        vector<ObjInstruction*>& insts = curBlock->getInsts();
        for(int j = insts.size() - 1; j > -1; j--) {
            ObjInstruction* curInst = insts[j];
            unordered_set <ObjReg*>& regUse = curInst->getUse();
            unordered_set <ObjReg*>& regDef = curInst->getDef();

            if(curInst->getClassName() == OBJMOVECLASS) {
                ObjMove* objMove = (ObjMove*) curInst;
                ObjOperand* src = objMove->getSrc();
                ObjOperand* dst = objMove->getDst();

                if(src->needsColor() && dst->needsColor()) {
                    liveness.erase((ObjReg*)src);
                    if(moveList_.find((ObjReg*)src) == moveList_.end()) {
                        moveList_[(ObjReg*)src] = unordered_set<ObjMove*>();
                    }
                    moveList_[(ObjReg*)src].insert(objMove);

                    if(moveList_.find((ObjReg*)dst) == moveList_.end()) {
                        moveList_[(ObjReg*)dst] = unordered_set<ObjMove*>();
                    }
                    moveList_[(ObjReg*)dst].insert(objMove);
                    worklistMoves_.insert(objMove);
                }
            }

            for(auto def:regDef){
                liveness.insert(def);
            }
            for(auto def:regDef){
                for(auto live:liveness) {
                    addEdge(def, live);
                }
            }

            for(auto def:regDef){
                liveness.erase(def);
            }
            for(auto use:regUse){
                liveness.insert(use);
            }
        }
    }
}

void GraphColor::addEdge(ObjReg *def, ObjReg *live) {
    if(adjSet_.find(new ObjPair(def, live)) == adjSet_.end() && def->getName() != live->
    getName()) {
        adjSet_.insert(new ObjPair(def, live));
        adjSet_.insert(new ObjPair(live, def));

        if(!def->isPreColored()) {
            if(adjList_.find(def) == adjList_.end()){
                adjList_[def] = unordered_set<ObjReg*, Reg_hash, Reg_cmp>();
            }
            adjList_[def].insert(live);
            degree_[def] = degree_[def] + 1;
        }
        if(!live->isPreColored()) {
            if(adjList_.find(live) == adjList_.end()){
                adjList_[live] = unordered_set<ObjReg*, Reg_hash, Reg_cmp>();
            }
            adjList_[live].insert(def);
            degree_[live] = degree_[live] + 1;
        }
    }
}

void GraphColor::makeWorklist(ObjFunction* function) {
    vector<ObjVirReg*>& virRegs = function->getUsedVirRegs();
    for(auto reg : virRegs) {
        if(degree_[reg] >= K) {
            spillWorklist_.insert(reg);
        }
        else if(moveRelated(reg)) {
            freezeWorklist_.insert(reg);
        } else {
            simplifyWorklist_.insert(reg);
        }
    }
}


void GraphColor::simplify(){
    ObjReg* now = (*simplifyWorklist_.begin());
    simplifyWorklist_.erase(now);
    selectStack_.push_back(now);

    unordered_set<ObjReg*, Reg_hash, Reg_cmp> list = getFAdjlist(now);
    for(auto adj : list) {
        decreaseDegree(adj);
    }
}
bool GraphColor::moveRelated(ObjReg* node) {
    return nodeMoves(node).size() != 0;
}
unordered_set<ObjReg*, Reg_hash, Reg_cmp> GraphColor::getFAdjlist(ObjReg* node) {
    unordered_set <ObjReg*, Reg_hash, Reg_cmp> rst;
    if(adjList_.find(node) == adjList_.end()) {
        return rst;
    }
    for(auto it : adjList_[node]) {
        if(!(find(selectStack_.begin(), selectStack_.end(), it) != selectStack_.end() ||
             coalescedNodes_.find(it) != coalescedNodes_.end())) {
            rst.insert(it);
        }
    }
    return rst;
}

/*
 * activeMoves + workListMoves
 */
unordered_set<ObjMove*> GraphColor::nodeMoves(ObjReg* node) {
    unordered_set <ObjMove*> rst;
    if(moveList_.find(node) == moveList_.end()) {
        return rst;
    }
    for(auto it : moveList_[node]) {
        if(activeMoves_.find(it) != activeMoves_.end() || worklistMoves_.find(it) != worklistMoves_.end()) {
            rst.insert(it);
        }
    }
    return rst;
}
/*
 * 将不具备合并的move加入到具备可能性,
 */
void GraphColor::enableMoves(ObjReg* node){
    unordered_set<ObjMove*> moves = nodeMoves(node);
    for(auto move : moves) {
        if(activeMoves_.find(move) != activeMoves_.end()) {
            activeMoves_.erase(move);
            worklistMoves_.insert(move);
        }
    }
    unordered_set <ObjReg*, Reg_hash, Reg_cmp> list = getFAdjlist(node);
    for(auto adj: list) {
        unordered_set<ObjMove*> adjMoves = nodeMoves(adj);
        for(auto move : adjMoves) {
            if(activeMoves_.find(move) != activeMoves_.end()) {
                activeMoves_.erase(move);
                worklistMoves_.insert(move);
            }
        }
    }
}
void GraphColor::decreaseDegree(ObjReg* node) {
    int old = degree_[node];
    degree_[node] = old - 1;
    if(old == K) {
        enableMoves(node);
        spillWorklist_.erase(node);
        if(moveRelated(node)) {
            freezeWorklist_.insert(node);
        } else {
            simplifyWorklist_.insert(node);
        }
    }
}
void GraphColor::addWorklist(ObjReg* node){
    if(!node->isPreColored() && !moveRelated(node) && degree_[node] < K) {
        freezeWorklist_.erase(node);
        simplifyWorklist_.insert(node);
    }
}
/*
 * 能否合并的方法 ： George
 */
bool GraphColor::adjOK(ObjReg* dst, ObjReg* src){
    bool ok = true;
    unordered_set <ObjReg*, Reg_hash, Reg_cmp> list = getFAdjlist(src);
    for(auto adj : list) {
        ok = ok && GeorgeOk(adj, dst);
    }
    return ok;
}

/*
 * 对于邻居t， 如果t的度数小于 K， t和r以冲突 , t是物理寄存器
 */
bool GraphColor::GeorgeOk(ObjReg* t, ObjReg* r) {
    return degree_[t] < K || t->isPreColored() || adjSet_.find(new ObjPair(t, r)) !=
    adjSet_.end();
}
ObjReg* GraphColor::getAlias(ObjReg* node) {
    ObjReg* dst = node;
    while(coalescedNodes_.find(dst) != coalescedNodes_.end()) {
        dst = alias_[dst];
    }
    return dst;
}
bool GraphColor::conservative(ObjReg* dst, ObjReg* src) {
    unordered_set <ObjReg*, Reg_hash, Reg_cmp> cup = getFAdjlist(dst);
    unordered_set <ObjReg*, Reg_hash, Reg_cmp> srcAdj = getFAdjlist(src);
    cup.insert(srcAdj.begin(), srcAdj.end());
    long long n = 0;
    for(auto adj : cup) {
        if(degree_[adj] >= K) {
            n++;
        }
    }
    return n < K;
}
void GraphColor::combine(ObjReg* dst, ObjReg* src){
    if(freezeWorklist_.find(src) != freezeWorklist_.end()) {
        freezeWorklist_.erase(src);
    } else {
        spillWorklist_.erase(src);
    }

    coalescedNodes_.insert(src);
    alias_[src] = dst;
    moveList_[dst].insert(moveList_[src].begin(), moveList_[src].end());
    unordered_set <ObjReg*, Reg_hash, Reg_cmp> list = getFAdjlist(src);

    /*------------------------------------------------------------------*/
//    unordered_set<ObjMove*> moves = nodeMoves(src);
//    for(auto move : moves) {
//        if(activeMoves_.find(move) != activeMoves_.end()) {
//            activeMoves_.erase(move);
//            worklistMoves_.insert(move);
//        }
//    }
    /*------------------------------------------------------------------*/


    for(auto adj:list) {
        addEdge(adj, dst);
        decreaseDegree(adj);
    }

    if(degree_[dst] >= K && freezeWorklist_.find(dst) != freezeWorklist_.end()){
        freezeWorklist_.erase(dst);
        spillWorklist_.insert(dst);
    }

}
void GraphColor::coalesce(){
    ObjMove* move = (*worklistMoves_.begin());
    /*
     * 咱就是说，进到worklistmoves里面的肯定都是需要着色的，那就一定是寄存器
     */
    ObjReg* dst = getAlias((ObjReg*)move->getDst());
    ObjReg* src = getAlias((ObjReg*)move->getSrc());

    //物理寄存器，合并的话留下来的是物理寄存器
    if(src->isPreColored()) {
        ObjReg* temp = dst;
        dst = src;
        src = temp;
    }

    worklistMoves_.erase(move);

    if(dst->getName() == src->getName()) {
        coalescedMoves_.insert(move);
        addWorklist(dst);
    } else if(src->isPreColored() ||
       adjSet_.find(new ObjPair(dst, src)) != adjSet_.end()) {
        constrainedMoves_.insert(move);
        addWorklist(dst);
        addWorklist(src);
    }
    else if((dst->isPreColored() && adjOK(dst, src)) ||
           (!dst->isPreColored() && conservative(dst, src))) {
        coalescedMoves_.insert(move);
        combine(dst, src);
        addWorklist(dst);
    }
    else {
        activeMoves_.insert(move);
    }
}

void GraphColor::freeze() {
    ObjReg* node = *(freezeWorklist_.begin());
    freezeWorklist_.erase(node);
    simplifyWorklist_.insert(node);
    freezeMoves(node);
}
void GraphColor::freezeMoves(ObjReg* dst) {
    unordered_set <ObjMove*> moves = nodeMoves(dst);
    for(auto move : moves) {
        if(activeMoves_.find(move) != activeMoves_.end()) {
            activeMoves_.erase(move);
        } else {
            worklistMoves_.erase(move);
        }
        frozenMoves_.insert(move);
        ObjReg* src = getAlias((ObjReg*)move->getDst())->getName() == getAlias(dst)->getName() ?
                        getAlias((ObjReg*)move->getSrc()) : getAlias((ObjReg*)move->getDst());
        if(!moveRelated(src) && degree_[src] < K) {
            freezeWorklist_.erase(src);
            simplifyWorklist_.insert(src);
        }
    }
}

void GraphColor::selectSpill() {
    ObjReg* max;
    int maxDegree = -1;
    for(auto reg: spillWorklist_) {
        if(degree_[reg] > maxDegree) {
            max = reg;
            maxDegree = degree_[reg];
        }
    }
    simplifyWorklist_.insert(max);
    spillWorklist_.erase(max);
    freezeMoves(max);
}

void GraphColor::assignColors(ObjFunction* function) {
    unordered_map<ObjReg*, ObjReg*, Reg_hash, Reg_cmp> colored;
    while(selectStack_.size() != 0) {
        ObjReg* n = selectStack_[selectStack_.size() - 1];
        selectStack_.pop_back();
        unordered_set <int> okColors;
        okColors.insert(ObjPhyReg::allocableRegs_.begin(), ObjPhyReg::allocableRegs_.end());

        if(adjList_.find(n) != adjList_.end()) {
            for(auto adj:adjList_[n]) {
                ObjReg* a = getAlias(adj);
                if(a->isAllocated() || a->isPreColored()) {
                    okColors.erase(((ObjPhyReg*) a)->getIndex());
                } else if(a->getClassName() == OBJVIRREGCLASS) {
                    if(colored.find(a) != colored.end()) {
                        ObjReg* color = colored[a];
                        okColors.erase(((ObjPhyReg*)color)->getIndex());
                    }
                }
            }
        }

        if(okColors.size() == 0) {
            spillNodes_.insert(n);
        } else {
            int color = *okColors.begin();
            colored[n] = new ObjPhyReg(color, true);
        }
    }

    if(spillNodes_.size() != 0) {
        return;
    }


    for(auto coalescedNode : coalescedNodes_) {
        ObjReg* alias = getAlias(coalescedNode);
        if(alias->isPreColored()) {
            colored[coalescedNode] = alias;
        } else {
            colored[coalescedNode] = colored[alias];
        }
    }

    vector<ObjBlock*>& blocks = function->getBlocks();
    for(auto block: blocks) {
        vector<ObjInstruction*>& insts = block->getInsts();
        for(auto inst : insts) {
            unordered_set <ObjReg*> defs = inst->getDef();
            unordered_set <ObjReg*> uses = inst->getUse();
            for(auto def:defs) {
                if(colored.find(def) != colored.end()){
                    inst->replaceReg(def, colored[def]);
                }
            }
            for(auto use:uses) {
                if(colored.find(use) != colored.end()) {
                    inst->replaceReg(use, colored[use]);
                }
            }
        }
    }
}

void GraphColor::rewriteProgram(ObjFunction* function) {
    for(auto spillReg : spillNodes_) {
        vector<ObjBlock*>& blocks = function->getBlocks();
        for(auto block: blocks) {
            vector<ObjInstruction*>& insts = block->getInsts();
            temp_ = nullptr;
            first_ = nullptr;
            last_ = nullptr;
            for(auto inst : insts) {
                unordered_set <ObjReg*> defs = inst->getDef();
                unordered_set <ObjReg*> uses = inst->getUse();
                for(auto use:uses) {
                    if(use->getName() == spillReg->getName()) {
                        if(temp_ == nullptr) {
                            temp_ = new ObjVirReg();
                            function->addVirReg(temp_);
                        }
                        inst->replaceReg(use, temp_);

                        if(first_ == nullptr && last_ == nullptr) {
                            first_ = inst;
                        }
                    }
                }
                for(auto def: defs) {
                    if(def->getName() == spillReg->getName()) {
                        if(temp_ == nullptr) {
                            temp_ = new ObjVirReg();
                            function->addVirReg(temp_);
                        }
                        inst->replaceReg(def, temp_);
                        last_ = inst;
                    }
                }
            }
            virToPhy(function, block);
        }
        function->addAllocaSize(4);
    }
}
void GraphColor::virToPhy(ObjFunction *function, ObjBlock* block) {
    if(first_ != nullptr) {
        ObjLoad* load = new ObjLoad(temp_, ObjPhyReg::SP(), new ObjImm(function->getAllocaSize()));
        block->addInstructionIndex(first_, load, 0);
        first_ = nullptr;
    }
    if(last_ != nullptr) {
        ObjStore* store = new ObjStore(temp_, ObjPhyReg::SP(), new ObjImm(function->getAllocaSize()));
        block->addInstructionIndex(last_, store, 1);
        last_ = nullptr;
    }
    temp_ = nullptr;
}
void GraphColor::process() {
    vector<ObjFunction*> funcs = module_->getFuncs();
    for(auto func:funcs) {
        bool finished = false;
        while(!finished) {
            init(func);
            build(func);
            makeWorklist(func);
            do {
                if(simplifyWorklist_.size() != 0) {
                    simplify();
                }
                if(worklistMoves_.size() != 0) {
                    coalesce();
                }
                if(freezeWorklist_.size() != 0) {
                    freeze();
                }
                if(spillWorklist_.size() != 0) {
                    selectSpill();
                }
            }while(!(simplifyWorklist_.size() == 0 && worklistMoves_.size() == 0 &&
                     freezeWorklist_.size() == 0 && spillWorklist_.size() == 0));
            assignColors(func);

            if(spillWorklist_.size() == 0) {
                finished = true;
            } else {
                rewriteProgram(func);
            }
        }
    }
}

