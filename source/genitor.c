#include "genitor.h"
//#define MAX_GA_ITER 18
//#define MAX_LEADER_ITER 10001

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
	unsigned leaderIter = 0;
	TreeWithScore* offspring;
	TreeWithScore** population;
	printf("hiiii1!\n");
	offspring = (TreeWithScore*)malloc(sizeof(TreeWithScore));
	population = (TreeWithScore**)malloc(sizeof(TreeWithScore*) * treeNum);
	for (i = 0;  i < treeNum; i++)
	{
		population[i] = treeWithScoreCreate(treeCopy(trees[i]->tree, 1), trees[i]->score);
	}
	//printf("that cant be true\n");
	treeWithScoreSort(population, treeNum);
	leaderScore = population[treeNum - 1]->score;
	seqNames = hashAlignmentGetSeqNames(alignment);
	srand(time(NULL));
	//printf("here?\n");
	
	printf("hiiii2!\n");
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
		//offspring = treeWithScoreCreate(population[i]->tree, population[i]->score);


		treeNames = treeGetNames(offspring->tree);
        permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);
		offspring->score = countScoreHash(alignment, offspring->tree, pwmMatrix, alpha, gapOpt,\
								hashScore, permutation);
		printf("Iter: %d, Score: %ld\n", t, offspring->score);
		if (offspring->score > population[0]->score)
		{
			population[0] = offspring;
			treeWithScoreSort(population, treeNum);
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
			printf("leader exceeds population count\n");
			return population[treeNum - 1];
		}

		t++;
	}
	printf("genitor finished, returning leader\n");
	return population[treeNum - 1];
}