#include "growTree.h" 


Tree* growThreeLeavesTree(char* leaf1, char* leaf2, char* leaf3)
{
    Tree* result;
    Node* node;
   
    result = treeCreate();
    result->nodes = (Node**)calloc(sizeof(Node*), 4);
    result->leaves = (Node**)calloc(sizeof(Node*), 3);
    node = nodeCreate();
    result->nodes[0] = node;
    node->pos = 0;
    node = leafCreate(leaf1);
    result->nodes[1] = node;
    result->leaves[0] = node;
    node->pos = 1;
    nodeAddNeighbour(result->nodes[0], node, 0);
    nodeAddNeighbour(node, result->nodes[0], 0);
    node = leafCreate(leaf2);
    node->pos = 2;
    result->nodes[2] = node;
    result->leaves[1] = node;
    nodeAddNeighbour(result->nodes[0], node, 0);
    nodeAddNeighbour(node, result->nodes[0], 0);
    node = leafCreate(leaf3);
    node->pos = 3;
    result->nodes[3] = node;
    result->leaves[2] = node;
    nodeAddNeighbour(result->nodes[0], node, 0);
    nodeAddNeighbour(node, result->nodes[0], 0);
    result->leavesNum = 3;
    result->nodesNum = 4;
    return result;
} /* growThreeLeavesTree */


TreeWithScore* getBestChild(HashAlignment* alignment, TreeWithScore* treeWS,\
        char* newLeafName, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix,  INT**** hashScore, int* permutation) 
{
    INT score;
    int i, j;
    Tree* parent;
    TreeWithScore* bestChild;
    Tree* curChild;
    Node* curNode;

    parent = treeCopy(treeWS->tree, 0);
    curChild = treeAddLeaf(treeWS->tree, 0, 0, newLeafName, 1, 1);
    score = recountScoreHash(alignment, curChild, pwmMatrix, 
                             alpha, gapOpt, hashScore, permutation);

    bestChild = treeWithScoreCreate(curChild,  score);

    for(i = 0; i < parent->nodesNum; ++i)
    {
        curNode = parent->nodes[i];
        for (j = 0; j < curNode->neiNum; ++j)
        {
            if (curNode->neighbours[j]->pos <= i)
            {
                continue;
            }

            curChild = treeAddLeaf(parent, i, j, newLeafName, 0, 1);

            score = recountScoreHash(alignment, curChild, pwmMatrix, \
                    alpha, gapOpt,\
                    hashScore, permutation);
            
            if(score > bestChild->score)
            {
                treeDelete(bestChild->tree);
                bestChild->tree = treeCopy(curChild, 0);
                bestChild->score = score;
            }
            parent = treeRemoveLeaf(parent, parent->leavesNum - 1, 0, 0);
           
        }
    }

    bestChild->score += treeWS->score;
    return bestChild;
} /* getBestChild */


TreeWithScore* treeGrow(HashAlignment* alignment, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix,  INT**** hashScore, char randLeaves)
{

    TreeWithScore* result;
    TreeWithScore* newResult;
    char** names;    
    int* permutation;
    int k;

    names = hashAlignmentGetSeqNames(alignment);
    if (randLeaves)
    {
        permutation = getPermutation(alignment->alignmentSize);
    }
    else
    {
        permutation = getRange(0, alignment->alignmentSize);
    }


    result = treeWithScoreCreate(growThreeLeavesTree(names[permutation[0]], 
                                                     names[permutation[1]], 
                                                     names[permutation[2]]), 
                                 0);

    for (k = 3; k < alignment->alignmentSize; ++k)
    {
        //printf("%d leaf\n", k + 1);
        newResult = getBestChild(alignment, result, names[permutation[k]],
                                 alpha, gapOpt,
                                 pwmMatrix, hashScore, permutation);
        treeWithScoreDelete(result);
        result = newResult;
    }

    free(permutation);

    return result;
} /* treeGrow */

TreeWithScore* oneTreeGrow(HashAlignment* alignment, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix, INT**** hashScore, char randLeaves)
// PWM - for protein is PWM matrix, for nucleotide - null pointer
// storeHash - create hash to store score function(faster)
// or recalculate them each time (work much longer)
// randLeaves - if 0 than first tree is growing on first three sequences of
// alignment, otherwise - number of sequences' ll choose by random;
{
    
    TreeWithScore* result;
    printf("Grow one tree by stepwise addition\n");
    result = treeGrow(alignment, alpha, gapOpt, pwmMatrix, hashScore, randLeaves);
    printf("Tree score is %lu\n", result->score);
    return result;
} /*  oneTreeGrow */

TreeWithScore** multipleTreeGrow(HashAlignment* alignment,
                                 int alpha, GapOpt gapOpt,
                                 PWM* pwmMatrix,  unsigned int treeNum, INT**** hashScore)
{
    TreeWithScore** treeArray;
    int i;

    treeArray = (TreeWithScore**)malloc(sizeof(TreeWithScore*) * treeNum);
    if (treeNum > 1)
    {
        printf("Grow %d trees by stepwise addition\n", treeNum);
    }
    else
    {
        printf("Grow one tree by stepwise addition\n");
    }
    for (i = 0; i < treeNum; ++i)
    {
        treeArray[i] = treeGrow(alignment, alpha, gapOpt, pwmMatrix, hashScore, 1);
        printf("Tree number %d, score = %lu\n", i + 1, treeArray[i]->score);
    }
    treeWithScoreSort(treeArray, treeNum);
    return treeArray;
} /* multipleTreeGrow */

