#ifndef _MAXAGREEMENT_H_
#define _MAXAGREEMENT_H_
#include "Tree.h"
#include "add.h"
#include "consensus.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

BranchArray* treeToBranchModified(Tree* tree, int* permutation);
Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount);
void treesPrune(Tree* tree1, Tree* tree2);
unsigned* treeTopSort(Tree* tree, unsigned* setPermutation);
int* calculateLeavesPermutation(Tree* tree1, Tree* tree2);
int branchGetLeavesPosNum(Branch* br, unsigned leavesNum, unsigned maxNum);
int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y, unsigned leavesNum, unsigned maxNum);
Branch* branchOR(Branch* br1, Branch* br2);
Tree* makeMAST(Branch*** TAB, Tree* tree1, unsigned nodesNum2, unsigned* set1);
void MAST(Tree* tree1, Tree* tree2);
/*
Tree* oneLeafTreeCreate(char* name);
Tree* merge1w1(Tree* tree1, Tree* tree2);
Tree* merge2w3(Tree* tree1, Tree* tree2);
Tree* merge3w3(Tree* tree1, Tree* tree2);
Tree* mergeTrees(Tree* tree1, Tree* tree2);
*/
#endif
