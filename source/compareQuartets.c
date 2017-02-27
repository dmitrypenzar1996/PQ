#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "Tree.h"
#include "add.h"


size_t* treeGetQuartetParameters(Tree* tree, size_t leaf11ID, size_t leaf12ID,
                                size_t leaf21ID, size_t leaf22ID)
{
    size_t* info = malloc(sizeof(size_t) * 5);
    size_t pos11 = tree->lcaFinder->inPos[tree->leaves[leaf11ID]->pos];
    size_t pos12 = tree->lcaFinder->inPos[tree->leaves[leaf12ID]->pos];
    size_t pos21 = tree->lcaFinder->inPos[tree->leaves[leaf21ID]->pos];
    size_t pos22 = tree->lcaFinder->inPos[tree->leaves[leaf22ID]->pos];
    
    size_t ans1 = sparseTableRMQ(tree->lcaFinder->sparceTable,
            pos11, pos12); 
    size_t ans2 = sparseTableRMQ(tree->lcaFinder->sparceTable,
            pos21, pos22); 

    size_t grand = sparseTableRMQ(tree->lcaFinder->sparceTable,
            ans1, ans2);
    
    size_t dist11_ans1 = tree->lcaFinder->deep[pos11] -
        tree->lcaFinder->deep[ans1];
    size_t dist12_ans1 = tree->lcaFinder->deep[pos12] -
        tree->lcaFinder->deep[ans1];
    size_t dist21_ans2 = tree->lcaFinder->deep[pos21] -
        tree->lcaFinder->deep[ans2];
    size_t dist22_ans2 = tree->lcaFinder->deep[pos22] -
        tree->lcaFinder->deep[ans2];
    size_t dist_ans1_ans2 = tree->lcaFinder->deep[ans1] +
        tree->lcaFinder->deep[ans2] - tree->lcaFinder->deep[grand] * 2;
    info[0] = dist11_ans1;
    info[1] = dist12_ans1;
    info[2] = dist21_ans2;
    info[3] = dist22_ans2;
    info[4] = dist_ans1_ans2;
    return info;
}



int main(int argsNum, char** args)
{ 
    assert(argsNum == 3);
    DIR* d = NULL;;
    size_t fileNum = 0;
    struct dirent* dir = NULL;
    Tree* fullEtalon = treeRead(args[1]);
    Tree* etalon = NULL;
    Tree* tree = NULL;
    int isAgree = 0;
    char* pathName = malloc(sizeof(char) * 1000);
    char** treeNames = NULL;
    char** etalonNames = NULL;
    int* permutation = NULL;
    size_t l1 = 0;
    size_t l2 = 0;
    size_t l3 = 0;
    size_t l4 = 0;

    size_t splitEtalonPos = 0;
    size_t splitTreePos = 0;
    size_t pos11 = 0;
    size_t pos12 = 0;
    size_t pos21 = 0;
    size_t pos22 = 0;
    size_t* params = NULL;

    d = opendir(args[2]);
    if(!d)
    {
        raiseError("Wrong dir name",
                __FILE__, __FUNCTION__, __LINE__);
    }

    while((dir = readdir(d)) != NULL)
    {
        if (!(
            strlen(dir->d_name) < 4 ||
            ( strcmp(".tre", dir->d_name + strlen(dir->d_name) - 4) && 
            strcmp(".nwk", dir->d_name + strlen(dir->d_name) - 4))
        ))
        {
           ++fileNum; 
        } 
    } 



    
    fprintf(stdout, "Tree_file_name;leaf11;leaf12;leaf21;leaf22;dist11ans1;dist12ans1;dist21ans2;dist22ans2;dist_ans1_ans2;AgreeWithEtalon\n");

    d = opendir(args[2]);
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
        sprintf(pathName, "%s/%s", args[2], dir->d_name);

        tree = treeRead(pathName);
    
        treeNames = treeGetNames(tree);

        etalon = treePrune(fullEtalon, treeNames,
            tree->leavesNum, 1); // error here 

        etalonNames = treeGetNames(etalon);
        
        permutation = calculatePermutation(
            treeNames, etalonNames,
            tree->leavesNum);
        if (!permutation)
        {
            raiseError("Dont find some names in etalon tree",
                __FILE__, __FUNCTION__, __LINE__);
        }



        for(l1 = 0; l1 < tree->leavesNum; ++l1)
        {
        for(l2 = l1 + 1; l2 < tree->leavesNum; ++l2)
        {
        for(l3 = l2 + 1; l3 < tree->leavesNum; ++l3)
        {
        for(l4 = l3 + 1; l4 < tree->leavesNum; ++l4)
        {
            splitTreePos = treeWhichSplit(tree, 
                l1, l2, l3, l4);

            splitEtalonPos = treeWhichSplit(etalon, permutation[l1],
                permutation[l2], permutation[l3], permutation[l4]);

            pos11 = permutation[l1];
            switch (splitEtalonPos)
            {
                case 2:
                    pos12 = permutation[l2];
                    pos21 = permutation[l3];
                    pos22 = permutation[l4];
                    break;
                case 3:
                    pos12 = permutation[l3];
                    pos21 = permutation[l2];
                    pos22 = permutation[l4];
                    break;
                case 4:
                    pos12 = permutation[l4];
                    pos21 = permutation[l2];
                    pos22 = permutation[l3];
                    break;
                default:
                    perror("Error. Unexpected behaviour");
                    exit(1);
            }     


            isAgree = splitTreePos == splitEtalonPos ? 1 : 0;

            params = treeGetQuartetParameters(etalon,
                pos11, pos12,
                pos21, pos22); 
            fprintf(stdout, "%s;%s;%s;%s;%s;%zu;%zu;%zu;%zu;%zu;%d\n", 
                pathName,
                etalon->leaves[pos11]->name,
                etalon->leaves[pos12]->name,
                etalon->leaves[pos21]->name,
                etalon->leaves[pos22]->name,
                params[0],
                params[1],
                params[2],
                params[3],
                params[4],
                isAgree);
        }
        }
        }
        }
        free(permutation);
        free(treeNames);
        free(etalonNames);
        treeDelete(tree);
        treeDelete(etalon);
        tree = NULL;
        etalon = NULL;
    }


    treeDelete(fullEtalon);
    free(pathName);
    return 0;
}
