#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include "HashAlignment.h"
#include "add.h"
#include "Record.h"
#include "stdlib.h"

extern const size_t DEFAULT_HASH_ALIGNMENT_TABLE_SIZE;
extern const size_t DEFAULT_HASH_ALIGNMENT_LIST_SIZE;
extern const size_t DEFAULT_HASH_ALIGNMENT_REORGANISE_ATTEMPTS;
char* seqPermutate(char* string, size_t* permutation, size_t size);
HashAlignment* hashAlignmentBootstrap(HashAlignment* source);
HashAlignment* hashAlignmentJackknife(HashAlignment* source, double fraction);

#endif
