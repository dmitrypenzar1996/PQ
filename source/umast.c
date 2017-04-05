#include "umast.h"
#include <time.h>

//Tree, growTree, add, consensus, countScore, countScoreHash, HashAlignment, RMQ, Record, TreeWS, RecordList, PWM, CutTree

BranchArray* treeRootedToBranchArray(Tree* tree, int* permutation){
    INT p = 1;
    int i = 0;
    int j = 0;
    unsigned branchNum = tree->nodesNum;
    BranchArray* ba = branchArrayCreate(branchNum);
    BranchArray* result = branchArrayCreate(tree->nodesNum);
    NodeStack* stack = nodeStackCreate(tree->nodesNum);
    Node* curNode = 0;
    Node* nextNode = 0;
    if (tree->leavesNum == 0){
        return 0;
    }
    if (tree == 0){
        perror("null Tree pointer");
        exit(1);
    }

    treeWash(tree);

    for(i = 0; i < branchNum; ++i){
        branchArrayAdd(ba, branchCreate(tree->leavesNum));
    }

    for(i = 0; i < tree->leavesNum; ++i){
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

    for(i = 0; i < ba->size; ++i)
    {
        branchArrayAdd(result, ba->array[i]);
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

    for (i = 0; i < tree1->leavesNum; i++){
    	for (j = 0; j < tree2->leavesNum; j++){
    		if (strcmp(tree1->leaves[i]->name, tree2->leaves[j]->name) == 0){
    			leavesToSave1[count1] = tree1->leaves[i]->name;
    			count1++;
    			break;
    		}
    	}
    }
    for (i = 0; i < tree2->leavesNum; i++){
    	for (j = 0; j < tree1->leavesNum; j++){
    		if (strcmp(tree2->leaves[i]->name, tree1->leaves[j]->name) == 0){
    			leavesToSave2[count2] = tree2->leaves[i]->name;
    			count2++;
    			break;
    		}
    	}
    }

    if (count1 > tree1->leavesNum){
    	perror("wrong count 1, umast:treesPrune");
    }
    if (count2 > tree2->leavesNum){
    	perror("wrong count 2, umast:treesPrune");
    }
    leavesToSave1 = realloc(leavesToSave1, (sizeof(char*) * (count1)));
    leavesToSave2 = realloc(leavesToSave2, (sizeof(char*) * (count2)));
    result[0] = treePrune(tree1, leavesToSave1, (size_t) count1, 0);
    result[1] = treePrune(tree2, leavesToSave2, (size_t) count2, 0);
    /* here is OK */
    return result;
}

Tree* treeRoot(Tree* tree, unsigned nodeID, unsigned neighbourID, char newTree){
	int i;
	Node* root;
	Node* prevNode;
	Tree* result;

	if (tree == 0){
	    fprintf(stderr, "Error, null tree pointer, umast:treeRoot\n");
	    exit(1);
	}
	if (tree->leavesNum == 0){
	    fprintf(stderr, "Error, cant root empty tree, umast:treeRoot\n");
	    exit(1);
	}
	if (nodeID >= tree->nodesNum){
       fprintf(stderr, "Error, node index is out, umast:treeRoot\n");
        exit(1);
    }
	if (neighbourID >= tree->nodes[nodeID]->neiNum){
        fprintf(stderr, "Error, neighbour index is out of range, (%d of %d)\
                umast:treeRoot\n", neighbourID, tree->nodes[nodeID]->neiNum);
       exit(1);
    }
	result = tree;
	if (newTree){
		result = treeCopy(tree, 0);
	}
	root = nodeCreate();
	root->pos = result->nodesNum;

	prevNode = result->nodes[nodeID]->neighbours[neighbourID];
	result->nodes[nodeID]->neighbours[neighbourID] = root;

	for(i = 0; i < prevNode->neiNum; i++) {
	    if (prevNode->neighbours[i] == result->nodes[nodeID]){
	    	prevNode->neighbours[i] = root;
	        break;
	    }
	}
	nodeAddNeighbour(root, result->nodes[nodeID], 0);
	nodeAddNeighbour(root, prevNode, 0);
	result->nodesNum += 1;
	result->nodes = realloc(result->nodes, sizeof(Node*) * (result->nodesNum));
	result->nodes[result->nodesNum - 1] = root;
	result->rootId = root->pos;//rooting of the tree done
	return result;
}

Tree* treeUnRoot(Tree* tree, char newTree){
	int i, j, k;
	Node* curnode;
	Tree* result;
	
	if (tree == 0){
	    fprintf(stderr, "Error, null tree pointer, umast:treeUnRoot\n");
	    exit(1);
	}
	if (tree->leavesNum == 0){
	    fprintf(stderr, "Error, cant root empty tree, umast:treeUnRoot\n");
	    exit(1);
	}
	result = tree;
	if (newTree){
		result = treeCopy(tree, 0);
	}
	if (tree->rootId != (tree->nodesNum - 1)){
		perror("error, root is not the last node");
	}
	curnode = result->nodes[result->rootId];
	for (i = 0; i < curnode->neiNum; i++){
		for (j = 0; j < curnode->neiNum; j++){
			if (i != j){
				for (k = 0; k < curnode->neighbours[i]->neiNum; k++){
					if (curnode->neighbours[i]->neighbours[k] == curnode){
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
}


unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation){
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
	//free(root);
	//free(prevNode);
	treeWash(tree);
	for (i = 0; i < tree->nodesNum; i++){
		for (j = 0; j < tree->nodesNum; j++){
	    	if (i == result[j]){
	    		setPermutation[i] = j;
	    	}
	    }
	 }
	/*here seems to be OK*/
	return result;
}


int* calculateLeavesPermutation(Tree* tree1, Tree* tree2){
    int i;
    char** leaves1;
    char** leaves2;
    int* permutation;
    leaves1 = (char**)malloc(sizeof(char*) * tree1->leavesNum);
    leaves2 = (char**)malloc(sizeof(char*) * tree2->leavesNum);
    for (i = 0; i < tree1->leavesNum; i++){
        leaves1[i] = tree1->leaves[i]->name;
    }
    for (i = 0; i < tree2->leavesNum; i++){
        leaves2[i] = tree2->leaves[i]->name;
    }
    permutation = calculatePermutation(leaves1, leaves2, tree1->leavesNum);
    return permutation;
}

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
}

int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y)
{
    int* variants;
    int* result;
    int i, k;
    variants = (int*)calloc(sizeof(int), 8);
    result = (int*)calloc(sizeof(int), 6);
    for (i = 0; i < 8; i++){
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
    return result;
}


int* getTreeLeavesPos(Tree* tree){
	int* leavesPosArr;
	int i, j;
	leavesPosArr = (int*)calloc(sizeof(int), tree->nodesNum);
	for (i = 0;  i < tree->nodesNum; i++){
		leavesPosArr[i] = -1;
	}
	for (i = 0; i < tree->leavesNum; i++){
		leavesPosArr[tree->leaves[i]->pos] = i;
	}
	/*for (i = 0;  i < tree->nodesNum; i++){
		}*/
	return leavesPosArr;
}

unsigned* branchToLeavesArr(Branch* br, unsigned leavesNum){
    int i, j;
    INT curInt;
    int curPos = 0;
    unsigned* leavesPosArr;
    int curSize;
    leavesPosArr = (unsigned*)calloc(sizeof(unsigned), leavesNum);
    for (i = 0; i < leavesNum; i++){
    	leavesPosArr[i] = 0; //no leaf
    }

    for (i = 0; i < branchGetIntSize(br); ++i){
    	curInt = br->branch[i];
        curSize = leavesNum - (i * intSize);
        curSize = curSize > intSize ? intSize : curSize;
    	for (j = 0; j < curSize; ++j){
    		if ((curInt & 1) == 1){
    			leavesPosArr[curPos] = 1;
    		}
    		++curPos;
    		curInt >>=1;
    	}
    }
    return leavesPosArr;
}

int** getAllRoots(Tree* tree){
	int i, j,count;
	unsigned* sortedSet;
	int** result;
	Node* prevNode;
	NodeStack* treeStack;
	treeStack = nodeStackCreate(tree->nodesNum);
	sortedSet = (unsigned*)calloc(sizeof(unsigned), tree->nodesNum);
	result = (int**)calloc(sizeof(int*), (tree->nodesNum - 1));
	for(i = 0; i < (tree->nodesNum - 1); i++){
	    result[i] = (int*)calloc(sizeof(int), 2);
	    }
	 for (i = 0; i < (tree->nodesNum - 1); i++){
	     for(j = 0; j < 2; j++){
	         result[i][j] = -1;
	     }
	 }
	treeWash(tree);
	for (i = 0; i < tree->nodesNum; i++){
		if (tree->nodes[i]->neiNum > 1){
			break;
		}
	}
	
	stackCycle(tree->nodes[i], treeStack);
	
	while (treeStack->curSize != 0){
		i = tree->nodesNum - treeStack->curSize;
		prevNode = nodeStackPeek(treeStack);
		sortedSet[i] = prevNode->pos;
		nodeStackPop(treeStack);
	}
	//
	treeWash(tree);
	count = 0;
	
	for (i = 0; i < tree->nodesNum; i++){
		prevNode = tree->nodes[sortedSet[i]];
		prevNode->color = BLACK;
		for (j = 0; j < prevNode->neiNum; j++){
			if (prevNode->neighbours[j]->color == WHITE){
				result[count][0] = prevNode->pos;
				result[count][1] = j;
				count ++;
			}
		}
	}
	if (count > (tree->nodesNum - 1)){
		perror("get all roots broken\n");
	}
	nodeStackDelete(treeStack);
	//printf("%s\n", "count all rots done");
	return result;
}

Tree* makeMAST(Branch* br, Tree* tree1){
	int i, count;
	unsigned leavesToSaveAmount = 0;
	unsigned* leavesPosArr;
	char** leavesToSave;
	Tree* result;
	FILE* logfile;
	logfile = fopen("umast.log", "a+");
	

	treeWash(tree1);
	result = treeCreate();
	leavesPosArr = branchToLeavesArr(br, tree1->leavesNum); //now we have list for needed and unneeded leaves
	for (i = 0; i < tree1->leavesNum; i++){
		if (leavesPosArr[i] == 1){
			leavesToSaveAmount++;
		}
	}

	if (leavesToSaveAmount == tree1->leavesNum){
		fprintf(logfile, "%s\n", "pruned trees match completely");
		result = treeCopy(tree1, 0);
		fprintf(logfile, "%s\n", treeToString(result));
		//printf("%d\n", result->leavesNum);
		return result;
	}
	else if (leavesToSaveAmount == 0){
		fprintf(logfile, "%s\n", "trees don't match");
		//printf("%s\n", treeToString(result));
		//printf("%d\n", leavesToSaveAmount);
		return result;
	}
	else {//some leaves are to be deleted but not all
		leavesToSave = (char**)malloc(sizeof(char*) * (leavesToSaveAmount));
		result = treeCopy(tree1, 0);
		count = 0;
		for (i = 0; i < tree1->leavesNum; i++){
			if (leavesPosArr[i] == 1){
				leavesToSave[count] = tree1->leaves[i]->name;
				count++;
			}
		}
		if (count > (leavesToSaveAmount)){
			perror("makeMast broken\n");
		}
		fprintf(logfile, "%d%s\n", tree1->leavesNum - count, " leaves are to be deleted");

		result = treePrune(tree1, leavesToSave, (size_t) leavesToSaveAmount, 0);

		fprintf(logfile, "%s\n", "Maximum agreement subtree found:");
		fprintf(logfile, "%s\n", treeToString(result));
		fprintf(logfile, "%d\n", result->leavesNum);
		fclose(logfile);
		return result;
	}
}

Branch* MAST(Tree* tree1, Tree* tree2, unsigned* set1, unsigned* set2, unsigned* setPermutation1, unsigned* setPermutation2)
{
	INT p;
    int b, c, x, y; b = 0; c = 0; x = 0; y = 0;
    int i, j, k;
    int posT, posU;
    int a, w;
    int* variants;
    int* permutation;			//permutation of leaf sets between tree1 and tree2
    int* leavesPosArr1;			//size = nodes; for every node has -1, for leaf has its pos in leaves
    int* leavesPosArr2;			//the same for tree2
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    Branch* intersection;
    Branch*** TAB;
    Tree* result;


    permutation = calculateLeavesPermutation(tree2, tree1);
    branchArr1 = treeRootedToBranchArray(tree1, getRange(0, tree1->leavesNum));
    branchArr2 = treeRootedToBranchArray(tree2, permutation);
    leavesPosArr1 = getTreeLeavesPos(tree1);
    leavesPosArr2 = getTreeLeavesPos(tree2);
    //printf("%s\n", "MAST loop started");
    TAB = (Branch***)calloc(sizeof(Branch**), tree1->nodesNum);
    for(i = 0; i < tree1->nodesNum; i++){
        TAB[i] = (Branch**)calloc(sizeof(Branch*), tree2->nodesNum);
    }
    for (i = 0; i < tree1->nodesNum; i++){
        for(j = 0; j < tree2->nodesNum; j++){
            TAB[i][j] = NULL;
        }
    }

    //this is where the main loop starts
    posT = 0;
    for (a = 0; a < tree1->nodesNum; a++){ // Until all lines in TAB are filled
        posU = 0;
        for (w = 0; w < tree2->nodesNum; w++){ //Until all cells in line are filled
        	b = -1; c = -1; x = -1; y = -1;
            if (tree1->nodes[set1[a]]->neiNum == 1 || tree2->nodes[set2[w]]->neiNum == 1){ //if a or w is a leaf
                if (tree1->nodes[set1[a]]->neiNum == 1 && tree2->nodes[set2[w]]->neiNum == 1){ // if a AND w are leaves
                    if (strcmp(tree1->nodes[set1[a]]->name, tree2->nodes[set2[w]]->name) == 0){// a and w are equal leaves (&& set1[a] == permutation[set2[w]]) !!!
                    	intersection = branchCreate(tree1->leavesNum);
                    	p = 1;
                    	p = p << (leavesPosArr1[set1[a]] & (intSize - 1));
                    	intersection->branch[leavesPosArr1[set1[a]] / intSize] |= p;
                    	//branchPrint(intersection);
                        TAB[posT][posU] = intersection;
                    }
                    else{ // leaves are not equal
                    	intersection = branchCreate(tree1->leavesNum);
                    	//branchPrint(intersection);
                        TAB[posT][posU] = intersection;
                    }
                }
               else{ //if a or w is a subtree
            	   intersection = branchCreate(tree1->leavesNum);
            	   if (tree1->nodes[set1[a]]->neiNum == 1){ //a is a leaf, w is a subtree
            		   p = 1;
            		   p = p << (leavesPosArr1[set1[a]] & (intSize - 1));
            		   intersection->branch[leavesPosArr1[set1[a]] / intSize] |= p;
            		   intersection = branchAnd(intersection, branchArr2->array[set2[w]]);
            		   //branchPrint(intersection);
            		   TAB[posT][posU] = intersection;
            	   }
            	   else{ //a is a subtree, w is a leaf
            		   p = 1;
            		   p = p << (permutation[leavesPosArr2[set2[w]]] & (intSize - 1));
            		   intersection->branch[permutation[leavesPosArr2[set2[w]]] / intSize] |= p;
            		   intersection = branchAnd(intersection, branchArr1->array[set1[a]]);
            		   //branchPrint(intersection);
            		   TAB[posT][posU] = intersection;
            	   }
                }
            }
            else{//nor a nor w are leaves
            	//printf("subtrees check started\n");
            	for (i = 0; i < tree1->nodes[set1[a]]->neiNum; i++){
            		if (setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos] < a){
            			b = setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos];
            			break;
            		}
            	}
            	for (i = 0; i < tree1->nodes[set1[a]]->neiNum; i++){
            		if ((setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos] < a) \
            				&& (setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos] != b)){
            			c = setPermutation1[tree1->nodes[set1[a]]->neighbours[i]->pos];
            			break;
            		}
            	}
            	for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++){
            		if (setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos] < w){
            			x = setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos];
            			break;
            		}
            	}
            	for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++){
            		if ((setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos] < w) \
            				&& (setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos] != x)){
            			y = setPermutation2[tree2->nodes[set2[w]]->neighbours[j]->pos];
            			break;
            		}
            	}

                variants = countVariants(TAB, a, w, b, c, x, y);
                //printf("countVariants done\n");
                k = 0;
                j = -1;
                for(i = 0; i < 6; i++){
                    if (variants[i] > j){
                        j = variants[i];
                        k = i;
                    }
                }

                if (j == -1) // here was j==0 check if needed - maybe shouldnt be here at all
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
                            TAB[posT][posU] = branchOR(TAB[b][x], TAB[c][y]);
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
                            TAB[posT][posU] = branchOR(TAB[b][y], TAB[c][x]);
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
                    //branchPrint(TAB[posT][posU]);
                }
            }
            posU++;
        }
        posT++;
    }
    //free(variants);
    //printf("%s\n", "Tree extraction started");
    k = 0; b = 0; c = 0;
    for (i = 0; i < tree1->nodesNum; i++){
    	for (j = 0; j < tree2->nodesNum; j++){
    		//branchPrint(TAB[i][j]);
    		if (branchGetLeavesPosNum(TAB[i][j]) > k){
    			k = branchGetLeavesPosNum(TAB[i][j]);
    			b = i;
    			c = j;
    		}
    	}
    }
    //branchPrint(TAB[b][c]);
    return TAB[tree1->nodesNum - 1][tree2->nodesNum - 1];
}



