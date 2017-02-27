#ifndef _MAXAGREEMENT_H_
#define _MAXAGREEMENT_H_
#include "Tree.h"
#include "add.h"
#include "consensus.h"
#include "growTree.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

BranchArray* treeToBranchModified(Tree* tree, int* permutation);
Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount);
void treesPrune(Tree* tree1, Tree* tree2);
unsigned* treeTopSort(Tree* tree);
int* calculateLeavesPermutation(Tree* tree1, Tree* tree2);
Tree* oneLeafTreeCreate(char* name);
int* countVariants(Tree*** TAB, int a, int w, int b, int c, int x, int y);
Tree* merge2w3(Tree* tree1, Tree* tree2);
Tree* merge3w3(Tree* tree1, Tree* tree2);
Tree* mergeTrees(Tree* tree1, Tree* tree2);
void MAST(Tree* tree1, Tree* tree2);
#endif
