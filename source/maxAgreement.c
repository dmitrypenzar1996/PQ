#include "maxAgreement.h"

//Tree, growTree, add, consensus, countScore, countScoreHash, HashAlignment, RMQ, Record, TreeWS, RecordList, PWM

BranchArray* treeToBranchModified(Tree* tree, int* permutation)
{
    INT p = 1;
    int i = 0;
    int j = 0;
    unsigned branchNum = tree->nodesNum;
    BranchArray* ba = branchArrayCreate(branchNum);
    BranchArray* result = branchArrayCreate(tree->nodesNum);
    NodeStack* stack = nodeStackCreate(tree->nodesNum);
    Node* curNode = 0;
    Node* nextNode = 0;
    if (tree->leavesNum == 0)
    {
        return 0;
    }
    if (tree == 0)
    {
        perror("null Tree pointer");
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

    if (tree->nodes[0]->name == 0)
    {
        curNode = tree->nodes[0];
    }
    else
    {
        curNode = tree->nodes[1];
    }

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
                for(j = 0; j < tree->leavesNum / 64 + 1; ++j)
                {
                    ba->array[curNode->pos]->branch[j] |= \
                            ba->array[curNode->neighbours[i]->pos]->branch[j];
                }
            }
            nodeStackPop(stack);
            curNode->color = BLACK;
        }
    }

    for(i = 0; i < ba->size; ++i)
    {
        branchArrayAdd(result, ba->array[i]);
    }

    for(i = 0; i < result->size; ++i)
    {
        branchNormalize(result->array[i]);
    }

    branchArrayDelete(ba);
    nodeStackDelete(stack);
    return result;
}

Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount)
{
    int i, j;
    Tree* result = treeCreate();
    result->nodes = (Node**)calloc(sizeof(Node*), tree->nodesNum);
    result->leaves = (Node**)calloc(sizeof(Node*), tree->leavesNum);
    result = treeCopy(tree, 0);
    for (i = 0; i < leavesToDeleteAmount; i++) {
        for (j=0; j < result->leavesNum; j++){
            if (strcmp(leavesToDelete[i], result->leaves[j]->name)==0){
                result = treeRemoveLeaf(result, j, 0, 0);
            }    
        }
    }
    return result;
}

void treesPrune(Tree* tree1, Tree* tree2) {
    int count1, count2;
    int i, j;
    int k, l;
    char** leavesToDelete1;
    char** leavesToDelete2;

    count1 = 0;
    count2 = 0;
    for (i = 0; i < tree1->leavesNum; i++) {
        for (j = 0; j < tree2->leavesNum; j++) {
            if (strcmp(tree1->leaves[i]->name, tree2->leaves[j]->name) == 0) {
                break;
            }
        }
        if (j == tree2->leavesNum - 1)
            count1++;
    }
    for (i = 0; i < tree2->leavesNum; i++){
        for (j = 0; j < tree1->leavesNum; j++){
            if (strcmp(tree2->leaves[i]->name, tree1->leaves[j]->name) == 0){
                break;
            }
        }
        if (j == tree1->leavesNum - 1)
            count2++;
    }
    leavesToDelete1 = (char**)malloc(sizeof(char*) * count1);
    leavesToDelete2 = (char**)malloc(sizeof(char*) * count2);
    k = 0;
    l = 0;
    while (k < count1){
        for (i = 0; i < tree1->leavesNum; i++){
            for(j = 0; j < tree2->leavesNum; j++){
            if (strcmp(tree1->leaves[i]->name, tree2->leaves[j]->name) == 0){
                break;
            }
        }
        if (j == tree2->leavesNum - 1)
            leavesToDelete1[k++] = tree1->leaves[i]->name;
        }
    }
    while (l < count2){
        for (i = 0; i < tree2->leavesNum; i++){
            for (j = 0; j < tree1->leavesNum; j++){
                if (strcmp(tree2->leaves[i]->name, tree1->leaves[j]->name) == 0){
                    break;
                }
             }
             if (j == tree1->leavesNum - 1)
                 leavesToDelete2[l++] = tree2->leaves[i]->name;
        }
    }
    tree1 = deleteLeaves(tree1, leavesToDelete1, count1);
    tree2 = deleteLeaves(tree2, leavesToDelete2, count2);
    /* here is OK */
    return;
}


