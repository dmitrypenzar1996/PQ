#ifndef _MAXAGREEMENT_H_
#define _MAXAGREEMENT_H_
#include "Tree.h"
#include "add.h"
#include "consensus.h"
#include "CutTree.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

BranchArray* treeRootedToBranchArray(Tree* tree, int* permutation);
Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount);
Tree** treesPrune(Tree* tree1, Tree* tree2);
unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation);
int* calculateLeavesPermutation(Tree* tree1, Tree* tree2);
int branchGetLeavesPosNum(Branch* br);
int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y);
Branch* branchOR(Branch* br1, Branch* br2);
unsigned* getTreeLeavesPos(Tree* tree);
unsigned* branchToLeavesArr(Branch* br, unsigned leavesNum);
Tree* makeMAST(Branch*** TAB, Tree* tree1, unsigned nodesNum2, unsigned* set1);
void MAST(Tree* tree1, Tree* tree2);
#endif
