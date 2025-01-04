#ifndef KFS_H

#define KFS_H
#include "BlockDevice.h"
#include "Partition.h"


// defiune a struct that represent the file system.
// with a magic number and a version number.

typedef struct KFS_HEADER
{
    char magic[4];
    unsigned int address;
    int version;
} KFS_HEADER;

typedef struct _KFS_Entry
{
    unsigned short inode;
    char type;
    unsigned int addr;
    unsigned int size;
} KFS_Entry;


typedef struct _KFS_directory
{
    unsigned short inode;
    char name[8];
    unsigned short parent;
} KFS_directory;

void buildKFSTable(unsigned int partitionStart);
void format(BlockDevice *device, PartitionTable* pt,unsigned int partitionIndex);

#endif // KFS_H