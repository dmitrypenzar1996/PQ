/*  Copyright 2016, 2017 Andrew Sigorskih, Sergei Spirin 
    Contact: sas@belozersky.msu.ru
    Homepage: http://mouse.belozersky.msu.ru/tools/umast.html

    This file is part of PQ.

    UMAST is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    UMAST is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with UMAST in a file named "COPYING.txt". 
    If not, see <http://www.gnu.org/licenses/>.
*/


#include "maxAgreement.h"
#include <time.h>

BranchArray* treeRootedToBranchArray(Tree* tree, int* permutation)
{
    INT p = 1;
    int i = 0;
    int j = 0;
    unsigned branchNum = tree->nodesNum;
    BranchArray* ba = branchArrayCreate(branchNum);
    NodeStack* stack = nodeStackCreate(tree->nodesNum);
    Node* curNode = 0;
    Node* nextNode = 0;
    if (tree->leavesNum == 0)
    {
        fprintf(stderr, "Error, tree has no leaves, umast:treeRootedToBranchArray\n");
        exit(1);
    }
    if (tree == 0){
        fprintf(stderr, "Error, null Tree pointer, umast:treeRootedToBranchArray\n");
        exit(1);
    }
    treeWash(tree);
    for(i = 0; i < branchNum; ++i)
    {
        branchArrayAdd(ba, branchCreate(tree->leavesNum));
    }

    for(i = 0; i < tree->leavesNum; ++i)
    {
        p = 1;
        p = p << (permutation[i] & (intSize - 1));
        ba->array[tree->leaves[i]->pos]->branch[permutation[i] / intSize] |= p;
        tree->leaves[i]->color = BLACK;
    }
    curNode = tree->nodes[tree->rootId];
    nodeStackPush(stack, curNode);
    curNode->color = GREY;

    while (stack->curSize != 0)
    {
        curNode = nodeStackPeek(stack);
        nextNode = 0;
        for(i = 0; i < curNode->neiNum; ++i)
        {
            if (curNode->neighbours[i]->color == WHITE)
            {
                nextNode = curNode->neighbours[i];
                break;
            }
        }
        if (nextNode)
        {
            nodeStackPush(stack, nextNode);
            nextNode->color = GREY;
        }
        else
        {
            for(i = 0; i < curNode->neiNum; ++i)
            {
                for(j = 0; j < branchGetIntSize(ba->array[curNode->pos]); ++j)
                {
                    ba->array[curNode->pos]->branch[j] |= \
                            ba->array[curNode->neighbours[i]->pos]->branch[j];
                }
            }
            nodeStackPop(stack);
            curNode->color = BLACK;
        }
    }

    nodeStackDelete(stack);
    return ba;
} //treeRootedToBranchArray

void stackCycle(Node* node, NodeStack* stack)
{
    int j;
    if (node->neiNum > 1)
    {
        node->color = BLACK;
        nodeStackPush(stack, node);
        for (j = 0; j < node->neiNum; j++)
        {
            if (node->neighbours[j]->color != BLACK)
            {
                stackCycle(node->neighbours[j], stack);
            }
        }
    }
    else // (node->neiNum == 1 or 0)
    {
        if (node->color !=BLACK)
        {
            node->color = BLACK;
            nodeStackPush(stack, node);
        }
    }
} //stackCycle

Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount)
{
    int i, j;
    Tree* result = treeCreate();
    result->nodes = (Node**)calloc(sizeof(Node*), tree->nodesNum);
    result->leaves = (Node**)calloc(sizeof(Node*), tree->leavesNum);
    result = treeCopy(tree, 0);
    for (i = 0; i < leavesToDeleteAmount; i++)
    {
        for (j=0; j < result->leavesNum; j++)
        {
            if (strcmp(leavesToDelete[i], result->leaves[j]->name)==0)
            {
                result = treeRemoveLeaf(result, j, 0, 0);
            }
        }
    }
    return result;
} //deleteLeaves

