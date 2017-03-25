#include "maxAgreement.h"

//Tree, growTree, add, consensus, countScore, countScoreHash, HashAlignment, RMQ, Record, TreeWS, RecordList, PWM, CutTree

BranchArray* treeRootedToBranchArray(Tree* tree, int* permutation)
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
                for(j = 0; j < tree->leavesNum / 64 + 1; ++j)
                {
                    ba->array[curNode->pos]->branch[j] |= \
                            ba->array[curNode->neighbours[i]->pos]->branch[j];
                    //branchPrint(ba->array[curNode->pos]);
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
        //branchNormalize(result->array[i]);
        branchPrint(result->array[i]);
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

void treesPrune(Tree* tree1, Tree* tree2)
{
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

Tree* treeRoot(Tree* tree, unsigned nodeID, unsigned neighbourID, char newTree){
	int i;
	Node* root;
	Node* prevNode;
	Tree* result;

	if (tree == 0)
	{
	    fprintf(stderr, "Error, null tree pointer, Tree:treeRoot\n");
	    exit(1);
	}
	if (tree->leavesNum == 0)
	{
	    fprintf(stderr, "Error, cant add to empty tree, Tree:treeRoot\n");
	    exit(1);
	}
	if (nodeID >= tree->nodesNum)
	{
       fprintf(stderr, "Error, node index is out, Tree:treeRoot\n");
        exit(1);
    }
	if (neighbourID >= tree->nodes[nodeID]->neiNum)
    {
        fprintf(stderr, "Error, neighbour index is out of range, (%d of %d)\
                Tree:treeRoot\n", neighbourID, tree->nodes[nodeID]->neiNum);
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
	//printf("%zd\t%s\n", tree->rootId, "rootId");
	//free(prevNode);
	return result;
}

void topSortCycle(Node* node, unsigned* result, unsigned maxNum){
	int j, count;
	//printf("%d\n", node->pos);
	count = maxNum - 1;
	printf("%d\t%s\n", node->pos, "node pos in cycle");
	if (node->neiNum > 1){
		node->color = BLACK;
		result[count] = node->pos;
		for (j = 0; j < node->neiNum; j++){
			if (node->neighbours[j]->color != BLACK){
				topSortCycle(node->neighbours[j], result, count);
			}
		}
	}
	else{
		if (node->color != BLACK){
			node->color = BLACK;
			printf("%d\t%s\n", node->pos, "leaf pos in cycle 3");
			result[count] = node->pos;
		}
	}
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
	//topSortCycle(root, result, tree->nodesNum);
	treeWash(tree);
	for (i = 0; i < tree->nodesNum; i++){
		for (j = 0; j < tree->nodesNum; j++){
	    	if (i == result[j]){
	    		setPermutation[i] = j;
	    	}
	    }
	 }
	//printf("%d\t%s\n", tree->leaves[0]->pos, "first leaf pos aftre sort");
	//printf("%d\t%s\n", root->pos, "root pos after sort");
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
            if (k != 32)
            {
                curSize++;
            }
            j += k + 1;
        }
    }/*
	int curSize = 0;
	int i;
	INT a;
	a = br->branch[0];
	while (a > 0){
		curSize += a%2;
		a = a/2;
	}*/
    branchPrint(br);
    printf("%d%s\n", curSize, " cursize");
    return curSize;
}

int* countVariants(Branch*** TAB, int a, int w, int b, int c, int x, int y)
{
    int* variants;
    int* result;
    int i, k;

    //printf("countVariants started\n");
    variants = (int*)calloc(sizeof(int), 8);
    result = (int*)calloc(sizeof(int), 6);
    for (i = 0; i < 8; i++)
    	variants[i] = 0;
    if (TAB[b][x] != NULL){
    	variants[0] = branchGetLeavesPosNum(TAB[b][x]);
    }
    if (TAB[c][y] != NULL){
    	variants[1] = branchGetLeavesPosNum(TAB[c][y]);
        }
    if (TAB[b][y] != NULL){
        	variants[2] = branchGetLeavesPosNum(TAB[b][y]);
        }
    if (TAB[c][x] != NULL){
        	variants[3] = branchGetLeavesPosNum(TAB[c][x]);
        }
    if (TAB[a][x] != NULL){
        	variants[4] = branchGetLeavesPosNum(TAB[a][x]);
        }
    if (TAB[a][y] != NULL){
        	variants[5] = branchGetLeavesPosNum(TAB[a][y]);
    }
    if (TAB[b][w] != NULL){
        	variants[6] = branchGetLeavesPosNum(TAB[b][w]);
        }
    if (TAB[c][w] != NULL){
        	variants[7] = branchGetLeavesPosNum(TAB[c][w]);
        }
    result[0] = variants[0] + variants[1];
    result[1] = variants[2] + variants[3];
    result[2] = variants[4];
    result[3] = variants[5];
    result[4] = variants[6];
    result[5] = variants[7];
    return result;
}

Branch* branchOR(Branch* br1, Branch* br2)
{
    int i = 0;
    Branch* orBranch = NULL;
    if (br1->size != br2->size)
    {
        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__,
                __LINE__);
    }
    orBranch = branchCreate(br1->size);
    for(i = 0; i < branchGetIntSize(br1); ++i)
    {
        orBranch->branch[i] = br1->branch[i] | br2->branch[i];
    }
    return orBranch;
}



