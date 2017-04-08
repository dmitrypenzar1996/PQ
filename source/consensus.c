#include "consensus.h"

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

size_t branchGetIntSize(Branch* br)
{
    if (br->size % intSize == 0)
    {
        return br->size / intSize;
    }

    return br->size / intSize + 1;
}

Branch* branchCreate(unsigned size)
{
    Branch* branch;
    branch = (Branch*)malloc(sizeof(Branch)); 
    branch->size = size;
    branch->branch = (uint64_t*)calloc(sizeof(uint64_t), branchGetIntSize(branch));
    return branch;
}

void branchDelete(Branch* branch)
{
    free(branch->branch);
    free(branch);
}

unsigned countZeroRightNum(uint64_t p)
{
    if (p == 0)
    {
        return 64;
    }
    uint64_t y = p;
    //find number of trailing zeros
    int r;            // result goes here
    static const char MultiplyDeBruijnBitPosition[64] = 
    {
            0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4,
            62, 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5,
            63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11,
            46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6
    };
    r = MultiplyDeBruijnBitPosition[((uint64_t)((y & -y) * 0x03F79D71B4CB0A89U)) >> 58];
    //printf("%lu %llu %d\n", p, y, r);
    return r;
}



size_t* branchGetLeavesPos(Branch* br, size_t* leavesNum, size_t maxNum)
{
    unsigned i = 0;
    unsigned j = 0;
    unsigned k = 0;
    size_t* positions;
    unsigned curSize = 0;

    positions = malloc(sizeof(size_t) * br->size);
    for(i = 0; i < branchGetIntSize(br); ++i)
    {
        j = 0;
        while(j < intSize)
        {
            k = countZeroRightNum((br->branch[i]) >> j);              
            if (k < intSize)
            {
                positions[curSize++] = k + j +  i * intSize;
            }
            j += k + 1;
            if (curSize >= maxNum)
            {
                break;
            }
        }
	if (curSize >= maxNum)
	{
		break;
	}
    }
    positions = realloc(positions, sizeof(size_t) * curSize);
    *leavesNum = curSize;
    return positions;
}


int branchCompare(Branch* br1, Branch* br2)
{
    int i = 0;

    if (br1->size != br2->size)
    {
        fprintf(stderr, "branchComplare: Branches are not of the same size\n");
        exit(1);
/*        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__, __LINE__); */
    }
    for(; i < branchGetIntSize(br1); ++i)
    {
        if (br1->branch[i] > br2->branch[i]) 
        {
            return 1;
        }
        else if (br1->branch[i] < br2->branch[i])
        {
            return -1;   
        }
    }
    return 0;
}

int vBranchCompare(const void* branch1, const void* branch2)
{
    int i = 0;
    Branch* brPtr1;
    Branch* brPtr2;

    brPtr1 = *(Branch**)branch1;
    brPtr2 = *(Branch**)branch2;

    return branchCompare(brPtr1, brPtr2);
}

void branchNormalize(Branch* br)
{
    int i = 0;
    uint64_t p = 0;
    if ((br->branch[0] & 1) == 1)
    {
        for(i = 0; i < branchGetIntSize(br); ++i)
        {
            br->branch[i] = ~(br->branch[i]);
        }

        --i;
        if (br->size % intSize)
        {
            p = 1 << (br->size);
            br->branch[i] = br->branch[i] % p; 
        }
    }
    return;
}


char* branchToString(Branch* br)
{
    int j = 0;
    int k = 0;
    uint64_t p = 0;
    int curSize = 0;
    char* str = malloc(sizeof(char) * (br->size + 1));
    str[br->size] = '\0';
    for(j = 0; j < branchGetIntSize(br); ++j)
    {
        p = 1;
        curSize = br->size - intSize * j;
        curSize = curSize > intSize ? intSize : curSize;
        
        for(k = 0; k < curSize; ++k)
        {
            sprintf(str + j * intSize + k, "%llu", (br->branch[j] & p) >> k);
            p <<= 1;   
        }
    }
    str[br->size] = '\0';
    return str; 
}

