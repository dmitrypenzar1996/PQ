#include "PWM.h"
#define CHAR_SIZE 128

int** matrixCreate(unsigned char size)
{
    int** matrix = (int**)calloc(sizeof(int*), size);
    unsigned int i;

    for (i = 0; i < size; ++i)
    {
        matrix[i] = (int*)calloc(sizeof(int), size);
    }
    return matrix;
} /* matrixCreate */

void matrixDelete(int** matrix, unsigned char size)
{
    unsigned int i;
    for (i = 0; i < size; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
} /* matrixDelete */

PWM* pwmCreate()
{
    PWM* pwm;
    pwm = (PWM*)malloc(sizeof(PWM));
    pwm->name = NULL;
    pwm->matrix = NULL;
    pwm->matrixSize = 0;
    pwm->alphabetSize = 0;
    pwm->alphabet = NULL;
    return pwm;
} /* pwmCreate */

void pwmDelete(PWM* pwm)
{
    if (pwm->name != 0)
    {
        free(pwm->name);
    }
    if (pwm->matrix != 0)
    {
        matrixDelete(pwm->matrix, pwm->matrixSize);
    }
    if (pwm->alphabet != 0)
    {
        free(pwm->alphabet);
    }
    free(pwm);
} /* pwmDelete */

PWM* pwmRead(char* inFileName)
{
    FILE* infile;
    PWM* pwm;
    char* line;
    char* title;
    unsigned int i;
    unsigned int curPos = 0; 
    int** matrix;
    unsigned int titleSize;
    unsigned int height = 0;
    char curElem;

    infile = fopen(inFileName, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Cannot open file %s with scoring matrix\n", inFileName);
        exit(1);
    }

    while ((line = readLine(infile))[0] == '#')
    {
        free(line);
        continue;
    }
    // now line is title line;
    title = (char*)malloc(sizeof(char) * CHAR_SIZE);    
    for (i = 0; i < strlen(line); ++i)
    {
        if (line[i] > 32)
        {
            title[curPos] = line[i];
            ++curPos;        
        } 
    }
    titleSize = curPos;
    
    matrix = matrixCreate(CHAR_SIZE);

    line = readLine(infile);
    while (line != NULL)
    {
        if (strlen(line) > 0) 
        {
            if (line[0] != '#')
            {
                i = 0;
                while (line[i] <= 32 && i < strlen(line))
                {
                    ++i;
                }
                if (i < strlen(line)) /* line is not empty */
                {
                    ++height;
                    curElem = line[i];
                    curPos = 0;
                    i++;
                    while(i < strlen(line))
                    {
                        while (line[i] <= 32 && i < strlen(line))
                        {
                            i++;
                        }
                        if (i < strlen(line))
                        {
                           if(curPos == titleSize)
                           {
                               fprintf(stderr, 
                                       "Wrong matrix format: \"%c\" row contains %d items and the title row %d letters.\n",
                                       curElem, curPos + 1, titleSize);
                               exit(1);
                            }
                            matrix[curElem][title[curPos]] = atof(line + i);
                            ++curPos;
                        }
                        while (line[i] > 32 && i < strlen(line))
                        {
                            ++i;
                        }
                    } /* while */ 
                    if (curPos != titleSize )
                    {
                        fprintf(stderr,
                                "Wrong matrix format: \"%c\" row contains %d items and the title row %d letters.\n",
                                curElem, curPos, titleSize);
                        exit(1);
                    }
                } /* if line[0] != '#'*/
            } /* if strlen(line) > 0 */
        } /* if not comment line */
        free(line);
        line = readLine(infile);
    }/* while */
    if (height != titleSize)
    {
        fprintf(stderr, "Wrong matrix format: matrix contains %d rows and %d columns\n", height, titleSize);
        exit(1);
    }

    pwm = pwmCreate();
    pwm->name = (char*)malloc(sizeof(char) * (strlen(inFileName) + 1));
    strcpy(pwm->name, inFileName);
    pwm->matrix = matrix;
    pwm->matrixSize = CHAR_SIZE;
    pwm->alphabetSize = titleSize;
    pwm->alphabet = title;
    return pwm;
} /* pwmRead */

char* pwmToString(PWM* pwm, unsigned char fieldSize)
{
    char* pwmString;
    int i = 0;
    int j = 0;
    size_t strSize = 0;

    if (pwm->name != 0)
    {
        strSize = (fieldSize + 1) * (pwm->alphabetSize + 1) * (pwm->alphabetSize + 1) + \
                  strlen(pwm->name) + 2 + 1;
    }
    else
    {
        strSize = (fieldSize + 1) * (pwm->alphabetSize + 1) * (pwm->alphabetSize + 1) + 2 + 1; 
    }

    if (pwm->alphabet == 0)
    {
        perror("Wrong matrix format");
        exit(1);
    }

    pwmString = (char*)malloc(sizeof(char) * strSize * 200); // *2 for great justice
    
    pwmString[0] = '\0'; 
    if (pwm->name != 0)
    { 
        sprintf(pwmString, "#%s\n", pwm->name);
    }    
    for (j = 0; j < fieldSize + 1; ++j)
    {
        sprintf(pwmString + strlen(pwmString)," ");
    }   
    for (i = 0; i < pwm->alphabetSize - 1; ++i)
    {
        sprintf(pwmString + strlen(pwmString),"%c ", pwm->alphabet[i]);
        for (j = 0; j < fieldSize - 1; ++j)
        {
            sprintf(pwmString + strlen(pwmString)," ");
        }   
    }

    sprintf(pwmString + strlen(pwmString),"%c\n", pwm->alphabet[i]);

    for(i = 0; i < pwm->alphabetSize; ++i)
    {
        sprintf(pwmString + strlen(pwmString),"%c ", pwm->alphabet[i]);
        for (j = 0; j < fieldSize - 1; ++j)
        {
            sprintf(pwmString + strlen(pwmString)," ");
        }   
        for (j = 0; j < pwm->alphabetSize - 1; ++j)
        {
            sprintf(pwmString + strlen(pwmString), "%*d", fieldSize, 
                    pwm->matrix[pwm->alphabet[i]][pwm->alphabet[j]]);    
        }
        sprintf(pwmString + strlen(pwmString), "%*d\n", fieldSize, 
                pwm->matrix[pwm->alphabet[i]][pwm->alphabet[j]]);    
    }
    pwmString = realloc(pwmString, sizeof(char) * (strlen(pwmString) + 1));
    return pwmString;
} /*pwmToString */

void pwmWriteToFile(char* outFileName, PWM* pwm, char** comments,\
        int commentsSize, int fieldSize) 
    // comments is array of lines without \n-symbols 
{
    FILE* outFile;
    char* pwmStr;

    outFile = fopen(outFileName, "w");
    if (comments != 0)
    {
        int i = 0;
        for (; i < commentsSize; ++i)
        {
            fprintf(outFile, "#%s\n", comments[i]);
        }
    }
    pwmStr = pwmToString(pwm, fieldSize);
    fprintf(outFile, "%s", pwmStr);
    free(pwmStr);

    fclose(outFile);
    return;
} /* pwmWriteToFile */

/* test     
void main(int argc, char **argv)
{
    PWM* pwm;
    int i, j;

    assert(argc == 3);
    pwm = pwmRead(argv[1]);
    for(i = 0; i < pwm->alphabetSize; ++i)
    {
        printf("%c ", pwm->alphabet[i]);
        for (j = 0; j < pwm->alphabetSize; ++j)
        {
            printf("% 5.1lf  ", pwm->matrix[pwm->alphabet[i]][pwm->alphabet[j]]);    
        }
        printf("\n");
    }
    //printf("%s", pwmToString(pwm, 4));
    char* comments[2];
    comments[0] = "It is example";
    comments[1] = "It works!!!";
    pwmWriteToFile(argv[2], pwm, comments, 2, 4);
    pwmDelete(pwm);
    return;
}
*/

