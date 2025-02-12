#define DEBUG
#include "./Headers/BlockDevice.h"
#include "./Headers/Partition.h"
#include "./Headers/KFS.h"
#include "./Headers/Terminal.h"
#include "./Headers/PartitionGUI.h"
#include "./Headers/Table.h"
#include "./Headers/Helper.h"


int main(int argc, char *argv[])
{   
    BlockDevice *device;
    Args args;
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
    // read the partition table header from block device
    int selectedPartitionID = 0;
    PartitionTable* partitionTalbe = partitionMenu(device,&selectedPartitionID,true);

    #ifdef DEBUG
        dumpPartitionTable(partitionTalbe);
    #endif

    // format the partitions
    KFS_Table kfsTable = KFSformatSystem(device, partitionTalbe,selectedPartitionID);
    #ifdef DEBUG
       dumpKFS(kfsTable);
    #endif

    // KFS is open and ready to use for the selectedPartitionID
    PartitionTable* pt = NULL;
    readPartitionTable(device, &pt);
    readKFS(device, partitionTalbe->partitions[selectedPartitionID]);

    printKFSLogo();
    volatile bool running = true;
    char cmd[256];
    while(running){
        printf("%s%s:$> ", pt->partitions[currentPartitionID].name,getPath());
        fgets(cmd, 256, stdin);
        cmd[strlen(cmd)-1] = '\0';
        if(strcmp(cmd, CMD_STR[CMD_EXIT]) == 0){
            running = false;
        }
        dispatch(cmd);
    }
    #ifdef DEBUG
    dumpBlockDevice(device, BLOCK_SIZE);    
    #endif
    closeBlockDevice(device);
    return 0;
}

