/**
 * @file KFS.c
 * @author Liran (liran.uzi.l@gmail)
 * @brief 
 * @version 0.1
 * @date 2024-12-18
 * 
 * @copyright Copyright (c) 2024
 * 
 * @section DESCRIPTION
 * KFS.c is the main file of the KFS project.
 * Containts the following  functions:
 */


#include "./Headers/KFS.h"

// Helper variables and data structures
BlockDevice blockDevice;

int FreeInodes[KFS_TABLE_SIZE] = {0};
KFS_Table KFS_table;

unsigned int partitionStart = 0;
Partition currentPartition;
int currentDirectory;           
char* currentPath;

/**
 * @brief Set the current partition to the selected partition
 * 
 * @param partition - a given selected partition
 */
void initializeKFS(Partition partition){
    currentPartition = partition;
    partitionStart = partition.addr;
    currentDirectory = 0;
    currentPath = "/";
}

/**
 * @brief Creatign a new KFS file system for etch partition.
 * @param device - the block device
 * @param pt - the partition table for all patition
 * @param selectedPartitionID - the selected partition ID
 * @algorithm: 
 * 1) For etch partition in partition table format the partition
 * 2) check if the index is the selected partition ID
 * 3) if it is the selected partition ID we format the partition
 * 3.1) set table to be the KFS table of the partition
 * 3.2) set the KFS_table to be the table to use later on runtime
 * 4) else we format the partition 
 * @return Table holding all the information for the KfileSystem
 */
KFS_Table KFSformatSystem(BlockDevice *device, PartitionTable* pt,int selectedPartitionID){
    // For etch partition in partition table format the partition
    KFS_Table table;
    for(size_t i = 0; i < pt->count; i++){ //[FIX from here]
        if(i == selectedPartitionID){
            table = KFSformat(device, pt->partitions[i]);
            KFS_table = table;
        }else{
            KFSformat(device, pt->partitions[i]);
        }
    }
    // return the KFS table of the partition
    return table;
}

/**
 * @brief Create a new KFS file system for a given partition if not formated.
 * 
 * @param device 
 * @param partition
 * @algorithm:
 * 1) Read the partition header from the block device
 * 2) Check if the magic number is equal to "KFS"
 * 3) if the magic number is equal to "KFS" then the partition is already formated
 * 3.1) load the KFS read the table  and mark the free inodes if the inode is unused
 * 4) if not formated then initialize the KFS
 * @return KFS_Table 
 */
KFS_Table KFSformat(BlockDevice *device, Partition partition){
    // format the block device
    KFS_Table table;
    partitionStart = partition.addr;
    KFS_HEADER header;
    readBlockDevice(device, partitionStart, sizeof(KFS_HEADER), (char*)&header);
    if( strcmp(header.magic, "KFS") == 0 ) {
         
        fprintf(stdout, "Loading KFS...\n");
        table = readKFS(device, partition);
        for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
            if(table[i].type == 'u'){
                FreeInodes[i] = 0;
            }else{
                FreeInodes[i] = 1;
            }
        }
    }else{
        /** Full break down of initializing the KFS
        *   1) create a header for the KFS
        *   2) write the header to the block device
        *   3) write the KFS table to the block device (can be junk, as it will be overwritten)
        *   4) set the global block device to the device
        *   5) create a root directory
        *   5.1) init root with "." and ".." entries
        *   6) write root the the block device
        *   7) set all pages to clear (start with "EOF")\
        *   8) set the current directory to root
        */   
        fprintf(stdout, "Initializing KFS...\n");
        strcpy(header.magic, "KFS");
        header.version = 1;
        header.address = partitionStart;
        table = buildKFSTable(partitionStart);
        writeBlockDevice(device, partitionStart, sizeof(KFS_HEADER), (char*)&header);

        writeBlockDevice(device, header.address + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)table);
        blockDevice = *device;
        KFS_directory root;
        for(size_t i = 0;i < KFS_TABLE_SIZE; i++){
            root.entries[i].inode = -2;
        }
        root.count = 2;
        root.entries[0].inode = 0;
        strcpy(root.entries[0].name, ".");
        root.entries[1].inode = -1;
        strcpy(root.entries[1].name, "..");

        writeBlockDevice(device, partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * KFS_TABLE_SIZE, sizeof(KFS_directory), (char*)&root);
        for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
            clearPage(partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * KFS_TABLE_SIZE + sizeof(KFS_directory) + PAGE_SIZE * i);
        }
    }
    currentDirectory = 0;
    currentPath = "/";
    return table;
}

