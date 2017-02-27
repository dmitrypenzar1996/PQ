#ifndef __P_W_M_H
#define __P_W_M_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "add.h"

typedef struct
{
    int** matrix;
    char* name;
    char* alphabet;
    unsigned char matrixSize;
    unsigned char alphabetSize;
}PWM;

int** matrixCreate(unsigned char size);

void matrixDelete(int** matrix, unsigned char size);

PWM* pwmCreate(void);

void pwmDelete(PWM* pwm);

PWM* pwmRead(char* inFileName);

char* pwmToString(PWM* pwm, unsigned char fieldSize);

void pwmWriteToFile(char* outFileName, PWM* pwm, char** comments,\
        int commentsSize, int fieldSize); 
#endif
