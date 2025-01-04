#include "./Headers/BlockDevice.h"
#include "./Headers/Partition.h"
#include "./Headers/KFS.h"
#include "./Headers/Terminal.h"


int main(int argc, char *argv[])
{
    BlockDevice *device;
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <block device name>\n", argv[0]);
        return 1;
    }
    device = createBlockDevice(argv[1], BLOCK_SIZE);
    if(device == NULL)
    {
        fprintf(stderr, "Error: failed to create block device\n");
        return 1;
    }

    PartitionTable* pt = NULL;
    readPartitionTable(device, &pt);
    if(pt == NULL || pt->partitions == NULL){
        //need to add KFS to the partition
        createPartitionTable(&pt, BLOCK_SIZE);
        PizzaPartitionSystem(&pt, device);
        writePartitionTable(device, pt);
    } else {
        printKFSLogo();
        char cmd[256];
        while(1){
            printf("%s:$> ", pt->partitions[currentPartition].name);
            fgets(cmd, 256, stdin);
            cmd[strlen(cmd)-1] = '\0';
            if(strcmp(cmd, CMD_STR[CMD_EXIT]) == 0){
                break;
            }
            dispatch(cmd);
        }
    }
    
    closeBlockDevice(device);
    return 0;
}