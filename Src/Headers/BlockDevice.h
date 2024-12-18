#ifndef BLOCK_DEVICE_H

#define BLOCK_DEVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>    

typedef enum {
    FILE_WRITE_SUCCESS = 1,
    FILE_READ_SUCCESS = 2,
    FILE_IO_ERROR = -1
} FILE_IO_STATUS;

typedef struct {
    unsigned int size;
    char *data;
    HANDLE hFile;
    HANDLE hMapFile;
} BlockDevice;



BlockDevice* createBlockDevice(char* blockName,unsigned int size);
FILE_IO_STATUS writeBlockDevice(BlockDevice *device,int addr, int size, char *ans);
FILE_IO_STATUS readBlockDevice(BlockDevice *device, int addr, int size, char *ans);
void closeBlockDevice(BlockDevice *device);

#endif // BLOCK_DEVICE_H