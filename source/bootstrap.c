#include "bootstrap.h"


char* seqPermutate(char* string, size_t* permutation, size_t size)
{
    int i = 0;
    char* permString = malloc(sizeof(char) * (size + 1));
    permString[size] = '\0';
    for(i = 0; i < size; ++i)
    {
        permString[i] = string[permutation[i]];
    }
    return permString;
}

HashAlignment* hashAlignmentBootstrap(HashAlignment* source)
{
    HashAlignment* bootstrapResult = 
        hashAlignmentCreate(DEFAULT_HASH_ALIGNMENT_TABLE_SIZE,
            DEFAULT_HASH_ALIGNMENT_LIST_SIZE,
            DEFAULT_HASH_ALIGNMENT_REORGANISE_ATTEMPTS);
    Record* record = NULL;
    char** seqNames = hashAlignmentGetSeqNames(source);
    size_t* permutation = getPermutationRep(source->sequenceSize); 
    int i = 0;
    char* name = NULL;
    
    for(i = 0; i < source->alignmentSize; ++i)
    {
        name = malloc(sizeof(char) * (strlen(seqNames[i]) + 1));
        record = hashAlignmentGet(source, seqNames[i]);
        hashAlignmentPut(bootstrapResult, 
                recordCreate(record->name, NULL, 
                    seqPermutate(record->sequence, permutation, 
                        source->sequenceSize)));
    }
    free(seqNames);
    free(permutation);
    return bootstrapResult;
}

HashAlignment* hashAlignmentJackknife(HashAlignment* source, double fraction)
// fraction to delete
{
    HashAlignment* jackKnifeResult = 
        hashAlignmentCreate(DEFAULT_HASH_ALIGNMENT_TABLE_SIZE,
            DEFAULT_HASH_ALIGNMENT_LIST_SIZE,
            DEFAULT_HASH_ALIGNMENT_REORGANISE_ATTEMPTS);
    Record* record = NULL;
    char** seqNames = hashAlignmentGetSeqNames(source);
    size_t partSize = source->sequenceSize * (1 - fraction);
    size_t* permutation = getPermutationPart(source->sequenceSize, partSize);
    char* name = NULL;
    int i = 0;

    for(i = 0; i < source->alignmentSize; ++i)
    {
        name = malloc(sizeof(char) * (strlen(seqNames[i]) + 1));
        record = hashAlignmentGet(source, seqNames[i]);
        hashAlignmentPut(jackKnifeResult, 
                recordCreate(record->name, NULL, 
                    seqPermutate(record->sequence, permutation, 
                        partSize)));
    }

    
    return jackKnifeResult;
}
