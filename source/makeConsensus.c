#include <stdio.h>
#include "Tree.h"
#include "consensus.h"


int f()
{
    return 0;
}


int main(int argNum, char** args)
{
    int i = 0;
    int j = 0;
    int k = 0;
    Tree* consensusTree = NULL;

    Tree** treeArr = malloc(sizeof(Tree*) * (argNum - 2));
    for(i = 2; i < argNum; ++i)
    {
        treeArr[i - 2] = treeRead(args[i]);
    }
    consensusTree = makeConsensus(treeArr, argNum - 2, 0.5, 0);
    for(i = 2; i < argNum; ++i)
    {
        treeDelete(treeArr[i - 2]);
    }
    printf("%s\n", treeConsensusToString(consensusTree));
    treeConsensusWrite(consensusTree, args[1]);
    treeDelete(consensusTree);
    return 0;
}
