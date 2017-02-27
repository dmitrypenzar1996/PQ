#ifndef __ADD__H
#define __ADD__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define true 1
#define false 0

//typedef uint_fast64_t INT; add #include <stdint.h> if uncomment this

typedef unsigned long INT; /* enough for protein alignments up to 3000 sequences */

int findSymbolPos(char* string, char symbol);

char* readLine(FILE* inFile);

int pow_int(int base, int power);

unsigned int hash(char* string);

int* randomChoice(unsigned int numToChoose, int* array, unsigned int arraySize);

int* getRange(int a, int b); // return array with elements from a to b-1

int* getPermutation(unsigned int arraySize);

size_t* getPermutationRep(size_t arraySize);

int* calculatePermutation(char** leaves, char** seqNames, int size);

char* seqShuffle(char* seq);

void raiseError(const char* message, const char* fileName, const char* funcName,
        int lineNum);

size_t* getPermutationPart(size_t arraySize, size_t partSize);
#endif
