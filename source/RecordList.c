#include "RecordList.h"

RecordList* recordListCreate(Record* record)
{
    RecordList* recordList;
   
    recordList = (RecordList*)malloc(sizeof(RecordList));
    recordList->record = record;
    recordList->next = 0;
    return recordList;
} /* recordListCreate */

void recordListDelete(RecordList* recordList)
{
    if (recordList != 0)
    {/*
        if (recordList->record != 0)
        {
            recordDelete(recordList->record);
        }*/
        free(recordList);
    }
    return;
} /* recordListDelete */


void recordListRemoveFromRoot(RecordList* root)
{
    RecordList* temp = NULL;
    while (root != 0)
    {
        temp = root->next;
        recordListDelete(root);
        root = temp;
    }
    return;
} /* recordListRemoveFromRoot */

int recordListInsertToEnd(RecordList* recordList, Record* record) 
//return List size after record insertion; 
{
    int i = 1; 
    while(recordList->next != 0)
    {
        recordList = recordList->next;
        ++i;
    }
    recordList->next = recordListCreate(record);
    
    return i;
} /* recordListInsertToEnd */

RecordList* recordListFind(RecordList* list, char* name)
{
    while (list != 0)
    {
        if (list->record != 0)
        {
            if (strcmp(list->record->name, name) == 0)
            {
                return list;
            }
        }
        list = list->next;
    }
    return 0;
} /* recordListFind */