unsigned* treeTopSort(Tree* tree){
/* Something's wrong! */
    int i, j, k, count; 
    unsigned* result;

    result = (unsigned*)calloc(sizeof(unsigned), tree->nodesNum);
    for (i=0; i < tree->leavesNum; i++){
        tree->leaves[i]->color = BLACK;
        result[i] = tree->leaves[i]->pos;
    }
    count = tree->leavesNum;
    while (count < tree->nodesNum){
        for (j = 0; j < tree->nodesNum; j++){
            if (tree->nodes[j]->neiNum > 1){
                for (k = 0; k < tree->nodes[j]->neiNum; k++){
                    if (tree->nodes[j]->neighbours[k]->color == BLACK){
                        tree->nodes[j]->color = GREY;
                    }    
                }
            }
        }

        for (j = 0; j < tree->nodesNum; j++){
            if (tree->nodes[j]->color == GREY){
/*
                if (count >= tree->nodesNum) 
                {
                    perror("Here!");
                    exit(9);
                }
*/
                tree->nodes[j]->color = BLACK;
                result[count] = tree->nodes[j]->pos;
                count++;
            }
        }
    }
    return result;
}

int* calculateLeavesPermutation(Tree* tree1, Tree* tree2){
    int i;
    char** leaves1;
    char** leaves2;
        int* permutation;

    leaves1 = (char**)malloc(sizeof(char*) * tree1->leavesNum);
    leaves2 = (char**)malloc(sizeof(char*) * tree2->leavesNum);

    for (i = 0; i < tree1->leavesNum; i++)
        leaves1[i] = tree1->leaves[i]->name;
    for (i = 0; i < tree2->leavesNum; i++)
        leaves2[i] = tree2->leaves[i]->name;
    permutation = calculatePermutation(leaves1, leaves2, tree1->leavesNum);
    return permutation;
}

Tree* oneLeafTreeCreate(char* name){
    Tree* subtree;
    Node* node;

    subtree = treeCreate();
    //printf("%s\n", treeToString(subtree));
    subtree->nodes = (Node**)calloc(sizeof(Node*), 1);
    //printf("%s\n", treeToString(subtree));
    subtree->leaves = (Node**)calloc(sizeof(Node*), 1);
    //printf("%s\n", treeToString(subtree));
    node = leafCreate(name);
    //printf("%s\n", treeToString(subtree));
    subtree->nodes[0] =  node;
    //printf("%s\n", treeToString(subtree));
    node->pos = 0;
    //printf("%s\n", treeToString(subtree));
    subtree->leaves[0] = subtree->nodes[0];
    //printf("%s\n", treeToString(subtree));
    subtree->nodes[0]->neiNum = 0;
    //printf("%s\n", treeToString(subtree));
    subtree->nodesNum = 1;
    printf("%s\n", treeToString(subtree));
    subtree->leavesNum = 1;
    printf("%s\n", treeToString(subtree));
    return subtree;
}

int* countVariants(Tree*** TAB, int a, int w, int b, int c, int x, int y){
    int* variants;
    int* result;

    variants = (int*)calloc(sizeof(int), 8);
    result = (int*)calloc(sizeof(int), 6);
    printf("wow such variants\n");
    if (TAB[b][x] == NULL)
        variants[0] = 0;
    else
        variants[0] = TAB[b][x]->nodesNum;
    if (TAB[c][y] == NULL)
        variants[1] = 0;
    else
        variants[1] = TAB[c][y]->nodesNum;
    printf("i love it here\n");
    if (TAB[b][y] == NULL)
        variants[2] = 0;
    else
        variants[2] = TAB[b][y]->nodesNum;
    if (TAB[c][x] == NULL)
        variants[3] = 0;
    else
        variants[3] = TAB[c][x]->nodesNum;
    if (TAB[a][x] == NULL)
        variants[4] = 0;
    else
        variants[4] = TAB[a][x]->nodesNum;
    if (TAB[a][y] == NULL)
        variants[5] = 0;
    else 
        variants[5] = TAB[a][y]->nodesNum;
    if (TAB[b][w] == NULL)
        variants[6] = 0;
    else
        variants[6] = TAB[b][w]->nodesNum;
    if (TAB[c][w] == NULL)
        variants[7] = 0;
    else
        variants[7] = TAB[c][w]->nodesNum;
    printf("nice count\n");
    result[0] = variants[0] + variants[1];
    result[1] = variants[2] + variants[3];
    result[2] = variants[4];
    result[3] = variants[5];
    result[4] = variants[6];
    result[5] = variants[7];
    return result;
}