Tree** treesPrune(Tree* tree1, Tree* tree2)
{
    int i, j;
    int count1, count2;
    char** leavesToSave1;
    char** leavesToSave2;
    Tree** result;

    count1 = 0;
    count2 = 0;
    result = (Tree**)calloc(sizeof(Tree*), 2);
    result[0] = treeCreate();
    result[1] = treeCreate();
    leavesToSave1 = (char**)calloc(sizeof(char*), tree1->leavesNum);
    leavesToSave2 = (char**)calloc(sizeof(char*), tree2->leavesNum);

    for (i = 0; i < tree1->leavesNum; i++)
    {
        for (j = 0; j < tree2->leavesNum; j++)
        {
            if (strcmp(tree1->leaves[i]->name, tree2->leaves[j]->name) == 0)
            {
                leavesToSave1[count1] = tree1->leaves[i]->name;
                count1++;
                break;
            }
        }
    }
    for (i = 0; i < tree2->leavesNum; i++)
    {
        for (j = 0; j < tree1->leavesNum; j++)
        {
            if (strcmp(tree2->leaves[i]->name, tree1->leaves[j]->name) == 0)
            {
                leavesToSave2[count2] = tree2->leaves[i]->name;
                count2++;
                break;
            }
        }
    }

    if (count1 > tree1->leavesNum)
    {
        fprintf(stderr, "Error, wrong count 1, umast:treesPrune\n");
        exit(1);
    }

    if (count2 > tree2->leavesNum)
    {
        fprintf(stderr, "Error, wrong count 2, umast:treesPrune\n");
        exit(1);
    }

    if ((count1 == 0) || (count2 == 0))
    {
        fprintf(stderr, "Error, trees do not have equal leaves, umast:treesPrune\n");
        exit(1);
    }

    if (count1 == tree1->leavesNum)
    {
        result[0] = treeCopy(tree1, 0);
    }
    else
    {
        leavesToSave1 = realloc(leavesToSave1, (sizeof(char*) * (count1)));
        result[0] = treePrune(tree1, leavesToSave1, (size_t) count1, 0);
    }

     if (count2 == tree2->leavesNum)
    {
        result[1] = treeCopy(tree2, 0);
    }
    else
    {
        leavesToSave2 = realloc(leavesToSave2, (sizeof(char*) * (count2)));
        result[1] = treePrune(tree2, leavesToSave2, (size_t) count2, 0);
    }

    free(leavesToSave1); 
    free(leavesToSave2);
    return result;
} //treesPrune

Tree* treeRoot(Tree* tree, unsigned nodeID, unsigned neighbourID, char newTree){
    int i;
    Node* root;
    Node* prevNode;
    Tree* result;

    if (tree == 0){
        fprintf(stderr, "Error, null tree pointer, umast:treeRoot\n");
        exit(1);
    }
    if (tree->leavesNum == 0)
    {
        fprintf(stderr, "Error, cannot root empty tree, umast:treeRoot\n");
        exit(1);
    }
    if (nodeID >= tree->nodesNum)
    {
       fprintf(stderr, "Error, node index is out of range, umast:treeRoot\n");
        exit(1);
    }
    if (neighbourID >= tree->nodes[nodeID]->neiNum)
    {
        fprintf(stderr, "Error, neighbour index is out of range, (%d of %d)\
                umast:treeRoot\n", neighbourID, tree->nodes[nodeID]->neiNum);
       exit(1);
    }
    result = tree;
    if (newTree)
    {
        result = treeCopy(tree, 0);
    }

    root = nodeCreate();
    root->pos = result->nodesNum;

    prevNode = result->nodes[nodeID]->neighbours[neighbourID];
    result->nodes[nodeID]->neighbours[neighbourID] = root;

    for(i = 0; i < prevNode->neiNum; i++)
    {
        if (prevNode->neighbours[i] == result->nodes[nodeID])
        {
            prevNode->neighbours[i] = root;
            break;
        }
    }

    nodeAddNeighbour(root, result->nodes[nodeID], 0);
    nodeAddNeighbour(root, prevNode, 0);
    result->nodesNum += 1;
    result->nodes = realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
    result->nodes[result->nodesNum - 1] = root;
    result->rootId = root->pos;
    return result;
} //treeRoot

