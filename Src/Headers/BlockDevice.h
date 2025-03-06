#ifndef BLOCK_DEVICE_H

#define BLOCK_DEVICE_H


#ifdef _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>  
#endif
#ifdef __linux__
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

// Sizes
#define KiloByte 1024
#define MegaByte 1024*1024
#define GigaByte 1024*1024*1024

#define BLOCK_SIZE 1024*1024



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

BlockDevice* createBlockDeviceW(char* blockName, unsigned int size);
FILE_IO_STATUS writeBlockDeviceW(BlockDevice *device,int addr, int size, char *ans);
FILE_IO_STATUS readBlockDeviceW(BlockDevice *device, int addr, int size, char *ans);
void closeBlockDeviceW(BlockDevice *device);

BlockDevice* createBlockDeviceL(char* blockName, unsigned int size);
FILE_IO_STATUS writeBlockDeviceL(BlockDevice *device,int addr, int size, char *ans);
FILE_IO_STATUS readBlockDeviceL(BlockDevice *device, int addr, int size, char *ans);
void closeBlockDeviceL(BlockDevice *device);

BlockDevice* createBlockDevice(char* blockName,unsigned int size);
FILE_IO_STATUS writeBlockDevice(BlockDevice *device,int addr, int size, char *ans);
FILE_IO_STATUS readBlockDevice(BlockDevice *device, int addr, int size, char *ans);
void closeBlockDevice(BlockDevice *device);

#endif // BLOCK_DEVICE_H