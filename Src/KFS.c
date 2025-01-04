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

KFS_Entry KFS_table[100];


void format(BlockDevice *device, PartitionTable* pt,unsigned int partitionIndex){
    // format the block device
    unsigned int partitionStart = pt->partitions[partitionIndex].addr;
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
        writeBlockDevice(device, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * 100, (char*)KFS_table);
    }
}

void buildKFSTable(unsigned int partitionStart){
    //get the partition and build the KFS table
    for(size_t i; i < 100; i++){
        KFS_table[i].inode = i;
    }
    KFS_table[0].type = 'd';
    KFS_table[0].addr = partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * 100;
    KFS_table[0].size = 0;
}