Tree* treeUnRoot(Tree* tree, char newTree)
{
    int i, j, k;
    Node* curnode;
    Tree* result;
    
    if (tree == 0)
    {
        fprintf(stderr, "Error, null tree pointer, umast:treeUnRoot\n");
        exit(1);
    }
    if (tree->leavesNum == 0)
    {
        fprintf(stderr, "Error, cant root empty tree, umast:treeUnRoot\n");
        exit(1);
    }

    result = tree;
    if (newTree)
    {
        result = treeCopy(tree, 0);
    }
    if (tree->rootId != (tree->nodesNum - 1))
    {
        fprintf(stderr, "Error, root is not the last node, umast:treeUnRoot\n");
        exit(1);
    }
    
    curnode = result->nodes[result->rootId];
    for (i = 0; i < curnode->neiNum; i++)
    {
        for (j = 0; j < curnode->neiNum; j++)
        {
            if (i != j)
            {
                for (k = 0; k < curnode->neighbours[i]->neiNum; k++)
                {
                    if (curnode->neighbours[i]->neighbours[k] == curnode)
                    {
                        curnode->neighbours[i]->neighbours[k] = curnode->neighbours[j];
                    }
                }
            }
        }
    }

    nodeDelete(curnode);
    result->rootId = -1;
    result->nodesNum--;
    result->nodes =  realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
    return result;
} //treeUnRoot


unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation)
{
    int i, j;
    unsigned* result;
    Node* root;
    Node* prevNode;
    NodeStack* treeStack;

    tree = treeRoot(tree, nodeID, neighbourID, 0);
    root = tree->nodes[tree->rootId];
    treeStack = nodeStackCreate(tree->nodesNum);
    result = (unsigned*)calloc(sizeof(unsigned), tree->nodesNum);
    treeWash(tree);
    stackCycle(root, treeStack);
    while (treeStack->curSize != 0)
    {
        i = tree->nodesNum - treeStack->curSize;
        prevNode = nodeStackPeek(treeStack);
        result[i] = prevNode->pos;
        nodeStackPop(treeStack);
    }
    nodeStackDelete(treeStack);
    treeWash(tree);
    for (i = 0; i < tree->nodesNum; i++)
    {
        for (j = 0; j < tree->nodesNum; j++)
        {
            if (i == result[j])
            {
                setPermutation[i] = j;
            }
        }
     }
    return result;
} //treeRootAndTopSort


int* calculateLeavesPermutation(Tree* tree1, Tree* tree2)
{
    int i;
    char** leaves1;
    char** leaves2;
    int* permutation;
    leaves1 = (char**)malloc(sizeof(char*) * tree1->leavesNum);
    leaves2 = (char**)malloc(sizeof(char*) * tree2->leavesNum);
    for (i = 0; i < tree1->leavesNum; i++)
    {
        leaves1[i] = tree1->leaves[i]->name;
    }
    for (i = 0; i < tree2->leavesNum; i++)
    {
        leaves2[i] = tree2->leaves[i]->name;
    }

    permutation = calculatePermutation(leaves1, leaves2, tree1->leavesNum);
    free(leaves1); 
    free(leaves2);
    return permutation;
} //calculateLeavesPermutation

int branchGetLeavesPosNum(Branch* br)
{
    unsigned i = 0;
    unsigned j = 0;
    unsigned k = 0;
    int curSize = 0;
    for(i = 0; i < branchGetIntSize(br); ++i)
    {
        j = 0;
        while(j < intSize)
        {
            k = countZeroRightNum((br->branch[i]) >> j);
            if (k != intSize)
            {
                curSize++;
            }
            j += k + 1;
        }
    }
    return curSize;
} //branchGetLeavesPosNum

int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y)
{
    int* variants;
    int* result;
    int i, k;
    variants = (int*)calloc(sizeof(int), 8);
    result = (int*)calloc(sizeof(int), 6);
    for (i = 0; i < 8; i++)
    {
        variants[i] = -1;
    }

    variants[0] = branchGetLeavesPosNum(TAB[b][x]);
    variants[1] = branchGetLeavesPosNum(TAB[c][y]);
    variants[2] = branchGetLeavesPosNum(TAB[b][y]);
    variants[3] = branchGetLeavesPosNum(TAB[c][x]);
    variants[4] = branchGetLeavesPosNum(TAB[a][x]);
    variants[5] = branchGetLeavesPosNum(TAB[a][y]);
    variants[6] = branchGetLeavesPosNum(TAB[b][w]);
    variants[7] = branchGetLeavesPosNum(TAB[c][w]);

    result[0] = variants[0] + variants[1];
    result[1] = variants[2] + variants[3];
    result[2] = variants[4];
    result[3] = variants[5];
    result[4] = variants[6];
    result[5] = variants[7];
    free(variants);
    return result;
} //countVariants


