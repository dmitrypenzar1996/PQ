#include "HashAlignment.h"

const size_t DEFAULT_HASH_ALIGNMENT_TABLE_SIZE = 10;
const size_t DEFAULT_HASH_ALIGNMENT_LIST_SIZE = 5;
const size_t DEFAULT_HASH_ALIGNMENT_REORGANISE_ATTEMPTS = 3;

RecordList** tableCreate(int tableSize)
{
    
    RecordList** table = (RecordList**)malloc(sizeof(RecordList*) * tableSize);
    int i;

    for(i = 0; i < tableSize; ++i)
    {
        table[i] = recordListCreate(0);
    }
    return table;
}

void tableDelete(RecordList** table, int tableSize)
{
    int i;

    for(i = 0; i < tableSize; ++i)
    {
        recordListRemoveFromRoot(table[i]);
    }
    free(table);
}

HashAlignment* hashAlignmentCreate(int tableSize, char listMaxSize, char maxReorganiseAttempts)
{
    HashAlignment* hashAlignment;

    hashAlignment = (HashAlignment*)malloc(sizeof(HashAlignment));

    hashAlignment->table = tableCreate(tableSize);
    hashAlignment->tableSize = tableSize;

    hashAlignment->sequenceSize = 0;
    hashAlignment->alignmentSize = 0;
    hashAlignment->listMaxSize = listMaxSize;
    hashAlignment->listCurMaxSize = 0;
    hashAlignment->reorganiseAttempts = 0;
    hashAlignment->maxReorganiseAttempts = maxReorganiseAttempts;
    
    return hashAlignment;
}

HashAlignment* hashAlignmentRead(char* inFileName)
{
    HashAlignment* hashAlignment;
    FILE* inFile;
    char* line;
    char firstLine = TRUE;
    char* sequence;
    char* name;
    char* description;
    int strSize;
    int spacePos;

    inFile = fopen(inFileName, "r");
    if(inFile == NULL)
    {
       fprintf(stderr, "Can not open alignment file %s for read\n", inFileName);
       exit(1);
    }

    hashAlignment = hashAlignmentCreate(DEFAULT_HASH_ALIGNMENT_TABLE_SIZE,
            DEFAULT_HASH_ALIGNMENT_LIST_SIZE, DEFAULT_HASH_ALIGNMENT_REORGANISE_ATTEMPTS);

    /* Parsing fasta */
    line = readLine(inFile);
    while (line != NULL)
    {
        if (line[0] == '>')
        {
            if (!firstLine) /*Push previous sequence to alignment */
            {
                sequence = realloc(sequence, sizeof(char) * (strlen(sequence) + 1));
                //printf("Name = %s;\nDescription = %s;\nSequence = %s;\n", name, description, sequence);
                hashAlignmentPut(hashAlignment, recordCreate(name, description, sequence));

            }
            /* New sequence */
            strSize = 400;
            sequence = (char*)malloc(sizeof(char) * strSize);
            sequence[0] = '\0';
            firstLine = FALSE; 
            spacePos = findSymbolPos(line, ' ');

            if (spacePos == -1) 
            {
                name = (char*)malloc(sizeof(char) * (strlen(line) + 1));
                name = memcpy(name, line + 1, strlen(line));
                description = (char*)malloc(sizeof(char));
                description[0] = '\0';
            }
            else
            {
                name = (char*)malloc(sizeof(char) * (spacePos + 1));
                memcpy(name, line + 1, spacePos - 1);
                name[spacePos - 1] = '\0';
                description = (char*)malloc(sizeof(char) * (strlen(line) + 1 - spacePos));
                memcpy(description, line + spacePos + 1, strlen(line) + 1 - spacePos); // \0 will copied too
            }
        }
        else if (line[0] != '>' && firstLine)
        {
            perror("Wrong format! Fasta required.\n");
            exit(1);
            /* some time here should be PHYLIP parser */
        }
        else
        {
            if (strlen(line) + strlen(sequence) >= strSize)
            {
                strSize = (strSize + strlen(sequence))* 3 / 2 + 1;
                sequence = realloc(sequence, sizeof(char) * (strSize + 1));
            }
            memcpy(sequence + strlen(sequence), line, strlen(line) + 1);
        }
        free(line);
        line = readLine(inFile);
    }

    sequence = realloc(sequence, sizeof(char) * (strlen(sequence) + 1)); /* free extra memory */
    //printf("Name = %s;\nDescription = %s;\nSequence = %s;\n", name, description, sequence);
    hashAlignmentPut(hashAlignment, recordCreate(name, description, sequence)); 
    fclose(inFile);
    return hashAlignment;
}

void hashAlignmentDelete(HashAlignment* hashAlignment)
{
    tableDelete(hashAlignment->table, hashAlignment->tableSize);
    free(hashAlignment);
    return;
}

