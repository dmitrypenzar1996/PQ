#ifndef _COUNT_SCORE_H
#define _COUNT_SCORE_H
#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"
#include "HashAlignment.h"
#include "PWM.h"

#define BASE_SCORE 1

typedef enum GapOpt {PASS_NOT, PASS_PAIR, PASS_ANY} GapOpt; 
//PASS_NOT - not pass tetrade with gap
//PASS_PAIR - pass any tetrade with two gaps
//PASS_ANY - pass any tetrade with gap

INT nucleotideScore(HashAlignment* alignment, Tree* intree,\
        int alpha, GapOpt gapOpt,\
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22);

int getMaxPenalty(PWM* pwm, char letter11, char letter12, char letter21, char letter22);

INT proteinScore(HashAlignment* alignment, Tree* intree, PWM* pwm,\
        int alpha, GapOpt gapOpt,\
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22);

INT alignmentScoreFiles(char* alignmentFileName, char* treeFileName,\
        char* pwmFileName, int alpha, GapOpt gapOpt);

INT recountScore(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt);

INT alignmentScore(HashAlignment* alignment, Tree* tree, PWM* pwm,\
    int alpha, GapOpt gapOpt);

INT getScore(HashAlignment* alignment, Tree* tree,\
        PWM* pwmMatrix, int alpha, GapOpt gapOpt, \
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22);
#endif