int* getTreeLeavesPos(Tree* tree)
{
    int* leavesPosArr;
    int i, j;
    leavesPosArr = (int*)calloc(sizeof(int), tree->nodesNum);
    for (i = 0;  i < tree->nodesNum; i++)
    {
        leavesPosArr[i] = -1;
    }
    for (i = 0; i < tree->leavesNum; i++)
    {
        leavesPosArr[tree->leaves[i]->pos] = i;
    }
    return leavesPosArr;
} //getTreeLeavesPos

unsigned* branchToLeavesArr(Branch* br, unsigned leavesNum)
{
    int i, j;
    INT curInt;
    int curPos = 0;
    unsigned* leavesPosArr;
    int curSize;
    leavesPosArr = (unsigned*)calloc(sizeof(unsigned), leavesNum);
    for (i = 0; i < leavesNum; i++)
    {
        leavesPosArr[i] = 0;
    }

    for (i = 0; i < branchGetIntSize(br); ++i)
    {
        curInt = br->branch[i];
        curSize = leavesNum - (i * intSize);
        curSize = curSize > intSize ? intSize : curSize;
        for (j = 0; j < curSize; ++j)
        {
            if ((curInt & 1) == 1)
            {
                leavesPosArr[curPos] = 1;
            }
            ++curPos;
            curInt >>=1;
        }
    }
    return leavesPosArr;
} //branchToLeavesArr

int** getAllRoots(Tree* tree)
{
    int i, j,count;
    unsigned* sortedSet;
    int** result;
    Node* prevNode;
    NodeStack* treeStack;
    treeStack = nodeStackCreate(tree->nodesNum);
    sortedSet = (unsigned*)calloc(sizeof(unsigned), tree->nodesNum);
    result = (int**)calloc(sizeof(int*), (tree->nodesNum - 1));
    for(i = 0; i < (tree->nodesNum - 1); i++)
    {
        result[i] = (int*)calloc(sizeof(int), 2);
    }
    for (i = 0; i < (tree->nodesNum - 1); i++)
    {
        for(j = 0; j < 2; j++)
        {
            result[i][j] = -1;
        }
    }
    treeWash(tree);
    for (i = 0; i < tree->nodesNum; i++)
    {
        if (tree->nodes[i]->neiNum > 1)
        {
            break;
        }
    }
    
    stackCycle(tree->nodes[i], treeStack);
    
    while (treeStack->curSize != 0)
    {
        i = tree->nodesNum - treeStack->curSize;
        prevNode = nodeStackPeek(treeStack);
        sortedSet[i] = prevNode->pos;
        nodeStackPop(treeStack);
    }

    treeWash(tree);
    count = 0;
    
    for (i = 0; i < tree->nodesNum; i++)
    {
        prevNode = tree->nodes[sortedSet[i]];
        prevNode->color = BLACK;
        for (j = 0; j < prevNode->neiNum; j++)
        {
            if (prevNode->neighbours[j]->color == WHITE)
            {
                result[count][0] = prevNode->pos;
                result[count][1] = j;
                count++;
            }
        }
    }

    if (count > (tree->nodesNum - 1))
    {
        fprintf(stderr, "Error:count > nodes num, umast:getAllRoots\n");
        exit(1);
    }

    nodeStackDelete(treeStack);
    free(sortedSet);
    return result;
} //getAllRoots

int* getRandMaxBranch(Branch*** TAB, int rows, int cols)
{
    int i, j, k = -1, count = 0, a = 0;
    int** coords;
    int* result;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (branchGetLeavesPosNum(TAB[i][j]) > k)
            {
                k = branchGetLeavesPosNum(TAB[i][j]);
            }
        }
    }

    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if (branchGetLeavesPosNum(TAB[i][j]) == k)
            {
                count++;
            }
        }
    }

    coords = (int**)calloc(sizeof(int*), count);
    for(i = 0; i < count; i++)
    {
        coords[i] = (int*)calloc(sizeof(int), 2);
    }
    for (i = 0; i < count; i++)
    {
        for(j = 0; j < 2; j++)
        {
            coords[i][j] = -1;
        }
    }

    result = (int*)calloc(sizeof(int), 2);
    for(j = 0; j < 2; j++)
    {
        result[j] = -1;
    }

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (branchGetLeavesPosNum(TAB[i][j]) == k)
            {
                coords[a][0] = i;
                coords[a][1] = j;
                a++;
            }
        }
    }

    if (a > count)
    {
        fprintf(stderr, "Error, failed to collect all branches of maximum size, umast:getRandMaxBranch\n");
        exit(1);
    }

    k = rand() % count;
    result[0] = coords[k][0];
    result[1] = coords[k][1];

    for (i = 0; i < count; i++)
    {
        free(coords[i]);
    }
    free(coords);
    return(result);
} //getRandMaxBranch

