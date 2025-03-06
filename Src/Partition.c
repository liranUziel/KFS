#include "./Headers/Partition.h"
#include "./Headers/PartitionGUI.h"
#include "./Headers/KFS.h"

int currentPartitionID = 0;
int FreeSpace = 0;
Partition_HEADER header;

/** 
 * @name partitionMenu
 * @brief Display the partition menu
 * @algorithm 
 *  1. display the partition menu 
 *  2. read the user input
 *      2.1 if c press create a new partition change to CREATE mode
 *      2.2 if CREATE mode read the size of the partition and change to NORMAL mode
 *      2.3 if f press format the selected partition
 *      2.4 if s press set the selected partition to be the current partition (will be the defult partition for the file system)
 *      2.5 if d press delete the selected partition (cannot delete the last partition)
 *      2.6 if q press quit the partition menu 
 *      2.7 reapet the above steps until q is pressed
 *  3. create Partition Header
 *  4. write the partition header
 *  5. for each partition in the partition table calculate the starting address of the partition
 *  6. write the partition table to the block device
 *  7. return the partition table
*/
PartitionTable* partitionMenu( BlockDevice* device, int* selectedPartitionID,bool GUI){
    // Step 0: check if parition table haeder exist
    char option;
    PartitionTable* partitionTable;
    Table pTable;

    if(isPartitionTableValid(device)){
        // Read the partition table and return it
        readPartitionTable(device, &partitionTable);
    }else{
        fprintf(stdout,"Partition Table not found\n");
        fprintf(stdout,"Would you like to create a new one? [Y\\n]\n");
        scanf("%c", &option);
        getchar();
        if(option == 'n'){
            return NULL;
        }
        // Step 1: print the partition menu
        if(GUI){
            pTable = partitionMenuGUI(selectedPartitionID);
        }else{
            pTable = parnitnionMenuText(selectedPartitionID);
        }
        printTable(&pTable);
        // Step 2: Generate Partition Header and the table
        buildPartitionTable(&partitionTable, pTable);
        
        // Step 3: Write the partition Header
        // Step 4: Write the partition Table to the block device
        writePartitionTable(device, partitionTable);
        
        // Note: The partitionTable don't have a starting address of etch partition
        // Starting Address should be sizeof(Partiotn_Header) + sizeof(Partition) * (partitionIndex)

    }
     // Step 6: Retrun partition table
    return partitionTable;
}

Table parnitnionMenuText(int* seletedPartition){
    int partitionCount = 0;
    int partitionToFormat = 0;
    int partitionSize = 0;
    char option;
    char partitionName;
    TableHeader header;
    bool isRunning = true;
    Table table = createTable(5, 10, &header,50);
    addTableHeaderColumn(&header, 0, "Name");
    addTableHeaderColumn(&header, 1, "Size");
    addTableHeaderColumn(&header, 2, "Formated");
    addTableHeaderColumn(&header, 3, "Selected");
    addTableHeaderColumn(&header, 4, "Default");
    addTableHeader(&table, header);
    fprintf(stdout,"PiPs\n");
    fprintf(stdout,"Pizza Partition Center\n");
    fprintf(stdout,"C - Create Partition\nF - Format Partition\nS - Select Partition\nD - Delete Parition\nQ - Quit\n");
    while(isRunning){
        fprintf(stdout,"Enter Option: ");
        scanf("%c", &option);
        getchar();
        switch (option)
        {
        case 'c':
        case 'C':
            // create a new partition and add it to the table
            // incress the partition counter to format
            fprintf(stdout,"Enter Partition Size: ");
            scanf("%d", &partitionSize);
            getchar();
            addPartitionToTable(&table, 'C' + partitionCount, partitionSize,false, partitionCount==0,partitionCount==0);
            partitionCount++;
            partitionToFormat++;
            break;
        case 'd':
            break;
        case 'f':
        case 'F':
            // ask the user for the partition to format to name
            // mark as the partition formatted
            // decress the partition counter to format
            fprintf(stdout,"Enter Partition Name: ");
            scanf("%c", &partitionName);
            getchar();
            markPartitionAsFormated(&table, partitionName, -1);
            partitionToFormat--;
            break;
        case 's':
        case 'S':
            //ask the user for the partition to select
            // mark the partition as selected
            fprintf(stdout,"Enter Partition Name: ");
            scanf("%c", &partitionName);
            getchar();
            markPartitionAsSelected(&table, partitionName, -1);
            break;
        case 'q':
        case 'Q':
            // check if the partition table is valid
            // check if the is at list one partition
            // check if all the partitions are marked as formatted
            if(partitionToFormat == 0 && partitionCount > 0){
                isRunning = false;
            }
            break;
        default:
            break;
        }
    }
    return table;
}

