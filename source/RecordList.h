#ifndef __RECORD_LIST__H
#define __RECORD_LIST__H

#include <stdlib.h>
#include "Record.h"

struct RecordList;
typedef struct RecordList RecordList;

struct RecordList
{
    Record* record;
    RecordList* next;
};

RecordList* recordListCreate(Record* record);

void recordListDelete(RecordList* list);

int recordListInsertToEnd(RecordList* recordList ,Record* record); 
//return List size after record insertion; 

RecordList* recordListFind(RecordList* list, char* name);

void recordListRemoveFromRoot(RecordList* root);

int recordListInsertToEnd(RecordList* recordList, Record* record); 
 
RecordList* recordListFind(RecordList* list, char* name);

#endif