Tree* merge2w3(Tree* tree1, Tree* tree2){
    Tree* result;
    Tree* intermediate;
    unsigned* set2;
    Node* newNode1;
    Node* newNode2;
    Node* prevNei;
    int i, j;

    result = treeCreate();
    intermediate = treeCreate();
    result = treeCopy(tree2, 0);
    intermediate = treeCopy(tree1, 0);
    set2 = treeTopSort(result);
    newNode1 = nodeCreate();
    newNode2 = nodeCreate();
    intermediate->nodes[0]->neighbours[0] = newNode1;
    intermediate->nodes[1]->neighbours[0] = newNode1;
    nodeAddNeighbour(newNode1, intermediate->nodes[0], 0);
    nodeAddNeighbour(newNode1, intermediate->nodes[1], 0);

    j = result->nodesNum - 2;
    while (j >= 0 )
    { //check!!!!!
        for(i = 0; i < result->nodes[set2[result->nodesNum - 1]]->neiNum; ++i)
        {
            if (result->nodes[set2[result->nodesNum - 1]]->neighbours[i] == result->nodes[set2[j]])
            {
                prevNei = result->nodes[set2[result->nodesNum - 1]]->neighbours[i];
                result->nodes[set2[result->nodesNum - 1]]->neighbours[i] = newNode2;
                j = 0;
                break;
            }
        }
        j--;
    }
                
    //prevNei = result->nodes[set2[result->nodesNum - 1]]->neighbours[0]; 
    for(i = 0; i < prevNei->neiNum; ++i)
        {
            if (prevNei->neighbours[i] == result->nodes[set2[result->nodesNum - 1]])
            {
                prevNei->neighbours[i] = newNode2;
                break;
            }
        }
    nodeAddNeighbour(newNode2, result->nodes[set2[result->nodesNum - 1]], 0);
    nodeAddNeighbour(newNode2, prevNei, 0);
    nodeAddNeighbour(newNode1, newNode2, 0);
    nodeAddNeighbour(newNode2, newNode1, 0);
    result->leaves = realloc(result->leaves, sizeof(Node*) * (result->leavesNum + intermediate->leavesNum));
    result->leavesNum = result->leavesNum + intermediate->leavesNum;
    for (i=0; i<intermediate->leavesNum; i++){
        result->leaves[result->leavesNum + i - 1] = intermediate->leaves[i];
    }

    result->nodesNum += intermediate->nodesNum;
    result->nodes = realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
    for (i=0; i<intermediate->nodesNum; i++){
        result->nodes[result->nodesNum - intermediate->nodesNum + i - 1] = intermediate->nodes[i];
        result->nodes[result->nodesNum - intermediate->nodesNum + i - 1]->pos = result->nodesNum - intermediate->nodesNum + i - 1;
    }
    result->nodesNum += intermediate->nodesNum + 2;
    result->nodes = realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
    result->nodes[result->nodesNum - 2] = newNode1;
    newNode1->pos = result->nodesNum - 2;
    result->nodes[result->nodesNum - 1] = newNode2;
    newNode2->pos = result->nodesNum - 1;
    return result;    
}

