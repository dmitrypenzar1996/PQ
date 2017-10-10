#include "countScore.h"
#define TRUE 1
#define FALSE 0

void printHelp(char* command)
{
    printf("Usage:\n");
    printf(" %s -a <alignment> -t <tree> [-m <matrix>] [-c <int>] [-g <0|1|2>]\n\n", command);
    return;
}

void printLongHelp(void)
{
    printf("PQ score version 1.1\n");
    printf("Calculate pq score of a given tree\n\n");
    printf(" ** Options:\n");
    printf("   -a <FileName>\n");
    printf("      File with alignment in fasta format\n");
    printf("   -t <FileName>\n");
    printf("      File with tree in Newick format\n");
    printf("   -m <FileName>\n");
    printf("      File with scoring matrix (optional)\n");
    printf("      If omitted, the diagonal matrix is used\n");
    printf("   -c <Positive integer>\n");
    printf("      Multiplication coefficient (optional)\n");
    printf("      Default: 0\n");
    printf("   -g <0|1|2>\n");
    printf("      Maximum number of gaps in a  position-quartet pair to score it (optional):\n");
    printf("          0 ignore all pq pairs with at least one gap,\n");
    printf("          1 ignore pq pairs with more than one gap,\n");
    printf("          2 score all pq pairs.\n\n");
    return;
}

void main(int argc, char **argv)
{
    INT score;
    char* alignmentFileName;
    char* treeFileName;
    char* pwmFileName = NULL;
    int alpha = 1;
    GapOpt gapOpt = 0;
    char *param;
    int alSet = FALSE;
    int trSet = FALSE;
    int known;
    int s;

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

    /* Parsing command line and reading options */
    param = (char *)malloc(sizeof(char) * 128);
    s = 1;
    while (s < argc)
    {
        strcpy(param, argv[s]);
        known = FALSE;      /* we do not recognize this argument yet */
        if(param[0] == '-')
        {
            if (strcmp(param, "-a") == 0)
            {
                known = TRUE;  /* yes, we know this argument name! */
                if (s + 1 < argc)
                {
                    alignmentFileName = (char*)malloc(sizeof(char) * strlen(argv[s + 1]));
                    strcpy(alignmentFileName, argv[s + 1]);
                    alSet = TRUE;
                }
            }
            if (strcmp(param, "-t") == 0)
            {
                known = TRUE;
                if (s + 1 < argc)
                {
                    treeFileName = (char *)malloc(sizeof(char) * (strlen(argv[s + 1] + 1)));
                    strcpy(treeFileName, argv[s + 1]);
                    trSet = TRUE;
                }
            }
            if (strcmp(param, "-m") == 0)
            {
                known = TRUE;
                if (s + 1 < argc)
                {
                    pwmFileName = (char *)malloc(sizeof(char) * (strlen(argv[s + 1] + 1)));
                    strcpy(pwmFileName, argv[s + 1]);
                }
            }
            if (strcmp(param, "-c") == 0)
            {
                known = TRUE;
                if (s + 1 < argc)
                {
                    alpha = atoi(argv[s + 1]);
                }
            }
            if (strcmp(param, "-g") == 0)
            {
                known = TRUE;
                if (s + 1 < argc)
                {
                   if (atoi(argv[s + 1]) == 0)
                   {
                       gapOpt = PASS_ANY;
                   }
                   else if (atoi(argv[s + 1]) == 1)
                   {
                       gapOpt = PASS_PAIR;
                   }
                   else if (atoi(argv[s + 1]) == 2)
                   {
                       gapOpt = PASS_NOT;
                   }
                   else
                   {
                       fprintf(stderr, "Unknown option \"-g %s\". Aborted.\n", argv[s + 1]);
                       exit(1);
                   }
               }
            }
            if (!known) fprintf(stderr, "Warning: unrecognized parameter \"%s\". Ignored.\n", param);
        }
        else
            fprintf(stderr, "Warning: misplaced value \"%s\"in command line. Ignored.\n", param);
        s += 2;
     } /* while */
     if (!alSet) fprintf(stderr, "Alignment is not given. Aborted.\n");
     if (!trSet) fprintf(stderr, "Tree is not given. Aborted.\n");
     if (!(alSet && trSet)) exit(1);

     /* Calculation */
     score = alignmentScoreFiles(alignmentFileName, treeFileName, pwmFileName, alpha, gapOpt);

     /* Output */
     printf("%llu\n", score);
     return;
}

