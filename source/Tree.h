#ifndef _TREE_H_
#define _TREE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "add.h"
#include "RMQ.h"


typedef enum
{
    WHITE, GREY, BLACK
}Color;

typedef struct Node
{
    char* name; // null if internal
    struct Node** neighbours;
    double* dist;
    Color color;
    char neiNum;
    char maxNeiNum;
    unsigned pos;
}Node;

typedef struct
{
    unsigned* deep; 
    unsigned* vertices;
    unsigned* inPos; 
    SparseTable* sparceTable;
}LCAFinder;

typedef struct Tree
{
    Node** nodes;
    Node** leaves;
    LCAFinder* lcaFinder;
    unsigned leavesNum;
    unsigned nodesNum;
    ssize_t rootId; //-1 if unrooted, root->pos if rooted
}Tree;

typedef struct
{
    Node** nodes;
    unsigned maxSize;
    unsigned curSize;
}NodeStack;

NodeStack* nodeStackCreate(unsigned maxSize);
void nodeStackDelete(NodeStack* stack);
void nodeStackPush(NodeStack* stack, Node* node);
Node* nodeStackPeek(NodeStack* stack);
void nodeStackPop(NodeStack* stack);
Node* nodeCreate(void);
Node* leafCreate(char* name);
void nodeDelete(Node* node);
void nodeAddNeighbour(Node* node, Node* neighbour, double dist);
Tree* treeCreate(void);
void treeDelete(Tree* tree);
double readLength(char* string, unsigned* pos);
char* readName(char* string, unsigned* pos);
Tree* treeCopy(Tree* source, char copyLCAFinder);
Tree* treeFromNewick(char* newick);
char* treeToString(Tree* tree);
void treeWash(Tree* tree);
Tree* treeAddLeaf(Tree* tree, unsigned nodeID, unsigned neighbourID, char* leafName,
                  char newTree, char calcLCAFinder);
Tree* treeRemoveLeaf(Tree* tree, unsigned leafPos, char newTree, char calcLCAFinder);

unsigned treeWhichSplit(Tree* tree, unsigned leaf1, unsigned leaf2, unsigned leaf3, unsigned leaf4);
void treeWrite(Tree* tree, char* outFileName);
Tree* treeRead(char* inFileName);

void  treeLCAFinderCalculate(Tree* tree);
LCAFinder* lcaFinderCreate(void);
void lcaFinderDelete(LCAFinder* lcaFinder);

Tree* treeNNIMove(Tree* tree, unsigned nodeID, unsigned neiID, char variant,
                   char newTree, char calcLCAFinder);

char** treeGetNames(Tree* tree);

Tree* treeSPRMove(Tree* tree, unsigned sourceNodeID, unsigned sourceNeiID,\
              unsigned destNodeID, unsigned destNeiID,\
              unsigned* newBranchNodeID, unsigned* newBranchNeiID,\
              char newTree, char calcLCAFinder);

unsigned treeFindLCA(Tree* tree, unsigned node1ID, unsigned node2ID);
unsigned treeGetDist(Tree* tree, unsigned node1ID, unsigned node2ID);
char* treeConsensusToString(Tree* tree);
void treeConsensusWrite(Tree* tree, char* outFileName);
Tree* treePrune(Tree* source, char** leavesNames, size_t leavesNum,
        char calculateLcaFinder);
#endif
