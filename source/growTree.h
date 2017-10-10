#ifndef __GROW_TREE__H
#define __GROW_TREE__H
#include <string.h>
#include "countScoreHash.h"
#include "HashAlignment.h"
#include "Tree.h"
#include "PWM.h"
#include "TreeWS.h"
#include "add.h"

Tree* growThreeLeavesTree(char* leaf1, char* leaf2, char* leaf3);

TreeWithScore* treeGrow(HashAlignment* alignment, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix, INT**** hashScore, char randLeaves);

TreeWithScore* oneTreeGrow(HashAlignment* alignment, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix,  INT**** hashScore, char randLeaves);

TreeWithScore** multipleTreeGrow(HashAlignment* alignment,\
        int alpha, GapOpt gapOpt, \
        PWM* pwmMatrix, unsigned int treeNum, INT**** hashScore);
#endif
