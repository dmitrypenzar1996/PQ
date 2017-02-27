#include "Tree.h"


static const char DEFAULT_MAX_NODE_SIZE = 3;
static const size_t default_strSize = 100;

NodeStack* nodeStackCreate(unsigned maxSize)
{
    NodeStack* stack;
    stack = (NodeStack*)malloc(sizeof(NodeStack));
    stack->nodes = (Node**)calloc(sizeof(Node*), maxSize);
    stack->curSize = 0;
    stack->maxSize = maxSize;
    return stack;
} /* nodeStackCreate */

void nodeStackDelete(NodeStack* stack)
{
    free(stack->nodes);
    free(stack);
    return;
} /* nodeStackDelete */

void nodeStackPush(NodeStack* stack, Node* node)
{
    if (stack->curSize >= stack->maxSize)
    {
        stack->maxSize = stack->maxSize * 3 / 2 + 1;
        stack->nodes = realloc(stack->nodes, sizeof(Node*) * stack->maxSize);
    }
    stack->nodes[stack->curSize] = node; 
    ++stack->curSize;
    return;
} /* nodeStackPush */

Node* nodeStackPeek(NodeStack* stack)
{
    if (stack->curSize == 0)
    {
        fprintf(stderr, "Error, attempt to peek from empty stack, \
                Tree:nodeStackPeek\n");
        exit(1);
    }
    return stack->nodes[stack->curSize - 1];
} /* nodeStackFees */

void nodeStackPop(NodeStack* stack)
{
    if (stack->curSize == 0)
    {
        fprintf(stderr, "Error, attempt to pop from empty stack, \
                Tree:nodeStackPop\n");
        exit(1);
    }
    --stack->curSize;
    return;
} /* nodeStackingPop */

Node* nodeCreate()
{
    Node* node;

    node = (Node*)malloc(sizeof(Node));
    node->name = 0;
    node->neighbours = (Node**)calloc(sizeof(Node*), DEFAULT_MAX_NODE_SIZE);
    node->dist = (double*)calloc(sizeof(double), DEFAULT_MAX_NODE_SIZE);
    node->neiNum = 0;
    node->maxNeiNum = DEFAULT_MAX_NODE_SIZE;
    node->color = WHITE;
    node->pos = -1;
    return node;
} /* nodeCreate */

Node* leafCreate(char* name)
{
    Node* node; 
    node = nodeCreate();
    if (name == 0)
    {
        fprintf(stderr, "Error: Null name for leaf, Tree:leafCreate\n");
        exit(1);
    }
    node->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(node->name, name);
    return node;
} /* lleafCreate */

void nodeDelete(Node* node)
{
    if (node == 0)
    {
        fprintf(stderr, "Error: Null node to delete, Tree:nodeDelete\n");
        exit(1);
    }
    if (node->name != 0)
    {
        free(node->name);
    }
    free(node->neighbours);
    free(node->dist);
    free(node);
    return;
} /* nodeDelete */

void nodeAddNeighbour(Node* node, Node* neighbour, double dist)
{
    if (neighbour == 0)
    {
        fprintf(stderr, "Error, Null node as neighbour, Tree:nodeAddNeighbour\n");
        exit(1);
    }
    else if (node == 0)
    {
        fprintf(stderr, "Error, Null node as node, Tree:nodeAddNeighbour\n");
        exit(1);
    }

    if ((node->neiNum == 1) && (node->name != 0))
    {
        fprintf(stderr, "Error, node has max number of possible neighbours\n");
        exit(1);
    }

    if (node->neiNum == node->maxNeiNum)
    {
        node->maxNeiNum = node->maxNeiNum * 3 / 2 + 1;
        node->neighbours = realloc(node->neighbours, sizeof(Node*) * 
                node->maxNeiNum);
        node->dist = realloc(node->dist, sizeof(Node*)  * 
                node->maxNeiNum);
    }

    node->neighbours[node->neiNum] = neighbour;
    node->dist[node->neiNum] = dist;
    ++node->neiNum;
    return;
} /* nodeAddNeighbour */

Tree* treeCreate()
{
    Tree* tree;
    tree = (Tree*)malloc(sizeof(Tree));
    tree->nodes = NULL; 
    tree->leaves = NULL;
    tree->leavesNum = 0;
    tree->nodesNum = 0;
    tree->lcaFinder = NULL;
    return tree;
} /* treeCreate */

void treeDelete(Tree* tree)
{
    if (tree->nodesNum != 0)
    {
        int i = 0;
        for(i = 0; i < tree->nodesNum; ++i)
        {
            nodeDelete(tree->nodes[i]);
        }
    }
    if (tree->nodes != 0)
    {
        free(tree->nodes);
    }
    if (tree->leaves != 0)
    {
        free(tree->leaves);
    }
    if (tree->lcaFinder != 0)
    {
        lcaFinderDelete(tree->lcaFinder);
    }
    free(tree);
    return;
} /* treeDelete */

double readLength(char* string, unsigned* pos)
{
    int j;
    double length;
   
    j = *pos;
    while(j < strlen(string) && string[j] != ')' && string[j] != ',')
    {
        if (('0' <= string[j] && string[j] <= '9') || string[j] == '.' ||
                string[j] == '-')
        {
            ++j;
        }
        else
        {
            printf("%s\n", string + j);
            fprintf(stderr, "Error: wrong Newick format, Tree.c:readLength\n");
            exit(1);
        }
    }

    length = atof(string + (*pos));
    *pos = j;
    return length;
} /* readLength */

char* readName(char* string, unsigned* pos)
{
    int j;
    char* name;
   
    j = *pos;
    while(j < strlen(string) && string[j] != ')' && string[j] != ',' && 
            string[j] != ':')
    {
        if (string[j] == '(')
        {
            fprintf(stderr, "Error: wrong Newick format, Tree:readName\n");
            exit(1);
        }
        ++j;
    }
    name = (char*)malloc(sizeof(char) * (j - *pos + 1));
    memcpy(name, string + *pos, j - *pos);
    name[j - *pos] = '\0';
    *pos = j;
    return name;
} /* readName */