Tree* merge3w3(Tree* tree1, Tree* tree2){
    Tree* result;
    Tree* intermediate;
    unsigned* set1;
    unsigned* set2;
    Node* newNode1;
    Node* newNode2;
    Node* prevNei1;
    Node* prevNei2;
    int i, j;

    result = treeCreate();
    intermediate = treeCreate();
    result = treeCopy(tree2,0);
    intermediate = treeCopy(tree1, 0);
    set1 = treeTopSort(intermediate);
    set2 = treeTopSort(result);
    newNode1 = nodeCreate();
    newNode2 = nodeCreate();
    //////////new node 2 insert in result(tree2)
    j = result->nodesNum - 2;
    while (j >= 0)
    {//check!!!!!
        for(i = 0; i < result->nodes[set2[result->nodesNum - 1]]->neiNum; ++i)
        {
            if (result->nodes[set2[result->nodesNum - 1]]->neighbours[i] == result->nodes[set2[j]])
            {
                prevNei2 = result->nodes[set2[result->nodesNum - 1]]->neighbours[i];
                result->nodes[set2[result->nodesNum - 1]]->neighbours[i] = newNode2;
                j = 0;
                break;
            }
        }
        j--;
    }             
    for(i = 0; i < prevNei2->neiNum; ++i)
    {
        if (prevNei2->neighbours[i] == result->nodes[set2[result->nodesNum - 1]])
        {
            prevNei2->neighbours[i] = newNode2;
            break;
        }
    }
    nodeAddNeighbour(newNode2, result->nodes[set2[result->nodesNum - 1]], 0);
    nodeAddNeighbour(newNode2, prevNei2, 0);
    
    ///////////////////////new node 1 insert in intermediate(tree1)

    j = intermediate->nodesNum - 2;
    while (j >= 0)
    {//check!!!!!
        for(i = 0; i < intermediate->nodes[set1[intermediate->nodesNum - 1]]->neiNum; ++i)
        {
            if (intermediate->nodes[set1[intermediate->nodesNum - 1]]->neighbours[i] == intermediate->nodes[set1[j]])
            {
                prevNei1 = intermediate->nodes[set1[intermediate->nodesNum - 1]]->neighbours[i];
                result->nodes[set1[intermediate->nodesNum - 1]]->neighbours[i] = newNode1;
                j = 0;
                break;
             }
        }
        j--;
    }            
    for(i = 0; i < prevNei1->neiNum; ++i)
    {
        if (prevNei1->neighbours[i] == intermediate->nodes[set1[intermediate->nodesNum - 1]])
        {
            prevNei1->neighbours[i] = newNode1;
            break;
        }
    }
    nodeAddNeighbour(newNode1, intermediate->nodes[set1[intermediate->nodesNum - 1]], 0);
    nodeAddNeighbour(newNode1, prevNei1, 0);
    nodeAddNeighbour(newNode1, newNode2, 0);
    nodeAddNeighbour(newNode2, newNode1, 0);
    result->leaves = realloc(result->leaves, sizeof(Node*) * (result->leavesNum + intermediate->leavesNum));
    result->leavesNum = result->leavesNum + intermediate->leavesNum;
    for (i=0; i<intermediate->leavesNum; i++){
        result->leaves[result->leavesNum + i - 1] = intermediate->leaves[i];
    }

    result->nodesNum += intermediate->nodesNum;
    result->nodes = realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
    for (i=0; i<intermediate->nodesNum; i++){
        result->nodes[result->nodesNum - intermediate->nodesNum + i - 1] = intermediate->nodes[i];
        result->nodes[result->nodesNum - intermediate->nodesNum + i - 1]->pos = result->nodesNum - intermediate->nodesNum + i - 1;
    }
    result->nodesNum += intermediate->nodesNum + 2;
    result->nodes = realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
    result->nodes[result->nodesNum - 2] = newNode1;
    newNode1->pos = result->nodesNum - 2;
    result->nodes[result->nodesNum - 1] = newNode2;
    newNode2->pos = result->nodesNum - 1;
    return result;    
}



