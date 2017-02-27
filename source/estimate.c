#include "estimate.h"

// TreeWithScore: return inTree copy at first position and its nniNeighbours 
TreeWithScore** getNNINeighbours(Tree* inTree, HashAlignment* alignment,
        PWM* pwmMatrix, int alpha, GapOpt gapOpt, INT**** hashScore)
{   
    int i, j, variant;
    char** treeNames;
    char** seqNames;
    int* permutation;
    INT score;
    TreeWithScore** neighbours;
    Tree* curTree;
    int curPos;

    treeNames = treeGetNames(inTree);
    seqNames = hashAlignmentGetSeqNames(alignment);
    permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);
    free(treeNames);
    free(seqNames);

    inTree = treeCopy(inTree, 1);

    neighbours = (TreeWithScore**)calloc(sizeof(TreeWithScore*), (inTree->leavesNum - 3) * 2);

    curPos = 0;
    for(i = 0; i < inTree->leavesNum * 2 - 2; ++i)
    {
        if (inTree->nodes[i]->neiNum == 3)
        {
            for(j = 0; j < 3; ++j)
            {
                if (inTree->nodes[i]->neighbours[j]->pos > i && \
                    inTree->nodes[i]->neighbours[j]->neiNum == 3)
                {    
                    for(variant = 1; variant < 3; ++variant)
                    {
                        curTree = treeNNIMove(inTree, i, j, variant, 1, 1);
                        score = countScoreHash(alignment, curTree, pwmMatrix,\
                                alpha, gapOpt, hashScore, permutation);
                        neighbours[curPos] = treeWithScoreCreate(curTree, score);
                        ++curPos;
                    }
                }
            } /* for j */
        }
    } /* for i */

    return neighbours;
} /*  getNNINeighbours */

TreeWithScore* randomTreeGrow(HashAlignment* alignment, int alpha, GapOpt gapOpt,\
        PWM* pwmMatrix,\
        INT**** hashScore)
{
    static int init = 0;
    int i, j, k, h, m;

    char** names;
    int* permutation;
    Tree* tree;
    TreeWithScore* result;

    names = hashAlignmentGetSeqNames(alignment);
    permutation = getPermutation(alignment->alignmentSize);
    tree = growThreeLeavesTree(names[permutation[0]], names[permutation[1]], names[permutation[2]]);

    init += time(0);
    srand(init);
    for(i = 3; i < alignment->alignmentSize; ++i)
    {        
        j = rand() % (tree->leavesNum * 2 - 3); // choose leaf
        m = 0;
        for(k = 0; k < tree->leavesNum * 2 - 2; ++k)
        {
            for(h = 0; h < tree->nodes[k]->neiNum; ++h)
            {
                if (tree->nodes[k]->neighbours[h]->pos > k)
                {
                    if (m == j)
                    {
                        tree = treeAddLeaf(tree, k, h,\
                                names[permutation[i]], 0, 0);
                        goto next;
                    }
                    ++m;
                }
            }
        }
        next:;
    }
    treeLCAFinderCalculate(tree);
    result = treeWithScoreCreate(tree,\
            countScoreHash(alignment, tree, pwmMatrix, alpha, gapOpt,\
                    hashScore, permutation));
    free(names);
    free(permutation);
    return result;
} /* randomTreeGrow */ 

TreeWithScore** growMultipleRandomTree(HashAlignment* alignment,
        int alpha, GapOpt gapOpt, PWM* pwmMatrix, unsigned treeNum,
        INT**** hashScore)
{
    TreeWithScore** treeWSAR; 
    unsigned i;

    treeWSAR = (TreeWithScore**) calloc(sizeof(TreeWithScore*), treeNum);
    for(i = 0; i < treeNum; ++i)
    {
        treeWSAR[i] = randomTreeGrow(alignment, alpha, gapOpt, pwmMatrix, hashScore);
    }
    return treeWSAR;
} /* growMultipleRandomTree */


Sample* sampleCreate()
{
    Sample* sample;

    sample = malloc(sizeof(Sample)); 
    sample->sample = 0;
    sample->sampleSize = 0;
    sample->mean = 0;
    sample->deviation = 0;

    return sample;
} /* sampleCreate */

void sampleDelete(Sample* sample)
{
    if (sample->sample != 0)
    {
        free(sample->sample);
    }
    free(sample);
    return;
} /* sampleDelete */

void sampleCalcMean(Sample* sample)
{
    double mean = 0.0;
    int i;

    for(i = 0; i < sample->sampleSize; ++i)
    {
        mean += sample->sample[i];
    }
    mean /= sample->sampleSize;
    sample->mean = mean;
    return;
} /* sampleCalcMean */

void sampleCalcDev(Sample* sample)
{
    int i;
    double diff, dev = 0.0;

    for(i = 0; i < sample->sampleSize; ++i)
    {
        diff = sample->sample[i] - sample->mean;
        dev += diff * diff ;
    }
    dev /= (sample->sampleSize - 1);
    sample->deviation = dev;
    return;
} /* sampleCalcDev */

Sample* sampleFromTreeWSAr(TreeWithScore** treeSample, unsigned treeNum)
{
    Sample* sample;
    int i;

    sample = sampleCreate();
    sample->sampleSize = treeNum;
    sample->sample = calloc(sizeof(INT), treeNum);

    for(i = 0; i < treeNum; ++i)
    {
        sample->sample[i]= treeSample[i]->score;
    }
    sampleCalcMean(sample);
    sampleCalcDev(sample);
    return sample;
} /* sampleFromTreeWSAr */

double sampleCalcZscore(Sample* sample, INT value)
{
    return (value - sample->mean) / sqrt(sample->deviation);
} /*  sampleCalcZscore */

