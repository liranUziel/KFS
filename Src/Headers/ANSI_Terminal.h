#ifndef ANSI_TERMINAL_H

#define ANSI_TERMINAL_H


#include <stdio.h>


typedef struct _TableHeader{
    int columnsCount;
    char** columns;
}TableHeader;

typedef struct _TableRow{
    int columnsCount;
    char** columns;
}TableRow;

typedef struct _Table{
    int columnsCount;
    int rowsCount;
    TableHeader header;
    TableRow* rows;
}Table;



#endif // ANSI_H