void branchPrint(Branch* br)
{
    int j = 0;
    int k = 0;
    uint64_t p = 0;
    int curSize = br->size;

    for(j = 0; j < branchGetIntSize(br); ++j)
    {
        p = 1;
        //curSize = curSize > intSize ? intSize : curSize;
        int curSize = intSize;
        for(k = 0; k < curSize; ++k)
        {
            printf("%llu", (br->branch[j] & p) >> k);
            p <<= 1;   
        }
        curSize -= intSize;
    }
    printf("\n");
}

char branchContradict(Branch* br1, Branch* br2)
{
    int i = 0;
    char isEmpty11 = 1;
    char isEmpty00 = 1;
    char isEmpty10 = 1; 
    char isEmpty01 = 1;

    if (br1->size != br2->size)
    {
        fprintf(stderr, "branchContradict: Branches are not of the same size\n");
        exit(1);

/*        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__, __LINE__); */
    }


    for(i = 0; i < branchGetIntSize(br1); ++i)
    {
        if (br1->branch[i] & br2->branch[i]) isEmpty11 = 0;
        if ((~ br1->branch[i]) & (~ br2->branch[i])) isEmpty00 = 0;
        if ((~ br1->branch[i]) & br2->branch[i]) isEmpty01 = 0;
        if (br1->branch[i] & (~ br2->branch[i])) isEmpty10 = 0;
    }


    if (! (isEmpty11 || isEmpty00 || isEmpty01 || isEmpty10)) return 1;
    return 0;
}


BranchArray* branchArrayCreate(unsigned startSize)
{
    BranchArray* ba = malloc(sizeof(BranchArray));
    ba->array = calloc(startSize, sizeof(Branch*));
    ba->maxSize = startSize;
    ba->size = 0;
    return ba;
}

void branchArrayDelete(BranchArray* ba)
{
    int i = 0;
    free(ba->array);
    free(ba);
}

void branchArrayAdd(BranchArray* ba, Branch* branch)
{
    if (ba->size && (ba->array[0]->size != branch->size))
    {
        fprintf(stderr, "branchArrayAdd: Branches are not of the same size\n");
        exit(1);

/*        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__, __LINE__);*/
    }

    if (ba->size == ba->maxSize)
    {
        ba->maxSize = ba->maxSize * 3 / 2 + 1;
        ba->array = realloc(ba->array, sizeof(Branch*) * ba->maxSize);
    }
    ba->array[ba->size++] = branch;
}

void branchArraySort(BranchArray* ba)
{
    qsort(ba->array, ba->size, sizeof(Branch*), vBranchCompare);
    return;
}

void branchArrayExtend(BranchArray* dest, BranchArray* source)
{
    unsigned newSize = dest->size + source->size;
    if (newSize >= dest->maxSize)
    {
        dest->maxSize = newSize * 3 / 2 + 1;
        dest->array = realloc(dest->array, sizeof(Branch*) * dest->maxSize);
    }
    
    memcpy(dest->array + dest->size, source->array, source->size * sizeof(Branch*));
    dest->size += source->size;
}



BranchOcc* branchOccCreate(Branch* branch, unsigned occurence)
{
    BranchOcc* brc = malloc(sizeof(BranchOcc));
    brc->branch = branch;
    brc->occurence = occurence;
    return brc;
}


void branchOccDelete(BranchOcc* brc)
{
    free(brc);
}


int branchOccCompare(BranchOcc* brc1, BranchOcc* brc2)
{
    return brc1->occurence - brc2->occurence;
}


int vBranchOccCompare(const void* brc1, const void* brc2)
{
    BranchOcc* brcPtr1 = *(BranchOcc**)brc1;
    BranchOcc* brcPtr2 = *(BranchOcc**)brc2;
    return branchOccCompare(brcPtr1, brcPtr2);
}

int branchOccCompareByBranch(BranchOcc* brc1, BranchOcc* brc2)
{
    return branchCompare(brc1->branch, brc2->branch);
}

int vBranchOccCompareByBranch(const void* brc1, const void* brc2)
{
    BranchOcc* brcPtr1 = *(BranchOcc**)brc1;
    BranchOcc* brcPtr2 = *(BranchOcc**)brc2;
    return branchOccCompareByBranch(brcPtr1, brcPtr2);
}