unsigned* getTreeLeavesPos(Tree* tree){
	unsigned* leavesPosArr;
	int i, j;
	leavesPosArr = (unsigned*)calloc(sizeof(unsigned), tree->nodesNum);
	for (i = 0;  i < tree->nodesNum; i++){
		leavesPosArr[i] = 0;
	}
	for (i = 0; i < tree->leavesNum; i++){
		printf("%d%s\n", i, " i");
		printf("%d%s\n", tree->leaves[i]->pos, " leaf pos in nodes");
		printf("%d%s\n", leavesPosArr[tree->leaves[i]->pos], " leavesPosArr1[i] - 1");
		leavesPosArr[tree->leaves[i]->pos] = i;
		printf("%d%s\n", leavesPosArr[tree->leaves[i]->pos], " leavesPosArr1[i] - 2");
	}
	for (i = 0;  i < tree->nodesNum; i++){
		printf("%d%s\n", leavesPosArr[i], " leavesPosArr1[i] - 3");
		}
	return leavesPosArr;
}

unsigned* branchToLeavesArr(Branch* br, unsigned leavesNum){
	int i, j;
	int curInt;
    int curPos = 0;
    int* leavesPosArr;
    leavesPosArr = (unsigned*)calloc(sizeof(unsigned), leavesNum);
    for (i = 0; i < leavesNum; i++){
    	leavesPosArr[i] = 0; //no leaf
    }
    for (i = 0; i < branchGetIntSize(br); ++i){
    	curInt = br->branch[i];
    	for (j = 0; j < intSize; ++j){
    		if (curInt & 1 == 1){
    			printf("%s%d\n", "Leaf at pos ",  curPos);
    			leavesPosArr[curPos] = 1;
    		}
    		++curPos;
    		curInt >>=1;
    	}
    }
    return leavesPosArr;
}

