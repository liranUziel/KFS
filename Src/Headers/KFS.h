#ifndef KFS_H

#define KFS_H
#include "./BlockDevice.h"

// defiune a struct that represent the file system.
// with a magic number and a version number.

typedef struct KFS_HEADER
{
    char magic[4];
    int version;
} KFS_HEADER;


void format(BlockDevice *device);

#endif // KFS_H