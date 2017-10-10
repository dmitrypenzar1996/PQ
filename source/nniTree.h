#ifndef __NNI_TREE_H
#define __NNI_TREE_H 

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "add.h"
#include "Tree.h"
#include "PWM.h"
#include "HashAlignment.h"
#include "countScore.h"
#include "countScoreHash.h"
#include "TreeWS.h"

typedef struct TrajectoryElement
{
    TreeWithScore* treeWS;
    unsigned long int time;
    void* next;
}TrajectoryElement;

typedef struct
{
    TrajectoryElement* bestPoint;
    TrajectoryElement* head;
    TrajectoryElement* tail;
    unsigned long int trajectoryTime;
    unsigned int temperature;
    unsigned int size;
}Trajectory;

Tree* treeNNI(Tree* intree, unsigned int branchindex, unsigned int which); 

TreeWithScore* simpleNNI(Tree* inTree, HashAlignment* alignment, 
    PWM* pwmMatrix, int alpha, GapOpt gapOpt, INT**** hashScore);

TreeWithScore* gradientNNI(Tree* inTree, HashAlignment* alignment, 
    PWM* pwmMatrix, int alpha, GapOpt gapOpt, INT**** hashScore);

Trajectory* trajectoryNNI(Tree* inTree, HashAlignment* alignment,
    PWM* pwmMatrix, int alpha, GapOpt gapOpt, INT**** hashScore,
    unsigned long int trajectoryTime, unsigned int temperature, unsigned int mcStyle);
#endif