Tree* makeMAST(Branch*** TAB, Tree* tree1, unsigned nodesNum2, unsigned* set1)
{
	Tree* result;
	Branch* br;
	//size_t* leavesPos;
	unsigned* leavesPosArr;
	//size_t mastLeavesNum = 0;
	//size_t buffer = 0;
	char** leavesToDelete;
	int i, count;
	unsigned leavesToDeleteAmount = 0;
	treeWash(tree1);
	br = TAB[tree1->nodesNum - 1][nodesNum2 - 1];
	printf("%s\n", "see here");
	branchPrint(br);
	result = treeCreate();
	leavesPosArr = branchToLeavesArr(br, tree1->leavesNum); //now we have list for needed and unneeded leaves
	for (i = 0; i < tree1->leavesNum; i++){
		if (leavesPosArr[i] == 0){
			leavesToDeleteAmount++;
		}
	}
	printf("%d\n", leavesToDeleteAmount);
	if (leavesToDeleteAmount == 0){
		printf("%s\n", "mast is full tree");
		result = treeCopy(tree1, 0);
		printf("%s\n", treeToString(result));
		return result;
	}
	else if (leavesToDeleteAmount == tree1->leavesNum){
		printf("%s\n", "trees dont match");
		printf("%s\n", treeToString(result));
		return result;
	}
	else {//some leaves are to be deleted
		leavesToDelete = (char**)malloc(sizeof(char*) * (leavesToDeleteAmount));
		result = treeCopy(tree1, 0);
		count = 0;
		for (i = 0; i < tree1->leavesNum; i++){
			if (leavesPosArr[i] == 0){
				leavesToDelete[count] = tree1->leaves[i]->name;
				count++;
			}
		}
		if (count > (leavesToDeleteAmount)){
			perror("makeMast broken\n");
		}
		result = deleteLeaves(result, leavesToDelete, count);
		printf("%s\n", treeToString(result));
		return result;
	}
	//mastLeavesNum = branchGetLeavesPosNum(br);
	//printf("%d%s\n", intSize, " intsize");

	/*//printf("%zu\t%s\n", mastLeavesNum, "mastleavesNum1");
	//buffer = mastLeavesNum;
	//printf("%zu%s\n", buffer, " buffer before");
	//leavesPos = branchGetLeavesPos(br, &buffer, (size_t) tree1->leavesNum); //so here smth strange happens
	//printf("%zu%s\n", buffer, " buffer after");
	//for (i = 0; i < buffer; i++){
	//	printf("%zu\n", leavesPos[i]);
	//}

	//printf("%zu\t%s\n", mastLeavesNum, "mastleavesNum2");

	leavesToDeleteAmount = tree1->leavesNum - ((unsigned) mastLeavesNum);

	printf("%d\t%s\n", leavesToDeleteAmount, "leavestodeleteamount2");

	leavesToDelete = (char**)malloc(sizeof(char*) * (leavesToDeleteAmount));
	printf("%s\n", "leavesToDeleteAmount successfully count");
	//printf("%d\n", (int) mastLeavesNum);
	for (i = 0; i < (int) mastLeavesNum; i++)
	{
		printf("%zu\n", leavesPos[i]);
		tree1->leaves[leavesPos[i]]->color = BLACK;
	}
	for (i = 0; i < tree1->leavesNum; i++)
	{
		if (tree1->leaves[i]->color != BLACK)
		{
			leavesToDelete[count] = tree1->leaves[i]->name;
			count++;
		}
		if (count > (leavesToDeleteAmount)){
			perror("makeMast broken\n");
		}
	}
	printf("%d%s\n", leavesToDeleteAmount, " HEEEEEEEEEY");
	treeWash(tree1);
	result = treeCreate();
	printf("%s\n", "here we go");
	//result = treeCopy(tree1, 0);
	result = tree1;
	printf("%s\n", "here we go");
	if (leavesToDeleteAmount > 0){
		result = deleteLeaves(result, leavesToDelete, count);
	}
	printf("makeMast in progress\n");
	printf("%s\n", treeToString(tree1));
	printf("%s\n", treeToString(result));
	return result;*/
}

