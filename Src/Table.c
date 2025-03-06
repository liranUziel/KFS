/*
Table createTable(int columnsCount, int rowsCount){
    Table table;
    table.columnsCount = columnsCount;
    table.rowsCount = rowsCount;
    table.header.columns = (char**)malloc(sizeof(char*) * columnsCount);
    table.rows = (TableRow*)malloc(sizeof(TableRow) * rowsCount);
    for(int i = 0; i < columnsCount; i++){
        table.header.columns[i] = (char*)malloc(sizeof(char) * 256);
    }
    for(int i = 0; i < rowsCount; i++){
        table.rows[i].columns = (char**)malloc(sizeof(char*) * columnsCount);
        for(int j = 0; j < columnsCount; j++){
            table.rows[i].columns[j] = (char*)malloc(sizeof(char) * 256);
        }
    }
    return table;
}
*/

#include "./Headers/Table.h"


TableHeader createTableHeader(int columnsCount,int headerLength){
    TableHeader header;
    header.HeaderLength = headerLength;
    header.columnsCount = columnsCount;
    header.columnsLength = floor(headerLength/columnsCount);
    header.columns = (char**)malloc(sizeof(char*) * columnsCount);
    return header;
}

TableError addTableHeaderColumn(TableHeader* header,unsigned int index,char* text){
    int len = strlen(text);
    if(index >= header->columnsCount){
        return TABLE_OUT_OF_BOUNDS;
    }
    header->columns[index] = (char*)malloc(sizeof(char) * len);
    strcpy(header->columns[index], text);
    return TABLE_OK;
}

void printHeader(TableHeader* header){
    //print the header line
    for(int i = 0; i < header->columnsCount; i++){
       //print the cloumn name with padding to match the header length
        printf("%-*s", header->columnsLength, header->columns[i]);
    }
    printf("\n");
}
Table createTable(int columnsCount, int maxRowsCount,TableHeader* header,unsigned int headerLength){
    Table table;
    table.columnsCount = columnsCount;
    table.maxRowsCount = maxRowsCount;
    table.rowsCount = 0;
    (*header) = createTableHeader(columnsCount, headerLength);
    // allocate memory for the rows
    table.rows = (TableRow*)malloc(sizeof(TableRow) * maxRowsCount);
    return table;
}
TableError addTableHeader(Table* table,TableHeader header){
    table->header = header;
    return TABLE_OK;
}
TableRow createTableRow(int columnsCount){
    TableRow row;
    row.columnsCount = columnsCount;
    row.columns = (char**)malloc(sizeof(char*) * columnsCount);
    return row;
}
TableError addTableRowColumn(TableRow* row,unsigned int index,char* text){
    int len = strlen(text);
    if(index >= row->columnsCount){
        return TABLE_OUT_OF_BOUNDS;
    }
    row->columns[index] = (char*)malloc(sizeof(char) * len);
    strcpy(row->columns[index], text);
    return TABLE_OK;
}
TableError createTableRowText(TableRow* row,char** text, int columnsCount){
    if(row->columnsCount != columnsCount){
        return TABLE_INVALID_ROW;
    }
    row->columns = (char**)malloc(sizeof(char*) * columnsCount);
    for(int i = 0; i < columnsCount; i++){
        int len = strlen(text[i]);
        row->columns[i] = (char*)malloc(sizeof(char) * len);
        strcpy(row->columns[i], text[i]);
    }
    return TABLE_OK;
}
TableError addTableRow(Table* table,TableRow row){
    if(table->rowsCount >= table->maxRowsCount){
        return TABLE_OUT_OF_BOUNDS;
    }
    table->rows[table->rowsCount] = row;
    table->rowsCount++;
    return TABLE_OK;
}
void printTable(Table* table){
    printHeader(&table->header);
    for(int i = 0; i < table->rowsCount; i++){
        for(int j = 0; j < table->columnsCount; j++){
            printf("%-*s", table->header.columnsLength, table->rows[i].columns[j]);
        }
        printf("\n");
    }
}

void Tablerun(){
    TableHeader header;
    Table table = createTable(4, 10, &header,100);
    addTableHeaderColumn(&header, 0, "Name");
    addTableHeaderColumn(&header, 1, "Type");
    addTableHeaderColumn(&header, 2, "Address");
    addTableHeaderColumn(&header, 3, "Size");
    addTableHeader(&table, header);
    TableRow row = createTableRow(5);
    //
    addTableRowColumn(&row, 0, "C");
    addTableRowColumn(&row, 1, "KFS");
    addTableRowColumn(&row, 2, "0x0000001A");
    addTableRowColumn(&row, 3, "8Mb");
    addTableRow(&table, row);
    //
    printTable(&table);

}