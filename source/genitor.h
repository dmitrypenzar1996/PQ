#ifndef _GENITOR_H_
#define _GENITOR_H_
#include "maxAgreement.h"
#include "Tree.h"
#include "TreeWS.h"
#include "countScore.h"
#include "growTree.h"

unsigned treeIsUnique(TreeWithScore* intree, unsigned pos, TreeWithScore** trees, unsigned treeNum);
TreeWithScore* crossover(TreeWithScore* tree1, TreeWithScore* tree2, HashAlignment* alignment,\
		int alpha, GapOpt gapOpt, PWM* pwmMatrix, INT**** hashScore);
TreeWithScore* genitor(TreeWithScore** trees, unsigned treeNum, HashAlignment* alignment,\
					int alpha, GapOpt gapOpt, PWM* pwmMatrix, INT**** hashScore, unsigned iterNum, unsigned iterLim);
#endif