void MAST(Tree* tree1, Tree* tree2)
{
	INT p;
    int b, c, x, y; b = 0; c = 0; x = 0; y = 0;
    int i, j, k;
    int posT, posU;
    int a, w;
    int* variants;
    int* permutation;
    unsigned* leavesPosArr1;
    unsigned* leavesPosArr2;
    unsigned* setPermutation1;//for every node of tree1 tells its pos in topologically sorted set
    unsigned* setPermutation2;//the same for tree2
    unsigned* set1;
    unsigned* set2;
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    Branch* intersection;
    Branch*** TAB;
    Tree* result;

    treesPrune(tree1, tree2);
    printf("%d\t%d\n", tree1->leavesNum, tree2->leavesNum);
    printf("%d\t%d\n", tree1->nodesNum, tree2->nodesNum);
    setPermutation1 = (unsigned*)calloc(sizeof(unsigned), tree1->nodesNum + 1);
    setPermutation2 = (unsigned*)calloc(sizeof(unsigned), tree2->nodesNum + 1);
    set1 = treeRootAndTopSort(tree1, 1, 0, setPermutation1); //topologically sorted nodes from rooted tree1
    set2 = treeRootAndTopSort(tree2, 1, 0, setPermutation2); //topologically sorted nodes from rooted tree2
    /*^here trees became rooted^*/
    printf("%d\t%d\n", tree1->leavesNum, tree2->leavesNum);
    printf("%d\t%d\n", tree1->nodesNum, tree2->nodesNum);
    permutation = calculateLeavesPermutation(tree1, tree2);
    branchArr1 = treeRootedToBranchArray(tree1, getRange(0, tree1->leavesNum));
    branchArr2 = treeRootedToBranchArray(tree2, permutation);
    leavesPosArr1 = getTreeLeavesPos(tree1);
    leavesPosArr2 = getTreeLeavesPos(tree2);
    printf("%s\n", "deBug goes well");
    TAB = (Branch***)calloc(sizeof(Branch**), tree1->nodesNum);
    for(i = 0; i < tree1->nodesNum; i++){
        TAB[i] = (Branch**)calloc(sizeof(Branch*), tree2->nodesNum);
    }
    for (i = 0; i < tree1->nodesNum; i++){
        for(j = 0; j < tree2->nodesNum; j++){
            TAB[i][j] = NULL;
        }
    }
    for (i = 0; i < branchArr1->size; i++){
    	branchPrint(branchArr1->array[i]);
    }

    for (i = 0; i < tree1->nodesNum; i++){
    	printf("%d\n", set1[i]);
    }
    //this is where algorithm starts
    posT = 0;
    for (a = 0; a < tree1->nodesNum; a++){ // Until all lines in TAB are filled
        posU = 0;
        for (w = 0; w < tree2->nodesNum; w++){ //Until all cells in line are filled
        	b = 0; c = 0; x = 0; y = 0;
            if (tree1->nodes[set1[a]]->neiNum == 1 || tree2->nodes[set2[w]]->neiNum == 1){ //if a or w is a leaf
                if (tree1->nodes[set1[a]]->neiNum == 1 && tree2->nodes[set2[w]]->neiNum == 1){ // if a AND w are leaves
                    if (strcmp(tree1->nodes[set1[a]]->name, tree2->nodes[set2[w]]->name) == 0){// a and w are equal leaves (&& set1[a] == permutation[set2[w]]) !!!
                    	intersection = branchCreate(tree1->leavesNum);
                    	p = 1;
                    	p = p << (leavesPosArr1[set1[a]] & (intSize - 1));
                    	intersection->branch[set1[a] / intSize] |= p;
                    	branchPrint(intersection);
                        TAB[posT][posU] = intersection;
                    }
                    else{ // leaves are not equal
                    	intersection = branchCreate(tree1->leavesNum);
                    	branchPrint(intersection);
                        TAB[posT][posU] = intersection;
                    }
                }
               else{ //if a or w is a subtree
            	   intersection = branchCreate(tree1->leavesNum);
            	   if (tree1->nodes[set1[a]]->neiNum == 1){ //a is a leaf, w is a subtree
            		   p = 1;
            		   p = p << (leavesPosArr1[set1[a]] & (intSize - 1));
            		   intersection->branch[set1[a] / intSize] |= p;
            		   intersection = branchAnd(intersection, branchArr2->array[set2[w]]);
            		   branchPrint(intersection);
            		   TAB[posT][posU] = intersection;
            	   }
            	   else{ //a is a subtree, w is a leaf
            		   p = 1;
            		   p = p << (leavesPosArr2[set2[w]] & (intSize - 1));
            		   intersection->branch[set2[w] / intSize] |= p;
            		   intersection = branchAnd(intersection, branchArr1->array[set1[a]]);
            		   branchPrint(intersection);
            		   TAB[posT][posU] = intersection;
            	   }
                }
            }
            else{//nor a nor w are leaves
            	printf("subtrees check started\n");
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
            	//get b, c, x, y
            	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", set1[a], set1[b], set1[c], set2[w], set2[x], set2[y]);


                variants = countVariants(TAB, a, w, b, c, x, y);
                //printf("countVariants done\n");
                k = 0;
                j = 0;
                for(i = 0; i < 6; i++){
                	//printf("%d\t%s\n", variants[i], "variant");
                    if (variants[i] > j){
                        j = variants[i];
                        k = i;
                    }
                }
                //printf("%d\t%s\n", k, "k");
                if (j == -1) // check if needed!!!
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
                    branchPrint(TAB[posT][posU]);
                }
            }
            posU++;
        }
        posT++;
    }
    printf("%s\n", "Tree extraction started");
    result = makeMAST(TAB,tree1, tree2->nodesNum, set1);
    //printf("%s\n", treeToString(result));
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
    //printf("%d\n", tree1->nodesNum);
    //printf("%d\n", tree2->nodesNum);
    puts("HIIIII 2");
    MAST(tree1, tree2);
    puts("BYE");
    return 0;
}