/**
 * @name buildPartitionTable
 * @brief Build the partition table
 * @algorithm
 * 1. Create partition header
 * 2. Create a partition table
 * 3. For etch row in the table calculate craete a partition
 * 4. Update the starting address of the partition
 * 5. Add the partition to the partition table
 * 6. Write the partition table to the block device
 * 
 * @param partitionTable 
 * @param table 
 */
void buildPartitionTable(PartitionTable **partitionTable,Table table){
    createPartitionTable(partitionTable, BLOCK_SIZE);
    for(int i = 0; i < table.rowsCount; i++){
        TableRow row = table.rows[i];
        addPartition(partitionTable, stringToInt(row.columns[1]));
    }
    header.count = (*partitionTable)->count;
    header.size = (*partitionTable)->size * sizeof(Partition);
    dumpPartitionHeader();
    
}

/**
 * @name isPartitionTableValid
 * @brief Check if the partition table is valid
 * @algorithm
 * 1. Read the first block of the block device
 * 2. Check if the magic number is equal to "PiPs"
 * 3. Return true if the magic number is equal to "PiPs" else return false
 * @param device 
 * @return true 
 * @return false 
 */
bool isPartitionTableValid(BlockDevice* device){
    Partition_HEADER header;
    readBlockDevice(device, 0, sizeof(header), (char*)&header);
    if(strcmp(header.magic, "PiPs") != 0){
        return false;
    }
    return true;
}

/**
 * @name updateStart
 * @brief Update the starting address of the partition
 * @algorithm 
 * 1. For etch partition in the partition table
 *     1.1 if the partition is the first partition set the starting address to be the size of the partition header + sizeof(Partition)
 *     1.2 else set the starting address to be the starting address of the previous partition + the size of the previous partition
 * @param pt 
 */
void updateStart(PartitionTable** pt){
    for(int i = 0; i < (*pt)->count; i++){
        if(i == 0){
            (*pt)->partitions[i].addr = sizeof(Partition_HEADER) + sizeof(Partition)*((*pt)->size);
        }else{
            (*pt)->partitions[i].addr = (*pt)->partitions[i-1].addr + (*pt)->partitions[i-1].size;
        }
    }
}

/**
 * @name createPartitionTable
 * @brief Create a Partition Table object
 * @algorithm
 * 1. Create a partition header
 * 2. Create a partition table
 * 3. Set the size of the partition table to be 1
 * 4. Set the count of the partition table to be 0
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

/**
* @name createPartition
* @brief Create a Partition object
* @algorithm
* 1. Create a partition object
* 2. Set the name of the partition
* 3. Set the size of the partition
* 4. Set the formatted flag to false
* @param name 
* @param size 
* @return Partition
*/
Partition createPartition(char* name,unsigned int size){
    Partition p;
    strcpy(p.name, name);
    p.size = size;
    p.formatted = false;
    return p;
}

