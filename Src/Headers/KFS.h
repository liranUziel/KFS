#ifndef KFS_H

#define KFS_H
#include "BlockDevice.h"
#include "Partition.h"

BlockDevice blockDevice;

#define PAGE_SIZE 256
#define KFS_TABLE_SIZE 10

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

typedef struct _KFS_directory_entry
{
    unsigned short inode;
    char name[8];
} KFS_directory_entry;
typedef struct _KFS_directory
{
    int count;
    KFS_directory_entry entries[KFS_TABLE_SIZE];
} KFS_directory;




// CMD
void createFile(char* path);

KFS_directory readDirectory(unsigned short inode);
void writeDirectory(unsigned short inode, KFS_directory* dir);
void loadKFS(BlockDevice *device, Partition partition);

void buildKFSTable(unsigned int partitionStart);
void format(BlockDevice *device, Partition partition);
void initializeKFS(Partition partition);


void dumpKFSHeader();
void dumpKFS();

#endif // KFS_H