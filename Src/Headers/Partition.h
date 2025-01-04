#ifndef PARTITION_H
#define PARTITION_H

#include "BlockDevice.h"

typedef struct _Partition_header{
    char magic[5];
    char name[8];
    int version;
    int count;
    int size;
}Partition_HEADER;


typedef struct _Partition{
    unsigned int addr;
    unsigned int size;
    char name[2]; 
}Partition;

typedef struct _PartitionTable{
    Partition* partitions;
    unsigned int size;
    unsigned int count;
} PartitionTable;
//PartitionTable

// extern const struct PartitionTable ptTable;


extern int currentPartition;
void createPartitionTable(PartitionTable** pt, int size);
void addPartition(PartitionTable** pt,unsigned int size);
void writePartitionTable(BlockDevice* device, PartitionTable* pt);
void readPartitionTable(BlockDevice* device, PartitionTable** pt);


void format(BlockDevice *device, PartitionTable* pt,unsigned int partitionIndex);

void PizzaPartitionSystem(PartitionTable **pt, BlockDevice *device);

void printPartitionTable(PartitionTable* pt);


#endif // PARTITION_H