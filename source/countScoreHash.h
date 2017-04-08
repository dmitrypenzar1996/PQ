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

SCORE**** getHashScore(unsigned int leavesNum);

void removeHashScore(SCORE**** hashScore, unsigned int leavesNum);


SCORE getScoreHash(HashAlignment* alignment, Tree* intree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        SCORE**** hashScore, int* permutation,\
        unsigned pos11, unsigned pos12,
        unsigned pos21, unsigned pos22);

SCORE recountScoreHash(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        SCORE**** hashScore, int* permutation);

SCORE countScoreHash(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        SCORE**** hashScore, int* permutation);

SCORE getMaxScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, SCORE**** hashScore, int* permutation,
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22);

SCORE recountMaxScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, SCORE**** hashScore, int* permutation);

SCORE* maxScoreAr(HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, SCORE**** hashScore, int* permutation);

SCORE countMaxScore(HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, SCORE**** hashScore, int* permutation);
#endif