void UMAST(Tree* tree1, Tree* tree2){
	int i, j, maxNum, rootNum;
	int** rootPositions;
    unsigned* sortedSet1;				//topologically sorted nodes from rooted tree1
    unsigned* sortedSet2;				//topologically sorted nodes from rooted tree2
    unsigned* setPermutation1;			//for every node of tree1 tells its pos in topologically sorted set
    unsigned* setPermutation2;			//the same for tree2
	Tree** prunedTrees;
	Tree* result;
	Branch** umastSet;
	
	prunedTrees = treesPrune(tree1, tree2);
	tree1 = prunedTrees[0];
	tree2 = prunedTrees[1];
	
	//printf("%s\n%s\n", treeToString(tree1), treeToString(tree2));
	
	rootNum = tree1->nodesNum - 1;
	rootPositions = getAllRoots(tree1);
	
	umastSet = (Branch**)calloc(sizeof(Branch*), rootNum);
    setPermutation1 = (unsigned*)calloc(sizeof(unsigned), tree1->nodesNum + 1);
    setPermutation2 = (unsigned*)calloc(sizeof(unsigned), tree2->nodesNum + 1);

    
    sortedSet2 = treeRootAndTopSort(tree2, 1, 0, setPermutation2);
    
    for (i = 0; i < rootNum; i++){
    	sortedSet1 = treeRootAndTopSort(tree1, rootPositions[i][0], rootPositions[i][1], setPermutation1);
    	umastSet[i] = MAST(tree1, tree2, sortedSet1, sortedSet2, setPermutation1, setPermutation2);
    	//result = makeMAST(umastSet[i], tree1);
    	tree1 = treeUnRoot(tree1, 0);
    	}
    maxNum = -1;
    for (i = 0; i < rootNum; i++){
    	if (branchGetLeavesPosNum(umastSet[i]) > maxNum){
    		maxNum = branchGetLeavesPosNum(umastSet[i]);
    		j = i;
    	}
    }

    //branchPrint(umastSet[j]);
    result = makeMAST(umastSet[j], tree1);
    printf("%d\n", result->leavesNum);
    
    return;
}

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
    //fprintf(logfile, "UMAST execution finished");
    return 0;
}



