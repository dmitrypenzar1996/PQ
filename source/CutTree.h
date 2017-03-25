#ifndef __CUT_TREE__
#define __CUT_TREE__
#include "Tree.h"
#include "TreeWS.h"
#include <stdlib.h>

Tree** CutTree(Tree* tree, int node1, int node2);
void StackCycle(Node* node, NodeStack* stack);
void treeUndegnerate(Node* node);
void treeProcess(Tree* tree);
#endif