Tree* treeFromNewick(char* newick)
{
    Tree* tree;
    NodeStack* stack;
    unsigned nodesNum;
    unsigned leavesNum;
    Node** leaves;
    Node** nodes;
    Node* node;
    Node* nei;
    Node* fuse;
    unsigned int curPos;
    char* name;
    int i;
    double dist;

    tree = treeCreate();
    stack = nodeStackCreate(strlen(newick));

    if (newick[0] != '(' || newick[strlen(newick) - 1] != ';')
    {
        fprintf(stderr, "Error, wrong newick format, Tree:treeFromNewick\n");
        exit(1);
    }


    leaves = (Node**)calloc(sizeof(Node*), strlen(newick));
    nodes = (Node**)calloc(sizeof(Node*), strlen(newick));

    node = nodeCreate();
    nodeStackPush(stack, node);
    nodesNum = 0;
    nodes[nodesNum] = node;
    node->pos = nodesNum;
    ++nodesNum;
    leavesNum = 0;
/* here is not yet! */
    curPos = 1;
    while(newick[curPos] != ';')
    {
        if (newick[curPos] == '(')
        {
            node = nodeCreate();
            nodeAddNeighbour(nodeStackPeek(stack), node, 0);
            nodeAddNeighbour(node, nodeStackPeek(stack), 0);
            nodes[nodesNum] = node;
            node->pos = nodesNum;
            ++nodesNum;
            nodeStackPush(stack, node);
            ++curPos;
        }
        else if (newick[curPos] == ')')
        {
            nodeStackPop(stack);
            ++curPos;
        }
        else if (newick[curPos] == ':')
        {
            ++curPos;

            node = nodeStackPeek(stack);
            dist = readLength(newick, &curPos);
            node->dist[node->neiNum - 1] = dist;
            nei = node->neighbours[node->neiNum - 1];

            for(i = 0; i < nei->neiNum; ++i)
            {
                if (node == nei->neighbours[i])
                {
                    nei->dist[i] = dist;
                    break;
                }
            }
        }
        else if (newick[curPos] == ' ' || newick[curPos] == ',' || 
                 newick[curPos] < 32)
        {
            ++curPos;
        }
        else
        {
            name = readName(newick, &curPos);
            node = leafCreate(name);
            free(name);
            leaves[leavesNum] = node;
            ++leavesNum;
            nodes[nodesNum] = node;
            node->pos = nodesNum;
            ++nodesNum;
            nodeAddNeighbour(nodeStackPeek(stack), node, 0);
            nodeAddNeighbour(node, nodeStackPeek(stack), 0);
        }
    }
 
    nodeStackDelete(stack);
    //if (nodes[0]->neiNum < 2)
   // {
  //      fprintf(stderr, "Error, tree must contain at least two leaves,\
  //              Tree:treeFromNewick\n");
  //      exit(1);
  //  }
  //

    //if (nodesNum != (leavesNum * 2 - 2) && nodesNum != (leavesNum * 2 - 1))
    //{
   //     fprintf(stderr, "Error, wrong newick format, Tree:treeFromNewick\n");
   //     exit(1);
   // }

    leaves = realloc(leaves, sizeof(Node*) * leavesNum);
    if (nodes[0]->neiNum == 2) //tree was rooted
    {
        fuse = nodes[0];
        --nodesNum;
        memmove(nodes, nodes + 1, sizeof(Node*) * nodesNum);
        fuse->neighbours[1]->neighbours[0] = fuse->neighbours[0];
        if (fuse->neighbours[0]->neiNum == 1) // is leaf
        {
            fuse->neighbours[0]->neighbours[0] = fuse->neighbours[1];
        }
        else
        {
            fuse->neighbours[0]->neighbours[0] = fuse->neighbours[0]->neighbours[1];
            fuse->neighbours[0]->neighbours[1] = fuse->neighbours[0]->neighbours[2];
            fuse->neighbours[0]->neighbours[2] = fuse->neighbours[1];
        }
        nodeDelete(fuse);
    }

    for(i = 0; i < nodesNum; ++i)
    {
        nodes[i]->pos = i;
    }

    nodes = realloc(nodes, sizeof(Node*) * nodesNum);
    tree->nodes = nodes;
    tree->leaves = leaves;
    tree->leavesNum = leavesNum;
    tree->nodesNum = nodesNum;

    treeLCAFinderCalculate(tree);

    return tree; 
} /*treeFromNewick */

void treeWash(Tree* tree)
{
    int i = 0;
    for(i = 0; i < (tree->nodesNum); ++i)
    {
        tree->nodes[i]->color = WHITE;
    }
    return;
} /* treeWash */

Tree* treeCopy(Tree* source, char copyLCAFinder)
{
    int i, j;
    Node* curNode = NULL;
    unsigned curLeafID = 0;
    Tree* dest;

    if (source == NULL)
    {
        fprintf(stderr, "Error, null Tree pointer, Tree:treeCopy\n");
        exit(1);
    }
    dest = treeCreate();
    dest->leavesNum = source->leavesNum;
    dest->nodesNum = source->nodesNum;
    dest->nodes = calloc(sizeof(Node*), source->nodesNum);
    dest->leaves = calloc(sizeof(Node*), source->leavesNum);

    for(i = 0; i < source->nodesNum; ++i)
    {
        curNode = source->nodes[i];
        if (curNode->name == NULL) /* internal */
        {
            dest->nodes[i] = nodeCreate();
            dest->nodes[i]->pos = i;
        }
        else /* leaf */
        {
            dest->nodes[i] = leafCreate(curNode->name);
            dest->leaves[curLeafID] = dest->nodes[i];
            dest->nodes[i]->pos = i;
            ++curLeafID;
        }
        
    }

    for(i = 0; i < source->nodesNum; ++i)
    {
        curNode = source->nodes[i];
        for(j = 0; j < curNode->neiNum; ++j)
        {
            nodeAddNeighbour(dest->nodes[i],\
                    dest->nodes[curNode->neighbours[j]->pos], 0);
        }
    }

    if (copyLCAFinder)
    {
        treeLCAFinderCalculate(dest);
    }
    return dest;
} /* treeCopy */


char* treeToString(Tree* tree)
{
    char* string = NULL;
    unsigned stringMaxSize = 100000;
    unsigned stringCurSize;
    NodeStack* stack;
    Node* curNode;
    Node* nextNode;
    int i;

    if (tree->leavesNum == 0)
    {
        string = (char*)malloc(sizeof(char) * 4);
        strcpy(string, "();");
        return string;
    }
    treeWash(tree);

    string = (char*)calloc(sizeof(char*), stringMaxSize + 1);
    
    stack = nodeStackCreate(tree->nodesNum);
    curNode = tree->nodes[0];
    stringCurSize = 0;
    string[stringCurSize] = '(';
    ++stringCurSize;
    
    nodeStackPush(stack, curNode);
    curNode->color = GREY;
    while(stack->curSize != 0)
    {
        curNode = nodeStackPeek(stack);
        nextNode = NULL;
        for (i = 0; i < curNode->neiNum ; ++i)
        {
            if (curNode->neighbours[i]->color == WHITE)
            {
                nextNode = curNode->neighbours[i];
                break;
            }
        }
        if (nextNode)
        {
            if (nextNode->name == 0)
            {
                if (stringCurSize >= stringMaxSize)
                {
                   stringMaxSize = stringMaxSize * 3 / 2 + 1; 
                   string = realloc(string, sizeof(char) * stringMaxSize);
                }
                string[stringCurSize] = '(';
                ++stringCurSize;
                nodeStackPush(stack, nextNode);
                nextNode->color = GREY;
            }
            else
            {
                nextNode->color = BLACK;
                if (stringCurSize + strlen(nextNode->name) >= \
                        stringMaxSize)
                {
                    stringMaxSize = stringMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * stringMaxSize);
                }
                strcpy(string + stringCurSize, nextNode->name);
                stringCurSize += strlen(nextNode->name);
                if (stringCurSize >= stringMaxSize)
                {
                    stringMaxSize = stringMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * stringMaxSize);
                }
                strcpy(string + stringCurSize, nextNode->name);
                string[stringCurSize] = ',';
                ++stringCurSize;
            }
        }
        else
        {
            nodeStackPop(stack);
            if (curNode->name != 0)
            {
                if (stringCurSize + strlen(curNode->name) >= \
                        stringMaxSize)
                {
                    stringMaxSize = stringMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * stringMaxSize);
                }
                strcpy(string + stringCurSize, curNode->name);
                stringCurSize += strlen(curNode->name);
                ++stringCurSize;
                if (stringCurSize >= stringMaxSize)
                {
                    stringMaxSize = stringMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * stringMaxSize);
                }   
            }
            string[stringCurSize - 1] = ')';
            string[stringCurSize] = ',';
            ++stringCurSize;
            curNode->color = BLACK;
        }
    }
    string[stringCurSize - 1] = ';';
    string[stringCurSize] = '\0';
    string = realloc(string, sizeof(char) * (stringCurSize + 1));
    
    treeWash(tree);
    return string;
} /* treeToString */