/**
 * @brief Make a new KFS table with all the needed information.
 * @algorithm:
 * 1) Create a new KFS table
 * 2) for etch line in the table set the inode to be the index
 * 2.1) set the type to be 'u' (unused)
 * 2.2) set the size to be 0
 * 2.3) set the address to be the skip the header + table + root directory + page size * i
 * 3) set root
 * @param partitionStart 
 * @return KFS_Table 
 */
KFS_Table buildKFSTable(unsigned int partitionStart){
    
    KFS_Table Table = (KFS_Table)malloc(sizeof(KFS_Entry) * KFS_TABLE_SIZE);
    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        Table[i].inode = i;
        Table[i].type = 'u';
        Table[i].size = 0;
        Table[i].addr = partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * KFS_TABLE_SIZE + sizeof(KFS_directory) + PAGE_SIZE * i;
    }
    Table[0].type = 'd';
    Table[0].addr = partitionStart + sizeof(KFS_HEADER) + sizeof(KFS_Entry) * KFS_TABLE_SIZE;
    Table[0].size = 0;
    FreeInodes[0] = 1;
    return Table;
}

/**
 * @brief Set the page to be clear (start with "EOF")
 * @param addr 
 */
void clearPage(int addr){
    //put "EOF" in the start of the page
    char* eof = "EOF";
    writeBlockDevice(&blockDevice, addr, 3, eof);
}



char* getPath(void){
    return currentPath;
}



