#ifndef __ESTIMATE_H__
#define __ESTIMATE_H__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Tree.h"
#include "HashAlignment.h"
#include "PWM.h"
#include "TreeWS.h"
#include "countScore.h"
#include "countScoreHash.h"

// return inTree copy at first position and its's nniNeighbours 
TreeWithScore** getNNINeighbours(Tree* inTree, HashAlignment* alignment,\
        PWM* pwmMatrix,\
        int alpha, GapOpt gapOpt, INT**** hashScore);

TreeWithScore* randomTreeGrow(HashAlignment* alignment, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix,\
        INT**** hashScore);

TreeWithScore** growMultipleRandomTree(HashAlignment* alignment, int alpha,\
        GapOpt gapOpt,\
        PWM* pwmMatrix, unsigned treeNum,\
        INT**** hashScore);

typedef struct Sample
{
    unsigned sampleSize;
    INT* sample;
    double deviation;
    double mean;
}Sample;

void sampleDelete(Sample* sample);

Sample* sampleCreate(void);

double sampleCalcZscore(Sample* sample, INT value);

Sample* sampleFromTreeWSAr(TreeWithScore** treeSample, unsigned treeNum);

void sampleCalcDev(Sample* sample);

void sampleCalcMean(Sample* sample);

#endif