Tree* mergeTrees(Tree* tree1, Tree* tree2){
    Tree* result;
    unsigned* set1;
    unsigned* set2;

    if (tree1->nodesNum == 1){//tree1 is a leaf
        if (tree2->nodesNum == 1){//pvp
            result = treeCopy(tree1, 0);
            result = treeAddLeaf(result, 0, 0, tree2->nodes[0]->name, 0, 0);
            return result;
        }
        else if (tree2->nodesNum == 2){//2v1
            result = treeCopy(tree2, 0);
            result = treeAddLeaf(result, 0, 0, tree1->nodes[0]->name, 0, 0);
            return result;
        }
        else{//1vall
            result = treeCopy(tree2, 0);
            set2 = treeTopSort(result);
            result = treeAddLeaf(result, set2[result->nodesNum - 1], 0, tree1->nodes[0]->name, 0, 0);
            return result;
        }
    }
    else if (tree1->nodesNum == 2){//tree1 is a 2-leaf tree
        if (tree2->nodesNum == 1)
        {//1 vs 2
            result = growThreeLeavesTree(tree1->nodes[0]->name, tree1->nodes[1]->name, tree2->nodes[0]->name);
            return result;
        }
        else if (tree2->nodesNum == 2){// 2 vs 2
            result = treeCreate(); 
            result = growThreeLeavesTree(tree1->nodes[0]->name, tree1->nodes[1]->name, tree2->nodes[0]->name);
            result = treeAddLeaf(result, 0, 0, tree2->nodes[1]->name, 0, 0);
            return result;
        }
        else{//2 vs many
            result = merge2w3(tree1, tree2);
            return result;
        }
    }
    else{//tree1 contains 3 leaves or more
        if (tree2->nodesNum == 1){//3v1
            result = treeCreate();
            result = treeCopy(tree1, 0);
            set1 = treeTopSort(result);
            result = treeAddLeaf(result, set1[result->nodesNum - 1], 0, tree2->nodes[0]->name, 0, 0);
            return result;
        }
        else if (tree2->nodesNum == 2){//3v2
            result = merge2w3(tree2, tree1);
            return result;
        }
        else{//really hard
            result = merge3w3(tree1, tree2);
            return result;
        }
    }
}


