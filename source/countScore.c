#include "countScore.h"


// +
INT nucleotideScore(HashAlignment* alignment, Tree* intree,
        int alpha, GapOpt gapOpt,
        unsigned int pos11, unsigned int pos12,
        unsigned int pos21, unsigned int pos22)
{
    int i;
    Record *rec11, *rec12, *rec21, *rec22;
    char *seq11, *seq12, *seq21, *seq22;
    INT result;

    rec11 = hashAlignmentGet(alignment, intree->leaves[pos11]->name);
    rec12 = hashAlignmentGet(alignment, intree->leaves[pos12]->name);
    rec21 = hashAlignmentGet(alignment, intree->leaves[pos21]->name);
    rec22 = hashAlignmentGet(alignment, intree->leaves[pos22]->name);
   

    if ((! rec11) || (! rec12) || (! rec21) || (! rec22))
    {
        perror("Alignment doesn't contain one or more of these sequences\n");
        printf("%s, %s, %s, %s\n", 
                intree->leaves[pos11]->name, intree->leaves[pos12]->name,\
                intree->leaves[pos21]->name, intree->leaves[pos22]->name);
        exit(1);
    }
    seq11 = rec11->sequence; 
    seq12 = rec12->sequence;
    seq21 = rec21->sequence;
    seq22 = rec22->sequence;


    result = 0;
    for (i = 0; i < alignment->sequenceSize; ++i)
    {
        if (
                ((gapOpt == PASS_PAIR) && ((seq11[i] == '-' && seq12[i] == '-')\
                        || (seq11[i] == '-' && seq21[i] == '-')\
                        || (seq11[i] == '-' && seq22[i] == '-')\
                        || (seq12[i] == '-' && seq21[i] == '-')\
                        || (seq12[i] == '-' && seq22[i] == '-')\
                        || (seq21[i] == '-' && seq22[i] == '-'))) ||\
                ((gapOpt == PASS_ANY) && (seq11[i] == '-' || seq12[i] == '-'\
                    || seq21[i] == '-' || seq22[i] == '-')))
        {
           ;
        }
        else if (seq11[i] == seq12[i])
        {
            if (seq21[i] == seq22[i])
            {
                if (seq21[i] != seq11[i])
                {
                    result += (BASE_SCORE + BASE_SCORE) * alpha;
                }
            }
            else if ((seq11[i] != seq21[i]) && (seq11[i] != seq22[i]))
            {
                result += BASE_SCORE;
            }
        }
        else if ((seq21[i] == seq22[i]) && (seq21[i] != seq11[i])\
                && (seq21[i] != seq12[i]))
        {
            result += BASE_SCORE;
        }    
    }
    return result;
} /*  nucleotideScore */

//+
int getMaxPenalty(PWM* pwm, char letter11, char letter12, char letter21, char letter22)
{
    int maxPenalty;
    maxPenalty = pwm->matrix[letter11][letter21];

    if (pwm->matrix[letter11][letter22] > maxPenalty)
    {
        maxPenalty = pwm->matrix[letter11][letter22];
    }
    if (pwm->matrix[letter12][letter21] > maxPenalty)
    {
        maxPenalty = pwm->matrix[letter12][letter21];
    }
    if (pwm->matrix[letter12][letter22] > maxPenalty)
    {
        maxPenalty = pwm->matrix[letter12][letter22];
    }
    return maxPenalty;
} /* getMaxPenalty */

//+
INT proteinScore(HashAlignment* alignment, Tree* intree,\
        PWM* pwm, int alpha, GapOpt gapOpt, \
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22)
{
    int i;
    int penalty, price1, price2;
    Record *rec11, *rec12, *rec21, *rec22;
    char *seq11, *seq12, *seq21, *seq22;
    INT result = 0;

    rec11 = hashAlignmentGet(alignment, intree->leaves[pos11]->name);
    rec12 = hashAlignmentGet(alignment, intree->leaves[pos12]->name);
    rec21 = hashAlignmentGet(alignment, intree->leaves[pos21]->name);
    rec22 = hashAlignmentGet(alignment, intree->leaves[pos22]->name);
   
    if ((! rec11) || (! rec12) || (! rec21) || (! rec22))
    {
        perror("Alignment doesn't contain one or more of these sequences\n");
        printf("%s, %s, %s, %s\n",\
                intree->leaves[pos11]->name, intree->leaves[pos12]->name,\
                intree->leaves[pos21]->name, intree->leaves[pos22]->name);
        exit(1);
    }
    seq11 = rec11->sequence; 
    seq12 = rec12->sequence;
    seq21 = rec21->sequence;
    seq22 = rec22->sequence;


    for (i = 0; i < alignment->sequenceSize; ++i)
    {
        if (
                ((gapOpt == PASS_PAIR) && ((seq11[i] == '-' && seq12[i] == '-')\
                        || (seq11[i] == '-' && seq21[i] == '-')\
                        || (seq11[i] == '-' && seq22[i] == '-')\
                        || (seq12[i] == '-' && seq21[i] == '-')\
                        || (seq12[i] == '-' && seq22[i] == '-')\
                        || (seq21[i] == '-' && seq22[i] == '-'))) ||\
                ((gapOpt == PASS_ANY) && (seq11[i] == '-' || seq12[i] == '-'\
                    || seq21[i] == '-' || seq22[i] == '-')))
        {
            ;
        }
        else
        {
            penalty = getMaxPenalty(pwm, seq11[i], seq12[i], seq21[i], seq22[i]);
            price1 = pwm->matrix[seq11[i]][seq12[i]] - penalty; 
            price2 = pwm->matrix[seq21[i]][seq22[i]] - penalty; 

            if (price1 > 0 && price2 > 0)
            {
                result += (price1 + price2) * alpha;
            }
            else if (price1 > 0)
            {
                result += price1;
            }
            else if (price2 > 0)
            {
                result += price2;
            }
        }
    }

    return result;
} /* proteinScore */

