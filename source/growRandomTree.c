#include "growRandomTree.h"
#include "Tree.h"
#include <stdio.h>
#include <string.h>

unsigned int randInterval(unsigned int min, unsigned int max);
Tree* growRandomTree(char** leavesarr, int arrlen)
{
	Tree* result;
	Node* curnode;
	int i, k, j;
	int* permutation = getPermutation(arrlen);
	if (arrlen < 4)
	{
		printf("Too few leaves, try again\n");
		exit(1);
	}
	else //arrlen = 4 or more
	{
		result = growThreeLeavesTree(leavesarr[permutation[0]], leavesarr[permutation[1]], leavesarr[permutation[2]]);
		result->nodes = (Node**)realloc(result->nodes, sizeof(Node*) * (2*arrlen - 2));
		result->leaves = (Node**)realloc(result->leaves, sizeof(Node*) * arrlen);
                for(k = 3; k < arrlen; ++k)
		{
		        size_t randomBranchIndex = randInterval(0, result->leavesNum*2 - 3);/*init*/
		        size_t curBranchIndex = 0;
			for (i = 0; i < result->nodesNum; i++)
			{
				curnode = result->nodes[i];
				for (j = 0; j < curnode->neiNum; ++j)
				{
					if (curnode->neighbours[j]->pos <= i)
		    			{
		        			continue;
		    			}
		                        if (curBranchIndex == randomBranchIndex)
		                        {
					    result = treeAddLeaf(result, i, j, leavesarr[permutation[k]], 0, 1);
		                        }
		                        ++curBranchIndex;
				}			
			}
		}
		return result;
	}
}


unsigned int randInterval(unsigned int min, unsigned int max)
{
        unsigned int j;
	j = (rand() % (max-min+1)) + min;
	return j;
}

int main(unsigned argc, char** argv)
{
	char *c;
	FILE *infile;
	int linescount = 0;
	char buffer[200];
	printf("growRandomTree execution started\n");
	if (argc < 2)
	{
		perror("No input file, please try again.\n");
		exit(1);
	}
	infile = fopen(argv[1], "r");
	if (infile == NULL)
	{
		perror("Invalid input file, please try again.\n");
	}	
	while (!feof(infile))
	{
		if (fgets(buffer, 200, infile))
			linescount++;
	}
	fclose(infile);
	infile = fopen(argv[1], "r");

        char** leavesArr = malloc(sizeof(char*) * linescount);
        char* curLeafName;
int i = 0;
        while ( curLeafName = readLine(infile))
        {
            leavesArr[i++] = curLeafName;
		printf("%s\n", leavesArr[i-1]);
        }
	Tree* result = growRandomTree(leavesArr, linescount);
	printf("%s\n", treeToString(result));
	fclose(infile);
	return 0;
	printf("growRandomTree execution ended\n");
}





