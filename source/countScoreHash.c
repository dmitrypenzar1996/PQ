#include "countScoreHash.h"

INT**** getHashScore(unsigned int leavesNum)
{
    INT**** hashScore;
    int i, j, k, h;

    hashScore =  (INT****)malloc(sizeof(INT***) * leavesNum);
    for (i = 0; i < leavesNum; ++i)
    {
        hashScore[i] = (INT***)malloc(sizeof(INT**) * leavesNum);
        for (j = 0; j < leavesNum; ++j)
        {
            hashScore[i][j] = (INT**)malloc(sizeof(INT*) * leavesNum);
            for (k = 0; k < leavesNum; ++k)
            {
                hashScore[i][j][k] = (INT*)malloc(sizeof(INT) * leavesNum);
                for (h = 0; h < leavesNum; ++h)
                {
                    hashScore[i][j][k][h] = -1;
                }
            }
        }
    }
    return hashScore;
} /* getHashScore */


void removeHashScore(INT**** hashScore, unsigned int leavesNum)
{
    int i, j, k;
    for (i = 0; i < leavesNum; ++i)
    {
        for (j = 0; j < leavesNum; ++j)
        {
            for (k = 0; k < leavesNum; ++k)
            {
                free(hashScore[i][j][k]);
            }
            free(hashScore[i][j]);
        }
        free(hashScore[i]);
    }
    free(hashScore);
} /* removeHashScore */


INT getScoreHash(HashAlignment* alignment, Tree* intree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        INT**** hashScore, int* permutation,\
        unsigned pos11, unsigned pos12,
        unsigned pos21, unsigned pos22)
{
    INT result;
    
    if (hashScore != 0)
    {
        if (hashScore[permutation[pos11]][permutation[pos12]][permutation[pos21]]\
                [permutation[pos22]] != -1)
        {
            return hashScore[permutation[pos11]][permutation[pos12]]\
                [permutation[pos21]][permutation[pos22]];
        } 
    }

    result = getScore(alignment, intree, pwm, alpha, gapOpt,\
            pos11, pos12, pos21, pos22); 
    
    if (hashScore != 0)
    {
        hashScore[permutation[pos11]][permutation[pos12]][permutation[pos21]]\
            [permutation[pos22]] = result;
        hashScore[permutation[pos11]][permutation[pos12]][permutation[pos22]]\
            [permutation[pos21]] = result;
        hashScore[permutation[pos12]][permutation[pos11]][permutation[pos21]]\
            [permutation[pos22]] = result;
        hashScore[permutation[pos12]][permutation[pos11]][permutation[pos22]]\
            [permutation[pos21]] = result;
        hashScore[permutation[pos21]][permutation[pos22]][permutation[pos11]]\
            [permutation[pos12]] = result;
        hashScore[permutation[pos21]][permutation[pos22]][permutation[pos12]]\
            [permutation[pos11]] = result;
        hashScore[permutation[pos22]][permutation[pos21]][permutation[pos11]]\
            [permutation[pos12]] = result;
        hashScore[permutation[pos22]][permutation[pos21]][permutation[pos12]]\
            [permutation[pos11]] = result;
    }
    return result;
} /* getScorehash */

INT recountScoreHash(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt, \
        INT**** hashScore, int* permutation)
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
                
                    recount += getScoreHash(alignment, tree,\
                            pwm, alpha, gapOpt,\
                            hashScore, permutation,\
                            pos11, pos12, pos21, pos22);
            }
        }    
    }

    return recount;
}  /* recountScoreHash */

INT countScoreHash(HashAlignment* alignment, Tree* tree,\
        PWM* pwm, int alpha, GapOpt gapOpt,\
        INT**** hashScore, int* permutation)
{
    int l1, l2, l3, l4;
    INT totalScore = 0;
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
                
                    totalScore += getScoreHash(alignment, tree,\
                            pwm, alpha, gapOpt,\
                            hashScore, permutation,\
                            pos11, pos12, pos21, pos22);
                }
            }
        }
    }
    return totalScore;
} /* countScoreHash */

