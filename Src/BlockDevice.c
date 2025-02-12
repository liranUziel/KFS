#include "./Headers/BlockDevice.h"

// #region Linux
#ifdef __linux__
/**
 * @brief Create a Block Device L object
 * @name createBlockDeviceL
 * @param blockName 
 * @param size 
 * @return BlockDevice* 
 */
BlockDevice* createBlockDeviceL(char* blockName, unsigned int size){
    BlockDevice *device = (BlockDevice*)malloc(sizeof(BlockDevice));
    if(device == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for block device\n");
        return NULL;
    }

    device->hFile = open(blockName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (device->hFile == -1) {
        fprintf(stderr, "Error: failed to open file\n");
        free(device);
        return NULL;
    }

    if (ftruncate(device->hFile, size * sizeof(char)) == -1) {
        fprintf(stderr, "Error: failed to set file size\n");
        close(device->hFile);
        free(device);
        return NULL;
    }

    device->pData = (char*)mmap(NULL, size * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, device->hFile, 0);
    if (device->pData == MAP_FAILED) {
        fprintf(stderr, "Error: failed to map file\n");
        close(device->hFile);
        free(device);
        return NULL;
    }

    return device;
}

int writeBlockDeviceL(BlockDevice* device, unsigned int offset, char* data, unsigned int size)
{
    if (device == NULL || data == NULL) {
        fprintf(stderr, "Error: invalid arguments\n");
        return -1;
    }

    memcpy(device->pData + offset, data, size);
    return 0;
}

int readBlockDeviceL(BlockDevice* device, unsigned int offset, char* buffer, unsigned int size)
{
    if (device == NULL || buffer == NULL) {
        fprintf(stderr, "Error: invalid arguments\n");
        return -1;
    }

    memcpy(buffer, device->pData + offset, size);
    return 0;
}

void closeBlockDeviceL(BlockDevice *device)
{
    if (device == NULL) {
        return;
    }

    if (device->pData != NULL) {
        munmap(device->pData, sizeof(device->pData));
    }
    if (device->hFile != -1) {
        close(device->hFile);
    }

    free(device);
}

#endif // __linux__
// #endregion

// #region Windows
#ifdef _WIN32

BlockDevice* createBlockDeviceW(char* blockName, unsigned int size){
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

FILE_IO_STATUS writeBlockDeviceW(BlockDevice *device, int addr, int size, char *ans){
    if(device == NULL) {
        fprintf(stderr, "Error: block device is NULL\n");
        return FILE_IO_ERROR;
    }
    if(addr < 0 || addr + size > device->size) {
        fprintf(stderr, "Error: write address out of bounds\n");
        return FILE_IO_ERROR;
    }
    memcpy(device->data + addr, ans, size);
    #ifdef DEBUG 
    #endif
    return FILE_WRITE_SUCCESS;
}

FILE_IO_STATUS readBlockDeviceW(BlockDevice *device, int addr, int size, char *ans){
     if(device == NULL) {
        fprintf(stderr, "Error: block device is NULL\n");
        return FILE_IO_ERROR;
    }
    if(addr < 0 || addr + size > device->size) {
        fprintf(stderr, "Error: read address out of bounds\n");
        return FILE_IO_ERROR;
    }
    memcpy(ans, device->data + addr, size);
    #ifdef DEBUG
    printf("Debug mode\n");
    #endif
    return FILE_READ_SUCCESS;
}

void closeBlockDeviceW(BlockDevice *device){
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

#endif // _WIN32
// #endregion

/**
 * @brief Create a Block Device object
 * @name createBlockDevice
 * @algorithm
 * 1. Check if the OS is Windows
 * 2. If it is, call createBlockDeviceW
 * 3. If it is not, call createBlockDeviceL
 * @param blockName 
 * @param size 
 * @return BlockDevice* 
 */
BlockDevice* createBlockDevice(char* blockName,unsigned int size)
{
    #ifdef _WIN32
        return createBlockDeviceW(blockName, size);
    #endif
    #ifdef __linux__
        return createBlockDeviceL(blockName, size);
    #endif
}

/**
 * @brief Write to a block device
 * @name writeBlockDevice
 * @algorithm
 * 1. Check if the OS is Windows
 * 2. If it is, call writeBlockDeviceW
 * 3. If it is not, call writeBlockDeviceL
 * @param device 
 * @param addr 
 * @param size 
 * @param ans 
 * @return FILE_IO_STATUS (FILE_WRITE_SUCCESS, FILE_READ_SUCCESS, FILE_IO_ERROR)
 */
FILE_IO_STATUS writeBlockDevice(BlockDevice *device, int addr, int size, char *ans)
{
    #ifdef _WIN32
        return writeBlockDeviceW(device, addr, size, ans);
    #endif
    #ifdef __linux__
        return writeBlockDeviceL(device, addr, ans, size);
    #endif
}

/**
 * @brief Read from a block device 
 * @name readBlockDevice
 * @algorithm
 * 1. Check if the OS is Windows
 * 2. If it is, call readBlockDeviceW
 * 3. If it is not, call readBlockDeviceL
 * @param device 
 * @param addr 
 * @param size 
 * @param ans 
 * @return FILE_IO_STATUS 
 */
FILE_IO_STATUS readBlockDevice(BlockDevice *device, int addr, int size, char *ans)
{
    #ifdef _WIN32
        return readBlockDeviceW(device, addr, size, ans);
    #endif
    #ifdef __linux__
        return readBlockDeviceL(device, addr, ans, size);
    #endif

}

/**
 * @brief Close a block device and free memory
 * @name closeBlockDevice
 * @algorithm
 * 1. Check if the OS is Windows
 * 2. If it is, call closeBlockDeviceW
 * 3. If it is not, call closeBlockDeviceL 
 * @param device 
 */

void closeBlockDevice(BlockDevice *device)
{
    #ifdef _WIN32
        closeBlockDeviceW(device);
    #endif
    #ifdef __linux__
        closeBlockDeviceL(device);
    #endif
}