char* treeConsensusToString(Tree* tree)
{
    size_t strCurSize = 0;
    size_t strMaxSize = default_strSize;
    char* string;
    NodeStack* stack;
    size_t* occStack;
    size_t occStackSize = 0;
    Node* curNode;
    Node* nextNode = NULL;
    int i = 0;
    size_t occurence = 0;
    char needComma = false;
    char* number;

    string = malloc(sizeof(char) * strMaxSize);
    string[strCurSize++] = '('; 
    stack = nodeStackCreate(tree->nodesNum);
    occStack = (size_t*)malloc(sizeof(size_t) * (tree->nodesNum));
    curNode = tree->nodes[0];
    number = (char*)malloc(sizeof(char) * 12);

    treeWash(tree);
    curNode->color = BLACK;
    nodeStackPush(stack, curNode);
    while(stack->curSize > 0)
    {
        curNode = nodeStackPeek(stack);
        nextNode = NULL;
        for (i = 0; i < curNode->neiNum ; ++i)
        {
            if (curNode->neighbours[i]->color == WHITE)
            {
                nextNode = curNode->neighbours[i];
                break;
            }
        }
        if (nextNode)
        {
            nextNode->color = BLACK; 
            if (nextNode->name == 0)
            {
                if (strCurSize + 1 >= strMaxSize)
                {
                    strMaxSize = strMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * strMaxSize);
                }
                if(needComma) string[strCurSize++] = ',';
                string[strCurSize++] = '(';
                nodeStackPush(stack, nextNode);
                if (curNode->dist[i] != 0)//dist from leaf
                {
                    occStack[occStackSize++] = curNode->dist[i]; 
                }
                needComma = false;
            }
            else
            {// leaf
                if (strCurSize + strlen(nextNode->name) + 1 >= strMaxSize)
                {
                    strMaxSize = strMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * strMaxSize);
                }
                if(needComma) string[strCurSize++] = ',';
                strcpy(string + strCurSize, nextNode->name);
                strCurSize = strCurSize + strlen(nextNode->name);
                needComma = true;
            }
        }
        else
        {
            nodeStackPop(stack);
            if (curNode->name == 0)
            {
                if(strCurSize + 10 >= strMaxSize)
                {
                    strMaxSize = strMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * strMaxSize);
                }
                string[strCurSize++] = ')';
                if (occStackSize > 0)
                {
                    string[strCurSize++] = ':';
                    sprintf(number, "%zu", occStack[--occStackSize]);
                    strcpy(string + strCurSize, number);
                    strCurSize += strlen(number);
                }
            }
            else//leaf (only for root it's possible)
            {
                if(strCurSize + strlen(curNode->name) + 1 >= strMaxSize)
                {
                    strMaxSize = strMaxSize * 3 / 2 + 1;
                    string = realloc(string, sizeof(char) * strMaxSize);
                }
                if (needComma) string[strCurSize++] = ',';
                strcpy(string + strCurSize, curNode->name);
                strCurSize = strCurSize + strlen(curNode->name);
            }
            needComma = true;
        }
    }

    if (strCurSize + 3 >= strMaxSize)
    {
        strMaxSize = strMaxSize * 3 / 2 + 1;
        string = realloc(string, sizeof(char) * strMaxSize);
    }
    string[strCurSize++] = ')';
    string[strCurSize++] = ';';
    string[strCurSize++] = '\0';
    string = realloc(string, sizeof(char) * strlen(string));
    treeWash(tree);
    free(number);
    nodeStackDelete(stack);
    free(occStack);
    return string;
}/*treeConsensusToString*/


void treeConsensusWrite(Tree* tree, char* outFileName)
{
    FILE* outFile;
    char* result; 
    int i = 0;
    int j = 0;

    if(strcmp(outFileName, "stdout") == 0)
    {
        outFile = stdout;
    }
    else 
    {
        outFile = fopen(outFileName, "w");
        if(outFile == NULL)
        {
            fprintf(stderr, "Cannot open %s for writing\n", outFileName);
            exit(1);
        }
    }

    result = treeConsensusToString(tree);

    while ( result[i] != '\0') {
        if ( j > 60 && result[i] == ',' ) 
        {
            putc(result[i], outFile);
            putc('\n', outFile);
            j = 0;
        }
        else 
        {
            putc(result[i], outFile);
        }
        i++;
        j++;
    }
    putc('\n', outFile);
    free(result);
    fclose(outFile);
    return;
} /* treeConsensusWrite */

Tree* treeAddLeaf(Tree* tree, unsigned nodeID, unsigned neighbourID, char* leafName,\
        char newTree, char calcLCAFinder)
{
    Tree* result;
    Node* prevNei;
    Node* newNode;
    Node* newLeaf;
    int i;

    if (tree == 0)
    {
        fprintf(stderr, "Error, null tree pointer, Tree:addLeaf\n");
        exit(1);
    }
    if (tree->leavesNum == 0)
    {
        fprintf(stderr, "Error, cant add to empty tree, Tree:addLeaf\n");
        exit(1);
    }
    if (nodeID >= tree->nodesNum)
    {
        fprintf(stderr, "Error, node index is out, Tree:addLeaf\n");
        exit(1);
    }
    if (neighbourID >= tree->nodes[nodeID]->neiNum)
    {
        fprintf(stderr, "Error, neighbour index is out of range, (%d of %d)\
                Tree:addLeaf\n", neighbourID, tree->nodes[nodeID]->neiNum);
        exit(1);
    }

    result = tree;
    if (newTree)
    {
        result = treeCopy(tree, 0);
    }

    prevNei = result->nodes[nodeID]->neighbours[neighbourID];
    newNode = nodeCreate();
    result->nodes[nodeID]->neighbours[neighbourID] = newNode;
    for(i = 0; i < prevNei->neiNum; ++i)
    {
        if (prevNei->neighbours[i] == result->nodes[nodeID])
        {
            prevNei->neighbours[i] = newNode;
            break;
        }
    }
    nodeAddNeighbour(newNode, result->nodes[nodeID], 0);
    nodeAddNeighbour(newNode, prevNei, 0);
    newLeaf = leafCreate(leafName);
    nodeAddNeighbour(newNode, newLeaf, 0);
    nodeAddNeighbour(newLeaf, newNode, 0);

    result->leaves = realloc(result->leaves, sizeof(Node*) * \
            (result->leavesNum + 1));
    result->leaves[result->leavesNum] = newLeaf;
    ++result->leavesNum;
    result->nodesNum += 2;
    result->nodes = realloc(result->nodes, sizeof(Node*) * \
            (result->nodesNum));
    result->nodes[result->nodesNum - 2] = newNode;
    result->nodes[result->nodesNum - 1] = newLeaf;
    newLeaf->pos = result->nodesNum - 1;
    newNode->pos = result->nodesNum - 2;

    if (result->lcaFinder!= 0)
    {
        lcaFinderDelete(result->lcaFinder);
        result->lcaFinder = 0;
    }

    if (calcLCAFinder)
    {
        treeLCAFinderCalculate(result);
    }
    return result;
} /* treeAddLeaf */