BranchCounter* branchCounterCreate(size_t startMaxSize)
{
    BranchCounter* bc = malloc(sizeof(BranchCounter));
    bc->size = 0;
    bc->maxSize = startMaxSize;
    bc->array = calloc(bc->maxSize, sizeof(BranchOcc*));
    return bc;
}


void branchCounterDelete(BranchCounter* bc)
{
    free(bc->array);
    free(bc);
}

void branchCounterAdd(BranchCounter* bc, Branch* br, size_t branchOccurence)
{
    if (bc->size == bc->maxSize)
    {
        bc->maxSize = bc->maxSize * 3 / 2 + 1;
        bc->array = realloc(bc->array, bc->maxSize * sizeof(BranchOcc*));
    }
    bc->array[bc->size++] = branchOccCreate(br, branchOccurence);
}

void branchCounterInc(BranchCounter* bc, unsigned pos)
{
    ++bc->array[pos]->occurence;
}

BranchCounter* branchCount(BranchArray* ba)
{
    BranchCounter* bc = branchCounterCreate(10);
    int i = 0;
    branchCounterAdd(bc, ba->array[i],1);
    for(i = 1; i < ba->size; ++i)
    {
        if (branchCompare(ba->array[i - 1], ba->array[i]))
        {
            branchCounterAdd(bc, ba->array[i], 1);
        }
        else
        {
            branchCounterInc(bc, bc->size - 1);
        }
    }

    return bc;
}

void branchCounterSort(BranchCounter* brc)
{
    qsort(brc->array, brc->size, sizeof(BranchOcc*), vBranchOccCompare);
    return;
}

void branchCounterSortByBranch(BranchCounter* brc)
{
    qsort(brc->array, brc->size, sizeof(BranchOcc*), vBranchOccCompareByBranch);
}

