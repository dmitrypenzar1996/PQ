#include <stdio.h>
#include <string.h>
#include "HashAlignment.h"
#include "bootstrap.h"

int main(int argNum, char** args)
{
    HashAlignment* inAlignment = NULL;
    HashAlignment* alignmentSample = NULL;

    if (argNum != 4 && argNum != 5)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }

    char* alignment_name = args[1];
    char* method = args[2];
    char* out_file_name = args[3];
    double removeFraction = 0.0;

    if (strcmp(method, "jackknife") == 0)
    {
        if (argNum != 5)
        {
            fprintf(stderr, "removeFraction option for jackknife is required\n");
            exit(1);
        }
        inAlignment = hashAlignmentRead(alignment_name);
        removeFraction = atof(args[4]);
        alignmentSample = hashAlignmentJackknife(inAlignment, removeFraction);
        hashAlignmentDelete(inAlignment);
    }
    else if (strcmp(method, "bootstrap") == 0)
    {
        if (argNum != 4)
        {
            fprintf(stderr, "Wrong number of arguments\n");
            exit(1);
        }
        inAlignment = hashAlignmentRead(alignment_name);
        alignmentSample = hashAlignmentBootstrap(inAlignment);
        hashAlignmentDelete(inAlignment);
    }
    else
    {
        fprintf(stderr, "Wrong option for sampling method\n");
        exit(1);
    }

    hashAlignmentWrite(alignmentSample, out_file_name);
    hashAlignmentDelete(alignmentSample);
}
