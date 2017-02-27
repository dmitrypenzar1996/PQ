#include "branchQuality.h"
#include <stdlib.h>
#include "Tree.h"
typedef struct bracnhNNIQuality
{
    double* quality;
    size_t len; 
}branchNNIQ;

bracnhNNIQ* branchNNIQCreate(size_t len)
{
    branchNNIQ q = malloc(sizeof(bracnhNNIQ));
    q->quality = malloc(sizeof(double) * len);
    q->len = len;
    return q;
}

void bracnhNNIQDelete(bracnhNNIQ* q)
{
    free(q->quality);
    free(q);
}

int main()
{

    return 0;
}
