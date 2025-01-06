#include "./Headers/Partition.h"
#include "./Headers/KFS.h"


int currentPartitionID = 0;
int FreeSpace = 0;
Partition_HEADER header;


void updateStart(PartitionTable** pt){
    for(int i = 0; i < (*pt)->count; i++){
        if(i == 0){
            (*pt)->partitions[i].addr = sizeof(Partition_HEADER) + sizeof(Partition)*((*pt)->count + 1);
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
    header.size = 0;
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
    p.formatted = false;
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
            p.addr = sizeof(Partition_HEADER) + sizeof(Partition);
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
            (*pt)->partitions[(*pt)->count] = p;
            (*pt)->count++;
            updateStart(pt);
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
    header.size = pt->size * sizeof(Partition);
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
    (*pt)->count = header.count;
    readBlockDevice(device, sizeof(header), header.size, (char*)(*pt)->partitions);
    currentPartitionID = 0;
}

void PizzaPartitionSystem(PartitionTable **pt, BlockDevice *device){
    fprintf(stdout,"\033[0m\033[1mP\033[22mizza \033[1mP\033[22martition \033[1mS\033[22mystem\033[0m\n");
    fprintf(stdout,"Commands:\n");
    fprintf(stdout,"c: Create a new partition\n");
    fprintf(stdout,"d: Delete a partition\n");
    fprintf(stdout,"s: Select a partition\n");
    fprintf(stdout,"f: Format a partition\n");
    fprintf(stdout,"q: Quit\n");
    char option;
    int mode = 0;
    int totalPartitions = 0;
    currentPartitionID = 0;
    bool allFormatted = true;
    char name;
    while(1){
        fprintf(stdout,"$> ");
        scanf("%c", &option);
        getchar();
        switch(option){
            case 'c':            
                fprintf(stdout,"Enter the size of the partition: ");
                int size;
                scanf("%d", &size);
                getchar();
                addPartition(pt, size);
                printPartitionTable(*pt);
                totalPartitions++;
                break;
            case 'd':
                if(totalPartitions < 2){
                        fprintf(stderr,"Error: Cannot delete the last partition\n");
                }else{
                    fprintf(stdout,"Enter Partition to delete: ");
                    scanf("%c", &name);
                    getchar();
                    for(int i = 0; i < (*pt)->count; i++){
                        if((*pt)->partitions[i].name[0] == name){
                            for(int j = i; j < (*pt)->count-1; j++){
                                (*pt)->partitions[j] = (*pt)->partitions[j+1];
                            }
                            (*pt)->count--;
                            totalPartitions--;
                            break;
                        }
                    }
                }
                break;
            case 's':
                fprintf(stdout,"Enter Partition to select: ");
                scanf("%c", &name);
                getchar();
                for(int i = 0; i < (*pt)->count; i++){
                    if((*pt)->partitions[i].name[0] == name){
                        currentPartitionID = i;
                        break;
                    }
                }
                break;
            case 'f':
                fprintf(stdout,"Enter Partition to format: ");
                scanf("%c", &name);
                getchar();
                for(int i = 0; i < (*pt)->count; i++){
                    if((*pt)->partitions[i].name[0] == name){
                        if((*pt)->partitions[i].formatted){
                            fprintf(stderr,"Error: Partition already formatted\n");
                        }else{
                            (*pt)->partitions[i].formatted = true;
                        }
                        break;
                    }
                }
                break;
            case 'q':
                for(int i = 0; i < (*pt)->count; i++){
                    if(!(*pt)->partitions[i].formatted){
                        fprintf(stderr,"Error: partition %s is not formatted\n", (*pt)->partitions[i].name);
                        allFormatted = false;
                    }
                }
                if(allFormatted){
                    for(int i = 0; i < (*pt)->count; i++){
                        format(device, (*pt)->partitions[i]);
                    }
                    return;
                }
                allFormatted = true;
                break;
            default:
                fprintf(stderr,"Invalid option\n");
        }
    }
    
}
void dumpPartitionHeader(){
    fprintf(stdout,"Partition Header\n");
    fprintf(stdout,"Magic: %s\n", header.magic);
    fprintf(stdout,"Name: %s\n", header.name);
    fprintf(stdout,"Version: %d\n", header.version);
    fprintf(stdout,"Count: %d\n", header.count);
    fprintf(stdout,"Size: %d\n", header.size);
}
void dumpPartitionTable(PartitionTable* pt)
{
    dumpPartitionHeader();
    fprintf(stdout,"Partition Table\n");
    fprintf(stdout,"Name        Addr  -  End      Size\n");
    for(int i = 0; i < pt->count; i++){
        fprintf(stdout,"%s      0x%.5x  0x%.5x    %d   \n", pt->partitions[i].name, pt->partitions[i].addr,pt->partitions[i].addr+pt->partitions[i].size, pt->partitions[i].size);
    }
}