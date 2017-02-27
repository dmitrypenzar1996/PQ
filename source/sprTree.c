#include "sprTree.h"

TreeWithScore* simpleSPR(Tree* inTree, HashAlignment* alignment,
                         PWM* pwmMatrix, int alpha, GapOpt gapOpt, INT**** hashScore)
{
    int i, j, k, h;
    char** treeNames;
    char** seqNames;
    int* permutation;
    int isUpdate;
    unsigned newBranchNodeID = 0;
    unsigned newBranchNeiPos = 0;
    INT score;
    TreeWithScore* curBestTree;
    TreeWithScore* result;
    int counter;    

    treeNames = treeGetNames(inTree);
    seqNames = hashAlignmentGetSeqNames(alignment);
    permutation = calculatePermutation(treeNames, seqNames,\
                                       alignment->alignmentSize);
    free(treeNames);
    free(seqNames);
    
    score = countScoreHash(alignment, inTree, pwmMatrix,\
                           alpha, gapOpt, hashScore, permutation);
    result = treeWithScoreCreate(treeCopy(inTree, 0), score);

    treeWash(inTree);
    printf("Performing simple SPR hill climbing, initial  score is %lu\n", score);

    isUpdate = TRUE;
    while (isUpdate)
    {
        isUpdate = FALSE;
        counter = 0;
        curBestTree = treeWithScoreCreate(treeCopy(result->tree, 0), result->score);
        for (i = 0; (i < result->tree->nodesNum) && (!isUpdate); ++i)
        {
            for(j = 0; (j < result->tree->nodes[i]->neiNum)  && (!isUpdate); ++j)
            {
                if (result->tree->nodes[i]->neighbours[j]->pos > i)
                {
                /* printf("Branch %d %d with\n", i, result->tree->nodes[i]->neighbours[j]->pos); */
                    for (k = 0; (k < result->tree->nodesNum)  && (!isUpdate); ++k)
                    {
                        if (k != i && k != result->tree->nodes[i]->neighbours[j]->pos)
                        {
                            for (h = 0; (h < inTree->nodes[k]->neiNum)  && (!isUpdate); ++h)
                            {
                                if (result->tree->nodes[k]->neighbours[h]->pos != i &&\
                                    result->tree->nodes[k]->neighbours[h]->pos !=\
                                    result->tree->nodes[i]->neighbours[j]->pos &&\
                                    result->tree->nodes[k]->neighbours[h]->pos > k)
                                {
                                    //printf("    Branch %d %d \n", k, result->tree->nodes[k]->neighbours[h]->pos);
                                    result->tree = treeSPRMove(result->tree, i, j, k, h,\
                                            &newBranchNodeID, &newBranchNeiPos, 0, 1); 
                                    score = countScoreHash(alignment, result->tree, pwmMatrix,\
                                            alpha, gapOpt, hashScore, permutation);
                                    counter++;
                                    printf("%d SPRs regarded\r", counter);
                                    if (score > curBestTree->score)
                                    {
                                        printf("\nTree with score %lu found\n", score);
                                        treeDelete(curBestTree->tree);
                                        curBestTree->tree = treeCopy(result->tree, 0);
                                        curBestTree->score = score;
                                        isUpdate = TRUE;
                                    }
                                    // revert change
                                    result->tree = treeSPRMove(result->tree, i, j, newBranchNodeID,\
                                                newBranchNeiPos, 0, 0, 0, 0);
                                }
                            }
                        }
                    }
                } 
            } 
        }
        
        treeWithScoreDelete(result);
        result = curBestTree;
    }
    printf("\nNo better trees found\n");
    return result;
}   /* simpleSPR */

TreeWithScore* gradientSPR(Tree* inTree, HashAlignment* alignment,
                           PWM* pwmMatrix, int alpha, GapOpt gapOpt, INT**** hashScore)
{
    int i, j, k, h;
    char** treeNames;
    char** seqNames;
    int* permutation;
    int isUpdate;
    unsigned newBranchNodeID = 0;
    unsigned newBranchNeiPos = 0;
    INT score;
    TreeWithScore* curBestTree;
    TreeWithScore* result;

    treeNames = treeGetNames(inTree);
    seqNames = hashAlignmentGetSeqNames(alignment);
    permutation = calculatePermutation(treeNames, seqNames,\
                                       alignment->alignmentSize);
    free(treeNames);
    free(seqNames);
    
    score = countScoreHash(alignment, inTree, pwmMatrix,\
                           alpha, gapOpt, hashScore, permutation);
    result = treeWithScoreCreate(treeCopy(inTree, 0), score);

    treeWash(inTree);
    printf("Performing gradient SPR hill climbing, initial  score is %lu\n", score);

    isUpdate = TRUE;
    while (isUpdate)
    {
        isUpdate = FALSE;
        curBestTree = treeWithScoreCreate(treeCopy(result->tree, 0), result->score);
        for (i = 0; i < result->tree->nodesNum; ++i)
        {
            for(j = 0; j < result->tree->nodes[i]->neiNum; ++j)
            {
                if (result->tree->nodes[i]->neighbours[j]->pos > i)
                {
                /* printf("Branch %d %d with\n", i, result->tree->nodes[i]->neighbours[j]->pos); */
                    for (k = 0; (k < result->tree->nodesNum)  && (!isUpdate); ++k)
                    {
                        if (k != i && k != result->tree->nodes[i]->neighbours[j]->pos)
                        {
                            for (h = 0; h < inTree->nodes[k]->neiNum; ++h)
                            {
                                if (result->tree->nodes[k]->neighbours[h]->pos != i &&\
                                    result->tree->nodes[k]->neighbours[h]->pos !=\
                                    result->tree->nodes[i]->neighbours[j]->pos &&\
                                    result->tree->nodes[k]->neighbours[h]->pos > k)
                                {
                                    //printf("    Branch %d %d \n", k, result->tree->nodes[k]->neighbours[h]->pos);
                                    result->tree = treeSPRMove(result->tree, i, j, k, h,\
                                            &newBranchNodeID, &newBranchNeiPos, 0, 1); 
                                    score = countScoreHash(alignment, result->tree, pwmMatrix,\
                                            alpha, gapOpt, hashScore, permutation);
                                    if (score > curBestTree->score)
                                    {
                                        printf("Tree with score %lu found\n", score);
                                        treeDelete(curBestTree->tree);
                                        curBestTree->tree = treeCopy(result->tree, 0);
                                        curBestTree->score = score;
                                        isUpdate = TRUE;
                                    }
                                    // revert change
                                    result->tree = treeSPRMove(result->tree, i, j, newBranchNodeID,\
                                                newBranchNeiPos, 0, 0, 0, 0);
                                }
                            }
                        }
                    }
                } 
            } 
        }
        
        treeWithScoreDelete(result);
        result = curBestTree;
    }
    printf("No better trees found\n");
    return result;
}   /* gradientSPR */