Tree* makeMAST(Branch* br, Tree* tree1)
{
    int i, count;
    unsigned leavesToSaveAmount = 0;
    unsigned* leavesPosArr;
    char** leavesToSave;
    Tree* result;

    treeWash(tree1);
    result = treeCreate();
    leavesPosArr = branchToLeavesArr(br, tree1->leavesNum);
    for (i = 0; i < tree1->leavesNum; i++)
    {
        if (leavesPosArr[i] == 1)
        {
            leavesToSaveAmount++;
        }
    }

    if (leavesToSaveAmount == tree1->leavesNum)
    {
        result = treeCopy(tree1, 0);
        free(leavesPosArr);
        free(leavesToSave);
        return result;
    }
    else if (leavesToSaveAmount == 0)
    {
 
        free(leavesPosArr);
        free(leavesToSave);
        return result;
    }
    else //some leaves are to be deleted but not all
    {
        leavesToSave = (char**)malloc(sizeof(char*) * (leavesToSaveAmount));
        result = treeCopy(tree1, 0);
        count = 0;
        for (i = 0; i < tree1->leavesNum; i++){
            if (leavesPosArr[i] == 1){
                leavesToSave[count] = tree1->leaves[i]->name;
                count++;
            }
        }

        if (count > (leavesToSaveAmount))
        {
            fprintf(stderr, "Error, count exceeds umast size, umast:makeMast\n");
            exit(1);
        }

        result = treePrune(tree1, leavesToSave, (size_t) leavesToSaveAmount, 0);
        free(leavesPosArr);
        free(leavesToSave); 
        return result;
    }
} //makeMAST

