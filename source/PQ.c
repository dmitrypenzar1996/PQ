#include <stdio.h>
#include <assert.h>
#include "growTree.h"
#include "nniTree.h"
#include "TreeWS.h"
#include "HashAlignment.h"
#include "PWM.h"
#include "Tree.h"
#include "sprTree.h"
#include "estimate.h"
#include "consensus.h"
#include "bootstrap.h"


#define TRUE 1
#define FALSE 0

void printLongHelp(void)
{
    printf("PQ version 2.3.2\n");
    printf("General options:\n");
    printf(" -alignment <FileName> \n");
    printf("       File with input alignment in fasta format\n");
    printf("       Required, no default value\n");
    printf(" -out <FileName>\n");
    printf("       Output file\n");
    printf("       Default: pq_out.tre\n");
    printf(" -iniTree <FileName> \n");
    printf("       File with initial tree in Newick format, optional\n");
    printf("       If -iniTree is given, growing tree by stepwise addition is omitted\n");
    printf("\n");
    printf("Scoring options:\n");
    printf(" -pwm <FileName>\n");
    printf("      File with scoring matrix (e.g. BLOSUM62.txt) \n");
    printf("      If not given, the identity matrix is used\n");
    printf(" -alpha <Positive integer>\n");
    printf("      Multiplication coefficient for pq-pairs supported twice\n");
    printf("      Default: 1\n");
    printf(" -gapOpt <0|1|2>\n");
    printf("      Maximum number of gaps among four letters in a position-quartet pair\n");
    printf("      If 0, pq-pairs with at least one gap are ignored\n");
    printf("      If 1, pq-pairs with one gap are scored, with two gaps are ignored\n");
    printf("      If 2, all gaps are scored as residues/nucleotides\n");
    printf("      Default: 0\n");
    printf("\n");
    printf("Optimization options:\n");
    printf(" -grType <String>\n");
    printf("      \"one\": grow a single tree by stepwise addition of leaves,\n");
    printf("      \"multiple\":  grow many trees\n");
    printf("      Default: \"multiple\"\n");
    printf("      --treeNum <Positive integer>\n");
    printf("           for \"multiple\" option: number of trees to grow\n");
    printf("           Default: 10\n");
    printf("      --chType <String>\n");
    printf("           \"bestScore\" : choose tree with best score\n");
    printf("           \"consensus\" : make consensus of trees\n");
    printf("      Default: \"bestScore\"\n");
    printf("      Growing is not performed if an initial tree is given\n");
    printf(" -randLeaves <0 or 1>\n");
    printf("      If 1, shuffle the input order of sequences\n");
    printf("      Switching to 0 makes sense for grType = \"one\")\n");
    printf("      Default: 1\n"); 
    printf(" -nniType <String>\n");
    printf("      \"none\": do not perform NNI search,\n");
    printf("      \"direct\" or \"gradient\": perform gradient NNI hill climbing,\n");
    printf("      \"simple\": perform simple NNI hill climbing,\n");
    printf("      \"trajectory\": perform NNI Monte-Carlo search\n");
    printf("      Default: \"direct\"\n"); 
    printf("      --trTime <Positive integer>\n"); 
    printf("           For \"trajectory\"option: number of iterations\n");
    printf("           Default : 1000\n"); 
    printf("      --initTemp <Positive integer>\n");
    printf("           For \"trajectory\"option: initial temperature\n");
    printf("           Default: 1000\n");
    printf("      --mcStyle <1|2|3>\n");
    printf("           For \"trajectory\"option: style of Monte-Carlo search\n");
    printf("           Default: 1\n");
    printf(" -sprType <String>\n");
    printf("      \"none\": do not perform SPR search,\n");
    printf("      \"direct\" or \"gradient\": perform gradient SPR hill climbing,\n");
    printf("      \"simple\": perform simple SPR hill climbing,\n");
    printf("      Default: \"none\"\n"); 
    printf("\n");
    printf("Result quality estimations:\n");
    printf(" -neiZscore <0 or 1>\n");
    printf("      If 1, calculate Z-score based on scores of neighboring trees\n");
    printf("      Default: 0\n");
    printf(" -randTreeZscore <0 or 1>\n");
    printf("      If 1, calculate Z-score based on scores of random trees\n");
    printf("      Default: 0\n");
    printf("      --randTreeNum <Positive integer>\n");
    printf("           For randTreeZscore: number of random trees\n");     
    printf("           Default : 10\n");
    printf(" -distrFile <FileName>\n");
    printf("      File name to write scores of neighboring and/or random trees\n");
    printf("      Optional, no default value\n");
    printf("Result statistics estimation:\n");
    printf(" -resultTreeNum <int>\n");
    printf("      Number of trees to generate\n");
    printf("      Default: 1\n");
    printf(" -sampleType <string>\n");
    printf("      how to generate sample trees\n");
    printf("      \"simple\": just generate multiple trees\n");
    printf("      \"bootstrap\": generate bootstrap trees(first tree  is result tree, others - bootstrap)\n");
    printf("      \"jackknife\": the same, but generate jackknife trees\n");
    printf("      Default: \"simple\"\n");
    printf("      --removeFraction <double>\n");
    printf("          fraction of positions to be removed during jackknife\n");
    printf("          Default: \"0.5\"\n");
    return;
} /* printLongHelp */