BranchArray* treeToBranch(Tree* tree, int* permutation)
{
    uint64_t p = 1;
    int i = 0;
    int j = 0;
    unsigned branchNum = tree->nodesNum;
    BranchArray* ba;
    BranchArray* result;
    NodeStack* stack;
    Node* curNode = 0;
    Node* nextNode = 0;
    char* isTrivial;

    ba = branchArrayCreate(branchNum);
    result = branchArrayCreate(tree->nodesNum - 1);
    stack = nodeStackCreate(tree->nodesNum);

    if (tree->leavesNum == 0)
    {
        return 0;
    }
    if (tree == 0)
    {
        perror("Function treeToBranch: null Tree pointer");
        exit(1);
/*        raiseError("null Tree pointer", __FILE__, __FUNCTION__, __LINE__); */
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

    isTrivial = (char*)calloc(sizeof(char), branchNum);
    isTrivial[curNode->pos] = 1;
    for (i = 0; i < tree->leavesNum; ++i)
    {
        isTrivial[tree->leaves[i]->pos] = 1;
    }

    
    for(i = 0; i < ba->size; ++i)
    {
        if (! isTrivial[i]) branchArrayAdd(result, ba->array[i]);
        else branchDelete(ba->array[i]);
    }

    for(i = 0; i < result->size; ++i)
    {
        branchNormalize(result->array[i]);
    }

    free(isTrivial);
    branchArrayDelete(ba);
    nodeStackDelete(stack);
    return result;
}


char branchIsSubset(Branch* br1, Branch* br2) // 0 - not 1 - br2 is subset br1,
    //-1 - br1 is subset br2
{
    int i = 0;
    char isSubset = 0;

    if (br1->size != br2->size)
    {
        fprintf(stderr, "branchIsSubset: Branches are not of the same size\n");
        exit(1);

/*        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__, __LINE__); */
    }

    for(i = 0; i < branchGetIntSize(br1); ++i)
    {
        if ((br1->branch[i] & br2->branch[i]) != 0) 
        {
            isSubset = 1;
            break;
        }       
    }

    if (!isSubset) return 0;

    for (i = 0; i < branchGetIntSize(br2); ++i)
    {
        if ((br1->branch[i] & (~ br2->branch[i])) != 0) return 1;
    }
    
    return -1;
}

Branch* branchAnd(Branch* br1, Branch* br2)
{
    int i = 0;
    Branch* andBranch = NULL;
    if (br1->size != br2->size)
    {
        fprintf(stderr, "branchAnd: Branches are not of the same size\n");
        exit(1);
/*        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__, __LINE__); */
    }
    andBranch = branchCreate(br1->size);

    for(i = 0; i < branchGetIntSize(br1); ++i)
    {
        andBranch->branch[i] = br1->branch[i] & br2->branch[i];
    }
    
    return andBranch;
}

Branch* branchOr(Branch* br1, Branch* br2)
{
    int i = 0;
    Branch* orBranch = NULL;
    if (br1->size != br2->size)
    {
        fprintf(stderr, "branchOr: Branches are not of the same size\n");
        exit(1);
/*        raiseError("Branches are not of the same size", __FILE__, __FUNCTION__, __LINE__); */
    }
    orBranch = branchCreate(br1->size);

    for(i = 0; i < branchGetIntSize(br1); ++i)
    {
        orBranch->branch[i] = br1->branch[i] | br2->branch[i];
    }

    return orBranch;
}

Branch* branchReverse(Branch* br)
{
    int i = 0;
    Branch* revBranch = branchCreate(br->size);
    for(i = 0; i < branchGetIntSize(br); ++i)
    {
        revBranch->branch[i] = ~br->branch[i];
    }
    return revBranch;
}

Branch* branchCopy(Branch* br)
{
    int i = 0;
    Branch* copy = branchCreate(br->size);
    for(i = 0; i < branchGetIntSize(br); ++i)
    {
        copy->branch[i] = br->branch[i];
    }
    return copy;
}

char branchIsZero(Branch* br)
{
    int i = 0;
    for(i = 0; i < branchGetIntSize(br); ++i)
    {
        if(br->branch[i] != 0) return false;
    }
    return true;
}


ParserNode* parserNodeCreate(BranchOcc* branchOcc)
{
    ParserNode* nd = malloc(sizeof(ParserNode));
    nd->branchOcc = branchOcc;
    nd->parent = NULL;
    nd->right = NULL;
    nd->left = NULL;
    nd->treeNode = NULL;
    nd->color = WHITE;
    return nd;
}


void parserNodeDelete(ParserNode* nd)
{
    free(nd);
}



ParserTree* parserTreeCreate()
{
    ParserTree* tree = malloc(sizeof(ParserTree));
    tree->size = 0;
    tree->root = NULL;
    return tree;
}

void parserTreeDelete(ParserTree* tree)
{
    ParserNode** nodes = malloc(sizeof(ParserNode*) * tree->size);
    size_t curStart = 0;
    size_t curEnd = 1;
    size_t newEnd = 0;
    int i = 0;
    nodes[0] = tree->root;

    while(curEnd != tree->size)
    {
        newEnd = curEnd;
        for(i = curStart; i < curEnd; ++i)
        {
            if (nodes[i]->left != NULL)
            {
                nodes[newEnd++] = nodes[i]->left;
            }    
            if (nodes[i]->right != NULL)
            {
                nodes[newEnd++] = nodes[i]->right;
            }
            free(nodes[i]);
        } 
        curStart = curEnd;
        curEnd = newEnd;
    }
    free(nodes);
}


void parserTreePrint(ParserTree* tree)
{
    ParserNode** nodes = malloc(sizeof(ParserNode*) * tree->size * 2);
    size_t curStart = 0;
    size_t curEnd = 1;
    size_t newEnd = 0;
    int i = 0;
    int k = 0;
    nodes[0] = tree->root;

    while(curEnd != curStart)
    {
        newEnd = curEnd;
        printf("Level %d\n", k++);

        for(i = curStart; i < curEnd; ++i)
        {
            if (nodes[i]->left != NULL)
            {
                nodes[newEnd++] = nodes[i]->left;
            }    
            if (nodes[i]->right != NULL)
            {
                nodes[newEnd++] = nodes[i]->right;
            }
            branchPrint(nodes[i]->branchOcc->branch);
        } 
        curStart = curEnd;
        curEnd = newEnd;
        printf("End\n");
    }
    free(nodes);
}




void parserTreeAdd(ParserTree* tree, BranchOcc* branchOcc,
        char* name) // 0 for inner, other for leaf
{
    ParserNode* curNode;
    ParserNode* temp = NULL;
    char isSubset = 1;
    
    ++(tree->size);
    if (tree->root == NULL)
    {
            tree->root = parserNodeCreate(branchOcc);
        if (name) tree->root->treeNode = leafCreate(name);
        return;
    }
    curNode = tree->root;
    temp = NULL;
    isSubset = 1;
    while(true)
    {
        if( (isSubset = 
                        branchIsSubset(branchOcc->branch,
                            curNode->branchOcc->branch)) == -1)
        {
            if (curNode->left != NULL)
            {
                curNode = curNode->left;
            }
            else
            {
                curNode->left = 
                    parserNodeCreate(branchOcc);
                curNode->left->parent = curNode;
                curNode = curNode->left;
                break;
            }
        }
        else if (isSubset == 0)
        {
            if (curNode->right != NULL)
            {
                curNode = curNode->right;
            }
            else
            {
                curNode->right = parserNodeCreate(branchOcc);
                curNode->right->parent = curNode;
                curNode = curNode->right;
                break;
            }
        }
        else // isSubset == 1
        {
            printf("is subset = 1\n");
            fprintf(stderr,"Perhaps something've gone wrong. Please,\
                    make sure you've give branches in reverse order,\
                    if yes - inform me");
            exit(1);
            temp = parserNodeCreate(branchOcc);
            if (curNode == tree->root)
            {
                tree->root = temp;
                tree->root->left = curNode;
                curNode->parent = tree->root;
            }
            else
            {
                if (curNode->parent->right == curNode) 
                {
                    curNode->parent->right = temp;
                }
                else
                { 
                    curNode->parent->left = temp;
                }
                temp->parent = curNode->parent;
                curNode->parent = temp;
                temp->left = curNode;
            }
            if ( curNode->right != NULL && (!branchIsSubset(temp->branchOcc->branch, 
                        curNode->right->branchOcc->branch)))
            {
                temp->right = curNode->right;
                curNode->right->parent = temp;
                curNode->right = NULL;
            }

            curNode = temp;
            break;
        }
    }
    if (name) curNode->treeNode = leafCreate(name);

    return;

}

Tree* branchCounterToTree(BranchCounter* bc, char** names)
{
    int i = 0;
    char isSubset = 0;
    ParserNode* temp = NULL;
    ParserNode* curNode = NULL;
    Branch* branch = NULL;
    uint64_t p = 1;
    size_t leavesNum;
    ParserTree* parser;
    Tree* tree;
    size_t curNodesNum = 0;
    size_t curLeavesNum = 0;
    BranchOcc** trivialBranches;
    Branch* orBranch = NULL;
    ParserNode** stack;
    size_t stackSize = 0;
    char ready;

    leavesNum = bc->array[0]->branch->size;
    parser = parserTreeCreate();
    tree = treeCreate(); 
    tree->nodes = (Node**)malloc(sizeof(Node*) * (leavesNum * 2 - 2));
    tree->leaves = (Node**)malloc(sizeof(Node*) * (leavesNum));
    tree->leavesNum = leavesNum;

    branch = branchCreate(leavesNum);
    branch->branch[0] = 1;
    branchNormalize(branch);

    trivialBranches = (BranchOcc**)malloc(sizeof(BranchOcc*) * leavesNum); 
    trivialBranches[0] = branchOccCreate(branch, 0);
    parserTreeAdd(parser, trivialBranches[0], names[0]);

    branchCounterSortByBranch(bc);


    for(i = bc->size - 1; i >= 0; --i)
    {
        parserTreeAdd(parser, bc->array[i], NULL);
    }

    for(i = 1; i < leavesNum; ++i)
    {// add trivial leaves to simplify algorithm
        p = 1;
        p = p << (i & (intSize - 1));
        branch = branchCreate(leavesNum);   
        branch->branch[i / intSize] |= p;
        
        trivialBranches[i] = branchOccCreate(branch, 0);
        parserTreeAdd(parser, trivialBranches[i], names[i]);
    }


    curNode = parser->root;

    stack = malloc(sizeof(ParserNode*) * (leavesNum * 2 - 2));
    stackSize = 0;
    stack[stackSize++] = parser->root;
    ready = 1;
    while(stackSize > 0)
    {
        ready = 1;
        curNode = stack[stackSize - 1];
        if (curNode->left != NULL && curNode->left->color != BLACK)
        {
            stack[stackSize++] = curNode->left;
            ready = 0;
        }
        if (curNode->right != NULL && curNode->right->color != BLACK)
        {
            stack[stackSize++] = curNode->right;
            ready = 0;
        }
        if (ready)
        {
            curNode->color = BLACK;

            if (curNode->left == NULL && curNode->right == NULL)
            {
/*
                      001100
                       /
                 001000 
                       \
                         000100  we are here 
*/
                tree->nodes[curNodesNum++] = curNode->treeNode;
                curNode->treeNode->pos = curNodesNum - 1;
                tree->leaves[curLeavesNum++] = curNode->treeNode;
            }
            else if (curNode->left == NULL )
            {   
/*
                      001100 or 001110
                       /
                 001000 we are here
                       \
                         000100  or 0001100 
*/

                tree->nodes[curNodesNum++] = curNode->treeNode;
                tree->leaves[curLeavesNum++] = curNode->treeNode;
                curNode->treeNode->pos = curNodesNum - 1;
                
                if (curNode->right->right == NULL)
                {
                    tree->nodes[curNodesNum++] = nodeCreate();              
                    tree->nodes[curNodesNum - 1]->pos = curNodesNum - 1;

                    nodeAddNeighbour(tree->nodes[curNodesNum - 1],
                        curNode->right->treeNode, 
                        curNode->right->branchOcc->occurence);
                    nodeAddNeighbour(curNode->right->treeNode,
                        tree->nodes[curNodesNum - 1], 
                        curNode->right->branchOcc->occurence);
                    nodeAddNeighbour(curNode->treeNode,
                        tree->nodes[curNodesNum - 1], 
                        0);
                    nodeAddNeighbour(tree->nodes[curNodesNum - 1],
                        curNode->treeNode, 
                        0);
                    curNode->treeNode = tree->nodes[curNodesNum - 1];
                }
                else // curNode->right->right != NULL
                {
                    if (curNode->treeNode != NULL)
                    {
                        nodeAddNeighbour(curNode->treeNode,
                                curNode->right->treeNode, 0);
                        nodeAddNeighbour(curNode->right->treeNode,
                                curNode->treeNode, 0);
                        curNode->treeNode = curNode->right->treeNode;
                    }
                    else
                    {
                        perror("Something've gone wrong");
                        exit(1);
/*                      raiseError("Something've gone wrong\n", __FILE__, __FUNCTION__, __LINE__); */
                    }
                }
                
            }
            else if (curNode->right == NULL)
            {
                if (curNode == parser->root)
                {
                    nodeAddNeighbour(curNode->left->treeNode, curNode->treeNode,
                        curNode->branchOcc->occurence);
                    nodeAddNeighbour(curNode->treeNode, curNode->left->treeNode,
                        curNode->branchOcc->occurence);

                    tree->nodes[curNodesNum++] = curNode->treeNode;
                    tree->leaves[curLeavesNum++] = curNode->treeNode;
                    curNode->treeNode->pos = curNodesNum - 1;
                }
                else
                {
                    curNode->treeNode = curNode->left->treeNode;
                }
            }
            else //(curNode->right != NULL && curNode->left != NULL)
            {
                /*orBranch = branchOr(curNode->branchOcc->branch,
                        curNode->right->branchOcc->branch);*/
                curNode->treeNode = nodeCreate();
                nodeAddNeighbour(curNode->treeNode, curNode->left->treeNode,
                    curNode->branchOcc->occurence);
                nodeAddNeighbour(curNode->left->treeNode, curNode->treeNode,
                    curNode->branchOcc->occurence);
                nodeAddNeighbour(curNode->treeNode, curNode->right->treeNode,
                    curNode->right->branchOcc->occurence);
                nodeAddNeighbour(curNode->right->treeNode, curNode->treeNode,
                    curNode->right->branchOcc->occurence);
                tree->nodes[curNodesNum++] = curNode->treeNode;
                curNode->treeNode->pos = curNodesNum - 1;
            }
                /*
                else
                {
                    curNode->treeNode = curNode->left->treeNode;
                    nodeAddNeighbour(curNode->treeNode, 
                                     curNode->right->treeNode,
                                     curNode->right->branchOcc->occurence);
                    nodeAddNeighbour(curNode->right->treeNode,
                                     curNode->treeNode,
                                     curNode->right->branchOcc->occurence);
                    
                }
                branchDelete(orBranch);*/
            --stackSize;
        }
    }

    for(i = 0; i < leavesNum; ++i)
    {
        branchDelete(trivialBranches[i]->branch);
        branchOccDelete(trivialBranches[i]);
    }
    
    tree->nodesNum = curNodesNum;

    free(stack);
    parserTreeDelete(parser);
    return tree;
}

// branches sorted by occurence in BranchCounter are required
BranchCounter* majorityRule(BranchCounter* bc, unsigned threshold)
{
    BranchCounter* consensus = branchCounterCreate(10);
    int i = 0;
    int j = 0;
    int k = 0;
    char take = 1;
    for(i = bc->size - 1; i >= 0; --i)
    {
        if (bc->array[i]->occurence > threshold)
        {
            branchCounterAdd(consensus, bc->array[i]->branch, bc->array[i]->occurence);
        }
        else break;
    }
    return consensus;
}

// branches sorted by occurence in BranchCounter are required
BranchCounter* majorityExtendedRule(BranchCounter* bc, unsigned threshold)
{
    BranchCounter* consensus = branchCounterCreate(10);
    int i = 0;
    int j = 0;
    int k = 0;
    char take = 1;
    for(i = bc->size - 1; i >= 0; --i)
    {
        if (bc->array[i]->occurence > threshold)
        {
            branchCounterAdd(consensus, bc->array[i]->branch, bc->array[i]->occurence);
        }
        else break;
    }
    j = i;
    // select non-contradicting branches
    for(i = j; i >= 0; --i)
    {
        take = 1;
        for (k = 0; k < consensus->size; ++k)
        {
            if (branchContradict(consensus->array[k]->branch, bc->array[i]->branch))
            {
                take = 0;
            }   
        }
        if (take)
        {
            branchCounterAdd(consensus, bc->array[i]->branch, bc->array[i]->occurence);
        }
    }
    
    return consensus;
}


Tree* makeConsensus(Tree** treeArray, size_t treeNum, double threshold,
        char extended)  
{
    int i = 0;
    char** initTreeNames = treeGetNames(treeArray[0]);
    int* permutation = getRange(0, treeArray[0]->leavesNum);
    BranchArray* ba = treeToBranch(treeArray[0], permutation);
    BranchArray* temp = NULL;
    char** treeNames = NULL;
    Tree* consensusTree = NULL;
    BranchCounter* consensus = NULL;
    BranchCounter* bc = NULL;
    free(permutation);

    for(i = 1; i < treeNum; ++i)
    {
        treeNames = treeGetNames(treeArray[i]); 
        permutation = calculatePermutation(treeNames, initTreeNames,
                treeArray[i]->leavesNum);
        temp = treeToBranch(treeArray[i], permutation);
        branchArrayExtend(ba, temp);
        free(permutation);
        branchArrayDelete(temp);
    }
    

    branchArraySort(ba);
    bc = branchCount(ba);
    branchCounterSort(bc);
    if (extended)
    {
        consensus = majorityExtendedRule(bc, treeNum * threshold);
    }
    else
    {
        consensus = majorityRule(bc, treeNum * threshold);
    }

    printf("Chosen branches:\n");
    for (i = 0; i < consensus->size; ++i)
    {
        printf("Branch: %s ", branchToString(consensus->array[i]->branch));
        printf("Occurence: %u\n", consensus->array[i]->occurence);
    }

    consensusTree = branchCounterToTree(consensus, initTreeNames);

    free(initTreeNames);
    free(treeNames);
    for(i = 0; i < consensus->size; ++i) branchOccDelete(consensus->array[i]);
    for(i = 0; i < bc->size; ++i) branchOccDelete(bc->array[i]);
    for(i = 0; i < ba->size; ++i) branchDelete(ba->array[i]);
    branchCounterDelete(consensus);
    branchCounterDelete(bc);
    branchArrayDelete(ba);
    
    return consensusTree;
}