Branch* MAST(Tree* intree1, Tree* intree2, unsigned* set1, unsigned* set2, unsigned* setPermutation1, unsigned* setPermutation2)
{
    INT p;
    int b, c, x, y; b = 0; c = 0; x = 0; y = 0;
    int i, j, k;
    int posT, posU;
    int a, w;
    int* variants;
    int* permutation;           //permutation of leaf sets between tree1 and tree2
    int* leavesPosArr1;         //size = nodes; for every node has -1, for leaf has its pos in leaves
    int* leavesPosArr2;         //the same for tree2
    int* randMax;
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    Branch* intersection;
    Branch* temp;
    Branch*** TAB;

    Tree* tree1;
    Tree* tree2;
    tree1 = treeCopy(intree1, 1);
    tree2 = treeCopy(intree2, 1);

    permutation = calculateLeavesPermutation(tree2, tree1);
    branchArr1 = treeRootedToBranchArray(tree1, getRange(0, tree1->leavesNum));
    branchArr2 = treeRootedToBranchArray(tree2, permutation);

    leavesPosArr1 = getTreeLeavesPos(tree1);
    leavesPosArr2 = getTreeLeavesPos(tree2);
    TAB = (Branch***)calloc(sizeof(Branch**), tree1->nodesNum);
    for(i = 0; i < tree1->nodesNum; i++)
    {
        TAB[i] = (Branch**)calloc(sizeof(Branch*), tree2->nodesNum);
    }

    for (i = 0; i < tree1->nodesNum; i++)
    {
        for(j = 0; j < tree2->nodesNum; j++)
        {
            TAB[i][j] = NULL;
        }
    }

    posT = 0;
    for (a = 0; a < tree1->nodesNum; a++) //Until all lines in TAB are filled
    { 
        posU = 0;
        for (w = 0; w < tree2->nodesNum; w++) //Until all cells in line are filled
        { 
            b = -1; c = -1; x = -1; y = -1;
            if ((tree1->nodes[set1[a]]->neiNum == 1) || (tree2->nodes[set2[w]]->neiNum == 1)) //if a or w is a leaf
            { 
                if ((tree1->nodes[set1[a]]->neiNum == 1) && (tree2->nodes[set2[w]]->neiNum == 1)) //if a AND w are leaves
                { 
                    if (strcmp(tree1->nodes[set1[a]]->name, tree2->nodes[set2[w]]->name) == 0) //a and w are equal leaves
                    {
                        intersection = branchCreate(tree1->leavesNum);
                        p = 1;
                        p = p << (leavesPosArr1[set1[a]] & (intSize - 1));
                        intersection->branch[leavesPosArr1[set1[a]] / intSize] |= p;
                        TAB[posT][posU] = intersection;
                    }
                    else //leaves are not equal
                    { 
                        intersection = branchCreate(tree1->leavesNum);
                        TAB[posT][posU] = intersection;
                    }
                }
                else //if a or w is a subtree
                { 
                    intersection = branchCreate(tree1->leavesNum);
                    if (tree1->nodes[set1[a]]->neiNum == 1) //a is a leaf, w is a subtree
                    { 
                       p = 1;
                       p = p << (leavesPosArr1[set1[a]] & (intSize - 1));
                       intersection->branch[leavesPosArr1[set1[a]] / intSize] |= p;
                       temp = branchAnd(intersection, branchArr2->array[set2[w]]);
                       branchDelete(intersection);
                       TAB[posT][posU] = temp;
                    }
                    else //a is a subtree, w is a leaf
                    { 
                       p = 1;
                       p = p << (permutation[leavesPosArr2[set2[w]]] & (intSize - 1));
                       intersection->branch[permutation[leavesPosArr2[set2[w]]] / intSize] |= p;
                       temp = branchAnd(intersection, branchArr1->array[set1[a]]);
                       branchDelete(intersection);
                       TAB[posT][posU] = temp;
                    }
                }
            }
            else //nor a nor w are leaves
            {
                for (i = 0; i < tree1->nodes[set1[a]]->neiNum; i++)
                {
                    if (setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos] < a)
                    {
                        b = setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos];
                        break;
                    }
                }
                for (i = 0; i < tree1->nodes[set1[a]]->neiNum; i++)
                {
                    if ((setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos] < a) \
                            && (setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos] != b))
                    {
                        c = setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos];
                        break;
                    }
                }
                for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++)
                {
                    if (setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos] < w)
                    {
                        x = setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos];
                        break;
                    }
                }
                for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++)
                {
                    if ((setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos] < w) \
                            && (setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos] != x))
                    {
                        y = setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos];
                        break;
                    }
                }

                variants = countVariants(TAB, a, w, b, c, x, y);
                k = 0;
                j = -1;

                for(i = 0; i < 6; i++)
                {
                    if (variants[i] > j)
                    {
                        j = variants[i];
                        k = i;
                    }
                }
                free(variants);

                if (j == -1)
                {
                    fprintf(stderr, "Critical error in main loop, umast:MAST\n");
                    exit(1);
                }

                else
                {
                    switch (k)
                    {
                    case 0:
                        if (TAB[b][x] == NULL && TAB[c][y] == NULL)
                            TAB[posT][posU] = NULL;
                        else if (TAB[b][x] == NULL && TAB[c][y] != NULL)
                            TAB[posT][posU] = TAB[c][y];
                        else if (TAB[b][x] != NULL && TAB[c][y] == NULL)
                            TAB[posT][posU] = TAB[b][x];
                        else //both subtrees exist
                        {
                            TAB[posT][posU] = branchOr(TAB[b][x], TAB[c][y]);
                        }
                        break;
                    case 1:
                        if (TAB[b][y] == NULL && TAB[c][x] == NULL)
                            TAB[posT][posU] = NULL;
                        else if (TAB[b][y] == NULL && TAB[c][x] != NULL)
                            TAB[posT][posU] = branchCopy(TAB[c][y]);
                        else if (TAB[b][y] != NULL && TAB[c][x] == NULL)
                            TAB[posT][posU] = branchCopy(TAB[b][x]);
                        else //both subtrees exist
                        {
                            TAB[posT][posU] = branchOr(TAB[b][y], TAB[c][x]);
                        }
                        break;
                    case 2:
                        TAB[posT][posU] = branchCopy(TAB[a][x]);
                        break;
                    case 3:
                        TAB[posT][posU] = branchCopy(TAB[a][y]);
                        break;
                    case 4:
                        TAB[posT][posU] = branchCopy(TAB[b][w]);
                        break;
                    case 5:
                        TAB[posT][posU] = branchCopy(TAB[c][w]);
                        break;
                    default:
                        fprintf(stderr, "Critical error in main loop, umast:MAST\n");
                        exit(1);
                    }
                }
            }
            posU++;
        }
        posT++;
    }
    
    randMax = getRandMaxBranch(TAB, tree1->nodesNum, tree2->nodesNum);
    intersection = branchCopy(TAB[randMax[0]][randMax[1]]);

    for (i = 0; i < tree1->nodesNum; i++)
    {
        for (j = 0; j < tree2->nodesNum; j++)
        {
            branchDelete(TAB[i][j]);
        }
    }
    for (i = 0; i < tree1->nodesNum; i++)
    {
        free(TAB[i]);
    }
    free(TAB);

    free(permutation); 
    free(leavesPosArr1); 
    free(leavesPosArr2);
    free(randMax);

    for(i = 0; i < branchArr1->size; ++i)
    {
        branchDelete(branchArr1->array[i]);
    }
    for(i = 0; i < branchArr2->size; ++i)
    {
        branchDelete(branchArr2->array[i]);
    }

    branchArrayDelete(branchArr1);
    branchArrayDelete(branchArr2);
    treeDelete(tree1);
    treeDelete(tree2);
    return intersection;
} //MAST

