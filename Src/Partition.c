#include "./Headers/Partition.h"
#include "./Headers/KFS.h"


int currentPartition = 0;
int FreeSpace = 0;
Partition_HEADER header;


void updateStart(PartitionTable** pt){
    for(int i = 0; i < (*pt)->count; i++){
        if(i == 0){
            (*pt)->partitions[i].addr = sizeof(Partition_HEADER) + sizeof(Partition);
        }else{
            (*pt)->partitions[i].addr = (*pt)->partitions[i-1].addr + (*pt)->partitions[i-1].size;
        }
    }
}

/**
 * @brief Create a Partition Table object
 * 
 * @param pt 
 * @param size 
 * @details Create a Partition Table object with a defualt sinle partition taken the whole disk
 * Note: The size of the defualt partition is  TotalSize - sizeof(FS_HEADER) - sizeof(PartitionTable)
 */

void createPartitionTable(PartitionTable** pt, int size){
    strcpy(header.magic, "PiPs");
    strcpy(header.name, "Pizza");
    header.version = 1;
    header.count = 0;
    FreeSpace = size - sizeof(Partition_HEADER);
    (*pt) = (PartitionTable*)malloc(sizeof(PartitionTable));   
    (*pt)->partitions = (Partition*)malloc(sizeof(Partition));
    (*pt)->size = 1;
    (*pt)->count = 0;
}

Partition createPartition(char* name,unsigned int size){
    Partition p;
    strcpy(p.name, name);
    p.size = size;
    return p;
}

void addPartition(PartitionTable** pt, unsigned int size){

    if((*pt)->count == 0){
        // create the first partition
        if(header.size + sizeof(Partition) > size){
            fprintf(stderr,"[addPartition] Error: invalid initial partition size\n");
            return;
        }else{
            Partition p = createPartition("C", size);
            p.addr = sizeof(Partition_HEADER);
            (*pt)->count = 1;
            (*pt)->partitions[0] = p;
            FreeSpace -= sizeof(Partition);
            header.count++;
        }
    }else{
        if(FreeSpace + sizeof(Partition) < size){
            fprintf(stderr, "[addPartition] Error: not enough free space\n");
            return;
        }else{
            if((*pt)->count <= (*pt)->size){
                (*pt)->size *= 2;
                (*pt)->partitions = (Partition*)realloc((*pt)->partitions, sizeof(Partition) * ((*pt)->size));
            }
            char name[2] = {(*pt)->partitions[(*pt)->count-1].name[0] + 1, '\0'};
            Partition p = createPartition(name, size);
            p.addr = (*pt)->partitions[(*pt)->count-1].addr + (*pt)->partitions[(*pt)->count-1].size;
            updateStart(pt);
            (*pt)->partitions[(*pt)->count] = p;
            (*pt)->count++;
            header.count++;
            FreeSpace -= sizeof(Partition);
        }
    }
}

void printPartitionTable(PartitionTable* pt){
    fprintf(stdout,"Partition Table\n");
    fprintf(stdout,"Name        Addr        Size\n");
    for(int i = 0; i < pt->count; i++){
        fprintf(stdout,"%s      0x%.5x      %d\n", pt->partitions[i].name, pt->partitions[i].addr, pt->partitions[i].size);
    }
}
    
void writePartitionTable(BlockDevice* device, PartitionTable* pt){
    writeBlockDevice(device, 0, sizeof(header), (char*)&header);
    writeBlockDevice(device, sizeof(header), header.size, (char*)pt->partitions);
}

void readPartitionTable(BlockDevice* device, PartitionTable** pt){
    Partition_HEADER header;
    readBlockDevice(device, 0, sizeof(header), (char*)&header);

    if(strcmp(header.magic, "PiPs") != 0){
        fprintf(stdout,"Initializing PiPs...\n");
        return;
    }

    *pt = (PartitionTable*)malloc(sizeof(PartitionTable));
    (*pt)->partitions = (Partition*)malloc(header.size);
    (*pt)->size = header.size / sizeof(Partition);
    (*pt)->count = (*pt)->size;
    
    readBlockDevice(device, sizeof(header), header.size, (char*)(*pt)->partitions);
    currentPartition = 0;

}

void PizzaPartitionSystem(PartitionTable **pt, BlockDevice *device){
    fprintf(stdout,"\033[0m\033[1mP\033[22mizza \033[1mP\033[22martition \033[1mS\033[22mystem\033[0m\n");
    // printPartitionTable(*pt);
    fprintf(stdout,"Commands:\n");
    fprintf(stdout,"c: Create a new partition\n");
    fprintf(stdout,"d: Delete a partition\n");
    fprintf(stdout,"s: Select a partition\n");
    fprintf(stdout,"f: Format a partition\n");
    fprintf(stdout,"q: Quit\n");
    char option;
    int mode = 0;
    while(1){
        fprintf(stdout,"$> ");
        scanf("%c", &option);
        getchar();
        switch(option){
            case 'c':
                if(mode == 0){
                    fprintf(stdout,"Enter the size of the partition: ");
                    int size;
                    scanf("%d", &size);
                    getchar();
                    addPartition(pt, size);
                    mode = 1;
                    printPartitionTable(*pt);
                }else{
                    fprintf(stdout,"Can't create new partition, please format the last one first\n");
                }
                break;
            case 'd':
                if(mode ==0){
                    if((*pt)->count == 1){
                        fprintf(stderr,"Error: Cannot delete the last partition\n");
                    }else{
                        fprintf(stdout,"Enter Partition to delete: ");
                        char name;
                        scanf("%c", &name);
                        getchar();
                        for(int i = 0; i < (*pt)->count; i++){
                            if((*pt)->partitions[i].name[0] == name){
                                for(int j = i; j < (*pt)->count-1; j++){
                                    (*pt)->partitions[j] = (*pt)->partitions[j+1];
                                }
                                (*pt)->count--;
                                break;
                            }
                        }
                    }

                }
                break;
            case 's':
                if(mode == 0){
                    fprintf(stdout,"Enter Partition to select: ");
                    char name;
                    scanf("%c", &name);
                    getchar();
                    for(int i = 0; i < (*pt)->count; i++){
                        if((*pt)->partitions[i].name[0] == name){
                            currentPartition = i;
                            break;
                        }
                    }
                    mode = 0;
                }
                break;
            case 'f':
                if(mode == 1){
                    fprintf(stdout,"Enter Partition to format: ");
                    char name;
                    scanf("%c", &name);
                    getchar();
                    for(int i = 0; i < (*pt)->count; i++){
                        if((*pt)->partitions[i].name[0] == name){
                            format(device, *pt, i);
                            break;
                        }
                    }
                    mode = 0;
                }
                break;
            case 'q':
                if(mode == 0){
                    return;
                }
            default:
                fprintf(stderr,"Invalid option\n");
        }
    }
    
}