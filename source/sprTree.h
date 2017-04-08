#include <stdio.h>
#include "add.h"
#include "Tree.h"
#include "PWM.h"
#include "HashAlignment.h"
#include "countScoreHash.h"
#include "TreeWS.h"

TreeWithScore* simpleSPR(Tree* inTree, HashAlignment* alignment,\
        PWM* pwmMatrix, int alpha, GapOpt gapOpt, SCORE**** hashScore);

TreeWithScore* gradientSPR(Tree* inTree, HashAlignment* alignment,\
        PWM* pwmMatrix, int alpha, GapOpt gapOpt, SCORE**** hashScore);

