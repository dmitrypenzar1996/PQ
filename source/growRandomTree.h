#ifndef __GROW_RANDOM_TREE__
#define __GROW_RANDOM_TREE__
#include "Tree.h"
#include "growTree.h"
#include <stdio.h>
#include <stdlib.h>

unsigned int randInterval(unsigned int min, unsigned int max);
Tree* growRandomTree(char** leavesarr, int arrlen);
#endif