void printHelp(char *command)
{
    /* printf("Help message for PQ ver 1.5\n"); */
    printf("Usage:\n");
    printf("%s -alignment <FileName> -out <FileName>\n", command);
    printf("\t[-iniTree <FileName>] [-pwm <FileName>]\n");
    printf("\t[-alpha <int>] [-gapOpt <0|1|2>]\n");
    printf("\t[-grType <one|multiple> [-randLeaves <0|1>]] [--treeNum <int>]\n");
    printf("\t\t[--chType <bestScore|consensus>]\n");
    printf("\t[-nniType <none|simple|direct|trajectory>\n");
    printf("\t\t[--trTime <int>] [--initTemp <int>] [--mcStyle <1|2|3>]]\n");
    printf("\t[-sprType <none|simple|direct>]\n");
    printf("\t[-neiZscore <0|1>] [-randTreeZscore <0|1>]\n");
    printf("\t\t[-distrFile <FileName>]\n");
    printf("\t[-resultTreeNum <int>] [-sampleType <simple|bootstrap|jackknife> ]\n");
    printf("\t\t[--removeFraction <int>]\n");
    printf("For description of parameters run %s -h\n", command);
    puts("Press Enter to continue");
    getchar();
    return;
}

int main(int argc, char** argv)
{
    HashAlignment* inAlignment;
    HashAlignment* alignment;
    HashAlignment** alignmentSample;
    char* outFileName = NULL;
    PWM* pwmMatrix = NULL;
    unsigned i;
    unsigned j;
    int alpha = 1;
    int randLeaves = 1;
    char* grType;
    unsigned treeNum = 10;
    char* nniType;
    unsigned long int trTime = 1000;
    unsigned int initTemp = 1000;
    unsigned int mcStyle = 1;
    char* sprType;
    char* sampleType;
    char* distrFileName = NULL;
    char* inTreeFileName = NULL;
    char doConsensus = 0;
    char extended = 1;
    unsigned randTreeNum = 10;
    unsigned resultTreeNum = 1;
    double removeFraction = 0.5;
    TreeWithScore** resultTrees = NULL;
    int maxScore = 1;
    int neiZscore = 0;
    int randTreeZscore = 0;
    GapOpt gapOpt = PASS_ANY;
    char* param;
    int known;
    int alignmentSet;
    int startOptionsNum;
    INT**** hashScore;
    Tree* iniTree;
    TreeWithScore* result = NULL;
    TreeWithScore** trees;
    Tree** treesTemp;
    TreeWithScore* nniResult;
    Trajectory* resultTrajectory;
    TreeWithScore* sprResult;
    double relativeScore;
    char** treeNames;
    char** seqNames;
    int* permutation;
    TreeWithScore** neighbours;
    Sample* sample;
    FILE* distrFile;
    TreeWithScore** treeSample;
    FILE* inTreeStream;
    char* newick;
    unsigned newickSize;
    unsigned end;
    char* chType; 
    char* fileName;

    /* default values for string parameters */
    grType = (char *)malloc(sizeof(char) * 12);
    strcpy(grType, "multiple");
    nniType = (char *)malloc(sizeof(char) * 12);
    strcpy(nniType, "direct");
    sprType = (char *)malloc(sizeof(char) * 12);
    strcpy(sprType, "none");
    chType = (char*)malloc(sizeof(char) * 12); 
    strcpy(chType, "bestScore");
    sampleType = (char*)malloc(sizeof(char) * 12);
    strcpy(sampleType, "simple");
   
    /* Help */  
    if (argc == 1){
        printHelp(argv[0]);
        exit(0);
    }
    if ( (strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "-help") == 0) || (strcmp(argv[1], "--help") == 0) )
    {
        printLongHelp();
        exit(0);
    }

    /* *********************************************
     *    Parsing command line and input 
     *    *****************************************/

    param = (char *)malloc(sizeof(char) * 128);
    alignmentSet = 0;
    startOptionsNum = 1;
    while (startOptionsNum < argc)
    {
        strcpy(param, argv[startOptionsNum]);

        known = 0;      /* we do not recognize this argument yet */
        if(param[0] == '-')
        {
            if (strcmp(param, "-alignment") == 0)
            {
                known = 1;  /* yes, we know this argument name! */
                if (startOptionsNum + 1 < argc)
                {
                    inAlignment = hashAlignmentRead(argv[startOptionsNum + 1]);
                    alignmentSet = 1;
                }
            }
            if (strcmp(param, "-out") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    outFileName = (char *)malloc(sizeof(char) * (strlen(argv[startOptionsNum + 1] + 1)));
                    strcpy(outFileName, argv[startOptionsNum + 1]);
                }
                printf("Start computing\n");
            }
            if (strcmp(param, "-pwm") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    pwmMatrix = pwmRead(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-alpha") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    alpha = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-storeHash") == 0)
            {
                known = 1;
                fprintf(stderr, "Warning: setting -storeHash has no effect in this version, it is always 1\n");
            }
            if (strcmp(param, "-randLeaves") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    randLeaves = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-grType") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    if (strlen(argv[startOptionsNum + 1]) < 12) 
                    {
                        strcpy(grType, argv[startOptionsNum + 1]);
                    }
                    else
                    {
                        fprintf(stderr, "Unknown grType: %s -- ignored\n", argv[startOptionsNum + 1]);
                    }
                }
            }
            if (strcmp(param, "--treeNum") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    treeNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--chType") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    chType = argv[startOptionsNum + 1];
                }
            }
            if (strcmp(param, "-nniType") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    if (strlen(argv[startOptionsNum + 1]) < 12) 
                    {
                        strcpy(nniType, argv[startOptionsNum + 1]);
                    }
                    else
                    {
                        fprintf(stderr, "Unknown nniType: %s -- ignored\n", argv[startOptionsNum + 1]);
                    }
                }
            }
            if (strcmp(param, "--trTime") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    trTime = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--initTemp") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    initTemp = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--mcStyle") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    mcStyle = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-sprType") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    if (strlen(argv[startOptionsNum + 1]) < 12) 
                    {
                        strcpy(sprType, argv[startOptionsNum + 1]);
                    }
                    else
                    {
                        fprintf(stderr, "Unknown sprType: %s -- ignored\n", argv[startOptionsNum + 1]);
                    }

                }
            }
            if (strcmp(param, "-tbrType") == 0)
            {
                known = 1;
                fprintf(stderr, "Warning: TBR is not suppoted in this version, setting -tbrType is ignored\n");
            }
            if (strcmp(param, "-maxScore") == 0)
            {
                known = 1;
                fprintf(stderr, "Warning: setting -maxScore has no effect in this version, it is always 1\n");
                if (startOptionsNum + 1 < argc)
                {
                     maxScore = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-neiZscore") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                     neiZscore = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-randTreeZscore") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    randTreeZscore = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-distrFile") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    distrFileName = (char *)malloc(sizeof(char) * (strlen(argv[startOptionsNum + 1] + 1)));
                    strcpy(distrFileName, argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--randTreeNum") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    randTreeNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-iniTree") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    inTreeFileName = (char *)malloc(sizeof(char) * (strlen(argv[startOptionsNum + 1] + 1)));
                    strcpy(inTreeFileName, argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-resultTreeNum") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    resultTreeNum = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-sampleType") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    sampleType = (char*)malloc(sizeof(char) *\
                            (strlen(argv[startOptionsNum + 1]) + 1));
                    strcpy(sampleType, argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "--removeFraction") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    removeFraction = atof(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-doConsensus") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    doConsensus = atoi(argv[startOptionsNum + 1]);
                }
            }
            if (strcmp(param, "-gapOpt") == 0)
            {
                known = 1;
                if (startOptionsNum + 1 < argc)
                {
                    if ((strcmp(argv[startOptionsNum + 1], "passNot") == 0) ||\
                        (strcmp(argv[startOptionsNum + 1], "2") == 0)) 
                    {
                        gapOpt = PASS_NOT;
                    }
                    else if ((strcmp(argv[startOptionsNum + 1], "passPair") == 0)||\
                             (strcmp(argv[startOptionsNum + 1], "1") == 0)) 
                    {
                        gapOpt = PASS_PAIR;
                    }
                    else if ((strcmp(argv[startOptionsNum + 1], "passAny") == 0)||\
                             (strcmp(argv[startOptionsNum + 1], "0") == 0)) 
                    {
                        gapOpt = PASS_ANY;
                    }
                    else
                    {
                        fprintf(stderr, "Error: wrong value for gapOpt: %s\n", argv[startOptionsNum + 1]);
                        fprintf(stderr, "gapOpt should be 0, 1 or 2\n");
                        exit(1);
                    }
                }
            }
        } /* if argument starts with '-' */
        if (known == 0) /* we cannot recognize this argument name ... */
        {
            printf("%s\n", argv[startOptionsNum]);
            fprintf(stderr, "Wrong command line argument: %s\n", param);
            exit(1);
        }
        else
        {
            startOptionsNum += 2;
        }
    }  /* while */

    if (alignmentSet == 0)
    {
        fprintf(stderr, "Error: no input alignment is given\n");
        exit(1);
    }
    if (outFileName == NULL)
    {
        fprintf(stderr, "Warning: no output file name is given; the result will be written to \"pq_out.tre\"\n");
        outFileName = (char *)malloc(sizeof(char) * 12);
        strcpy(outFileName, "pq_out.tre");
    }

