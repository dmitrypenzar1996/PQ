#include "TreeWS.h"

TreeWithScore* treeWithScoreCreate(Tree* tree, INT score)
{
    TreeWithScore* treeWS;
    treeWS = (TreeWithScore*)malloc(sizeof(TreeWithScore));
    treeWS->tree = tree;
    treeWS->score = score;
    return treeWS;
}

void treeWithScoreDelete(TreeWithScore* treeWS)
{
    treeDelete(treeWS->tree);
    free(treeWS);
}

int treeWithScoreCompare(const void* treeWS1, const void* treeWS2)
{
    return (*(TreeWithScore**)treeWS1)->score - (*(TreeWithScore**)treeWS2)->score;
}

void treeWithScoreSort(TreeWithScore** treeWSArray, size_t size)
{
    qsort(treeWSArray, size, sizeof(TreeWithScore*), treeWithScoreCompare);
    return;
}

