#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "./Headers/Helper.h"

// Define colors
#define ORANGE 214
#define LIGHT_ORANGE 222
#define GREEN 36
#define CYAN_GRAY 66
#define LIGHT_BLUE 117
#define DEFAULT 252
#define RESET_COLOR "\033[0m"

#define COLUMNS 16

#define PARTITION_HEADER_SIZE sizeof(Partition_HEADER)
#define KFS_HEADER_SIZE sizeof(KFS_HEADER)
#define KFS_TABLE_TOTAL_SIZE (sizeof(KFS_Entry) * KFS_TABLE_SIZE)

// Code to get array of args and make them as array of tokens
// Token are args with name and value

// flag start with '-'
// flag args come after flag
// Algorithm:
// 1) get args count
// 2) run for etch arg in args (i = 1 ...argc)
// 3) check if arg is flag if so create token with flag type
// 3.1) set mode to fArgs to get flag args one or more
// 4) if the mode is fArgs then push arg[i] to token value and set mode to defult
// 5) if the mode is defult chcek if the path was defined if so then error
// 6) if not then set path to arg[i] and mark as defined
// 7) return the token array and path

//split path to part but don't change the original path
Path splitPath(char *path) {
    Path p;
    p.count = 0;
    char* temp = (char*)malloc(strlen(path) + 1);
    strcpy(temp, path);
    char *token = strtok(temp, "/");
    p.parts = (PathPart*)malloc(sizeof(PathPart) * 10);
    while (token != NULL) {
        p.parts[p.count].dir = (char*)malloc(strlen(token) + 1);
        strcpy(p.parts[p.count].dir, token);
        token = strtok(NULL, "/");
        p.count++;
    }
    return p;
}

Args splitArgs(char *input){
    Args args;
    args.count = 0;
    char *token = strtok(input, " ");
    args.args = (Arg*)malloc(sizeof(Arg) * 10);
    while (token != NULL) {
        args.count++;
        args.args[args.count].name = (char*)malloc(strlen(token) + 1);
        strcpy(args.args[args.count].name, token);
        token = strtok(NULL, " ");
        if (token != NULL) {
            args.args[args.count].value = (char*)malloc(strlen(token) + 1);
            strcpy(args.args[args.count].value, token);
            token = strtok(NULL, " ");
            args.count++;
        } else {
            args.args[args.count].value = NULL;
        }
    }
    return args;
}

bool isFlag(char* str){
    return str[0] == '-';
}

void setColor(int color) {
    printf("\033[38;5;%dm", color);
}

/**
 * @name dumpBlockDevice
 * @brief Print the block device as hexDump with colors
 * 
 * @algorithm
 *          1) 
 * 
 * @note Colors:
 *          Patition Header : Orange
 *          Partition Table : Light Orange
 *          KFS Header : Green
 *          KFS Table : Cyan Gray
 *          KFS Content : Light Blue
 * 
 * @param device 
 */
