#ifndef __RECORD__H_
#define __RECORD__H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "add.h"
#include "Record.h"

typedef struct
{
    size_t size;
    char* sequence;
    char* name;
    char* description;
}Record;

Record* recordCreate(char* name, char* description, char* sequence);

void recordDelete(Record* record);

Record* recordRead(FILE* inFile);
#endif