/***********************************************************
 *                     Calculations                        *
 ***********************************************************/
    
    alignmentSample = malloc(sizeof(HashAlignment*) * resultTreeNum);
    alignmentSample[0] = inAlignment;
    if (strcmp(sampleType, "simple") == 0)
    {
        for(i = 1; i < resultTreeNum; ++i)
        {
            alignmentSample[i] = inAlignment;
        }
    }
    else if (strcmp(sampleType, "bootstrap") == 0)
    {
        for(i = 1; i < resultTreeNum; ++i)
        {
            alignmentSample[i] = hashAlignmentBootstrap(inAlignment); 
        }
    } 
    else if (strcmp(sampleType, "jackknife") == 0)
    {
        for(i = 1; i < resultTreeNum; ++i)
        {
            alignmentSample[i] = hashAlignmentJackknife(inAlignment, removeFraction);
        }
    }
    else
    {
        fprintf(stderr, "Wrong value for sampleType: %s\n", sampleType);
/*
        raiseError("Wrong value of argument for sampleType", __FILE__, __FUNCTION__, __LINE__); */
    }

    resultTrees = malloc(sizeof(TreeWithScore*) * resultTreeNum);
    for(j = 0; j < resultTreeNum; ++j)
    {
        alignment = alignmentSample[j];
        hashScore = getHashScore(alignment->alignmentSize);
    if (inTreeFileName == NULL)
    {
        if (strcmp(grType, "one") == 0)
        {
            result = oneTreeGrow(alignment, alpha, gapOpt, pwmMatrix, hashScore, randLeaves);
        }
        else if (strcmp(grType, "multiple") == 0)
        {
            trees = multipleTreeGrow(alignment, alpha, gapOpt, pwmMatrix, treeNum, hashScore);

            if (strcmp(chType, "bestScore") == 0)
            {
               result = trees[treeNum - 1];
               for(i = 0; i < treeNum - 1; ++i)
               {
                   treeWithScoreDelete(trees[i]);
               }
            }
            else if (strcmp(chType, "consensus") == 0)
            {
                printf("Making consensus\n");
                //fprintf(stderr, "Option %s for multiple grow us not ready yet, PQ:main\n", chType);
                //exit(1);
                treesTemp = malloc(sizeof(Tree*) * treeNum);
                for(i = 0; i < treeNum; ++i)
                {
                    treesTemp[i] = trees[i]->tree;
                }
                result = treeWithScoreCreate(makeConsensus(treesTemp, treeNum, 
                            0.5, extended), 0);
                treeNames = treeGetNames(result->tree);
                seqNames = hashAlignmentGetSeqNames(alignment);
                permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);

                treeLCAFinderCalculate(result->tree);
                result->score = countScoreHash(alignment, result->tree,
                        pwmMatrix, alpha, gapOpt, hashScore, permutation);

                for(i = 0; i < treeNum; ++i)
                {
                    treeWithScoreDelete(trees[i]);
                }
                free(treesTemp);
            }
            else
            {
                fprintf(stderr, "Wrong value for chType: %s, PQ:main\n", chType);
                exit(1);
            }
        }                      
        else if (strcmp(grType, "saturation") == 0)
        {
            fprintf(stderr, "Option \"%s\" of grType is not ready yet\n", grType);
            exit(1);
        }
        else
        {
            fprintf(stderr, "Wrong value for grType: %s\n", grType);
            exit(1);
        }
    } 
    else /* inTreeFileName is given => do not grow initial tree */
    {
        printf("Initial tree is from file %s\n", inTreeFileName);
        iniTree = treeRead(inTreeFileName);

        if(iniTree->leavesNum != alignment->alignmentSize)
        {
            fprintf(stderr, "Tree in %s does not correspond to alignment\n", inTreeFileName);
            exit(1);
        }
        treeNames = treeGetNames(iniTree);
        seqNames = hashAlignmentGetSeqNames(alignment);
        permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);
        if (permutation == NULL)
        {
            fprintf(stderr, "Tree in %s does not correspond to alignment\n", inTreeFileName);
            exit(1);
        }
        result = treeWithScoreCreate(iniTree, countScoreHash(alignment, iniTree, pwmMatrix,
                                                             alpha, gapOpt, hashScore, permutation));
    } /* if inTreeFileName is defined */

    treeLCAFinderCalculate(result->tree);
    if (strcmp(nniType, "none") == 0)
    {
        ;
    }
    else if ( (strcmp(nniType, "direct") == 0) || (strcmp(nniType, "gradient") == 0) )
    {
        nniResult = gradientNNI(result->tree, alignment, pwmMatrix,\
            alpha, gapOpt, hashScore);
        treeWithScoreDelete(result);
        result = nniResult;
    }
    else if ( strcmp(nniType, "simple") == 0 )
    {
        nniResult = simpleNNI(result->tree, alignment, pwmMatrix,\
            alpha, gapOpt, hashScore);
        treeWithScoreDelete(result);
        result = nniResult;
    }
    else if (strcmp(nniType, "trajectory") == 0)
    {
        resultTrajectory = trajectoryNNI(result->tree, alignment, pwmMatrix, alpha, gapOpt, hashScore, trTime, initTemp, mcStyle);
        treeWithScoreDelete(result);
        result = resultTrajectory->bestPoint->treeWS;
        nniResult = gradientNNI(result->tree, alignment, pwmMatrix, 
                                alpha, gapOpt, hashScore); /* perform hill climbing after trajectory */
        treeWithScoreDelete(result);
        result = nniResult;
    }
    else
    {
        fprintf(stderr, "Wrong value for nniType: %s\n", nniType);
        exit(1);
    }

    treeLCAFinderCalculate(result->tree);
    if (strcmp(sprType, "none") == 0)
    {
        ;
    }
    else if ( (strcmp(sprType, "direct") == 0) || (strcmp(sprType, "gradient") == 0) )
    {
        sprResult = gradientSPR(result->tree, alignment, pwmMatrix, alpha, gapOpt, hashScore);
        treeWithScoreDelete(result);
        result = sprResult;
    }
    else if (strcmp(sprType, "simple") == 0)
    {
        sprResult = simpleSPR(result->tree, alignment, pwmMatrix, alpha, gapOpt, hashScore);
        treeWithScoreDelete(result);
        result = sprResult;
    }
    else if (strcmp(sprType, "trajectory") == 0)
    {
        fprintf(stderr, "Option \"%s\" for sprType is not ready yet\n", sprType);
        exit(1);
    }
    else
    {
        fprintf(stderr, "Wrong value for sprType: %s\n", sprType);
        exit(1);
    }

    fprintf(stdout, "Score of result tree is %lu\n", result->score);

    if(maxScore == 1) /* calculation of relative score */
    {
        relativeScore = result->score;
        treeNames = treeGetNames(result->tree);
        seqNames = hashAlignmentGetSeqNames(alignment);
        permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);
        relativeScore /= countMaxScore(alignment, pwmMatrix, alpha, gapOpt, hashScore, permutation);
        free(treeNames);
        free(seqNames);
        free(permutation);
        printf("Relative score of result tree is %lf\n", relativeScore);
    }

    if (neiZscore == 1) /* calculation of Z-score based on NNI neighbors */
    {
        neighbours = getNNINeighbours(result->tree, alignment,\
                pwmMatrix, alpha, gapOpt, hashScore);
        sample = sampleFromTreeWSAr(neighbours,(result->tree->leavesNum - 3) * 2);
        printf("Neighbors Z-score is %lf\n", sampleCalcZscore(sample, result->score));

        if (distrFileName != NULL)
        {
            if(strcmp(distrFileName, "stdout") == 0)
            {
                distrFile = stdout;
            }
            else
            {
                distrFile = fopen(distrFileName, "w");
            }
            fprintf(distrFile, "Neighbours of result tree\n");
            fprintf(distrFile, "neiNum\tScore\n");
            for(i = 0; i < result->tree->leavesNum * 2 - 6; ++i)
            {
                fprintf(distrFile, "%d\t%lu\n", i + 1, neighbours[i]->score);
            }
            if((strcmp(distrFileName, "stdout") != 0) &&
               (randTreeZscore == 0))
            { 
                fclose(distrFile);
            }
        }
        sampleDelete(sample);

        for(i = 0; i < result->tree->leavesNum * 2 - 6; ++i)
        {
            treeWithScoreDelete(neighbours[i]);
        }
        free(neighbours);
    }

    if (randTreeZscore == 1) /* calculation of Z-score based on random trees */
    {
        treeSample = growMultipleRandomTree(alignment, alpha, gapOpt,\
            pwmMatrix, randTreeNum, hashScore);
        sample = sampleFromTreeWSAr(treeSample, randTreeNum);
        fprintf(stdout, "Random tree Z-score is %lf\n", sampleCalcZscore(sample, result->score));
        if (distrFileName != NULL)
        {
            if (neiZscore == 0)
            {
                if(strcmp(distrFileName, "stdout") == 0)
                {
                    distrFile = stdout;
                }
                else
                {         
                    distrFile = fopen(distrFileName, "w");
                }   
            }
            fprintf(distrFile, "Random trees\n");
            fprintf(distrFile, "treeNum\tScore\n");
            for(i = 0; i < randTreeNum; ++i)
            {
                fprintf(distrFile, "%d\t%lu\n", i + 1, treeSample[i]->score);
            }
            if(strcmp(distrFileName, "stdout") != 0) fclose(distrFile);
        }
        for(i = 0; i < randTreeNum; ++i)
        {
            treeWithScoreDelete(treeSample[i]);
        }
        free(treeSample);
    }

        resultTrees[j] = result;
        removeHashScore(hashScore, alignment->alignmentSize);
    }
    /* Output */

    if (doConsensus && resultTreeNum > 1)
    {
        treesTemp = malloc(sizeof(Tree*) * resultTreeNum);
        for(i = 0; i < resultTreeNum; ++i)
        {
            treesTemp[i] = resultTrees[i]->tree;
        }

        result = treeWithScoreCreate(makeConsensus(treesTemp, resultTreeNum,
                    0.5, extended), 0);
        treeNames = treeGetNames(result->tree);
        seqNames = hashAlignmentGetSeqNames(alignment);
        permutation = calculatePermutation(treeNames, seqNames, alignment->alignmentSize);
        treeLCAFinderCalculate(result->tree);
        result->score = countScoreHash(alignmentSample[0], result->tree,
        pwmMatrix, alpha, gapOpt, NULL, permutation);
        free(treesTemp);
        treeConsensusWrite(result->tree, outFileName);
        treeDelete(result->tree);
    }
    else
    {
        if (doConsensus)
        {
            fprintf(stderr, "Num of trees is equal to 1, isn't nessesary to do consensus\n");
        }
        for(i = 0; i < resultTreeNum; ++i)
        {
            fileName = malloc(sizeof(char) * (strlen(outFileName) + 6 + i / 10));
            sprintf(fileName, "%s_%u.nwk", outFileName, i);
            treeWrite(resultTrees[i]->tree, fileName);
        }
    }
    

    for(i = 0; i < resultTreeNum; ++i)
    {
            treeWithScoreDelete(resultTrees[i]);
    }
    free(resultTrees);


    if (strcmp(sampleType, "simple") == 0)
    {
        hashAlignmentDelete(alignmentSample[0]);
    }
    else
    {
        for(i = 0; i < resultTreeNum; ++i)
        {
            hashAlignmentDelete(alignmentSample[i]);
        }
    }
    free(alignmentSample);


    return 0;
} /* main */

