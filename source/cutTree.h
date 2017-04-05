#ifndef __CUT_TREE__
#define __CUT_TREE__
#include "Tree.h"

#include <stdlib.h>

Tree** cutTree(Tree* tree, int nodeId, int neiId);
void stackCycle(Node* node, NodeStack* stack);
void treeUndegnerate(Node* node);
void treeProcess(Tree* tree);
#endif
