#ifndef _UMAST_H_
#define _UMAST_H_
#include "Tree.h"
#include "add.h"
#include "consensus.h"
#include "cutTree.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

BranchArray* treeRootedToBranchArray(Tree* tree, int* permutation);
Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount);
Tree** treesPrune(Tree* tree1, Tree* tree2);
Tree* treeRoot(Tree* tree, unsigned nodeID, unsigned neighbourID, char newTree);
Tree* treeUnRoot(Tree* tree, char newTree);
unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation);
int* calculateLeavesPermutation(Tree* tree1, Tree* tree2);
int branchGetLeavesPosNum(Branch* br);
int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y);
Branch* branchOR(Branch* br1, Branch* br2);
int* getTreeLeavesPos(Tree* tree);
unsigned* branchToLeavesArr(Branch* br, unsigned leavesNum);
int** getAllRoots(Tree* tree);
Tree* makeMAST(Branch* br, Tree* tree1);
Branch* MAST(Tree* tree1, Tree* tree2, unsigned* set1, unsigned* set2, unsigned* setPermutation1, unsigned* setPermutation2);
void UMAST(Tree* tree1, Tree* tree2);
#endif