// Function to print a block device in hex dump format
void dumpBlockDevice(BlockDevice *device,unsigned int size) {
    PRINT_STATE state = INIT;
    PRINT_STATE_ASCII stateAscii = ASCII_INIT;
    unsigned char *data = device->data;
    unsigned int rows;
    size_t r, c;
    rows = size / COLUMNS;

    Partition_HEADER partition_header;
    PartitionTable* partition_table;
    KFS_HEADER kfs_header;
    
    unsigned int totalPartitions;
    unsigned int currentKFS_hex = 0;
    unsigned int currentKFS_ascii = 0;
    
    int partitionTableBytes;
    int partitionHeaderBytes;
    int pritingDataSize;
    int printingDataSizeHex;
    int printingDataSizeAscii;
    int HexPrinted;
    int AsciiPrinted;
    int dotentLineCount = 0;
    
    int zeroLineCount = 0;
    fprintf(stdout,"[DEBUG]: block device memory dump\n");
    // First print the HexDump header Address | Hex | Ascii
    printf("Address | ");
    for(c = 0;c < COLUMNS;c++){
        printf("%02x ", c);
    }
    printf("\n");
    // HecDump will print all row that hold data
    for (r = 0;r < rows;r++){
        // Print lsat row (ALWAYS)
        if(r + 1 == rows){
             printf("%08x| ", r * COLUMNS);
                for(size_t i = 0;i < COLUMNS;i++){
                    printf(".  ");
                }
                printf("| ");
                for(size_t i = 0;i < COLUMNS;i++){
                    printf("x ");
                }
                printf("\n");
            printf("End of file\n");
        }else{
        // FIRST check for DOTTED mode
        // in DOTTED mode Three 3 dot line will be printed
        if(state == KFS_DOTTED_LINE_hex){
            setColor(DEFAULT);
            if(dotentLineCount < 5){
                printf("%08x| ", r * COLUMNS);
                for(size_t i = 0;i < COLUMNS;i++){
                    printf(".  ");
                }
                printf("| ");
                for(size_t i = 0;i < COLUMNS;i++){
                    printf("x ");
                }
                printf("\n");
                dotentLineCount++;
            }else if(dotentLineCount == 5){
                printf("\t\t...\t\t\n");
                dotentLineCount++;
            }
        }else{
            // First print the address in hex
            printf("%08x| ", r * COLUMNS);
            // For etch line (print the hex data) column by column
            for(c = 0;c < COLUMNS;c++){
                switch (state){
                    case INIT:
                    // First state (init the printing);
                    readPartitionHeader(device, &partition_header);
                    readPartitionTable(device, &partition_table);
                    printingDataSizeHex = sizeof(Partition_HEADER);
                    totalPartitions = partition_header.count;
                    HexPrinted = 0;
                    if(checkValidAddress(0,r,c)){
                        state = PARTITION_HEADER_PRINT;
                    }
                    c--;
                    break;
                    case PARTITION_HEADER_PRINT:
                    if(HexPrinted < printingDataSizeHex){
                        setColor(ORANGE);
                        printf("%02x ", data[r * COLUMNS + c]);
                        HexPrinted++;
                    }else{
                        if(checkValidAddress(sizeof(Partition_HEADER),r,c)){
                            state = PARTITION_TABLE_PRINT;
                            printingDataSizeHex = partition_header.size;
                            HexPrinted = 0;
                            c--;
                        }
                    }
                    break;
                    case PARTITION_TABLE_PRINT:
                        if(HexPrinted < printingDataSizeHex){
                            setColor(LIGHT_ORANGE);
                            printf("%02x ", data[r * COLUMNS + c]);
                            HexPrinted++;
                        }else{
                            if(checkValidAddress(sizeof(Partition_HEADER) + partition_header.size,r,c)){
                                state = KFS_HEADER_PRINT;
                                readKFSHeader(device, &kfs_header, currentKFS_hex, sizeof(Partition_HEADER) + partition_header.size);
                                HexPrinted = 0;
                                printingDataSizeHex = KFS_HEADER_SIZE;
                                c--;
                            }
                        }
                        break;
                    case KFS_HEADER_PRINT:
                        if(HexPrinted < printingDataSizeHex){
                            setColor(GREEN);
                            printf("%02x ", data[r * COLUMNS + c]);
                            HexPrinted++;
                        }else{
                            if(currentKFS_hex < totalPartitions){
                                readKFSHeader(device, &kfs_header, currentKFS_hex, sizeof(Partition_HEADER) + partition_header.size); 
                                state = KFS_TABLE_PRINT;
                                printingDataSizeHex =  (sizeof(KFS_Entry) * KFS_TABLE_SIZE);
                                HexPrinted = 0;
                                c--;
                                currentKFS_hex++;
                            }else{
                                state = KFS_DOTTED_LINE_hex;
                            }
                        }
                        break;
                    case KFS_TABLE_PRINT:
                        if(HexPrinted < printingDataSizeHex){
                            setColor(CYAN_GRAY);
                            printf("%02x ", data[r * COLUMNS + c]);
                            HexPrinted++;
                        }else{
                            int currentPatitionSize = partition_table->partitions[currentKFS_hex - 1].size;
                            printingDataSizeHex = currentPatitionSize - HexPrinted - sizeof(KFS_HEADER);
                            state = KFS_CONTENT_PRINT;
                            HexPrinted = 0;
                            c--;
                        }
                        break;
                    case KFS_CONTENT_PRINT:
                        if(HexPrinted < printingDataSizeHex){
                            setColor(LIGHT_BLUE);
                            printf("%02x ", data[r * COLUMNS + c]);
                            HexPrinted++;
                        }else{
                            if(currentKFS_hex < totalPartitions){
                                state = KFS_HEADER_PRINT;
                                HexPrinted = 0;
                                printingDataSizeHex =  KFS_HEADER_SIZE;
                            }else{
                                state = KFS_DOTTED_LINE_hex;
                            }
                            c--;
                        }
                    break;
                    case KFS_DOTTED_LINE_hex:
                        setColor(DEFAULT);
                        printf(".  ");
                    break;
                    default:
                    break;
                }   
            }
            //set color white
            setColor(DEFAULT);
            // print the date in ascii
            printf("| ");          
            for(c = 0;c < COLUMNS;c++){
                switch (stateAscii){
                    case ASCII_INIT:
                        // First state (init the printing);
                        printingDataSizeAscii = sizeof(Partition_HEADER);
                        AsciiPrinted = 0;
                        if(checkValidAddress(0,r,c)){
                            stateAscii = ASCII_PARTITION_HEADER_PRINT;
                        }
                        c--;
                    break;
                    case ASCII_PARTITION_HEADER_PRINT:
                    // print the data in ascii format
                    if(AsciiPrinted < printingDataSizeAscii){
                        if(data[r * COLUMNS + c] >= 32 && data[r * COLUMNS + c] <= 126){
                            setColor(ORANGE);
                            printf("%c", data[r * COLUMNS + c]);
                        }else{                            
                            setColor(DEFAULT);
                            printf(".");
                        }
                        AsciiPrinted++;
                    }else{
                        if(checkValidAddress(sizeof(Partition_HEADER),r,c)){
                            stateAscii = ASCII_PARTITION_TABLE_PRINT;
                            printingDataSizeAscii = partition_header.size;
                            AsciiPrinted = 0;
                            c--;
                        }
                    }
                    break;
                    case PARTITION_TABLE_PRINT:
                        // print the data in ascii format
                        if(AsciiPrinted < printingDataSizeAscii){
                            if(data[r * COLUMNS + c] >= 32 && data[r * COLUMNS + c] <= 126){
                                setColor(LIGHT_ORANGE);
                                printf("%c", data[r * COLUMNS + c]);
                            }else{
                                setColor(DEFAULT);
                                printf(".");
                            }
                            AsciiPrinted++;
                        }else{
                            if(checkValidAddress(sizeof(Partition_HEADER) + partition_header.size,r,c)){
                                stateAscii = ASCII_KFS_HEADER_PRINT;
                                AsciiPrinted = 0;
                                printingDataSizeAscii = KFS_HEADER_SIZE;
                                c--;
                            }
                        }
                    break;
                    case ASCII_KFS_HEADER_PRINT:
                        if(AsciiPrinted < printingDataSizeAscii){
                            if(data[r * COLUMNS + c] >= 32 && data[r * COLUMNS + c] <= 126){
                                setColor(GREEN);
                                printf("%c", data[r * COLUMNS + c]);
                            }else{
                                setColor(0);
                                printf(".");
                            }
                            AsciiPrinted++;
                        }else{
                            if(currentKFS_ascii < totalPartitions){
                                stateAscii = ASCII_KFS_TABLE_PRINT;
                                printingDataSizeAscii = (sizeof(KFS_Entry) * KFS_TABLE_SIZE);
                                AsciiPrinted = 0;
                                currentKFS_ascii++;
                            }else{
                                KFS_DOTTED_LINE_ascii;
                            }
                            c--;
                        }
                    break;
                    case ASCII_KFS_TABLE_PRINT:
                        if(AsciiPrinted < printingDataSizeAscii){
                            if(data[r * COLUMNS + c] >= 32 && data[r * COLUMNS + c] <= 126){
                                setColor(CYAN_GRAY);
                                printf("%c", data[r * COLUMNS + c]);
                            }else{
                                setColor(DEFAULT);
                                printf(".");
                            }
                            AsciiPrinted++;
                        }else{
                            int currentPatitionSize = partition_table->partitions[currentKFS_hex - 1].size;
                            printingDataSizeAscii = currentPatitionSize - AsciiPrinted - sizeof(KFS_HEADER);
                            stateAscii = ASCII_CONTENT_PRINT;
                            AsciiPrinted = 0;
                            c--;
                        }
                    break;
                    case ASCII_CONTENT_PRINT:
                        if(AsciiPrinted < printingDataSizeAscii){
                            if(data[r * COLUMNS + c] >= 32 && data[r * COLUMNS + c] <= 126){
                                setColor(LIGHT_BLUE);
                                printf("%c", data[r * COLUMNS + c]);
                            }else{
                                setColor(DEFAULT);
                                printf(".");
                            }
                            AsciiPrinted++;
                        }else{
                            if(currentKFS_hex < totalPartitions){
                                stateAscii = ASCII_KFS_HEADER_PRINT;
                                AsciiPrinted = 0;
                                printingDataSizeAscii = KFS_HEADER_SIZE;
                            }else{
                                stateAscii = KFS_DOTTED_LINE_ascii;
                            }
                            c--;
                        }
                    break;
                    case KFS_DOTTED_LINE_ascii:
                        setColor(DEFAULT);
                        printf("x");
                    break;
                }
            }
            setColor(DEFAULT);
            printf("\n");
        }

        }
    }
    
}

bool checkValidAddress(unsigned int address, unsigned int row, unsigned int col){
    return address >= row * COLUMNS && address < (row + 1) * COLUMNS;
    
}

bool sumOfLineZ(unsigned char *data, unsigned int row, unsigned int size){
    unsigned int sum = 0;
    for(unsigned int i = 0;i < size;i++){
        sum += data[row * COLUMNS + i];
    }
    return sum == 0;
}