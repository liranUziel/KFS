/**
 * @file KFS.c
 * @author Liran (liran.uzi.l@gmail)
 * @brief 
 * @version 0.1
 * @date 2024-12-18
 * 
 * @copyright Copyright (c) 2024
 * 
 * @section DESCRIPTION
 * KFS.c is the main file of the KFS project.
 * Containts the following  functions:
 */


#include "./Headers/KFS.h"

KFS_Entry KFS_table[KFS_TABLE_SIZE];
int FreeInodes[KFS_TABLE_SIZE] = {0};

BlockDevice blockDevice;
Partition currentPartition;
unsigned int partitionStart = 0;


void initializeKFS(Partition partition){
    currentPartition = partition;
    partitionStart = partition.addr;
    dumpKFS();
}


void format(BlockDevice *device, Partition partition){
    // format the block device
    unsigned int partitionStart = partition.addr;
    KFS_HEADER header;
    readBlockDevice(device, partitionStart, sizeof(KFS_HEADER), (char*)&header);
    if( strcmp(header.magic, "KFS") == 0 ) {
        fprintf(stdout, "Loading KFS...\n");
    }else{
        fprintf(stdout, "Initializing KFS...\n");
        strcpy(header.magic, "KFS");
        header.version = 1;
        header.address = partitionStart;
        buildKFSTable(partitionStart);
        writeBlockDevice(device, partitionStart, sizeof(KFS_HEADER), (char*)&header);
        writeBlockDevice(device, header.address + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    }
    blockDevice = *device;
    
    KFS_directory root;
    root.count = 0;
    writeBlockDevice(device, partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * KFS_TABLE_SIZE, sizeof(KFS_directory), (char*)&root);
}

void loadKFS(BlockDevice *device, Partition partition){
    blockDevice = *device;
    initializeKFS(partition);
    readBlockDevice(device, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        if(KFS_table[i].type == 'u'){
            FreeInodes[i] = 0;
        }else{
            FreeInodes[i] = 1;
        }
    }
    
}

void buildKFSTable(unsigned int partitionStart){
    //get the partition and build the KFS table

    // create root directory

    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        KFS_table[i].inode = i;
        KFS_table[i].type = 'u';
    }
    KFS_table[0].type = 'd';
    KFS_table[0].addr = partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * KFS_TABLE_SIZE;
    KFS_table[0].size = 0;
    FreeInodes[0] = 1;
}
/**
 * @brief Create a File object
 * 
 * @param path 
 * @param size 
 * @details Find a free inode and assign it to the file
 */
void createFile(char* path){

    int freeBlock = -1;
    for(size_t i = 0;i < KFS_TABLE_SIZE && freeBlock == -1; i++){
        if(FreeInodes[i] == 0){
            FreeInodes[i] = 1;
            KFS_table[i].type = 'f';
            KFS_table[i].size = 0;
            KFS_table[i].addr = KFS_table[0].addr + PAGE_SIZE;
            freeBlock = i;
        }
    }
    writeBlockDevice(&blockDevice, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    if(freeBlock == -1){
        fprintf(stderr, "Error: No free inodes\n");
        return;
    }
    KFS_directory root = readDirectory(0);
    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        if(root.entries[i].inode == 0){
            root.entries[i].inode = freeBlock;
            strcpy(root.entries[i].name, path);
            break;
        }
    }
    writeDirectory(0, &root);
}

KFS_directory readDirectory(unsigned short inode){
    KFS_directory dir;
    int address = KFS_table[inode].addr;
    readBlockDevice(&blockDevice, address, sizeof(KFS_directory), (char*)&dir);
    return dir;   
}

void writeDirectory(unsigned short inode, KFS_directory* dir){
    int address = KFS_table[inode].addr;
    writeBlockDevice(&blockDevice, address, sizeof(KFS_directory), (char*)dir);
}


void dumpKFSHeader(){
    KFS_HEADER header;
    readBlockDevice(&blockDevice, partitionStart, sizeof(KFS_HEADER), (char*)&header);
    fprintf(stdout, "KFS Header\n");
    fprintf(stdout, "Magic: %s\n", header.magic);
    fprintf(stdout, "Version: %d\n", header.version);
    fprintf(stdout, "Address: %d\n", header.address);
}

void dumpKFS(){
    fprintf(stdout, "Partion Name: %s\n", currentPartition.name);
    dumpKFSHeader();
    fprintf(stdout, "KFS Table\n");
    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        if(KFS_table[i].type != 'u'){
            fprintf(stdout, "Inode: %d, Type: %c, Address: %d, Size: %d\n", KFS_table[i].inode, KFS_table[i].type, KFS_table[i].addr, KFS_table[i].size);
        }
    }
}