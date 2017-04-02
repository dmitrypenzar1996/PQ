#include "cutTree.h"
#include "Tree.h"
#include <stdlib.h>
#include <assert.h>

void stackCycle(Node* node, NodeStack* stack);
void treeUndegenerate(Node* node);
void treeProcess(Tree* tree);

Tree** cutTree(Tree* tree, int nodeId, int neiId)
{
    if (tree->nodes[nodeId]->neiNum <= neiId)
    {
        fprintf(stderr, "Wrong nei id\n");
        return NULL;
    }
    printf("CutTree START\n");
	Node* rootnode1; 
	Node* rootnode2;
	Tree** result = calloc(sizeof(Tree*), 2);
	Tree* intermediate = treeCreate();
	intermediate = treeCopy(tree, 0);
	Tree* tree1 = treeCreate();
	tree1->nodes = (Node**)calloc(sizeof(Node*), intermediate->nodesNum);
	tree1->leaves = (Node**)calloc(sizeof(Node*), intermediate->leavesNum);
	Tree* tree2 = treeCreate();
	tree2->nodes = (Node**)calloc(sizeof(Node*), intermediate->nodesNum);
	tree2->leaves = (Node**)calloc(sizeof(Node*), intermediate->leavesNum);
	rootnode1 = intermediate->nodes[nodeId];
	rootnode2 = intermediate->nodes[nodeId]->neighbours[neiId];
	if (rootnode1->neiNum-1 != neiId)
	{
		Node* buffer;
		buffer = rootnode1->neighbours[rootnode1->neiNum-1];
		rootnode1->neighbours[rootnode1->neiNum-1] = rootnode1->neighbours[neiId];
		rootnode1->neighbours[neiId] = buffer;	
	}
	int nodelink;
	nodelink = 0;
	while (rootnode2->neighbours[nodelink] != intermediate->nodes[nodeId])
	{
		nodelink++;
	}
	if (rootnode2->neiNum-1 != nodelink)
	{
		Node* buffer;
		buffer = rootnode2->neighbours[rootnode2->neiNum-1];
		rootnode2->neighbours[rootnode2->neiNum-1] = rootnode2->neighbours[nodelink];
		rootnode2->neighbours[nodelink] = buffer;
	}
	--rootnode1->neiNum;
	--rootnode2->neiNum;
	NodeStack* treeStack = nodeStackCreate(intermediate->nodesNum);
	stackCycle(rootnode1, treeStack);
	tree1->nodesNum = treeStack->curSize;
	while (treeStack->curSize != 0)
	{
		int count;
		count = tree1->nodesNum - treeStack->curSize;
		tree1->nodes[count] = nodeStackPeek(treeStack);
		nodeStackPop(treeStack);
	}
	treeProcess(tree1);
	result[0] = tree1;
	printf("tree1 done\n"); 
	printf("%s\n", treeToString(tree1));
	stackCycle(rootnode2, treeStack);
	tree2->nodesNum = treeStack->curSize;
	while (treeStack->curSize != 0)
	{
		int count;
		count = tree2->nodesNum - treeStack->curSize;
		tree2->nodes[count] = nodeStackPeek(treeStack);
		nodeStackPop(treeStack);
	}
	treeProcess(tree2);
	result[1] = tree2;
	printf("tree2 done\n");
	printf("%s\n", treeToString(tree2));
	nodeStackDelete(treeStack);
    	printf("CutTree END\n");
        return result;
}


void stackCycle(Node* node, NodeStack* stack){
	int j;
	if (node->neiNum > 1){
		node->color = BLACK;
		nodeStackPush(stack, node);
		for (j = 0; j < node->neiNum; j++){
			if (node->neighbours[j]->color != BLACK)
			stackCycle(node->neighbours[j], stack);
		}
	}
    else{// (node->neiNum == 1 or 0)
		if (node->color !=BLACK){
			node->color = BLACK;
			nodeStackPush(stack, node);
		}
    }
}


void treeUndegenerate(Node* node)
{
	int i, j, k;	
	if (node->neiNum != 0)
	{
		for (i =0; i < node->neiNum; i++)
		{
			for (j = 0; j < node->neiNum; j++)
			{
				if (i != j)
				{
					for (k = 0; k < node->neighbours[i]->neiNum; k++)
					{
						if (node->neighbours[i]->neighbours[k] == node)
							node->neighbours[i]->neighbours[k] = node->neighbours[j];
					}
				}
			}
		}
		nodeDelete(node);	
	}

	else //node has no neighbours
	{
		;	
	}
}

void treeProcess(Tree* tree)
{
	int i;
	Node* curnode;
	Node* rootnode;
	rootnode = tree->nodes[tree->nodesNum - 1];
	if (tree->nodesNum > 1)
	{
		treeUndegenerate(rootnode);
		tree->leavesNum = 0;
		for (i = 0; i < tree->nodesNum - 1; i++)
		{	
		
			curnode = tree->nodes[i]; 
                	curnode->pos = i;
			if (curnode->neiNum == 1)
                	{
		    		tree->leavesNum = tree->leavesNum + 1;
                    		tree->leaves[tree->leavesNum - 1] = curnode;
                	}
		}
	}
	else
	{
		tree->leavesNum = 1;
		rootnode->pos = 1;
		tree->leaves[0] = rootnode;
	}
	tree->nodes = (Node**)realloc(tree->nodes, sizeof(Node*) * tree->nodesNum);
	tree->leaves = (Node**)realloc(tree->leaves, sizeof(Node*) * tree->leavesNum);
        printf("%s\n", treeToString(tree));
	//treeLCAFinderCalculate(tree);
}

/*int main(int argsNum, char** args)
{
    if (argsNum != 4)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        return 0;
    }
    puts("HIIIII");
    Tree* tree = treeRead(args[1]);
    puts("HIIIII");
    Tree** result = CutTree(tree, atoi(args[2]), atoi(args[3]));

    return 0;
}*/