/**
 * @name addPartition
 * @brief Add a partition to the partition table
 * @algorithm
 * 1. If the partition table is empty create the first partition
 * 2. If the partition table is not empty and the free space is less than the size of the partition return
 * 3. If the partition table is not empty and the count of the partition table is less than the size of the partition table
 *      3.1 Double the size of the partition table
 *      3.2 Create a new partition
 *      3.3 Add the partition to the partition table
 *      3.4 Update the starting address of the partition
 *      3.5 Increment the count of the partition table
 *      3.6 Increment the count of the partition header
 *      3.7 Decrement the free space
 * @param pt
 */
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

/**
 * @name printPartitionTable
 * @brief Print the partition table
 * @algorithm
 * 1. Print the partition table header
 * 2. For etch partition in the partition table print the name, address, and size of the partition
 * @param pt
*/
void printPartitionTable(PartitionTable* pt){
    fprintf(stdout,"Partition Table\n");
    fprintf(stdout,"Name        Addr        Size\n");
    for(int i = 0; i < pt->count; i++){
        fprintf(stdout,"%s      0x%.5x      %d\n", pt->partitions[i].name, pt->partitions[i].addr, pt->partitions[i].size);
    }
}

/**
 * @name writePartitionTable
 * @brief Write the partition table to the block device
 * @algorithm
 * 1. Set the size of the partition table in the partition header
 * 2. Write the partition header to the block device
 * 3. Write the partition table to the block device
 * @param device 
 * @param pt 
 */  
void writePartitionTable(BlockDevice* device, PartitionTable* pt){
    header.size = pt->size * sizeof(Partition);
    writeBlockDevice(device, 0, sizeof(header), (char*)&header);
    writeBlockDevice(device, sizeof(header), header.size, (char*)pt->partitions);
}

/**
 * @name readPartitionTable
 * @brief Read the partition table from the block device
 * @algorithm
 * 1. Read the partition table header from the block device
 * 2. If the magic number is not equal to "PiPs" return
 * 3. Allocate memory for the partition table
 * 4. Read the partition table from the block device
 * @param device 
 * @param pt 
 */
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

/**
 * @name readPartitionHeader
 * @brief Read the partition header from the block device
 * @algorithm
 * 1. Read the partition header from the block device
 * @param device 
 * @param header 
 */
void readPartitionHeader(BlockDevice* device, Partition_HEADER* header) {
    readBlockDevice(device, 0, sizeof(Partition_HEADER), (char*)header);
}


/**
 * @name dumpPartitionHeader
 * @brief Print the partition header
 * @algorithm
 * 1. Print the magic number
 * 2. Print the name
 * 3. Print the version
 * 4. Print the count
 * 5. Print the size
 */
void dumpPartitionHeader(){
    fprintf(stdout,"Partition Header\n");
    fprintf(stdout,"Magic: %s\n", header.magic);
    fprintf(stdout,"Name: %s\n", header.name);
    fprintf(stdout,"Version: %d\n", header.version);
    fprintf(stdout,"Count: %d\n", header.count);
    fprintf(stdout,"Size: %d\n", header.size);
}

/**
 * @name dumpPartitionTable
 * @brief Print the partition table
 * @algorithm
 * 1. Print the name, address, and size of etch partition in the partition table
 * @param pt 
 */
void dumpPartitionTable(PartitionTable* pt)
{
    fprintf(stdout,"Partition Table\n");
    fprintf(stdout,"Name        Addr  -  End      Size\n");
    for(int i = 0; i < pt->count; i++){
        fprintf(stdout,"%s  0x%.5x(%d) - 0x%.5x(%d) %d\n", pt->partitions[i].name, pt->partitions[i].addr,pt->partitions[i].addr,pt->partitions[i].addr+pt->partitions[i].size,pt->partitions[i].addr+pt->partitions[i].size, pt->partitions[i].size);
        // fprintf(stdout,"%s      0x%.5x  0x%.5x    %d   \n", pt->partitions[i].name, pt->partitions[i].addr,pt->partitions[i].addr+pt->partitions[i].size, pt->partitions[i].size);
    }
}