void hashAlignmentReorganise(HashAlignment* hashAlignment)
{
    RecordList** oldTable;
    int i;
    int oldSize;

    if (hashAlignment->reorganiseAttempts > hashAlignment->maxReorganiseAttempts)
    {
        perror("Failed to decrease listSize by increasing tableSize. \
                MaxListSize've been increased.\n");
        hashAlignment->listMaxSize = hashAlignment->listMaxSize * 3 / 2 + 1;
        return;
    }

    ++hashAlignment->reorganiseAttempts;
    
    oldTable = hashAlignment->table;
    oldSize = hashAlignment->tableSize;
    
    hashAlignment->tableSize = oldSize * 3 / 2 + 1;
    
    hashAlignment->alignmentSize = 0;
    hashAlignment->table = tableCreate(hashAlignment->tableSize);
    hashAlignment->listCurMaxSize = 0;
    
    for(i = 0; i < oldSize; ++i)
    {
        RecordList* root = oldTable[i]->next;
        while (root != 0)
        {
            if (root->record != 0)
            {
                hashAlignmentPut(hashAlignment, root->record);
            }
            root = root->next;
        }
    }

    tableDelete(oldTable, oldSize);
    hashAlignment->reorganiseAttempts = 0;
    return;
}

void hashAlignmentPut(HashAlignment* hashAlignment, Record* record)
{
    size_t index;
    int size;
    RecordList* list;

    if (hashAlignment->alignmentSize != 0  && hashAlignment->sequenceSize != record->size)
    {
        perror("Sequences are not the same size");
        exit(1);
    }
    else if(hashAlignment->alignmentSize == 0)
    {
        hashAlignment->sequenceSize = strlen(record->sequence);
    }
   
    list = hashAlignmentGetList(hashAlignment, record->name);     
    if ( list != NULL)
    {
        if (list->record != record) // equality of pointers, it isn't error
        {
            free(list->record);
            list->record = record;
            return;
        }    
    }
    //printf("%s\n", record->name);
    index = hash(record->name) % hashAlignment->tableSize;
    size = recordListInsertToEnd(hashAlignment->table[index], record);
    if (size > hashAlignment->listCurMaxSize)
    {
        if ((hashAlignment->listCurMaxSize = size) > hashAlignment->listMaxSize)
        {
            hashAlignmentReorganise(hashAlignment);
            return;
        }
    }
    ++hashAlignment->alignmentSize;
    return;
} /* hashAlignmentPut */
// put record
// return size list was been updated

Record*  hashAlignmentGet(HashAlignment* hashAlignment, char* name)
{
    int index;
    RecordList* recordList;
   
    index = hash(name) % hashAlignment->tableSize;
    recordList = recordListFind(hashAlignment->table[index], name);
    if (recordList != 0)
    {
        return recordList->record;
    }
    else
    {
        return 0;
    }
} //return record by name

RecordList*  hashAlignmentGetList(HashAlignment* hashAlignment, char* name)
{
    int index;
    RecordList* recordList;

    index = hash(name) % hashAlignment->tableSize;
    recordList = recordListFind(hashAlignment->table[index], name);
    if (recordList != NULL)
    {
        return recordList;
    }
    else
    {
        return NULL;
    }
} //return recordList by name

char** hashAlignmentGetSeqNames(HashAlignment* alignment)
{
    char** names = (char**)malloc(sizeof(char*) * (alignment->alignmentSize));
    int i;
    int k = 0;
    for (i = 0; i < alignment->tableSize; ++i)
    {
        RecordList* current = alignment->table[i]->next;
        while (current != 0)
        {
            names[k] = current->record->name;
            ++k;
            current = current->next;
        }
    }
    return names;
}


HashAlignment* hashAlignmentSeqShuffle(HashAlignment* alignment)
{
    int i;
    Record* curRecord;
    Record* newRecord;
    char** names;
    HashAlignment* shAlignment;

    names = hashAlignmentGetSeqNames(alignment);
    shAlignment = hashAlignmentCreate(alignment->tableSize, 
             alignment->listMaxSize, alignment->maxReorganiseAttempts);
    
    for(i = 0; i < alignment->alignmentSize; ++i)
    {
        curRecord = hashAlignmentGet(alignment, names[i]);
        newRecord = recordCreate(\
                curRecord->name,
                curRecord->description,
                seqShuffle(curRecord->sequence));
        //printf("%lu %u\n", strlen(curRecord->sequence), alignment->sequenceSize);
        //printf(">%s\n%s\n", curRecord->name, curRecord->sequence);
        //printf(">%s\n%s\n", newRecord->name, newRecord->sequence);
        hashAlignmentPut(shAlignment, newRecord);
    }
    free(names);

    return shAlignment;
}

void hashAlignmentWrite(HashAlignment* alignment, char* out_file_name)
{
    int i = 0;
    FILE* in_file = fopen(out_file_name, "w");
    char** names = hashAlignmentGetSeqNames(alignment);
    Record* curRecord = NULL;
    for(i = 0; i < alignment->alignmentSize; ++i)
    {
        curRecord = hashAlignmentGet(alignment, names[i]);
        fprintf(in_file, ">%s\n%s\n", curRecord->name, curRecord->sequence);
    }
    free(names);
    fclose(in_file);
    return;
}
