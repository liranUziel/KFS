#include "./Headers/BlockDevice.h"
#include "./Headers/KFS.h"

int main(int argc, char *argv[])
{
    BlockDevice *device;
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <block device name>\n", argv[0]);
        return 1;
    }
    device = createBlockDevice(argv[1], 1024);
    if(device == NULL)
    {
        fprintf(stderr, "Error: failed to create block device\n");
        return 1;
    }
    
    closeBlockDevice(device);
    return 0;
}