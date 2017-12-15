#include "genitor.h"
//#define MAX_GA_ITER 18
//#define MAX_LEADER_ITER 10001

unsigned treeIsUnique(TreeWithScore* intree, unsigned pos, TreeWithScore** trees, unsigned treeNum)
{
	int i;
	unsigned unique = TRUE;
	Tree* intersection;

	for (i = 0; i < treeNum; i++)
	{
		if (i != pos)
		{
			if (trees[i]->score == intree->score)
			{
				intersection = UMAST(trees[i]->tree, intree->tree);

				if (intersection->leavesNum == intree->tree->leavesNum)
				{
					unique = FALSE;
				}

				treeDelete(intersection);
			}
		}
	}
	return unique;
}

TreeWithScore* crossover(TreeWithScore* tree1, TreeWithScore* tree2,\
			HashAlignment* alignment, int alpha,\
			 GapOpt gapOpt, PWM* pwmMatrix, INT**** hashScore)
{
	int* indexes;
	int* permutation;
	unsigned i, j;
	TreeWithScore* result;
	char** seqNames;
	char** treeNames;
	char** leavesToGrow;

	indexes = (int*)calloc(sizeof(int), tree1->tree->leavesNum);

	for (i = 0; i < tree1->tree->leavesNum; i++)
	{
		indexes[i] = 0;
	}

	result = treeWithScoreCreate(UMAST(tree1->tree, tree2->tree), 0);
	leavesToGrow = (char**)calloc(sizeof(char*), tree1->tree->leavesNum);

	for (i = 0; i < tree1->tree->leavesNum; i++)
	{
		for (j = 0; j < result->tree->leavesNum; j++)
		{
			if (strcmp(tree1->tree->leaves[i]->name, result->tree->leaves[j]->name) == 0)
			{
				indexes[i] = 1;
				break;
			}
		}
	}

	for (i = 0; i < result->tree->leavesNum; i++)
	{
		leavesToGrow[i] = result->tree->leaves[i]->name;
	}

	i = result->tree->leavesNum;

	for(j = 0; j < tree1->tree->leavesNum; j++)
	{
		if (i > tree1->tree->leavesNum) 
		{
			fprintf(stderr, "failed to copy leaves, genitor:crossover\n");
			exit(1);
		}
		if (indexes[j] == 0) 
		{
			leavesToGrow[i] = tree1->tree->leaves[j]->name;
			i++;
		}
	}

	seqNames = hashAlignmentGetSeqNames(alignment);
	permutation = calculatePermutation(leavesToGrow, seqNames, alignment->alignmentSize); //leavesNum == alignmensSise?

	for (i = result->tree->leavesNum; i < tree1->tree->leavesNum; i++)
	{
		result = getBestChild(alignment, result, leavesToGrow[i], alpha, gapOpt,\
        					pwmMatrix, hashScore, permutation);
	}

	treeLCAFinderCalculate(result->tree);
	result->score = 0; //maybe not needed
	result->score = countScoreHash(alignment, result->tree, pwmMatrix, alpha, gapOpt,\
								hashScore, permutation);
	free(indexes); 
	free(permutation); 
	free(seqNames); 
	free(treeNames); 
	free(leavesToGrow);
	return result;
}

TreeWithScore* genitor(TreeWithScore** trees, unsigned treeNum, HashAlignment* alignment,\
					int alpha, GapOpt gapOpt, PWM* pwmMatrix, INT**** hashScore, unsigned iterNum, unsigned iterLim)
{
	INT leaderScore;
	int* permutation;
	char** seqNames;
	char** treeNames;
	unsigned i, j;
	unsigned t = 0;
	unsigned failure = 0;
	unsigned leaderIter = 0;
	unsigned* uniqueSet;
	TreeWithScore* offspring;
	TreeWithScore** population;
	//printf("hiiii1!\n");
	offspring = (TreeWithScore*)malloc(sizeof(TreeWithScore));
	population = (TreeWithScore**)malloc(sizeof(TreeWithScore*) * treeNum);
	for (i = 0;  i < treeNum; i++)
	{
		population[i] = treeWithScoreCreate(treeCopy(trees[i]->tree, 1), trees[i]->score);
	}
	treeWithScoreSort(population, treeNum);
	leaderScore = population[treeNum - 1]->score;
	seqNames = hashAlignmentGetSeqNames(alignment);
	srand(time(NULL));

	uniqueSet = (unsigned*)calloc(sizeof(unsigned), treeNum);
	for (i = 0; i < treeNum; i++)
	{
		uniqueSet[i] = treeIsUnique(population[i], i, population, treeNum);
		printf("%d\t", uniqueSet[i]);
	}
	printf("\n");
	//printf("hiiii2!\n");
	while (t < iterNum)
	{
		i = rand() % treeNum;
		j = rand() % treeNum;
		while (i == j) 
		{
			j = rand() % treeNum;
		}

		offspring = crossover(population[i], population[j], alignment, alpha,\
							gapOpt, pwmMatrix, hashScore);

		treeNames = treeGetNames(offspring->tree);
        permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);
		offspring->score = countScoreHash(alignment, offspring->tree, pwmMatrix, alpha, gapOpt,\
								hashScore, permutation);

		printf("Iter: %d, Score: %ld\n", t, offspring->score);

		printf("offspring is unique: %d\n", treeIsUnique(offspring, -1, population, treeNum));
		if ((offspring->score > population[0]->score) && (treeIsUnique(offspring, -1, population, treeNum)))
		{
			failure = 0;
			population[0] = offspring;
			treeWithScoreSort(population, treeNum);
		}
		else 
		{
			failure++;
		}

		if (failure == iterLim)
		{
			printf("failures exceed iterLim count\n");
			return population[treeNum - 1];
		}

		if (population[treeNum - 1]->score != leaderScore)
		{
			leaderScore = population[treeNum - 1]->score;
			leaderIter = 0;
		}
		else
		{
			leaderIter++;
		}

		if (leaderIter == iterLim) 
		{
			printf("leader exceeds iterLim count\n");
			return population[treeNum - 1];
		}

		t++;
	}

	free(uniqueSet);
	printf("genitor finished, returning leader\n");
	return population[treeNum - 1];
}