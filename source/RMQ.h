#ifndef __RMQ__H__
#define __RMQ__H__
#include "add.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned* segment;
    unsigned** table;
    unsigned length;
    unsigned height;
}SparseTable;

void sparseTableDelete(SparseTable* sparseTable);

SparseTable* sparseTableCalculate(unsigned* segment, unsigned length);

unsigned sparseTableRMQ(SparseTable* sparseTable, unsigned i, unsigned j);


#endif
