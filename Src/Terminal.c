#include "./Headers/Terminal.h"

const char *CMD_STR[] = {
    "exit",
    "help"
};

void printHelp(){
    printf("List of commands:\n");
    printf("exit: Exit the terminal\n");
    printf("help: Print this help message\n");
}

void dispatch(char *cmd){
    if(strcmp(cmd, CMD_STR[CMD_HELP]) == 0){
        printHelp();
    }
    else{
        printf("Unknown command\n");
    }
}

