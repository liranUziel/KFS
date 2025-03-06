/**
 * @file Terminal.c
 * @author Liran Uziel (liran.uz.l@gmail.com)
 * @brief This code manage the terminal for KFS for all the given commands
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "./Headers/Terminal.h"

const char *CMD_STR[] = {
    "exit",
    "help",
    "touch",
    "ls",
    "mkdir",
    "find",
    "cd",
    "cat",
    "edit",
    "tree",
};
/**
 * @brief Print the KFS logo
 * 
 */
void printKFSLogo(){
    printf("\033[0;31m");
    printf("        ,--.                       \n");
    printf("    ,--/  /|     ,---,.  .--.--.    \n");
    printf(" ,---,': / '   ,'  .' | /  /    '.  \n");
    printf(" :   : '/ /  ,---.'   ||  :  /`. /  \n");
    printf(" |   '   ,   |   |   .';  |  |--`   \n");
    printf(" '   |  /    :   :  :  |  :  ;_     \n");
    printf(" |   ;  ;    :   |  |-, \\  \\    `.  \n");
    printf(" :   '   \\   |   :  ;/|  `----.   \\ \n");
    printf(" |   |    '  |   |   .'  __ \\  \\  | \n");
    printf(" '   : |.  \\ |   :  '   /  /`--'  / \n");
    printf(" |   | '_\\.' |   |  |  '--'.     /  \n");
    printf(" '   : |     |   :  \\    `--'---'   \n");
    printf(" ;   |,'     |   | ,'                \n");
    printf(" '---'       `----'                  \n");
    printf("\033[0;37m");
    printf("Welcome to KFS, ");
    printf("\033[1m");
    printf("Krazy File System\n");
    printf("\033[0m");
}

/**
 * @brief Print the Help menu for the terminal
 * 
 */
void printHelp(){
    printf("List of commands:\n");
    printf("exit: Exit the terminal\n");
    printf("help: Print this help message\n");
    printf("touch: Create a new file\n");
    printf("mkdir: Create a new directory\n");
    printf("ls: List files in the current directory\n");
    printf("cd: Change directory\n");
    printf("cat: Print the content of a file\n");
    printf("edit: Edit a file\n");
    printf("find: Find a directory\n");
}
/**
 * @brief Execute the commands
 * @algo
 *  1. find if the command match any pattern
 *  
 * 
 * Note: Some command can get arguments as well, for example touch, mkdir, cd, cat, edit
 * To make sure the pattern is correct we need to check the first word of the command
 * Using startWith function (from Ctring.c) we can check if the command start with the pattern
 * @param cmd 
 */
void dispatch(char* cmd){
    if(strcmp(cmd, CMD_STR[CMD_HELP]) == 0){
        printHelp();
    }
    else if(startWith(cmd, CMD_STR[CMD_TOUCH])){
        char *path = cmd + strlen(CMD_STR[CMD_TOUCH]) + 1;
        createFile(path);     
    } else if (startWith(cmd, CMD_STR[CMD_LS])){
        char *path = cmd + strlen(CMD_STR[CMD_LS]) + 1;
        listFile(path);
    }else if (startWith(cmd, CMD_STR[CMD_MKDIR])){
        char *path = cmd + strlen(CMD_STR[CMD_MKDIR]) + 1;
        createFolder(path);
    } else if (startWith(cmd, CMD_STR[CMD_FIND])){
        char *path = cmd + strlen(CMD_STR[CMD_FIND]) + 1;
        int inode = findDirectory(path);
        if(inode == -1){
            printf("Directory not found\n");
        }else{
            if(inode == 0){
                printf("Directory found at root\n");
            }else{
                printFolderName(inode);
            }
        }
    }else if(startWith(cmd,"edit")){
        char *path = cmd + strlen("edit") + 1;
        int inode = findPageInode(path);
        if(inode == -1){
            printf("Directory not found\n");
        }else{
            setPageContent(getPageAddress(inode),"Hello World");
        }
    }else if(startWith(cmd,"cd")){
        char *path = cmd + strlen("cd") + 1;
        if(validPath(path)){
            changeDir(path);
        }else{
            printf("Invalid path\n");
        }
    }else if(startWith(cmd,"addr")){
        char *path = cmd + strlen("addr") + 1;
        int inode = findPageInode(path);
        if(inode == -1){
            printf("Directory not found\n");
        }else{
            printf("Content at %x\n",getPageAddress(inode));
        }
    } else if(startWith(cmd,"cat")){
        char *path = cmd + strlen("cat") + 1;
        if(validPath(path)){
        }
        int inode = findPageInode(path);
        if(inode == -1){
            printf("Directory not found\n");
        }else{
            printf("%s",getPageContent(getPageAddress(inode)));
        }
    }else if(startWith(cmd,"tree"))
    {
        printTree();
        
    } else if(strcmp(cmd, CMD_STR[CMD_EXIT]) == 0){
        printf("Exiting terminal\n");
        return;
    }
        else{
        printf("Unknown command use help to see all commands\n");
    }
}

