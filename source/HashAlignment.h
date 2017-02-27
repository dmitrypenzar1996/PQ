#ifndef __HASH_ALIGN__H
#define __HASH_ALIGN__H

#include <stdio.h>
#include <stdlib.h>
#include "Record.h"
#include "RecordList.h"
#include "add.h"

typedef struct
{
    RecordList** table;
    unsigned tableSize;
    unsigned sequenceSize;
    unsigned alignmentSize;
    char listMaxSize;
    char listCurMaxSize;
    char reorganiseAttempts;
    char maxReorganiseAttempts;
}HashAlignment;

HashAlignment* hashAlignmentCreate(int tableSize, char listMaxSize,\
        char maxReorganiseAttempts);

HashAlignment* hashAlignmentRead(char* inFileName);

void hashAlignmentDelete(HashAlignment* hashAlignment);

void hashAlignmentReorganise(HashAlignment* hashAlignment);

void hashAlignmentPut(HashAlignment* hashAlignment, Record* record); // put record
// return size of list was been updated

Record* hashAlignmentGet(HashAlignment* hashAlignment, char* name); //return sequence by name

RecordList* hashAlignmentGetList(HashAlignment* hashAlignment, char* name); //return sequence by name

char** hashAlignmentGetSeqNames(HashAlignment* alignment);

HashAlignment* hashAlignmentSeqShuffle(HashAlignment* alignment);
#endif