/*

void topSortCycle(Node* node, unsigned* result, unsigned maxNum){
	int j, count;
	printf("%d\n", node->pos);
	count = maxNum - 1;
	if (node->neiNum > 1){
		node->color = BLACK;
		result[count] = node->pos;
		for (j = 0; j < node->neiNum; j++){
			if (node->neighbours[j]->color != BLACK){
				topSortCycle(node->neighbours[j], result, count);
			}
		}
	}
	else{
		if (node->color != BLACK){
			node->color = BLACK;
			result[count] = node->pos;
		}
	}
}

/*unsigned* treeTopSort(Tree* tree, unsigned* setPermutation){
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
                tree->nodes[j]->color = BLACK;
                result[count] = tree->nodes[j]->pos;
                count++;
            }
        }
    }
    treeWash(tree);
    for (i = 0; i < tree->nodesNum; i++){
    	for (j = 0; j < tree->nodesNum; j++){
    		if (i == result[j])
    			setPermutation[i] = j;
    	}
    }
    return result;
}

Tree* makeMAST(Branch*** TAB, Tree* tree1, unsigned nodesNum2, unsigned* set1){
	int* coords;
	Branch* maxBranch;
    char** leavesToDelete;
    Tree* result;
	int i, j, k, count, maxNum;
	coords = (int*)calloc(sizeof(int), 2);
	maxNum = 0;
	for (i = 0; i < tree1->nodesNum; i++){
		for (j = 0; j < nodesNum2; j++){
			count = 0;
			for (k = 0; k < TAB[i][j]->size; k++){
				if (TAB[i][j]->branch[k] == 1)
					count++;
			}
			if (count > maxNum){
				maxNum = count;
				coords[0] = i;
				coords[1] = j;
			}
		}
	}
	maxBranch = TAB[coords[0]][coords[1]];
	count = tree1->leavesNum - maxNum; //count changes!
	leavesToDelete = (char**)malloc(sizeof(char*) * count);
	j = 0;
	for (i = 0; i < tree1->leavesNum; i++){
		if (maxBranch->branch[i] == 0){
			leavesToDelete[j] = tree1->nodes[set1[i]]->name;
			j++;
		}
	}
	if (j != count - 1) perror("something is wrong!!!!!!");
	result = treeCreate();
	result = treeCopy(tree1, 0);
	result = deleteLeaves(tree1, leavesToDelete, count);
	return result;
}

//2 trees - prune - calculate permutation for leaves - treeToBranch - branchAnd for intersections of leaves
void MAST(Tree* tree1, Tree* tree2)
{
    int* permutation;
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    int b, c, x, y;
    b = 0; c = 0; x = 0; y = 0;
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
            	printf("subtrees check started\n");
                for (i = 0; i < a; i++){
                    for (j = 0; j < tree1->nodes[set1[a]]->neiNum; j++){
                        if (tree1->nodes[set1[a]]->neighbours[j]->pos == tree1->nodes[set1[i]]->pos)
                            b = i;
                    }
                }
                for (i = 0; i < a; i++){
                    for (j = 0; j < tree1->nodes[set1[a]]->neiNum; j++){
                        if (tree1->nodes[set1[a]]->neighbours[j]->pos == tree1->nodes[set1[i]]->pos){
                        	if (i != b)
                        		 c = i;
                        }
                    }
                }
                for ( i = 0; i < w; i++){
                    for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++){
                        if (tree2->nodes[set2[w]]->neighbours[j]->pos == tree2->nodes[set2[i]]->pos)
                            x = i;
                    }
                }
                for (i = 0; i < w; i++){
                    for (j = 0; j < tree2->nodes[set2[w]]->neiNum; j++){
                        if (tree2->nodes[set2[w]]->neighbours[j]->pos == tree2->nodes[set2[i]]->pos){
                            if (i != x)
                            	y = i;
                        }
                    }
                }//get b, c, x, y
                printf("%d\n", b);
                printf("%d\n", c);
                printf("%d\n", x);
                printf("%d\n", y);
                variants = countVariants(TAB, a, w, b, c, x, y);
                printf("countVariants done\n");
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
                            printf("trees merged and stored\n");
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
                            printf("trees merged and stored\n");
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
                printf("%s\n", treeToString(TAB[a][w]));
                }
            //else printf("%d\n", 0);
            posU++;
            printf("%d\n", posU);
            printf("%d\n", w);
        }
        posT++;
        printf("%d\n", posT);
        printf("%d\n", a);
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
*/