/*
INT getNormalizeScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, TWM* twmMatrix,\
        int alpha, INT**** hashScore, int* permutation,\
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22,
        unsigned (*normalize)(unsigned _dist12, unsigned _dist34,\
                              unsigned _dist12_34))
{
    INT score = getScore(tree, alignment, pwmMatrix, twmMatrix,\
            alpha, hashScore, permutation, pos11, pos12, pos21, pos22);

    unsigned lca12Pos = treeFindLCA(tree, pos11, pos12);
    unsigned lca34Pos = treeFindLCA(tree, pos21, pos22);

    
    unsigned dist12 = treeGetDist(tree, tree->leaves[pos11]->pos,\
            tree->leaves[pos12]->pos);
    unsigned dist34 = treeGetDist(tree, tree->leaves[pos21]->pos,\
            tree->leaves[pos22]->pos);
    unsigned dist12_34 = treeGetDist(tree, lca12Pos, lca34Pos);
    
    return score * normalize(dist12, dist34, dist12_34);
}
*/

INT getMaxScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation,
        unsigned int pos11, unsigned int pos12,\
        unsigned int pos21, unsigned int pos22)
{
    INT score1, score2, score3, max;

    score1 = getScoreHash(alignment, tree,\
            pwmMatrix, alpha, gapOpt,\
            hashScore, permutation,
            pos11, pos12, pos21, pos22); 
    score2 = getScoreHash(alignment, tree,\
            pwmMatrix, alpha, gapOpt,\
            hashScore, permutation,
            pos11, pos21, pos12, pos22); 
    score3 = getScoreHash(alignment, tree,\
            pwmMatrix, alpha, gapOpt,\
            hashScore, permutation,
            pos11, pos22, pos12, pos21); 

    max = score1;
    if (score2 > max)
    {
        max = score2;
    }
    if (score3 > max)
    {
        max = score3;
    }

    return max;
} /* getMAxScore */

INT recountMaxScore(Tree* tree, HashAlignment* alignment,\
        PWM* pwmMatrix, \
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation)
{
    
    int lastLeafPos;
    INT recount = 0;
    int l1, l2, l3;

    lastLeafPos = tree->leavesNum - 1;
    for (l1 = 0; l1 < lastLeafPos; ++l1)
    {
        for (l2 = l1 + 1; l2 < lastLeafPos; ++l2)
        {
            for (l3 = l2 + 1; l3 < lastLeafPos; ++l3)
            {
                recount += getMaxScore(tree, alignment, pwmMatrix,\
                            alpha, gapOpt, hashScore, permutation, 
                            lastLeafPos, l1, l2, l3);
            }
        }    
    }
    return recount;
} /* recountScoreHash */

INT* maxScoreAr(HashAlignment* alignment,\
        PWM* pwmMatrix,\
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation)
{
    char** names;
    INT* maxScoreArray;
    Tree* dummy;
    int i;
    
    names = hashAlignmentGetSeqNames(alignment);    

    maxScoreArray = (INT*)calloc(sizeof(INT), alignment->alignmentSize);
    maxScoreArray[0] = 0; maxScoreArray[1] = 0; maxScoreArray[2] = 0;
    
    dummy = growThreeLeavesTree(names[permutation[0]], names[permutation[1]], names[permutation[2]]);

    for (i = 3; i < alignment->alignmentSize; ++i)
    {
        dummy = treeAddLeaf(dummy, 0, 0, names[permutation[i]], 0, 1);
        maxScoreArray[i] = recountMaxScore(dummy, alignment,\
                                   pwmMatrix,\
                                   alpha, gapOpt, hashScore, permutation); 
    }

    treeDelete(dummy);
    free(names);
    return maxScoreArray;
} /* maxScoreAr */

INT countMaxScore(HashAlignment* alignment,\
        PWM* pwmMatrix,\
        int alpha, GapOpt gapOpt, INT**** hashScore, int* permutation)
{
    char** names;
    Tree* dummy;
    int maxScore = 0;
    int i;
    
    names = hashAlignmentGetSeqNames(alignment);    

    dummy = growThreeLeavesTree(\
                names[permutation[0]],\
                names[permutation[1]],\
                names[permutation[2]]);

    for (i = 3; i < alignment->alignmentSize; ++i)
    {
        dummy = treeAddLeaf(dummy, 0, 0, names[permutation[i]], 0, 1);
        maxScore += recountMaxScore(dummy, alignment,\
                                   pwmMatrix,\
                                   alpha, gapOpt, hashScore, permutation); 
    }

    treeDelete(dummy);
    free(names);

    return maxScore;
} /* countMaxScore */

