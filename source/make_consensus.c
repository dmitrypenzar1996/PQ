#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "Tree.h"
#include "consensus.h"


int main(int argNum, char** args)
{
    int i = 0;
    size_t curTreeArrSize = 0;
    size_t maxTreeArrSize = 100;
    Tree** trees = NULL;
    Tree* consensus = NULL;
    struct dirent* dir = NULL;
    DIR* d;
    char* pathName = malloc(sizeof(char) * 10000);
    char* dirName = NULL;
    char* outFileName = NULL;

    if (argNum != 3)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }

    dirName = args[1];
    outFileName = args[2];

    d = opendir(dirName);
    if(!d)
    {
        raiseError("Wrong dir name",
                __FILE__, __FUNCTION__, __LINE__);
    }

    trees = malloc(sizeof(maxTreeArrSize) * sizeof(Tree*));

    while((dir = readdir(d)) != NULL)
    {
        if (
            strlen(dir->d_name) < 4 ||
            ( strcmp(".tre", dir->d_name + strlen(dir->d_name) - 4) && 
            strcmp(".nwk", dir->d_name + strlen(dir->d_name) - 4))
        )
        {
            continue;
        } 

        if (curTreeArrSize == maxTreeArrSize)
        {
            maxTreeArrSize = maxTreeArrSize * 3 / 2 + 1;
            trees = realloc(trees, sizeof(Tree*) * maxTreeArrSize);
        }
        sprintf(pathName, "%s/%s", dirName, dir->d_name);
        printf("%s\n", pathName);
        trees[curTreeArrSize] = treeRead(pathName);
        ++curTreeArrSize;
    }
    assert(1==0);

    trees = realloc(trees, sizeof(Tree*) * curTreeArrSize);

    printf("HII\n");
    consensus = makeConsensus(trees, curTreeArrSize, 0.5, 1);
    printf("HII1\n");
    treeConsensusWrite(consensus, outFileName);
    for(i = 0; i < curTreeArrSize; ++i)
    {
        treeDelete(trees[i]);
    }
    free(trees);
    free(consensus);
}
