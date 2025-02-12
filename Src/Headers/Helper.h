#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include "./BlockDevice.h" 
#include "./Partition.h"
#include "./KFS.h"
#include "./Terminal.h"
#include "./Ctring.h"
#include "./Partition.h"

typedef struct _Arg{
    char* name;
    char* value;
}Arg;

typedef struct _Args{
    Arg* args;
    int count;
}Args;

// typedef Arg* Args;

typedef struct _PathPart{
    char* dir;
}PathPart;

typedef struct _Path{
    PathPart* parts;
    int count;
}Path;

typedef enum _PRINT_STATE{
    INIT,
    PARTITION_HEADER_PRINT,
    PARTITION_TABLE_PRINT,
    KFS_HEADER_PRINT,
    KFS_TABLE_PRINT,
    KFS_CONTENT_PRINT,
    KFS_DOTTED_LINE_hex,
} PRINT_STATE;

typedef enum _PRINT_STATE_ASCII{
    ASCII_INIT,
    ASCII_PARTITION_HEADER_PRINT,
    ASCII_PARTITION_TABLE_PRINT,
    ASCII_KFS_HEADER_PRINT,
    ASCII_KFS_TABLE_PRINT,
    ASCII_CONTENT_PRINT,
    KFS_DOTTED_LINE_ascii,
} PRINT_STATE_ASCII;


// Memory dump helper functions
void setColor(int color);
void dumpBlockDevice(BlockDevice *device,unsigned int size);
bool checkValidAddress(unsigned int address, unsigned int row, unsigned int col);
bool sumOfLineZ(unsigned char *data, unsigned int row, unsigned int size);


Path splitPath(char *path);
Args splitArgs(char *input);



#define NO_GUI_FLAG "-nGUI"             // disable GUI of partition
#define NO_PARTITION_FLAG "-nP"         // disable partitioning
#define NO_KFS_FLAG "-nKFS"             // disable KFS
#define PAGE_SIZE_FLAG "-pS"           // define the size of pages
#define BLOCK_DEVICE_SIZE_FLAG "-bdS"   // define the size of block device

#define CURRENT_DIR "."
#define PARENT_DIR ".."


// char* args_list [] = {
//     NO_GUI_FLAG,
//     NO_PARTITION_FLAG,
//     NO_KFS_FLAG,
//     PAGE_SIZE_FLAG,
//     BLOCK_DEVICE_SIZE_FLAG
// };

#endif // HELPER_H

/*
if(HexPrinted < printingDataSizeHex){
    setColor(LIGHT_BLUE);
    printf("%02x ", data[r * COLUMNS + c]);
    HexPrinted++;
}else{
    state = KFS_HEADER_PRINT;
    HexPrinted = 0;
    printingDataSizeHex =  KFS_HEADER_SIZE;
    c--;
}
*/