Tree* UMAST(Tree* intree1, Tree* intree2){
    int i, j, maxNum, rootNum;
    int** rootPositions;
    unsigned* sortedSet1;           //topologically sorted nodes from rooted tree1
    unsigned* sortedSet2;           //topologically sorted nodes from rooted tree2
    unsigned* setPermutation1;      //for every node of tree1 tells its pos in topologically sorted set
    unsigned* setPermutation2;      //the same for tree2
    Tree** prunedTrees;
    Tree* tree1;
    Tree* tree2;
    Tree* result;
    Branch** umastSet;

    tree1 = (Tree*)malloc(sizeof(Tree));
    tree2 = (Tree*)malloc(sizeof(Tree));
    prunedTrees = treesPrune(intree1, intree2);
    tree1 = prunedTrees[0];
    tree2 = prunedTrees[1];
    rootNum = tree1->nodesNum - 1;
    rootPositions = getAllRoots(tree1);
    umastSet = (Branch**)calloc(sizeof(Branch*), rootNum);
    setPermutation1 = (unsigned*)calloc(sizeof(unsigned), tree1->nodesNum + 1);
    setPermutation2 = (unsigned*)calloc(sizeof(unsigned), tree2->nodesNum + 1);
    sortedSet2 = treeRootAndTopSort(tree2, 1, 0, setPermutation2);

    for (i = 0; i < rootNum; i++)
    {
        sortedSet1 = treeRootAndTopSort(tree1, rootPositions[i][0], rootPositions[i][1], setPermutation1);
        umastSet[i] = MAST(tree1, tree2, sortedSet1, sortedSet2, setPermutation1, setPermutation2);
        tree1 = treeUnRoot(tree1, 0);
        free(sortedSet1); 
        }

    maxNum = -1;
    for (i = 0; i < rootNum; i++)
    {
        if (branchGetLeavesPosNum(umastSet[i]) > maxNum)
        {
            maxNum = branchGetLeavesPosNum(umastSet[i]);
            j = i;
        }
    }

    result = makeMAST(umastSet[j], tree1);

    for (i = 0; i < rootNum; i++)
    {
        free(rootPositions[i]);
    }

    free(rootPositions); 
    free(sortedSet2); 
    free(setPermutation1); 
    free(setPermutation2);

    for (i = 0; i < 2; i++)
    {
        treeDelete(prunedTrees[i]);
    }
    free(prunedTrees);

    for (i = 0; i < rootNum; i++)
    {
        branchDelete(umastSet[i]);
    }
    free(umastSet);
    treeLCAFinderCalculate(result);
    return result;
} //UMAST

/*
int main(int argc, char** argv){
    Tree* tree1;
    Tree* tree2;
    FILE* logfile;

    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s tree1 tree2\n", argv[0]);
        exit(1);
    }

    logfile = fopen("umast.log", "w");
    time_t t = time(NULL);
    struct tm* aTm = localtime(&t);

    fprintf(logfile, "UMAST execution started\n");
    fprintf(logfile, "%04d/%02d/%02d %02d:%02d:%02d\n", aTm->tm_year+1900, aTm->tm_mon+1,\
            aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
    tree1 = treeRead(argv[1]);
    tree2 = treeRead(argv[2]);
    fprintf(logfile, "Trees read successfully\n");
    fprintf(logfile, "%s\n", treeToString(tree1));
    fprintf(logfile, "%s\n", treeToString(tree2));
    fclose(logfile);
    UMAST(tree1, tree2);
    return 0;
}*/