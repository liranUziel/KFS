#ifndef KFS_H

#define KFS_H
#include "BlockDevice.h"
#include "Partition.h"
#include "Helper.h"
#include "FileTree.h"

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
    short inode;
    char type;
    unsigned int addr;
    unsigned int size;
} KFS_Entry;

typedef KFS_Entry* KFS_Table;


typedef struct _KFS_directory_entry
{
    short inode;
    char name[8];
} KFS_directory_entry;
typedef struct _KFS_directory
{
    int count;
    KFS_directory_entry entries[KFS_TABLE_SIZE];
} KFS_directory;

typedef enum _LS_FLAGS{
    ls_NONE,
    ls_ALL,
    ls_LIST,
    ls_LIST_ALL,
} LS_FLAGS;


// CMD
// TOUCH
void createFile(char* path);
// LS
void listFile(char* path);

void createFolder(char* path);
int findDirectory(char* path);

void printFolderName(int inode);

char* getPath(void);
void changeDir(char* path);

bool validPath(char* path);
void pathSimplyfy(char* path);

int getPageAddress(int indoe);
int findPageInode(char* path);
void clearPage(int addr);
void setPageContent(int addr,char* content);
char* getPageContent(int addr);

KFS_directory readDirectory(unsigned short inode);
void writeDirectory(unsigned short inode, KFS_directory* dir);
void loadKFS(BlockDevice *device, Partition partition);

KFS_Table buildKFSTable(unsigned int partitionStart);
KFS_Table KFSformatSystem(BlockDevice *device, PartitionTable* pt,int selectedPartitionID);
KFS_Table readKFS(BlockDevice *device, Partition partition);
KFS_Table KFSformat(BlockDevice *device, Partition partition);
void initializeKFS(Partition partition);
void readKFSHeader(BlockDevice *device, KFS_HEADER* kfs_header,unsigned int kfsIndex, unsigned int partitionOffset);
void readKFSTable(BlockDevice *device, KFS_Table table, unsigned int kfsIndex, unsigned int partitionOffset);
void buildFileTree(KFS_directory root);
void buildSubTree(FileNode* node, KFS_directory dir,const char* path);
void printTree(void);
void printTreeInfo(FileNode* root);
void printSubTreeInfo(FileNode* node);

void dumpKFSHeader();
void dumpKFS(KFS_Table table);

#endif // KFS_H