Tree* treeRemoveLeaf(Tree* tree, unsigned leafPos, char newTree, char calcLCAFinder)
{
    Tree* result;
    Node* delNode;
    Node* nei1;
    Node* nei2;
    int i, j;
    int pos1, pos2;
    int delNodePos, delLeafPos;

    if (leafPos >= tree->leavesNum)
    {
        fprintf(stderr, "Error, leaf pos is out of range, Tree:removeLeaf\n");
        exit(1);
    }
    i = 0;
    result = tree;
    if (newTree)
    {
        result = treeCopy(tree, 0);
    }
    j = 0;
    if (tree->leavesNum == 1)
    {
        free(tree->nodes);
        result->nodes = 0;
        free(tree->leaves);
        result->leaves = 0;
        --result->leavesNum;
        return tree;
    }

    delNode = result->leaves[leafPos]->neighbours[0];
    if (delNode->neiNum == 1) // good tree:) (A, B);
    {
        delNode->neighbours[0] = 0;
        delNode->neiNum = 0;
        if (result->nodes[result->leaves[leafPos]->pos]->pos == 0)
        {
            result->nodes[0] = result->nodes[1];
        }
        if (leafPos == 0)
        {
            nodeDelete(result->leaves[0]);
            result->leaves[0] = result->leaves[1];
            result->leaves[0]->pos = 0;
        }
        result->nodes = realloc(result->nodes, sizeof(Node*));
        result->leaves = realloc(result->leaves, sizeof(Node*));
        --result->leavesNum;
    }
    else
    {
        nei1 = NULL;
        nei2 = NULL;
        if (delNode->neighbours[0] == result->leaves[leafPos])
        {
            nei1 = delNode->neighbours[1];
            nei2 = delNode->neighbours[2];
        }
        else if (delNode->neighbours[1] == result->leaves[leafPos])
        {
            nei1 = delNode->neighbours[0];
            nei2 = delNode->neighbours[2];
        }
        else
        {
            nei1 = delNode->neighbours[0];
            nei2 = delNode->neighbours[1];
        }

        pos1 = 0;      
        pos2 = 0;
        for(i = 0; i < nei1->neiNum; ++i)
        {
            if (nei1->neighbours[i] == delNode)
            {
                pos1 = i;
                break;
            }
        }
        for(i = 0; i < nei2->neiNum; ++i)
        {
            if (nei2->neighbours[i] == delNode)
            {
                pos2 = i;
                break;
            }
        }
        nei1->neighbours[pos1] = nei2;
        nei2->neighbours[pos2] = nei1;

        delNodePos = delNode->pos;
        delLeafPos = result->leaves[leafPos]->pos;
        nodeDelete(result->leaves[leafPos]);
        nodeDelete(delNode);
        --result->leavesNum;
        result->nodesNum = result->nodesNum - 2; 

        result->nodes[delNodePos] = result->nodes[result->nodesNum - 1];
        result->nodes[delLeafPos] = result->nodes[\
                                                  result->nodesNum];
        result->leaves[leafPos] = result->leaves[result->leavesNum];

        if (leafPos < result->leavesNum)
        {
            result->leaves[leafPos]->pos = leafPos;
        }
        if (delNodePos < result->nodesNum )
        {
            result->nodes[delNodePos]->pos = delNodePos;
        }

        result->leaves = realloc(result->leaves, sizeof(Node*) *\
            result->leavesNum);
        result->nodes = realloc(result->nodes, sizeof(Node*) *\
            (result->nodesNum));
    }

    if (result->lcaFinder != 0)
    {
        lcaFinderDelete(result->lcaFinder);
        result->lcaFinder = 0;
    }
    if (calcLCAFinder)
    {
        treeLCAFinderCalculate(result);
    }
    return result;
} /* treeRemoveLeaf */

void treeWrite(Tree* tree, char* outFileName)
{
    FILE* outFile;
    char* result; 
    int i = 0;
    int j = 0;

    if(strcmp(outFileName, "stdout") == 0)
    {
        outFile = stdout;
    }
    else 
    {
        outFile = fopen(outFileName, "w");
        if(outFile == NULL)
        {
            fprintf(stderr, "Cannot open %s for writing\n", outFileName);
            exit(1);
        }
    }

    result = treeToString(tree);

    while ( result[i] != '\0') {
        if ( j > 60 && result[i] == ',' ) 
        {
            putc(result[i], outFile);
            putc('\n', outFile);
            j = 0;
        }
        else 
        {
            putc(result[i], outFile);
        }
        i++;
        j++;
    }
    putc('\n', outFile);
    free(result);
    fclose(outFile);
    return;
} /* treeWrite */

Tree* treeRead(char* inFileName)
{
    int strTreeSize = 100;
    FILE* inFile = NULL;
    char* line = NULL;
    int strSize = 0;
    char* strNewickTree = NULL;
    Tree* result = NULL;

    inFile = fopen(inFileName, "r");
    if (inFile == NULL)
    {
        fprintf(stderr, "Wrong file name or no access to read file, Tree:treeRead\n");
        exit(1);
    } 
    strSize = strTreeSize;
    strNewickTree = (char*)malloc(sizeof(char) * strSize);
    strNewickTree[0] = '\0';

    while ((line = readLine(inFile)) != NULL)
    {
        if ( (strlen(strNewickTree) + strlen(line)) >= strSize)
        {
            strSize = (strSize + strlen(line)) * 3 / 2 + 1;
            strNewickTree = realloc(strNewickTree, sizeof(char) * strSize);
        }
        memcpy(strNewickTree + strlen(strNewickTree), line, strlen(line) + 1);
        free(line);
    }

    result = treeFromNewick(strNewickTree);
    free(strNewickTree);
    fclose(inFile);
    return result;
} /* treeRead */


