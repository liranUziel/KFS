#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct _TableHeader{
    int columnsCount;
    int HeaderLength;
    int columnsLength;
    char** columns;
}TableHeader;

typedef struct _TableRow{
    int columnsCount;
    char** columns;
}TableRow;

typedef struct _Table{
    int columnsCount;
    int maxRowsCount;
    int rowsCount;
    TableHeader header;
    TableRow* rows;
}Table;

typedef enum _TableError{
    TABLE_OK,
    TABLE_OUT_OF_BOUNDS,
    TABLE_INVALID_COLUMN,
    TABLE_INVALID_ROW
}TableError;

TableHeader createTableHeader(int columnsCount,int headerLength);
TableError addTableHeaderColumn(TableHeader* header,unsigned int index,char* text);
TableError addTableHeader(Table* table,TableHeader header);
Table createTable(int columnsCount, int maxRowsCount,TableHeader* header,unsigned int headerLength);
TableRow createTableRow(int columnsCount);
TableError addTableRowColumn(TableRow* row,unsigned int index,char* text);
TableError createTableRowText(TableRow* row,char** text, int columnsCount);
TableError addTableRow(Table* table,TableRow row);
void printHeader(TableHeader* header);
void printTable(Table* table);
void Tablerun();



#endif // TABLE_H