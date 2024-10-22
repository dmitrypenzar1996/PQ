#ifndef _MAX_AGREEMENT_H_
#define _MAX_AGREEMENT_H_
#include "Tree.h"
#include "add.h"
#include "consensus.h"
#include <stdlib.h>
#include <string.h>

BranchArray* treeRootedToBranchArray(Tree* tree, int* permutation);
void stackCycle(Node* node, NodeStack* stack);
Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount);
Tree** treesPrune(Tree* tree1, Tree* tree2);
Tree* treeRoot(Tree* tree, unsigned nodeID, unsigned neighbourID, char newTree);
Tree* treeUnRoot(Tree* tree, char newTree);
unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation);
int* calculateLeavesPermutation(Tree* tree1, Tree* tree2);
int branchGetLeavesPosNum(Branch* br);
int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y);
int* getTreeLeavesPos(Tree* tree);
unsigned* branchToLeavesArr(Branch* br, unsigned leavesNum);
int** getAllRoots(Tree* tree);
int* getRandMaxBranch(Branch*** TAB, int rows, int cols);
Tree* makeMAST(Branch* br, Tree* tree1);
Branch* MAST(Tree* intree1, Tree* intree2, unsigned* set1, unsigned* set2, unsigned* setPermutation1, unsigned* setPermutation2);
Tree* UMAST(Tree* tree1, Tree* tree2);
#endif