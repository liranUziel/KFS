#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "FileTree.h"
#include "KFS.h"
#include "Ctring.h"


//List of commands
typedef enum CMD_TYPE{
    CMD_EXIT,
    CMD_HELP,
    CMD_TOUCH,
    CMD_LS,
    CMD_MKDIR,
    CMD_FIND,
    CMD_CD,
    CMD_ADDR,
    CMD_UNKNOWN
}CMD_TYPE;

//List of commands string to print or compare
extern const char *CMD_STR[];

void printHelp();
void printKFSLogo();
void printPizzaLogo();
void dispatch(char* cmd);
// void touch(char* path);

#endif // TERMINAL_H