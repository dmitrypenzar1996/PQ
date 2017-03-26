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
	printf("%s\n", "treesPrune started");
	printf("%s\t%s\n", treeToString(tree1), treeToString(tree2));
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

    printf("%d\t%d\n", count1, count2);
    for (i = 0; i < tree1->leavesNum; i++){
    	for (j = 0; j < tree2->leavesNum; j++){
    		//printf("%d\t%d\t%d\n", i, j, count1);
    		if (strcmp(tree1->leaves[i]->name, tree2->leaves[j]->name) == 0){
    			//printf("%s\n", tree1->leaves[i]->name);
    			leavesToSave1[count1] = tree1->leaves[i]->name;
    			printf("%s", leavesToSave1[count1]);
    			count1++;
    			break;
    		}
    	}
    }
    printf("\n tree1leaves done\n");
    for (i = 0; i < tree2->leavesNum; i++){
    	for (j = 0; j < tree1->leavesNum; j++){
    		if (strcmp(tree2->leaves[i]->name, tree1->leaves[j]->name) == 0){
    			leavesToSave2[count2] = tree2->leaves[i]->name;
    			printf("%s", leavesToSave2[count2]);
    			count2++;
    			break;
    		}
    	}
    }
    printf("\n tree2leaves done\n");
    for (i = 0; i< count1; i++){
    	printf("%s", leavesToSave1[i]);
    }
    printf(" - leaves to save from tree1\n");
    for (i = 0; i< count2; i++){
        	printf("%s", leavesToSave2[i]);
    }
    printf(" - leaves to save from tree2\n");
    printf("%d\t%d\n", count1, count2);
    if (count1 > tree1->leavesNum){
    	perror("wrong count 1, maxAgreement:treesPrune");
    }
    if (count2 > tree2->leavesNum){
    	perror("wrong count 2, maxAgreement:treesPrune");
    }
    leavesToSave1 = realloc(leavesToSave1, (sizeof(char*) * (count1)));
    leavesToSave2 = realloc(leavesToSave2, (sizeof(char*) * (count2)));
    printf("%d\t%d\t%s\n", count1, count2, "leaves to save");
    result[0] = treePrune(tree1, leavesToSave1, (size_t) count1, 0);
    result[1] = treePrune(tree2, leavesToSave2, (size_t) count2, 0);

    /*
    */
    /* here is OK */
    return result;
}

Tree* treeRoot(Tree* tree, unsigned nodeID, unsigned neighbourID, char newTree){
	int i;
	Node* root;
	Node* prevNode;
	Tree* result;

	if (tree == 0)
	{
	    fprintf(stderr, "Error, null tree pointer, maxAgreement:treeRoot\n");
	    exit(1);
	}
	if (tree->leavesNum == 0)
	{
	    fprintf(stderr, "Error, cant add to empty tree, maxAgreement:treeRoot\n");
	    exit(1);
	}
	if (nodeID >= tree->nodesNum)
	{
       fprintf(stderr, "Error, node index is out, maxAgreement:treeRoot\n");
        exit(1);
    }
	if (neighbourID >= tree->nodes[nodeID]->neiNum)
    {
        fprintf(stderr, "Error, neighbour index is out of range, (%d of %d)\
                maxAgreement:treeRoot\n", neighbourID, tree->nodes[nodeID]->neiNum);
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
	//free(prevNode);
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
            if (k != 32)
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
		leavesPosArr[tree->leaves[i]->pos] = i;
	}
	for (i = 0;  i < tree->nodesNum; i++){
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
    			//printf("%s%d\n", "Leaf at pos ",  curPos);
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
	int i, count;
	unsigned leavesToDeleteAmount = 0;
	unsigned* leavesPosArr;
	char** leavesToDelete;
	Branch* br;
	Tree* result;

	treeWash(tree1);
	br = TAB[tree1->nodesNum - 1][nodesNum2 - 1];
	//printf("%s\n", "see here");
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
		printf("%s\n", "trees match completely");
		result = treeCopy(tree1, 0);
		printf("%s\n", treeToString(result));
		return result;
	}
	else if (leavesToDeleteAmount == tree1->leavesNum){
		printf("%s\n", "trees dont match");
		printf("%s\n", treeToString(result));
		return result;
	}
	else {//some leaves are to be deleted but not all
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
}

void MAST(Tree* tree1, Tree* tree2)
{
	INT p;
    int b, c, x, y; b = 0; c = 0; x = 0; y = 0;
    int i, j, k;
    int posT, posU;
    int a, w;
    int* variants;
    int* permutation;			//permutation of leaf sets between tree1 and tree2
    unsigned* leavesPosArr1;	//size = nodes; for every node has 0, for leaf has its pos in leaves
    unsigned* leavesPosArr2;	//soon will be updated - will be -1 for nodes that are not leaves
    unsigned* setPermutation1;	//for every node of tree1 tells its pos in topologically sorted set
    unsigned* setPermutation2;	//the same for tree2
    unsigned* set1;				//topologically sorted nodes from rooted tree1
    unsigned* set2;				//topologically sorted nodes from rooted tree2
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    Branch* intersection;
    Branch*** TAB;
    Tree* result;
    Tree** prunedTrees;
    printf("%d\t%d\t%s\n", tree1->leavesNum, tree2->leavesNum, "leavesNum before cleavage");
    prunedTrees = treesPrune(tree1, tree2);
    tree1 = prunedTrees[0];
    tree2 = prunedTrees[1];
    printf("%d\t%d\t%s\n", tree1->leavesNum, tree2->leavesNum, "leavesNum after cleavage");
    printf("%s\t%s\n", treeToString(tree1), treeToString(tree2));
    printf("%d\t%d\n", tree1->nodesNum, tree2->nodesNum);
    setPermutation1 = (unsigned*)calloc(sizeof(unsigned), tree1->nodesNum + 1);
    setPermutation2 = (unsigned*)calloc(sizeof(unsigned), tree2->nodesNum + 1);
    set1 = treeRootAndTopSort(tree1, 1, 0, setPermutation1);
    set2 = treeRootAndTopSort(tree2, 1, 0, setPermutation2);
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

    //this is where the main loop starts
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
            		   intersection->branch[set1[a] / intSize] |= p;
            		   intersection = branchAnd(intersection, branchArr2->array[set2[w]]);
            		   //branchPrint(intersection);
            		   TAB[posT][posU] = intersection;
            	   }
            	   else{ //a is a subtree, w is a leaf
            		   p = 1;
            		   p = p << (leavesPosArr2[set2[w]] & (intSize - 1));
            		   intersection->branch[set2[w] / intSize] |= p;
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
            	//get b, c, x, y
            	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", set1[a], set1[b], set1[c], set2[w], set2[x], set2[y]);


                variants = countVariants(TAB, a, w, b, c, x, y);
                //printf("countVariants done\n");
                k = 0;
                j = 0;
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

/*




//mastLeavesNum = branchGetLeavesPosNum(br);
	//printf("%d%s\n", intSize, " intsize");

	//printf("%zu\t%s\n", mastLeavesNum, "mastleavesNum1");
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
	return result;

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






int count1, count2;
    int i, j;
    int k, l;
    char** leavesToDelete1;
    char** leavesToDelete2;
    printf("%s\n", "treesPrune started");
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
    printf("%d\t%d\n", count1, count2);
    //tree1 = deleteLeaves(tree1, leavesToDelete1, count1);
    //tree2 = deleteLeaves(tree2, leavesToDelete2, count2);
*/