//+
INT getScore(HashAlignment* alignment, Tree* tree,
        PWM* pwmMatrix, int alpha, GapOpt gapOpt,
        unsigned int pos11, unsigned int pos12,
        unsigned int pos21, unsigned int pos22)
{
    INT score;

    if (pwmMatrix != NULL)
    {
        score = proteinScore(alignment, tree, pwmMatrix, alpha, gapOpt,\
                pos11, pos12, pos21, pos22);
    }
    else
    {
        score = nucleotideScore(alignment, tree, alpha, gapOpt,\
                pos11, pos12, pos21, pos22);
    }

    return score;
} /* getScore */

//+
INT alignmentScore(HashAlignment* alignment, Tree* tree, PWM* pwm,\
    int alpha, GapOpt gapOpt)
{
    INT totalScore = 0;
    int l1, l2, l3, l4;
    int pos11, pos12, pos21, pos22;
    int splitPos;

    for (l1 = 0; l1 < tree->leavesNum; ++l1)
    {
        for (l2 = l1 + 1; l2 < tree->leavesNum; ++l2)
        {
            for (l3 = l2 + 1; l3 < tree->leavesNum; ++l3)
            {
                for (l4 = l3 + 1; l4 < tree->leavesNum; ++l4)
                {
                    pos11 = l1;
                    splitPos = treeWhichSplit(tree, l1, l2, l3, l4);
                    switch (splitPos)
                    {
                        case 2:
                            pos12 = l2;
                            pos21 = l3;
                            pos22 = l4;
                            break;
                        case 3:
                            pos12 = l3;
                            pos21 = l2;
                            pos22 = l4;
                            break;
                        case 4:
                            pos12 = l4;
                            pos21 = l2;
                            pos22 = l3;
                            break;
                        default:
                            perror("Error. Unexpected behaviour");
                            exit(1);
                    }    
                    totalScore += getScore(alignment, tree, pwm, alpha,\
                            gapOpt, pos11, pos12, pos21, pos22);
                }
            }
        }
    }
    return totalScore;
} /* alignmentScore */

//+
INT recountScore(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt) 
{
    int lastLeafPos;
    INT recount = 0;
    int l1, l2, l3;
    int pos11, pos12, pos21, pos22;
    int splitPos;

    lastLeafPos = tree->leavesNum - 1;
    for (l1 = 0; l1 < lastLeafPos; ++l1)
    {
        for (l2 = l1 + 1; l2 < lastLeafPos; ++l2)
        {
            for (l3 = l2 + 1; l3 < lastLeafPos; ++l3)
            {
                    pos11 = l1;
                    splitPos = treeWhichSplit(tree, l1, l2, l3, lastLeafPos);
                    switch (splitPos)
                    {
                        case 2:
                            pos12 = l2;
                            pos21 = l3;
                            pos22 = lastLeafPos;
                            break;
                        case 3:
                            pos12 = l3;
                            pos21 = l2;
                            pos22 = lastLeafPos;
                            break;
                        case 4:
                            pos12 = lastLeafPos;
                            pos21 = l2;
                            pos22 = l3;
                            break;
                        default:
                        perror("Error. Unexpected behaviour");
                        exit(1);
                    }                
                
                    recount += getScore(alignment, tree, pwm, alpha, gapOpt,\
                            pos11, pos12, pos21, pos22);
            }
        }    
    }

    return recount;
} /* recountScore */

INT alignmentScoreFiles(char* alignmentFileName, char* treeFileName,\
        char* pwmFileName, int alpha, GapOpt gapOpt)
{
    INT totalScore;
    PWM* pwm = NULL;
    HashAlignment* alignment;
    Tree* tree;

    alignment = hashAlignmentRead(alignmentFileName);
    tree = treeRead(treeFileName);

    if (pwmFileName != NULL)
    { 
        pwm = pwmRead(pwmFileName);
    }

    totalScore = alignmentScore(alignment, tree, pwm, alpha, gapOpt);
    hashAlignmentDelete(alignment);
    treeDelete(tree);
    if (pwm != NULL)
    {
        pwmDelete(pwm);
    }

    return totalScore;
} /* alignmentScoreFiles */