//2 trees - prune - calculate permutation for leaves - treeToBranch - branchAnd for intersections of leaves
void MAST(Tree* tree1, Tree* tree2)
{
    int* permutation;
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    int b, c, x, y;        
    int i, j, k;
    Tree*** TAB;
    int posT, posU;
    int a, w;
    unsigned* set1;
    unsigned* set2;
    int* variants;
    Branch* intersection;
    Tree* result;

    treesPrune(tree1, tree2);
    permutation = calculateLeavesPermutation(tree1, tree2);
    branchArr1 = treeToBranchModified(tree1, getRange(0, tree1->leavesNum));
    branchArr2 = treeToBranchModified(tree2, permutation);
    TAB = (Tree***)calloc(sizeof(Tree**), tree1->nodesNum);
    for(i = 0; i < tree1->nodesNum; i++){
        TAB[i] = (Tree**)calloc(sizeof(Tree*), tree2->nodesNum);
    }
    for (i = 0; i < tree1->nodesNum; i++){
        for(j = 0; j < tree2->nodesNum; j++){
            TAB[i][j] = NULL;
        }
    }

    set1 = treeTopSort(tree1); //topologically sorted nodes from tree1
    set2 = treeTopSort(tree2); //topologically sorted nodes from tree2


    posT = 0;
    for (a = 0; a < tree1->nodesNum; a++){ // untill all lines in TAB are filled 
        posU = 0;
        for (w = 0; w < tree2->nodesNum; w++){ //untill all cells in line are filled
            if (tree1->nodes[set1[a]]->neiNum == 1 || tree2->nodes[set2[w]]->neiNum == 1){ //if a or w is a leaf
                if (tree1->nodes[set1[a]]->neiNum == 1 && tree2->nodes[set2[w]]->neiNum == 1){ // if a AND w are leaves
                    if (strcmp(tree1->nodes[set1[a]]->name, tree2->nodes[set2[w]]->name) == 0){ // a and w are equal leaves
                        result = oneLeafTreeCreate(tree1->nodes[set1[a]]->name);
                        TAB[posT][posU] = result;
                    }
                    else{ // leaves are not equal
                        TAB[posT][posU] = NULL; 
                    }     
                }
                else{ //if a or w is a subtree
                    intersection = branchAnd(branchArr1->array[a], branchArr2->array[w]);
                    if (tree1->nodes[set1[a]]->neiNum == 1){ //a is a leaf, w is a subtree
                        if (intersection->branch[a] == 1) //w contains a
                            TAB[posT][posU] = oneLeafTreeCreate(tree1->nodes[set1[a]]->name);
                        else //w does not contain a
                            TAB[posT][posU] = NULL;
                    }
                    else{//a is a subtree, w is a leaf
                        if (intersection->branch[w] == 1) //a contains w
                            TAB[posT][posU] = oneLeafTreeCreate(tree2->nodes[set2[w]]->name);
                        else //a does not contain w
                            TAB[posT][posU] = NULL;

                    }
                }
            }
            else{//nor a nor w are leaves
                for (i = 0; i < a; i++){
                    for (j = 0; j < tree1->nodes[set1[a]]->neiNum; j++){
                        if (tree1->nodes[set1[a]]->neighbours[j]->pos == tree1->nodes[set1[i]]->pos)
                            b = i;
                    }
                }
                for (i = b+1; i < a; i++){
                    for (j = 0; j < tree1->nodes[set1[a]]->neiNum; j++){
                        if (tree1->nodes[set1[a]]->neighbours[j]->pos == tree1->nodes[set1[i]]->pos)
                            c = i;
                    }
                }
                for ( i = 0; i < w; i++){
                    for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++){
                        if (tree2->nodes[set2[w]]->neighbours[j]->pos == tree2->nodes[set2[i]]->pos)
                            x = i;
                    }
                }
                for (i = x+1; i < w; i++){
                    for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++){
                        if (tree2->nodes[set2[w]]->neighbours[j]->pos == tree2->nodes[set2[i]]->pos)
                            y = i;
                    }
                }//get b, c, x, y
                printf("what a nice place upon variants\n");
                variants = countVariants(TAB, a, w, b, c, x, y);
                printf("what a nice place\n");
                k = 0;
                j = 0;
                for(i = 0; i < 6; i++){
                    if (variants[i] > j){
                        j = variants[i];
                        k = i;
                    }
                }
                if (j == 0) // all subtrees are NULL
                    TAB[posT][posU] = NULL;
                else{//a real case
                    switch (k){
                    case 0:
                        if (TAB[b][x] == NULL && TAB[c][y] == NULL)
                            TAB[posT][posU] = NULL;
                        else if (TAB[b][x] == NULL && TAB[c][y] != NULL)
                            TAB[posT][posU] = TAB[c][y];
                        else if (TAB[b][x] != NULL && TAB[c][y] == NULL)
                            TAB[posT][posU] = TAB[b][x];
                        else{//both subtrees exist
                            TAB[posT][posU] = mergeTrees(TAB[b][x], TAB[c][y]);
                        }
                        break;
                    case 1:
                        if (TAB[b][y] == NULL && TAB[c][x] == NULL)
                            TAB[posT][posU] = NULL;
                        else if (TAB[b][y] == NULL && TAB[c][x] != NULL)
                            TAB[posT][posU] = TAB[c][y];
                        else if (TAB[b][y] != NULL && TAB[c][x] == NULL)
                            TAB[posT][posU] = TAB[b][x];
                        else{//both subtrees exist
                            TAB[posT][posU] = mergeTrees(TAB[b][y], TAB[c][x]);
                        }
                        break;
                    case 2:
                        TAB[posT][posU] = TAB[a][x];
                        break;
                    case 3:
                        TAB[posT][posU] = TAB[a][y];
                        break;
                    case 4:
                        TAB[posT][posU] = TAB[b][w];
                        break;
                    case 5:
                        TAB[posT][posU] = TAB[c][w];
                        break;
                    default:
                        perror("Something has gone wrong...\n");
                    }
                }
            }
            if (TAB[posT][posU] != NULL){
                printf("%d\n", TAB[posT][posU]->nodesNum);
                //printf("%s\n", treeToString(TAB[a][w]));
                }
            else printf("%d\n", 0);
            posU++;
        }
        posT++;
    }
    k = 0; a = 0; w = 0;
    for (i = 0; i < tree1->nodesNum; i++){
        for (j = 0; j < tree2->nodesNum; j++){
            if (TAB[i][j]->nodesNum > k){
                k = TAB[i][j]->nodesNum;
                a = i;
                w = j;
            }
        }
    }
    printf("%s\n", treeToString(TAB[a][w]));
}

int main(int argc, char** argv){
    Tree* tree1;
    Tree* tree2;

    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s tree1 tree2\n", argv[0]);
        exit(1);
    }
    puts("HIIIII 1");
    tree1 = treeRead(argv[1]);
    tree2 = treeRead(argv[2]);
    printf("%d\n", tree1->nodesNum);
    printf("%d\n", tree2->nodesNum);
    puts("HIIIII 2");
    MAST(tree1, tree2);
    puts("BYE");
    return 0;
}



