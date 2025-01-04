#include "./Headers/BlockDevice.h"

BlockDevice* createBlockDevice(char* blockName,unsigned int size)
{
    BlockDevice *device = (BlockDevice*)malloc(sizeof(BlockDevice));
    if(device == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for block device\n");
        return NULL;
    }

    device->hFile = CreateFile(blockName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (device->hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: failed to create file\n");
        free(device);
        return NULL;
    }

    device->hMapFile = CreateFileMapping(device->hFile, NULL, PAGE_READWRITE, 0, size * sizeof(char), NULL);
    if (device->hMapFile == NULL) {
        fprintf(stderr, "Error: failed to create file mapping\n");
        CloseHandle(device->hFile);
        free(device);
        return NULL;
    }

    device->data = (char*)MapViewOfFile(device->hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (device->data == NULL) {
        fprintf(stderr, "Error: failed to map view of file\n");
        CloseHandle(device->hMapFile);
        CloseHandle(device->hFile);
        free(device);
        return NULL;
    }

    device->size = size;
    return device;
}

FILE_IO_STATUS writeBlockDevice(BlockDevice *device, int addr, int size, char *ans)
{
    if(device == NULL) {
        fprintf(stderr, "Error: block device is NULL\n");
        return FILE_IO_ERROR;
    }
    if(addr < 0 || addr + size > device->size) {
        fprintf(stderr, "Error: write address out of bounds\n");
        return FILE_IO_ERROR;
    }
    memcpy(device->data + addr, ans, size); // Corrected size calculation
    return FILE_WRITE_SUCCESS;
}

FILE_IO_STATUS readBlockDevice(BlockDevice *device, int addr, int size, char *ans)
{
    if(device == NULL) {
        fprintf(stderr, "Error: block device is NULL\n");
        return FILE_IO_ERROR;
    }
    if(addr < 0 || addr + size > device->size) {
        fprintf(stderr, "Error: read address out of bounds\n");
        return FILE_IO_ERROR;
    }
    memcpy(ans, device->data + addr, size); // Corrected size calculation
    return FILE_READ_SUCCESS;
}

void closeBlockDevice(BlockDevice *device)
{
    if (device->data != NULL) {
        UnmapViewOfFile(device->data);
    }
    if (device->hMapFile != NULL) {
        CloseHandle(device->hMapFile);
    }
    if (device->hFile != NULL) {
        CloseHandle(device->hFile);
    }
    free(device);
}