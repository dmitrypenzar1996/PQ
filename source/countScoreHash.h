#ifndef _COUNT_SCORE_HASH_H
#define _COUNT_SCORE_HASH_H
#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"
#include "HashAlignment.h"
#include "PWM.h"
#include "countScore.h"
#include "growTree.h"
#include <math.h>

INT**** getHashScore(unsigned int leavesNum);

void removeHashScore(INT**** hashScore, unsigned int leavesNum);


INT getScoreHash(HashAlignment* alignment, Tree* intree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        INT**** hashScore, int* permutation,\
        unsigned pos11, unsigned pos12,
        unsigned pos21, unsigned pos22);

INT recountScoreHash(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        INT**** hashScore, int* permutation);

INT countScoreHash(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        INT**** hashScore, int* permutation);

INT getMaxScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation,
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22);

INT recountMaxScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation);

INT* maxScoreAr(HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation);

INT countMaxScore(HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation);
#endif