LCAFinder* lcaFinderCreate()
{
    LCAFinder* lcaFinder;
    lcaFinder = malloc(sizeof(LCAFinder));
    lcaFinder->sparceTable = 0;
    lcaFinder->vertices = 0;
    lcaFinder->inPos = 0;
    lcaFinder->deep = 0;
    return lcaFinder;
}

void lcaFinderDelete(LCAFinder* lcaFinder)
{
    if (lcaFinder->sparceTable != 0)
    {
        sparseTableDelete(lcaFinder->sparceTable);
    }
    if (lcaFinder->inPos != 0)
    {
        free(lcaFinder->inPos);
    }
    if (lcaFinder->deep != 0)
    {
        free(lcaFinder->deep);
    }
    if (lcaFinder->vertices != 0)
    {
        free(lcaFinder->vertices);
    }
    free(lcaFinder);
    lcaFinder = 0;
    return;
} /* lacaFinderDelete */

void  treeLCAFinderCalculate(Tree* tree)
{
    int i = 0;
    NodeStack* stack;
    Node* curNode;
    Node* nextNode = NULL;
    unsigned curDeep = 0;
    unsigned curPos = 0;
    LCAFinder* lcaFinder;

    if (tree->lcaFinder != 0)
    {
        lcaFinderDelete(tree->lcaFinder);
        tree->lcaFinder = 0;
    }

    treeWash(tree);
    lcaFinder = lcaFinderCreate();
    lcaFinder->vertices = calloc(sizeof(unsigned), tree->nodesNum * 2 - 1);
    lcaFinder->deep = calloc(sizeof(unsigned), tree->nodesNum * 2 - 1);
    lcaFinder->inPos = calloc(sizeof(unsigned), tree->nodesNum);

    stack = nodeStackCreate(tree->nodesNum);

    i = 0;
    while (tree->nodes[i]->name != 0){++i;}
    curNode = tree->nodes[i];
    nextNode = 0;

    nodeStackPush(stack, curNode);
    curNode->color = GREY;
    lcaFinder->inPos[curNode->pos] = curPos;
    
    while (stack->curSize != 0)
    {
        curNode = nodeStackPeek(stack);
        lcaFinder->vertices[curPos] = curNode->pos;
        lcaFinder->deep[curPos] = curDeep;
        ++curPos; 
        nextNode = 0;
        for (i = 0; i < curNode->neiNum; ++i)
        {
            if (curNode->neighbours[i]->color == WHITE)
            {
                nextNode = curNode->neighbours[i];
                break;
            }
        }
        if (nextNode)
        {
            ++curDeep;
            lcaFinder->inPos[nextNode->pos] = curPos;
            nodeStackPush(stack, nextNode);
            nextNode->color = GREY;
        }
        else
        {
            nodeStackPop(stack);
            --curDeep;
        }
    }

    if (curPos != tree->nodesNum * 2 - 1)
    {
        fprintf(stderr, "Error, something have gone wrong, Tree:LCAFinderCalculate\n");
        fprintf(stderr, "nodesNum : %u; curPos : %u\n", tree->nodesNum, curPos);
        fprintf(stderr, "%s\n", treeToString(tree));
        exit(1);
    }
    lcaFinder->sparceTable = sparseTableCalculate(lcaFinder->deep,\
            curPos);
    tree->lcaFinder = lcaFinder;
    treeWash(tree);
    return;
} /*  treeLCAFinderCalculate */ 

unsigned treeFindLCADeep(Tree* tree, unsigned leaf1ID, unsigned leaf2ID)
{
    if (tree->lcaFinder == 0)
    {
        fprintf(stderr, "Error, to use function treeFindLCADeep you must\
                calculate tree's lcaFinder first, Tree:treeFindLCADeep\n");
        exit(1);
    }
    return tree->lcaFinder->deep[sparseTableRMQ(tree->lcaFinder->sparceTable,\
            tree->lcaFinder->inPos[tree->leaves[leaf1ID]->pos],\
            tree->lcaFinder->inPos[tree->leaves[leaf2ID]->pos])];
} /* treeFindLCADeep */

unsigned treeFindLCA(Tree* tree, unsigned node1ID, unsigned node2ID)
{
    if (tree->lcaFinder == 0)
    {
        fprintf(stderr, "Error, to use function treeFindLCA you must\
                calculate tree's lcaFinder first, Tree:treeFindLCA\n");
        exit(1);
    }
    return tree->lcaFinder->vertices[sparseTableRMQ(tree->lcaFinder->sparceTable,\
            tree->lcaFinder->inPos[tree->nodes[node1ID]->pos],\
            tree->lcaFinder->inPos[tree->nodes[node2ID]->pos])];
} /* treeFindLCA */

unsigned treeWhichSplit(Tree* tree,\
        unsigned leaf1, unsigned leaf2,\
        unsigned leaf3, unsigned leaf4)
{
    unsigned deep12;
    unsigned deep13;
    unsigned deep14;
    unsigned deep23;
    unsigned deep24;
    unsigned deep34;
    int result = 2;
    int maxDeep;

    
    if (leaf1 >= tree->leavesNum || leaf2 >= tree->leavesNum ||\
            leaf3 >= tree->leavesNum || leaf4 >= tree->leavesNum)
    {
        fprintf(stderr, "Error, leaf index is out of range, \
                Tree:whichSplit");
        exit(1);
    }

    deep12 = treeFindLCADeep(tree, leaf1, leaf2);
    deep13 = treeFindLCADeep(tree, leaf1, leaf3);
    deep14 = treeFindLCADeep(tree, leaf1, leaf4);
    deep23 = treeFindLCADeep(tree, leaf2, leaf3);
    deep24 = treeFindLCADeep(tree, leaf2, leaf4);
    deep34 = treeFindLCADeep(tree, leaf3, leaf4);

    result = 2;
    maxDeep = deep12; 
    if (deep13 > maxDeep)
    {
        result = 3;
        maxDeep = deep13;
    }
    if (deep14 > maxDeep)
    {
        result = 4;
        maxDeep = deep14;
    }
    if (deep23 > maxDeep)
    {
        result = 4;
        maxDeep = deep23;
    } 
    if (deep24 > maxDeep)
    {
        result = 3;
        maxDeep = deep24;
    }
    if  (deep34 > maxDeep)
    {
        result = 2;
        maxDeep = deep34;
    }
    return result;
} /* whichSplitTree */


