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


void format(BlockDevice *device){
    // format the block device
    KFS_HEADER header;
    readBlockDevice(device, 0, sizeof(KFS_HEADER), (char*)&header);
    if( strcmp(header.magic, "KFS") == 0 ) {
        fprintf(stdout, "Loading KFS...\n");
    }else{
        fprintf(stdout, "Initializing KFS...\n");
        strcpy(header.magic, "KFS");
        header.version = 1;
        writeBlockDevice(device, 0, sizeof(KFS_HEADER), (char*)&header);
    }
}




