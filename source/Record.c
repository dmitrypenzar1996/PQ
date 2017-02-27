#include "Record.h"

Record* recordCreate(char* name, char* description, char* sequence)
{
    Record* record;
    record = (Record*)malloc(sizeof(Record));
    record->name = name;
    record->description = description;
    record->sequence = sequence;
    record->size = strlen(sequence);
    return record;
}


void recordDelete(Record* record)
{
    free(record->name);
    free(record->description);
    free(record->sequence);
    free(record);
    return;
}


Record* recordRead(FILE* inFile)
{   
    
    char* line;
    int spacePos;
    int strSize;
    char* name;
    char* description;
    char* sequence;

    line = readLine(inFile);
    if (line == 0)
    {
        return 0;        
    }
    else if (line[0] != '>')
    {
        perror("Wrong file format. Fasta required");
        exit(1);
    }
    //printf("%s", line);
    spacePos = findSymbolPos(line, ' ');
    //printf("%d\n", spacePos);
    if (spacePos == -1) 
    {
        name = line;
        description = (char*)malloc(sizeof(char));
        description[0] = '\0';
    }
    else
    {
        name = (char*)malloc(sizeof(char) * (spacePos + 1));
        memcpy(name, line + 1, spacePos - 1);
        name[spacePos] = '\0';
    
        description = (char*)malloc(sizeof(char) * (strlen(line) + 1 - spacePos));
        memcpy(description, line + spacePos + 1, strlen(line) + 1 - spacePos); // \0 will copied too
        free(line);
    }
    
    strSize = 400;
    sequence = (char*)malloc(sizeof(char) * strSize);
    sequence[0] = '\0';
    while ((line = readLine(inFile)) != 0)
    {
        if (line[0] == '>')
        {
            perror("This function must be applied to files containing single sequence");
            exit(1);
        }
        if (strlen(line) + strlen(sequence) >= strSize)
        {
            strSize = strSize * 3 / 2 + 1;
            sequence = realloc(sequence, sizeof(char) * strSize);
        } 
        memcpy(sequence + strlen(sequence), line, strlen(line) + 1);
    } 
    sequence = realloc(sequence, sizeof(char) * (strlen(sequence) + 1));
    
    fclose(inFile);
    //printf("%s\n", sequence);
    return recordCreate(name, description, sequence);
} /* recordRead */ 