// can be used to check if the path is valid
void pathSimplyfy(char* path){
    if(strlen(path) == 0){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    char* simplifiedPath = (char*)malloc(strlen(path));
    Path path_struct;
    // if the path is absolute it's easy to simplify
    int currentInode;
    char lastPathPart[16];
    if(path[0] != '.'){
        // path is absolute start at root
        currentInode = 0;
        path_struct = splitPath(path);
        simplifiedPath[0] = '/';
        simplifiedPath[1] = '\0';
        for(size_t i = 0;i < path_struct.count;i++){
            if(strcmp(path_struct.parts[i].dir,".")==0){
                continue;
            }else if(strcmp(path_struct.parts[i].dir,"..")==0){
                // path " .../path_part/.." is the same as ".../" so just remove the last part
                // if current path is root
                if(currentInode == 0){
                    printf("Error: Invalid path\n");
                    return;
                }else{
                    KFS_directory dir = readDirectory(currentInode);
                    currentInode = dir.entries[1].inode;
                    // remvove last part from the path
                    int prevPart = strlen(lastPathPart);
                    simplifiedPath[strlen(simplifiedPath) - prevPart] = '\0';
                }
            }else{
                // check if the string of name is in the directory
                bool found = false;
                KFS_directory dir = readDirectory(currentInode);
                for(size_t j = 2; j < dir.count && !found; j++){
                    if(strcmp(path_struct.parts[i].dir, dir.entries[j].name) == 0){
                        currentInode = dir.entries[j].inode;
                        if(simplifiedPath[strlen(simplifiedPath) - 1] != '/'){
                            strcat(simplifiedPath, "/"); 
                        }
                        strcpy(lastPathPart, path_struct.parts[i].dir);
                        strcat(simplifiedPath, path_struct.parts[i].dir);
                        found = true;
                    }
                }
                if(!found){
                    fprintf(stderr, "Error: Invalid path\n");
                    return;
                }
            }
        }
    }

    printf("Simplified path: %s\n", simplifiedPath);

}

//#region Commands

// CD
void changeDir(char* path){
    if(strlen(path) == 0){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    if(strcmp(path,"/") == 0){
        currentDirectory = 0;
        currentPath = "/";
        return;
    }
    Path path_struct = splitPath(path);
    if(!validPath(path)){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    int inode = findDirectory(path);
    if(inode == -2){
        fprintf(stderr, "Error: Directory not found\n");
        return;
    }
    currentDirectory = inode;

    if (path[0] == '.') {
        char *temp = (char*)malloc(strlen(path) - 1);
        strcpy(temp, path + 1);
        path = temp;
    }
    if (path[0] != '/') {
        char *temp = (char*)malloc(strlen(path) + 3);
        strcpy(temp, "/");
        strcat(temp, path);
        path = temp;
    }
    currentPath = path;
}

// EDIT
void setPageContent(int addr,char* content){
    char pageContent[PAGE_SIZE] = {0};
    char buffer[PAGE_SIZE];
    int totalLength = 0;
    int consecutiveEnters = 0;

    printf("Enter content (press Enter twice to finish):\n");

    while (totalLength < PAGE_SIZE - 3) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        int length = strlen(buffer);

        if (length == 1 && buffer[0] == '\n') {
            consecutiveEnters++;
            if (consecutiveEnters == 2) {
                break;
            }
        } else {
            consecutiveEnters = 0;
        }

        if (totalLength + length >= PAGE_SIZE - 3) {
            length = PAGE_SIZE - 3 - totalLength;
            buffer[length] = '\0';
        }

        strcat(pageContent, buffer);
        totalLength += length;
    }

    // Add EOF marker
    strcat(pageContent, "EOF");
    totalLength += 3;

    pageContent[totalLength] = '\0';

    // Write the content to the block device
    writeBlockDevice(&blockDevice, addr, totalLength, pageContent);
}

// CAT
char* getPageContent(int addr){
    static char pageContent[PAGE_SIZE];
    readBlockDevice(&blockDevice, addr, PAGE_SIZE, pageContent);
    int length = strlen(pageContent);
    if (length >= 4 && strcmp(&pageContent[length - 3], "EOF") == 0) {
        pageContent[length - 3] = '\0'; // Remove "EOF"
        length -= 3;
    }
    if (length > 0 && pageContent[length - 1] == '\n') {
        pageContent[length - 1] = '\0'; // Remove the last newline
    }

    return pageContent;
}

// TOUCH
/**
 * @brief Create a File object
 * 
 * @param path 
 * @param size 
 * @details Find a free inode and assign it to the file
 */
void createFile(char* path){
    // check if the path is valid and the file does not exist
       // if we get path that don't start with ./ we can concate ./ to the start of path then split it
    if (path[0] != '/') {
        char *temp = (char*)malloc(strlen(path) + 3);
        strcpy(temp, "./");
        strcat(temp, path);
        path = temp;
    }
    if(strlen(path) == 0){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    Path path_struct = splitPath(path);
    if(!validPath(path)){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    int freeBlock = -1;
    for(size_t i = 0;i < KFS_TABLE_SIZE && freeBlock == -1; i++){
        if(FreeInodes[i] == 0){
            FreeInodes[i] = 1;
            KFS_table[i].type = 'f';
            KFS_table[i].size = 0;
            freeBlock = i;
        }
    }
    if(freeBlock == -1){
        fprintf(stderr, "Error: No free inodes\n");
        return;
    }
    int inode = findDirectory(path_struct.parts[path_struct.count-2].dir);
    if(inode == -2){
        fprintf(stderr, "Error: Directory not found\n");
        return;
    }
    KFS_directory dir = readDirectory(inode);
    // printFolderName(inode);
    writeBlockDevice(&blockDevice, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    for(size_t i = dir.count; i < KFS_TABLE_SIZE; i++){
        if(dir.entries[i].inode == -2){
            dir.entries[i].inode = freeBlock;
            strcpy(dir.entries[i].name, path_struct.parts[path_struct.count-1].dir);
            break;
        }
    }
    dir.count++;
    writeDirectory(inode, &dir);

    // KFS_directory root = readDirectory(0);
    // for(size_t i = root.count; i < KFS_TABLE_SIZE; i++){
    //     // printf("inode: %d\n", root.entries[i].inode);
    //     if(root.entries[i].inode == -1 || root.entries[i].inode == 65535){
    //         root.entries[i].inode = freeBlock;
    //         strcpy(root.entries[i].name, path);
    //         break;
    //     }
    // }
    // root.count++;
    // writeDirectory(0, &root);
}

// MKDIR
void createFolder(char* path){
    // find a free inode
       // if we get path that don't start with ./ we can concate ./ to the start of path then split it
    if (path[0] != '/') {
        char *temp = (char*)malloc(strlen(path) + 3);
        strcpy(temp, "./");
        strcat(temp, path);
        path = temp;
    }
    if(strlen(path) == 0){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    Path path_struct = splitPath(path);
    if(!validPath(path)){
        fprintf(stderr, "Error: Invalid path\n");
        return;
    }
    int freeBlock = -1;
    for(size_t i = 0;i < KFS_TABLE_SIZE && freeBlock == -1; i++){
        if(FreeInodes[i] == 0){
            FreeInodes[i] = 1;
            KFS_table[i].type = 'd';
            KFS_table[i].size = 0;
            freeBlock = i;
        }
    }
    writeBlockDevice(&blockDevice, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    if(freeBlock == -1){
        fprintf(stderr, "Error: No free inodes\n");
        return;
    }
    int inode = findDirectory(path_struct.parts[path_struct.count-2].dir);
    if(inode == -2){
        fprintf(stderr, "Error: Directory not found\n");
        return;
    }
    KFS_directory dir = readDirectory(inode);
    for(size_t i = dir.count; i < KFS_TABLE_SIZE; i++){
        if(dir.entries[i].inode == -2){
            dir.entries[i].inode = freeBlock;
            strcpy(dir.entries[i].name, path_struct.parts[path_struct.count-1].dir);
            break;
        }
    }
    dir.count++;
    writeDirectory(inode, &dir);
    KFS_directory sub_dir;
    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        sub_dir.entries[i].inode = -2;
    }
    sub_dir.count = 2;
    sub_dir.entries[0].inode = freeBlock;
    strcpy(sub_dir.entries[0].name, ".");
    sub_dir.entries[1].inode = 0;
    strcpy(sub_dir.entries[1].name, "..");
    writeDirectory(freeBlock, &sub_dir);
}

// FIND
int findDirectory(char* path){
    if(strlen(path) == 0){
        fprintf(stdout,"Error: Invalid path\n");
        return -2;
    }
    if(strcmp(path, "/") == 0){
        return 0;
    }
    KFS_directory dir = readDirectory(0);
    Path path_struct = splitPath(path);
    for(size_t i = 0; i < path_struct.count; i++){
        bool found = false;
        for(size_t j = 0; j < dir.count; j++){
            if(strcmp(path_struct.parts[i].dir, dir.entries[j].name) == 0){
                dir = readDirectory(dir.entries[j].inode);
                found = true;
            }
        }
        if(!found){
            return -2;
        }   
    }
    return dir.entries[0].inode;

}

// LS
void listFile(char* path){
    KFS_directory dir;
    if(strlen(path) == 0){
        dir = readDirectory(currentDirectory);
        for(size_t i = 0; i < dir.count; i++){
        //if the inode is for a folder print the name of the folder in bold
            if(dir.entries[i].name[0] == '.'){
                    continue;
            }else{
                if(KFS_table[dir.entries[i].inode].type == 'd'){
                    printf("\033[1m%s\033[0m\n", dir.entries[i].name);
                }else{
                    printf("%s\n", dir.entries[i].name);
                }
            }
        }
    }else{
        int inode = findDirectory(path);
        if(inode == -2){
            fprintf(stderr, "Error: Directory not found\n");
            return;
        }
        dir = readDirectory(inode);
        for(size_t i = 0; i < dir.count; i++){
            //if the inode is for a folder print the name of the folder in bold
            if(dir.entries[i].name[0] == '.'){
                    continue;
            }else{
                if(KFS_table[dir.entries[i].inode].type == 'd'){
                    printf("\033[1m%s\033[0m\n", dir.entries[i].name);
                }else{
                    printf("%s\n", dir.entries[i].name);
                }
            }
        }
    } 
}

//#region Helper_functions
bool validPath(char* path){
    if(strlen(path) == 0){
        return false;
    }
    if(strcmp(path, "/") == 0){
        return true;
    }
    Path path_struct = splitPath(path);
    KFS_directory dir;
    int startInode = currentDirectory;
    int curInode = currentDirectory;
    if((strcmp(path_struct.parts[0].dir, ".")) == 0 ||
    (strcmp(path_struct.parts[0].dir, "..") == 0)){
        // use relative path
        dir = readDirectory(startInode);
        for(size_t i = 0; i < path_struct.count;i++){
            if(strcmp(path_struct.parts[i].dir, ".") == 0){
                continue;
            }
            if(strcmp(path_struct.parts[i].dir, "..") == 0){
                if(curInode == 0){
                     return false;
                }
                else{
                    curInode = dir.entries[1].inode;
                    dir = readDirectory(curInode);
                }
            }else{
                bool found = false;
                for(size_t j = 2; j < dir.count && !found; j++){
                    if(strcmp(path_struct.parts[i].dir, dir.entries[j].name) == 0){
                        curInode = dir.entries[j].inode;
                        dir = readDirectory(curInode);
                        found = true;
                    }
                }
            }
        }
    }else {
        // use absolute path
        dir = readDirectory(0);
        for(size_t i = 0;i < path_struct.count; i++){
            bool found = false;
            for(size_t j = 2; j < dir.count && !found; j++){
                if(strcmp(path_struct.parts[i].dir, dir.entries[j].name) == 0){
                    curInode = dir.entries[j].inode;
                    dir = readDirectory(curInode);
                    found = true;
                }
            }
            if(!found){
                return false;
            }
        }
    }  
    return true;
}

int getPageAddress(int indoe){
    return KFS_table[indoe].addr;
}


//#endregion


//#region read/write

KFS_directory readDirectory(unsigned short inode){
    if(inode == -1){
        // fprintf(stderr, "Error: Invalid inode\n");
        return (KFS_directory){0};
    }
    if(KFS_table[inode].type != 'd'){
        // fprintf(stderr, "Error: Not a directory\n");
        return (KFS_directory){0};
    }
    KFS_directory dir;
    int address = KFS_table[inode].addr;
    readBlockDevice(&blockDevice, address, sizeof(KFS_directory), (char*)&dir);
    return dir;   
}

void readKFSHeader(BlockDevice *device, KFS_HEADER* kfs_header,unsigned int kfsIndex, unsigned int partitionOffset){
    // partition_header + partition_size(in partition header) is the partition offset 
    // kfsAddress = partition offset + kfsIndex * sizeof(KFS_HEADER)
    unsigned int address = partitionOffset + kfsIndex * sizeof(KFS_HEADER);
    readBlockDevice(device, address, sizeof(KFS_HEADER), (char*)kfs_header);
}

void readKFSTable(BlockDevice *device, KFS_Table table, unsigned int kfsIndex, unsigned int partitionOffset){
    // partition_header + partition_size(in partition header) is the partition offset 
    // kfsAddress = partition offset + kfsIndex * sizeof(KFS_HEADER)
    unsigned int address = partitionOffset + sizeof(KFS_HEADER) + kfsIndex * sizeof(KFS_Entry);
    readBlockDevice(device, address, sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)table);
}

void writeDirectory(unsigned short inode, KFS_directory* dir){
    int address = KFS_table[inode].addr;
    writeBlockDevice(&blockDevice, address, sizeof(KFS_directory), (char*)dir);
}

KFS_Table readKFS(BlockDevice *device, Partition partition){
    blockDevice = *device;
    KFS_HEADER header;
    KFS_Table table;
    readBlockDevice(device, partition.addr, sizeof(KFS_HEADER), (char*)&header);
  
    if( strcmp(header.magic, "KFS") != 0 ) {
        fprintf(stderr, "Error: Not a KFS partition\n");
        return NULL;
    }

    int address = header.address + sizeof(KFS_HEADER);
    //writeBlockDevice(&blockDevice, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    table = (KFS_Entry*)malloc(sizeof(KFS_Entry) * KFS_TABLE_SIZE);
    readBlockDevice(device, address, sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)table);
    // dumpKFS(table);
    return table;
}

// maybe not needed
void loadKFS(BlockDevice *device, Partition partition){ 
    blockDevice = *device;
    initializeKFS(partition);
    readBlockDevice(device, partitionStart + sizeof(KFS_HEADER), sizeof(KFS_Entry) * KFS_TABLE_SIZE, (char*)KFS_table);
    for(size_t i = 0; i < KFS_TABLE_SIZE; i++){
        if(KFS_table[i].type == 'u'){
            FreeInodes[i] = 0;
        }else{
            FreeInodes[i] = 1;
        }
    }
    
}

//#region prints

void dumpKFSHeader(){
    KFS_HEADER header;
    readBlockDevice(&blockDevice, partitionStart, sizeof(KFS_HEADER), (char*)&header);
    fprintf(stdout, "KFS Header\n");
    fprintf(stdout, "Magic: %s\n", header.magic);
    fprintf(stdout, "Version: %d\n", header.version);
    fprintf(stdout, "Address: %d\n", header.address);
}

void dumpKFS(KFS_Table table){
    printf("dumpKFS\n");
    fprintf(stdout, "KFS Table\n");
    for(size_t i = 0; i < 10; i++){//KFS_TABLE_SIZE
        fprintf(stdout, "Inode: %d, Type: %c, Address: %d, Size: %d\n", table[i].inode, table[i].type, table[i].addr, table[i].size);
    }
}

// #endregion

void printFolderName(int inode){
    KFS_directory dir = readDirectory(0);
    for(size_t i = 0; i < dir.count; i++){
        if(dir.entries[i].inode == inode){
            printf("Directory found: %s\n", dir.entries[i].name);
        }
    }
    
    // find the name of the directory
    
}

// find Page Inode
int findPageInode(char* path){
    if(strlen(path) == 0){
        fprintf(stdout,"Error: Invalid path\n");
        return -2;
    }
    if(strcmp(path, "/") == 0){
        return 0;
    }
    Path path_struct = splitPath(path);
    KFS_directory dir;
    int startInode = currentDirectory;
    int curInode = currentDirectory;
    if((strcmp(path_struct.parts[0].dir, ".")) == 0 ||
    (strcmp(path_struct.parts[0].dir, "..") == 0)){
        dir = readDirectory(startInode);
        for(size_t i = 0; i < path_struct.count;i++){
            if(strcmp(path_struct.parts[i].dir, ".") == 0){
                continue;
            }
            if(strcmp(path_struct.parts[i].dir, "..") == 0){
                if(curInode == 0){
                     return false;
                }
                else{
                    curInode = dir.entries[1].inode;
                    dir = readDirectory(curInode);
                }
            }else{
                bool found = false;
                for(size_t j = 2; j < dir.count && !found; j++){
                    if(strcmp(path_struct.parts[i].dir, dir.entries[j].name) == 0){
                        curInode = dir.entries[j].inode;
                        dir = readDirectory(curInode);
                        found = true;
                    }
                }
            }
            
        }
    }else {
        dir = readDirectory(0);
        for(size_t i = 0;i < path_struct.count; i++){
            bool found = false;
            for(size_t j = 2; j < dir.count && !found; j++){
                if(strcmp(path_struct.parts[i].dir, dir.entries[j].name) == 0){
                    curInode = dir.entries[j].inode;
                    dir = readDirectory(curInode);
                    found = true;
                }
            }
            if(!found){
                return false;
            }
        }

    }  
    return curInode;
}