Tree* treeNNIMove(Tree* tree, unsigned nodeID, unsigned neiID, char variant,\
        char newTree, char calcLCAFinder)
{
    int i = 0;
    Tree* result;
    Node* end1;
    Node* end2;
    Node* temp;
    char nei11 = 0;
    char nei12 = 0;
    char nei21 = 0;
    char nei22 = 0;
    char nei2ID = 0;


    if (tree->leavesNum == 0)
    {
        fprintf(stderr, "Error, can't apply nni to the empty tree, Tree:nniMove\n");
        exit(1);
    }
    if (nodeID >= tree->nodesNum)
    {
        fprintf(stderr, "Error, node id is out of range, Tree:treeNNIMove");
        exit(1);
    }
    if (neiID >= tree->nodes[nodeID]->neiNum)
    {
        fprintf(stderr, "Error, neighbour id is out of range, Tree:treeNNIMove");
        exit(1);
    }
    if (tree->nodes[nodeID]->neiNum == 1 || tree->nodes[nodeID]\
            ->neighbours[neiID]->neiNum == 1)
    {
        fprintf(stderr, "Error, can't apply nni to branche having leaf as one end\n");
        exit(1);
    }
    if (variant != 2 && variant != 1)
    {
        fprintf(stderr, "Error, variant variable must be 1 or 2\n");
        exit(1);
    }

    result = tree;
    if (newTree)
    {
        result = treeCopy(tree, 0);
    }


    end1 = result->nodes[nodeID];
    end2 = result->nodes[nodeID]->neighbours[neiID];
    switch (neiID)
    {
        case 0:
            nei11 = 1;
            nei12 = 2;
            break;
        case 1:
            nei11 = 0;
            nei12 = 2;
            break;
        case 2:
            nei11 = 0;
            nei12 = 1;
            break;
        default:
            fprintf(stderr, "Error, some wrong with neiID\n");
            exit(1);
    }

    nei21 = 0; 
    nei22 = 0;
    nei2ID = 0;
    if (end2->neighbours[0] == end1)
    {
        nei2ID = 0;
        nei21 = 1;
        nei22 = 2;
    }
    else if(end2->neighbours[1] == end1)
    {
        nei2ID = 1;
        nei21 = 0;
        nei22 = 2;
    }
    else if(end2->neighbours[2] == end1)
    {
    
        nei2ID = 2;
        nei21 = 0;
        nei22 = 1;
    }

    if (variant == 1)
    {

        for(i = 0; i < end1->neighbours[nei11]->neiNum; ++i)
        {
            if (end1->neighbours[nei11]->neighbours[i] == end1)
            {
                end1->neighbours[nei11]->neighbours[i] = end2;
                break;
            }
        }
        for(i = 0; i < end2->neighbours[nei21]->neiNum; ++i)
        {
            if (end2->neighbours[nei21]->neighbours[i] == end2)
            {
                end2->neighbours[nei21]->neighbours[i] = end1;
                break;
            }
        }
        temp = end1->neighbours[nei11];
        end1->neighbours[nei11] = end2->neighbours[nei21];
        end2->neighbours[nei21] = temp;
    }
    else
    {
        for(i = 0; i < end1->neighbours[nei11]->neiNum; ++i)
        {
            if (end1->neighbours[nei11]->neighbours[i] == end1)
            {
                end1->neighbours[nei11]->neighbours[i] = end2;
                break;
            }
        }
        for(i = 0; i < end2->neighbours[nei22]->neiNum; ++i)
        {
            if (end2->neighbours[nei22]->neighbours[i] == end2)
            {
                end2->neighbours[nei22]->neighbours[i] = end1;
                break;
            }
        }
        temp = end1->neighbours[nei11];
        end1->neighbours[nei11] = end2->neighbours[nei22];
        end2->neighbours[nei22] = temp;
    }

    if (result->lcaFinder != 0)
    {
        lcaFinderDelete(result->lcaFinder);
        result->lcaFinder = 0;
    }
    if (calcLCAFinder)
    {
        treeLCAFinderCalculate(result);
    }
    return result;
} /*  treeNNIMove */
 
char** treeGetNames(Tree* tree)
{
    int i;
    char** namesArr;
    namesArr = calloc(sizeof(char*), tree->leavesNum);
    for(i = 0; i < tree->leavesNum; ++i)
    {
        namesArr[i] = tree->leaves[i]->name;
    }
    return namesArr;
} /* treeGetnames */

Tree* treeSPRMove(Tree* tree, unsigned sourceNodeID, unsigned sourceNeiPos,\
              unsigned destNodeID, unsigned destNeiID,\
              unsigned* newBranchNodeID, unsigned* newBranchNeiID,\
              char newTree, char calcLCAFinder)
{
    int i;
    Tree* result;
    unsigned sourceChild;
    unsigned sourceParent;
    unsigned nei1Pos, nei2Pos = 0;
    unsigned upID, bottomID;
    unsigned newNei1ID, newNei2ID;


    if (tree->leavesNum == 0)
    {
        fprintf(stderr, "Error, cant apply SPR to empty tree, Tree:treeSPRMove\n");
        exit(1);
    }
    if (sourceNodeID >= tree->nodesNum ||\
            destNodeID >= tree->nodesNum)
    {
        fprintf(stderr, "Error, node id is out of range, Tree:treeSPRMove\n");
        exit(1);
    }
    if (sourceNeiPos >= tree->nodes[sourceNodeID]->neiNum ||\
            destNeiID >= tree->nodes[destNodeID]->neiNum)
    {
        fprintf(stderr, "Error, neighbour id is out of range, Tree:treeSPRMove\n");
        exit(1);
    }


    if (sourceNodeID == destNodeID && destNeiID == sourceNeiPos)
    {
        fprintf(stderr, "Error, source and nei branch is the same one,\
                Tree:treeSPRMove\n");
        exit(1);
    }
    if (sourceNodeID == destNodeID || \
            sourceNodeID == tree->nodes[destNodeID]->neighbours[destNeiID]->pos\
            || tree->nodes[sourceNodeID]->neighbours[sourceNeiPos]->pos == destNodeID\
            || tree->nodes[sourceNodeID]->neighbours[sourceNeiPos]->pos ==\
            tree->nodes[destNodeID]->neighbours[destNeiID]->pos)
    {
        fprintf(stderr, "source branch is neihghbour of dest, Tree:treeSPRMove\n");
        exit(1);
    }

    result = tree;
    if (newTree)
    {
        result = treeCopy(tree, 0);
    }
    if (tree->lcaFinder == 0)
    {
        treeLCAFinderCalculate(tree);
    }

    sourceChild = tree->nodes[sourceNodeID]->neighbours[sourceNeiPos]->pos;
    sourceParent = sourceNodeID;

    if (tree->lcaFinder->inPos[sourceChild] < tree->lcaFinder->inPos[sourceParent])
    {
        sourceParent = sourceChild;
        sourceChild = sourceNodeID;
    }

    nei1Pos = 0;
    nei2Pos = 0;
    upID = 0;
    bottomID = 0;
    // nei1-up-nei2               nei1 - nei2
    //      |          ->             + 
    //    bottom                       up
    //    |    |                        |
    //                                bottom
    //                                |    |


    if (treeFindLCA(tree, sourceChild, destNodeID) == sourceChild)
    {
        upID = sourceChild;
        bottomID = sourceParent;
    }
    else
    {
        upID = sourceParent;
        bottomID = sourceChild;
    }

    
    if (result->nodes[upID]->neighbours[0]->pos == bottomID)
    {
        nei1Pos = 1;
        nei2Pos = 2;
    }
    else if (result->nodes[upID]->neighbours[1]->pos == bottomID)
    {
        nei1Pos = 0;
        nei2Pos = 2;
    }
    else
    {
        nei1Pos = 0;
        nei2Pos = 1;
    }

    for(i = 0; i < result->nodes[upID]->neighbours[nei1Pos]->neiNum; ++i)
    {
        if (result->nodes[upID]->neighbours[nei1Pos]->neighbours[i]->pos == upID)
        {

            if (newBranchNodeID != 0)
            {
                *newBranchNodeID = result->nodes[upID]->neighbours[nei1Pos]->pos;
                *newBranchNeiID = i;
            }
            result->nodes[upID]->neighbours[nei1Pos]->neighbours[i] = \
            result->nodes[upID]->neighbours[nei2Pos];
            break;
        } 
    }


    for(i = 0; i < result->nodes[upID]->neighbours[nei2Pos]->neiNum; ++i)
    {
        if (result->nodes[upID]->neighbours[nei2Pos]->neighbours[i]->pos == upID)
        {
            result->nodes[upID]->neighbours[nei2Pos]->neighbours[i] = \
                result->nodes[upID]->neighbours[nei1Pos];
            break;
        } 
    } 

    //result->nodes[upID]->neighbours[nei1Pos] = 0;
    //result->nodes[upID]->neighbours[nei2Pos] = 0;

    newNei1ID = destNodeID;
    newNei2ID = result->nodes[destNodeID]->neighbours[destNeiID]->pos;

    result->nodes[upID]->neighbours[nei1Pos] = result->nodes[newNei1ID];
    result->nodes[upID]->neighbours[nei2Pos] = result->nodes[newNei2ID];

    for(i = 0; i < result->nodes[newNei1ID]->neiNum; ++i)
    {
        if (result->nodes[newNei1ID]->neighbours[i]->pos == newNei2ID)
        {
            result->nodes[newNei1ID]->neighbours[i] = result->nodes[upID];
            break;
        }
    }

    for(i = 0; i < result->nodes[newNei2ID]->neiNum; ++i)
    {
        if (result->nodes[newNei2ID]->neighbours[i]->pos == newNei1ID)
        {
            result->nodes[newNei2ID]->neighbours[i] = result->nodes[upID];
            break;
        }
    }

    if (result->lcaFinder != 0)
    {
        lcaFinderDelete(result->lcaFinder);
        result->lcaFinder = 0;
    }
    if (calcLCAFinder)
    {
        treeLCAFinderCalculate(result);
    }

    return result;
} /* treeSPRmove */

