#ifndef PARTITION_H
#define PARTITION_H

#include "BlockDevice.h"
#include "Table.h"
#include <stdbool.h>
typedef struct _Partition_header{
    char magic[5];
    char name[8];
    char defultPartition;
    int version;
    int count;
    int size;
}Partition_HEADER;


typedef struct _Partition{
    unsigned int addr;
    unsigned int size;
    bool formatted;
    char name[2]; 
}Partition;

typedef struct _PartitionTable{
    Partition* partitions;
    unsigned int size;
    unsigned int count;
} PartitionTable;
//PartitionTable

typedef enum _PartitionError{
    PARTITION_OK,
    PARTITION_OUT_OF_BOUNDS,
    PARTITION_INVALID_NAME,
    PARTITION_INVALID_SIZE
}PartitionError;

// extern const struct PartitionTable ptTable;


extern int currentPartitionID;
// extern unsigned int FreeSpace;
// extern Partition_HEADER header;

void createPartitionTable(PartitionTable** pt, int size);
void addPartition(PartitionTable** pt,unsigned int size);
void writePartitionTable(BlockDevice* device, PartitionTable* pt);
void readPartitionTable(BlockDevice* device, PartitionTable** pt);
void readPartitionHeader(BlockDevice* device, Partition_HEADER* header); 

PartitionTable* partitionMenu( BlockDevice* device, int* selectedPartitionID,bool GUI);
Table parnitnionMenuText(int* seletedPartition);
bool isPartitionTableValid(BlockDevice* device);
void buildPartitionTable(PartitionTable **partitionTable,Table table);


void printPartitionTable(PartitionTable* pt);

void dumpPartitionHeader();
void dumpPartitionTable(PartitionTable* pt);

#endif // PARTITION_Hq