#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <string.h>


typedef enum CMD_TYPE{
    CMD_EXIT,
    CMD_HELP,
    CMD_UNKNOWN
}CMD_TYPE;

//List of commands string to print or compare
extern const char *CMD_STR[];

void printHelp();
void dispatch(char *cmd);

#endif // TERMINAL_H