unsigned treeGetDist(Tree* tree, unsigned node1ID, unsigned node2ID)
{
    unsigned node1Deep,  node2Deep, lcaDeep;
    if (node1ID >= tree->leavesNum || node2ID >= tree->leavesNum)
    {
        fprintf(stderr, "Error, leaf ID is out of range");
        exit(1);
    }
    if (tree->lcaFinder == 0)
    {
        fprintf(stderr, "Error, to use treeGetDist you must calculate LCAFinder first\n");
        exit(1);
    }

    node1Deep = tree->lcaFinder->deep[tree->lcaFinder->inPos[tree->nodes[node1ID]->pos]];
    node2Deep = tree->lcaFinder->deep[tree->lcaFinder->inPos[tree->nodes[node1ID]->pos]];
    lcaDeep = tree->lcaFinder->deep[sparseTableRMQ(tree->lcaFinder->sparceTable,\
            tree->lcaFinder->inPos[tree->nodes[node1ID]->pos],\
            tree->lcaFinder->inPos[tree->nodes[node2ID]->pos])];

    return node1Deep - lcaDeep + node2Deep - lcaDeep;
} /* treeGetDist */


Tree* treePrune(Tree* source, char** leavesNames, size_t leavesNum,
        char calculateLcaFinder)
{
    Tree* result;
    int i = 0;
    int j = 0;
    int isFound = 0;
    int* takeInTree;
    size_t foundNum = 0;
    NodeStack* stack;
    NodeStack* pruneStack;
    Node* curNode = NULL;
    Node* nextNode = NULL;
    Node** neiResult = NULL;
    Node* newNode = NULL; 
    Node* nei1 = NULL;
    Node* nei2 = NULL;
    size_t rootPos;

    treeWash(source);
    result = treeCreate();
    result->nodesNum = 0;
    result->leavesNum = 0;
    result->leaves = (Node**)calloc(sizeof(Node*), leavesNum);
    result->nodes = (Node**)calloc(sizeof(Node*), leavesNum * 2 - 1);
    takeInTree = (int*)calloc(source->nodesNum, sizeof(int));

    for(i = 0; i < leavesNum; ++i)
    {
        isFound = 0;
        for(j = 0; (j < source->leavesNum) && (!isFound); ++j)
        {
            if (strcmp(leavesNames[i], source->leaves[j]->name) == 0)
            {
                isFound = 1;
                ++foundNum;
                takeInTree[source->leaves[j]->pos] = 1;
            }
        }
        if (!isFound)
        {
            fprintf(stderr, "%s\n", leavesNames[i]);
            fprintf(stderr, "No such leaf in source tree\n");
            exit(1);
        }
    }

    stack = nodeStackCreate(source->nodesNum);
    pruneStack = nodeStackCreate(source->nodesNum);
    rootPos = 0;
    while(source->nodes[rootPos]->neiNum == 1)
    {
        ++rootPos;
        if (rootPos >= source->nodesNum) 
        {  
            perror("Wrong tree structure\n");
            exit(1);
        }
    }

    nodeStackPush(stack, source->nodes[rootPos]);
    source->nodes[rootPos]->color = GREY;

    while(stack->curSize > 0)
    {
        curNode = nodeStackPeek(stack);
        nextNode = NULL;
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
            nextNode->color = GREY;
            nodeStackPush(stack, nextNode);
        }
        else // all childs are processed
        {
            curNode->color = BLACK;
            nodeStackPop(stack);

            if (curNode->neiNum == 1)
            {
               assert(curNode->name != NULL);
               if(takeInTree[curNode->pos])
               {
                   newNode = leafCreate(curNode->name);
                   result->nodes[result->nodesNum] = newNode;
                   newNode->pos = result->nodesNum++;
                   result->leaves[result->leavesNum++] = newNode;
                   nodeStackPush(pruneStack, newNode);
                   
               }
               else
               {
                   nodeStackPush(pruneStack, NULL);
               }
            }
            else
            {
                size_t resNum = 0;
                neiResult = malloc(sizeof(Node*) * curNode->neiNum);
            
                for(j = 0; j < curNode->neiNum; ++j)
                {
                    if (curNode->neighbours[j]->color == BLACK)
                    {
                        neiResult[resNum] = nodeStackPeek(pruneStack);
                        nodeStackPop(pruneStack);
                        if (neiResult[resNum] != NULL) ++resNum;
                    }
                }

                if (resNum == 0)
                {
                    nodeStackPush(pruneStack, NULL);
                }
                else if (resNum == 1 && curNode->pos == rootPos &&
                        neiResult[0]->neiNum == 2)
                {
                        nei1 = neiResult[0]->neighbours[0];
                        nei2 = neiResult[0]->neighbours[1];
                        nei1->neighbours[nei1->neiNum - 1] = nei2;
                        nei2->neighbours[nei2->neiNum - 1] = nei1;
                        nodeDelete(neiResult[0]);
                        --result->nodesNum;
                        nodeStackPush(pruneStack, nei1);
                }
                else if (resNum == 1)
                {
                    nodeStackPush(pruneStack, neiResult[0]);
                }
                else if (resNum == 2 && curNode->pos == rootPos)
                {
                    nodeAddNeighbour(neiResult[0], neiResult[1], 0);
                    nodeAddNeighbour(neiResult[1], neiResult[0], 0);
                    nodeStackPush(pruneStack, neiResult[0]);
                }
                else
                {
                    newNode = nodeCreate();
                    result->nodes[result->nodesNum] = newNode;
                    newNode->pos = result->nodesNum++;

                    for(j = 0; j < resNum; ++j)
                    {
                        nodeAddNeighbour(neiResult[j], newNode, 0);
                        nodeAddNeighbour(newNode, neiResult[j], 0);
                    }
                    nodeStackPush(pruneStack, newNode);
                }
                free(neiResult);
            }

        }
        
    }

    result->nodes = realloc(result->nodes, sizeof(Node*) * (result->leavesNum * 2 - 2));
    if ((pruneStack->curSize != 1) || (result->leavesNum != leavesNum) ||\
            (result->leavesNum * 2 - 2 != result->nodesNum))
    {
        fprintf(stderr, "pruneStackSize : %d\n", pruneStack->curSize);
        fprintf(stderr, "result Leaves Num %d need %d\n", result->leavesNum, leavesNum);
        fprintf(stderr, "Something've gone wrong\n");
        exit(1);
    }
    nodeStackPop(pruneStack);


    if (calculateLcaFinder)
    {
        treeLCAFinderCalculate(result);
    }

    nodeStackDelete(stack);
    nodeStackDelete(pruneStack);
    free(takeInTree);
    treeWash(source);
    
    return result;
}/* treePrune */


/* test 
int main()
{
    int i = 0, j = 0, k = 0;
    unsigned neiPos = 0;
    unsigned revertNodeID = 0;
    unsigned revertNeiID = 0;

    //char* newick = "((rec, (rec1, rec2)), ((dim, aunt) ,(aim,(bimmm, uiuu))));";
    char* newick = "(rec, (aunt,(aim,(bimmm, uiuu))));";
    
    Tree* tree = treeFromNewick(newick);

    printf("inPos\n");
    for(i = 0; i < tree->nodesNum; ++i)
    {
        printf("%d ", tree->lcaFinder->inPos[i]);
    }
    printf("\n");
    printf("Vertices\n");
    for(i = 0; i < tree->lcaFinder->sparceTable->length; ++i)
    {
        printf("%d ", tree->lcaFinder->vertices[i]);
    }
    printf("\n");
    printf("Deeps\n");
    for(i = 0; i < tree->lcaFinder->sparceTable->length; ++i)
    {
        printf("%d ", tree->lcaFinder->deep[i]);
    }
    printf("\n");
    printf("Sparse Table\n");

    int length = tree->lcaFinder->sparceTable->length;
    for(i = 0; i < tree->lcaFinder->sparceTable->height; ++i)
    {
        for(j = 0; j < length; ++j)
        {
            printf("%d ", tree->lcaFinder->sparceTable->table[i][j]);
        }
        printf("\n");
        length -= (1 << i);
    }
    


    printf("LCA is %u\n", treeFindLCADeep(tree, 2, 4));
    printf("Split is %u\n", treeWhichSplit(tree, 2, 4, 1, 3));
    printf("%u\n", tree->leavesNum);
    printf("%s\n", tree->leaves[4]->name);
    Tree* newTree = treeCopy(tree, 0);
    printf("%u\n", newTree->leavesNum);
    printf("%s\n", newTree->leaves[4]->name);
    char* newickNew = treeToString(tree);
    printf("%s\n", newickNew);
    free(newickNew);
    newickNew = treeToString(newTree);
    printf("%s\n", newickNew);
    free(newickNew);

    Tree* newTreeAdd = treeAddLeaf(tree, 1, 1, "urrr", 1, 0);
    Tree* newTreeRem = treeRemoveLeaf(tree, 3, 0, 0);
    newTreeRem = treeRemoveLeaf(newTreeRem, 0, 0, 0);
    newTreeRem = treeRemoveLeaf(newTreeRem, 0, 0, 0);
    newickNew = treeToString(newTreeRem);
    printf("%s\n", newickNew);
    free(newickNew);
    newTreeRem = treeRemoveLeaf(newTreeRem, 0, 0, 0);
    newTreeRem = treeRemoveLeaf(newTreeRem, 0, 0, 0);
    newickNew = treeToString(newTreeAdd);
    printf("%s\n", newickNew);
    free(newickNew);
    newickNew = treeToString(newTreeRem);
    printf("%s\n", newickNew);
    free(newickNew);

    newickNew = treeToString(newTree);
    printf("%s\n", newickNew);
    free(newickNew);
    Tree* new2Tree = treeSPRMove(newTree, 0, neiPos, 3, 2, &revertNodeID,\
            &revertNeiID, 0, 0);
    newickNew = treeToString(new2Tree);
    printf("%s\n", newickNew);
    free(newickNew);
    new2Tree = treeSPRMove(newTree, 0, neiPos, revertNodeID, revertNeiID,\
            &revertNodeID, &revertNeiID, 0, 0);
    newickNew = treeToString(new2Tree);
    printf("%s\n", newickNew);
    free(newickNew);

    unsigned newBranch1NodeID = 0;
    unsigned newBranch1NeiPos = 0;
    unsigned newBracnh2NodeID = 0;
    unsigned newBracnh2NeiPos = 0;

    new2Tree = treeTBRMove(newTree, 1, 2, 0, 0, 5, 0,\
            &newBranch1NodeID, &newBranch1NeiPos,\
            &newBracnh2NodeID, &newBracnh2NeiPos,
            0, 0);
    newickNew = treeToString(new2Tree);
    printf("TBR %s\n", newickNew);
    free(newickNew);

    printf("%d %d %d %d\n",\
        newBranch1NodeID, newBranch1NeiPos,
        newBracnh2NodeID, newBracnh2NeiPos);
    new2Tree = treeTBRMove(newTree, 1, 2, \
            newBranch1NodeID, newBranch1NeiPos,
            newBracnh2NodeID, newBracnh2NeiPos,\
            0, 0, 0, 0, 0, 0);
    newickNew = treeToString(new2Tree);
    printf("TBR %s\n", newickNew);
    free(newickNew);

    treeDelete(tree);
    treeDelete(newTree);
    return 0;
}
*/
