#include "./Headers/PartitionGUI.h"

// #region WINDOWS

#ifdef _WIN32
void enableRowModeW(){
    // enable raw mode for windows
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(hStdin, mode);
}

void disableRowModeW(){
    // disable raw mode for windows
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode |= (ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(hStdin, mode);
}
void getCursorPositionW(int* x, int* y){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        *x = csbi.dwCursorPosition.X;
        *y = csbi.dwCursorPosition.Y;
    }
}
void getConsoleSizeW(int* x, int* y){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        *x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

}
void hideCursorW(){
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
void showCursorW(){
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
#endif // _WIN32
// #endregion WINDOWS

// #region LINUX
#ifdef __linux__
void enableRowModeL(){
    // enable raw mode for linux
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disableRowModeL(){
    // disable raw mode for linux
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void getCursorPositionL(int* x, int* y){
    printf("\033[6n");
    scanf("\033[%d;%dR", y, x);
}

void getConsoleSizeL(int* x, int* y){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *x = w.ws_col;
    *y = w.ws_row;
}
#endif // __linux__

// #endregion LINUX



// #region GUI

void disableRowMode(){
    #ifdef _WIN32
    disableRowModeW();
    #endif
    #ifdef __linux__
    disableRowModeL();
    #endif
}

void enableRowMode(){
    #ifdef _WIN32
    enableRowModeW();
    #endif
    #ifdef __linux__
    enableRowModeL();
    #endif
}

void getConsoleSize(int* x, int* y){
    #ifdef _WIN32
    getConsoleSizeW(x, y);
    #endif
    #ifdef __linux__
    getConsoleSizeL(x, y);
    #endif
}

unsigned int CONSOLE_WIDTH = 0;
unsigned int CONSOLE_HEIGHT = 0;

// #region data
// char** ScreenBuffer;
const unsigned int MAX_ROWS = 10;
unsigned int rowCount = 0;
unsigned int formatedQueue = 0;
STATE menuState = IDLE;

// #endregion data

// Screen COLORS

int backgroundColor = 4;
int forgroundColor = 7;
int hightlightColor = 7;
int hightlightForground = 4;

// #region print_in_color

void printCharRGB(char c, int line,RGB color){
    //move curser to line
    if(line > 0  && line < CONSOLE_HEIGHT){
        moveCursor(0, line);
    }
    printf("\033[38;2;%d;%d;%dm%c", color.r, color.g, color.b, c);
}

void printCharANSI(char c, int line,BACKGROUND_COLOR bg_color, FORGROUND_COLOR fg_color){
    if(line > 0  && line < CONSOLE_HEIGHT){
        moveCursor(0, line);
    }
    printf("\033[48;5;%dm", bg_color);
    printf("\033[38;5;%dm%c", fg_color, c);
}

void printSTringRGB(char* str, int line, RGB color){
    if(line > 0  && line < CONSOLE_HEIGHT){
        moveCursor(0, line);
    }
    printf("\033[38;2;%d;%d;%dm%s", color.r, color.g, color.b, str);
}

void printStringANSI(char* str, int line, int bg_color, int fg_color){
    int x,y;
    getCursorPosition(&x, &y);
    if(line > 0  && line < CONSOLE_HEIGHT){
        moveCursor(x, line);
    }
    printf("\033[48;5;%dm", bg_color);
    printf("\033[38;5;%dm%s", fg_color, str);
}

void printCharRGBxy(char c,RGB color, int x, int y){
    moveCursor(x, y);
    printf("\033[38;2;%d;%d;%dm%c", color.r, color.g, color.b, c);
}

void printCharANSIxy(char c,int bg_color, int fg_color, int x, int y){
    moveCursor(x, y);
    printf("\033[48;5;%dm", bg_color);
    printf("\033[38;5;%dm%c", fg_color, c);
}

void fiilScreenRGB(RGB color){
    if(CONSOLE_HEIGHT == 0 && CONSOLE_WIDTH == 0){
        getConsoleSize(&CONSOLE_WIDTH, &CONSOLE_HEIGHT);
    }
    moveCursor(0, 0);
    for(size_t row = 0; row < CONSOLE_HEIGHT; row++){
        for(size_t col = 0; col < CONSOLE_WIDTH; col++){
            printCharRGB(' ', row, color);
        }
    }
}

void fillScreenANSI(BACKGROUND_COLOR bg_color){
    if(CONSOLE_HEIGHT == 0 && CONSOLE_WIDTH == 0){
        getConsoleSize(&CONSOLE_WIDTH, &CONSOLE_HEIGHT);
    }

    moveCursor(0, 0);
    for(size_t row = 0; row < CONSOLE_HEIGHT; row++){
        for(size_t col = 0; col < CONSOLE_WIDTH; col++){
            printCharANSIxy(' ',backgroundColor, forgroundColor, col, row);
        }
    }
    moveCursor(0, 0);
}

void clearScreen(){
    printf("\033[2J");
    printf("\033[1;1H");
}

void clearLine(int line, int bg_color, int fg_color){
    int x, y;
    getCursorPosition(&x, &y);
    moveCursor(0, line);
    printf("\033[48;5;%dm", bg_color);
    printf("\033[38;5;%dm", fg_color);
    int w, h;
    getConsoleSize(&w, &h);
    for(int i = 0; i < w; i++){
        printf(" ");
    }
    moveCursor(x, y);
}

void setBackGroundColorANSI(BACKGROUND_COLOR color){
    printf("\033[48;5;%dm", color);
}

// #endregion print_in_color


// #region curser

void moveCursor(int x, int y){
    printf("\033[%d;%dH", y, x);
}

void moveCursorMiddle(){
    int x, y;
    getConsoleSize(&x, &y);
    moveCursor(x/2, y);
}

void moveCursorCenter(){
    int x, y;
    getConsoleSize(&x, &y);
    moveCursor(x/2, y/2);
}

void shiftCursor(int offsetX,int offsetY){
    int x, y;
    getCursorPosition(&x, &y);
    moveCursor(x + offsetX, y + offsetY);
}

void getCursorPosition(int* x, int* y){
    #ifdef _WIN32
    getCursorPositionW(x, y);
    #endif
    #ifdef __linux__
    getCursorPositionL(x, y);
    #endif
}

void hideCursor(){
    #ifdef _WIN32
    hideCursorW();
    #endif
}

void showCursor(){
    #ifdef _WIN32
    showCursorW();
    #endif
}

// #endregion curser

// #endregion GUI

unsigned int getSelectedRow(Table* table){
    for(int i = 0; i < table->rowsCount; i++){
        if(strcmp(table->rows[i].columns[3], "Active") == 0){
            return i;
        }
    }
}

/**
 * @name partitionMenuGUI
 * @algorithm
 * 1. Create a table with the following columns
 *      1.1 Partition Name
 *      1.2 Partition Size
 *      1.3 Formated
 *      1.4 Selected
 * 2. Display menu bar with the following options
 *      2.1 Create Partition
 *      2.2 Delete Partition
 *      2.3 Format Partition
 *      2.4 Select Partition
 *      2.5 Quit
 * 3. Read the user input
 *     3.1 if the user press c create a new partition
 *     3.2 if the user press d delete the selected partition
 *     3.3 if the user press f format the selected partition
 *     3.4 if the user press s select the selected partition
 *     3.5 if the user press q quit the partition menu 
 * 4. For etch partition created add a new row to the table
 * 5. Return the table
 * @return Table 
 */
Table partitionMenuGUI(int *selectedPartitionID){
    getConsoleSize(&CONSOLE_WIDTH, &CONSOLE_HEIGHT);

    bool isRunning = true;

    int key;
    int selectedPartition = -1;
    int defaultPartition = -1;

    TableHeader header;
    Table table = createTable(5, 10, &header,50);
    addTableHeaderColumn(&header, 0, "Name");
    addTableHeaderColumn(&header, 1, "Size");
    addTableHeaderColumn(&header, 2, "Formated");
    addTableHeaderColumn(&header, 3, "Selected");
    addTableHeaderColumn(&header, 4, "Default");
    addTableHeader(&table, header);
    
    // Start GUI
    // ScreenBuffer = (char**)malloc(sizeof(char*) * CONSOLE_HEIGHT);
    // for(int i = 0; i < CONSOLE_HEIGHT; i++){
    //     ScreenBuffer[i] = (char*)malloc(sizeof(char) * CONSOLE_WIDTH);
    // }
    fillScreenANSI(backgroundColor);
    fillLineChar(' ', hightlightColor, 1);
    moveCursorCenter();
    int xOffset = strlen("PiPS") / 2;
    shiftCursor(-xOffset, 0);
    printStringANSI("PiPS", 1, hightlightColor, hightlightForground);
    fillLineChar(' ', hightlightColor, 2);
    xOffset = strlen("Pizza Partition Center") / 2;
    shiftCursor(-xOffset, 0);
    printStringANSI("Pizza Partition Center", 2, hightlightColor, hightlightForground);
    // printTable(table);// maybe not needed
    fillLineChar(' ', hightlightColor, CONSOLE_HEIGHT-1);
    moveCursor(0, CONSOLE_HEIGHT-1);
    printStringANSI(" Create Partition\tFormat Partition\tSelect Partition\tDelete Parition\tQuit", CONSOLE_HEIGHT-1, hightlightColor, hightlightForground);
    moveCursor(0, 5);
    hightlightOn(backgroundColor);
    int x, y;
    getConsoleSize(&x, &y);
    int tableWidth = header.columnsLength * header.columnsCount;
    int tableX = (x - tableWidth) / 2;
    moveCursor(tableX, 5);
    printTableHeader(header);
    // printTableT(table);
    hideCursor();
    enableRowMode();
    while(isRunning){
        key = getKeyPress();
        switch (menuState)
        {
            case CREATE_PARTITION:
                if(key >= 48 && key <= 57){
                    // Print the digit
                    if(key == 48){
                        printf("%c", key);
                        // Igone the leading zeros
                        while(key == 48){
                            key = getKeyPress();                    
                        }
                    }else{
                        printf("%c", key);
                    }
                    // Read the number to a buffer
                    char numberString[256] = {0};
                    numberString[0] = key;
                    readNumber(numberString+1, true);
                    // Conver the number to string
                    int sizeNumber = stringToInt(numberString);
                    // Add the partition to the table
                    addPartitionToTable(&table, 'C' + rowCount, sizeNumber,false, rowCount==0,rowCount==0);
                    if(rowCount == 0){
                        defaultPartition = rowCount;
                    }
                    // Print the row to the table
                    selectedPartition = rowCount==0?0:selectedPartition;
                    rowCount++;
                    setBackGroundColorANSI(backgroundColor);
                    printTableRowI(table.rows[table.rowsCount-1], table.rowsCount-1);
                    // reset the commant to IDLE
                    moveCursor(0, CONSOLE_HEIGHT-1);
                    fillLineChar(' ', hightlightColor, CONSOLE_HEIGHT-1);
                    printStringANSI(" Create Partition\tFormat Partition\tSelect Partition\tDelete Parition\tQuit", CONSOLE_HEIGHT-1, hightlightColor, hightlightForground);
                    setBackGroundColor(backgroundColor);
                    menuState = IDLE;
                }
                break;
            case IDLE:
                switch (key)
                {
                case 'c':
                    formatedQueue++;
                    moveCursor(0, CONSOLE_HEIGHT-1);
                    fillLineChar(' ', hightlightColor, CONSOLE_HEIGHT-1);
                    xOffset = strlen("\t\tEnter Patition size in Bytes: ") / 2;
                    shiftCursor(-xOffset, 0);
                    printStringANSI("\t\tEnter Patition size in Bytes: ", CONSOLE_HEIGHT-1, hightlightColor, hightlightForground);
                    menuState = CREATE_PARTITION;
                    break;
                case 'd':
                    break;
                case 'f':
                    if(formatedQueue > 0){
                        markPartitionAsFormated(&table, 'C' + selectedPartition, selectedPartition);
                        printTableRowI(table.rows[selectedPartition], selectedPartition);
                        formatedQueue--;
                    }
                    break;
                case 's':
                    if(selectedPartition != defaultPartition){
                        unmarkPartitionAsDefault(&table, 'C' + defaultPartition, defaultPartition);
                        markPartitionAsDefault(&table, 'C' + selectedPartition, selectedPartition);
                        defaultPartition = selectedPartition;
                    }
                    break;
                case 'q':
                    if(formatedQueue == 0 && rowCount > 0){
                        isRunning = false;
                    }
                    break;
                // Arrows up | down | left | right
                case UP_ARROW:
                    // up
                    // make sure the current selected row is not the first row
                    // need to change the current selected row
                    // redraw the new current selected row and row below it
                    if(selectedPartition >= 1){
                        selectedPartition--;
                        moveCursor(0, CONSOLE_HEIGHT-4);
                        markPartitionAsSelected(&table, ' ', selectedPartition);
                        unmarkPartitionAsSelected(&table, ' ', selectedPartition+1);
                        printTableRowI(table.rows[selectedPartition], selectedPartition);
                        printTableRowI(table.rows[selectedPartition+1], selectedPartition+1);
                    }
                    break;
                case DOWN_ARROW:
                    // down
                    // make sure the current selected row is not the last row
                    // need to change the current selected row
                    // redraw the new current selected row and row above it
                    if(selectedPartition < rowCount-1){
                        selectedPartition++;
                        moveCursor(0, CONSOLE_HEIGHT-4);
                        markPartitionAsSelected(&table, ' ', selectedPartition);
                        unmarkPartitionAsSelected(&table, ' ', selectedPartition-1);
                        printTableRowI(table.rows[selectedPartition-1], selectedPartition-1);
                        printTableRowI(table.rows[selectedPartition], selectedPartition);
                    }
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
        }     
    }
    resetColor();
    resetScreen();
    disableRowMode();
    showCursor();
    *selectedPartitionID = selectedPartition;
    return table;
}

void printTableT(Table partitionTable){
    // make the table print the in the middle of the screen
    int x, y;
    getConsoleSize(&x, &y);
    int tableWidth = partitionTable.header.columnsLength * partitionTable.header.columnsCount;
    int tableX = (x - tableWidth) / 2;
    moveCursor(tableX, 5);
    printTableHeader(partitionTable.header);
    for (int i = 0; i < partitionTable.rowsCount; i++) {
        printTableRowI(partitionTable.rows[i], i);
    }
}

void printTableRow(TableRow row){
    if(strcmp(row.columns[3], "Active") == 0){
        printf("[*]       ");
    }else{
        printf("[ ]       ");
    }
    printf("%-*s %-*s ", 10, row.columns[0], 10, row.columns[1]);
    if(strcmp(row.columns[2], "Formatted") == 0){
        printf("KFS");
    }
    printf("\n");
    
}

void printTableHeader(TableHeader header){
    //print header etch column take header.columnsLength
    for (int i = 0; i < header.columnsCount-2; i++) {
        printf("%-*s ", header.columnsLength, header.columns[i]);
    }
    printf("\n");
}

void printTableRowI(TableRow row, int index){
    //print the row so Name is offset to the start of the table
    int width = 0;
    int high = 0;
    getConsoleSize(&width, &high);
    int x = (width - 30) / 2;
    x -= 18;
    moveCursor(x, 6 + index);
    printTableRow(row);
}

/**
 * @name addPartitionToTable
 * @algorithm 
 *  create a new row with the following columns
 * @param partitionTable 
 * @param name 
 * @param size 
 * @param formated 
 * @param selected 
 */
void addPartitionToTable(Table* partitionTable, char name, int size, bool formated, bool selected,bool isDefault){
    if(name < 'C' || name > 'Z'){
        return;
    }
    if(size < 0){
        return;
    }
    if (partitionTable->rowsCount >= partitionTable->maxRowsCount) {
        printf("Table is full, cannot add more partitions.\n");
        return;
    }
    TableRow row = createTableRow(partitionTable->columnsCount);
    char sizeStr[20];
    sprintf(sizeStr, "%d", size);
    char partitionName[2] = {name, '\0'};
    addTableRowColumn(&row, 0, partitionName);
    addTableRowColumn(&row, 1, sizeStr);
    addTableRowColumn(&row, 2, formated ? "Formatted" : "Unformatted");
    addTableRowColumn(&row, 3, selected ? "Active" : "Inactive");
    addTableRowColumn(&row, 4, isDefault ? "Default" : "Not Default");
    addTableRow(partitionTable, row);
}

void markPartitionAsFormated(Table* partitionTable, char name,int index){
    if(index == -1){
        for(int i = 0; i < partitionTable->rowsCount; i++){
            if(partitionTable->rows[i].columns[0][0] == name){
                partitionTable->rows[i].columns[2] = "Formatted";
            }
        }
    }
    else{
        partitionTable->rows[index].columns[2] = "Formatted";
    }   
    }

void markPartitionAsSelected(Table* partitionTable, char name,int index){
    if(index == -1){
        for(int i = 0; i < partitionTable->rowsCount; i++){
            if(partitionTable->rows[i].columns[0][0] == name){
                partitionTable->rows[i].columns[3] = "Active";
            }
        }
    }
    else{
        partitionTable->rows[index].columns[3] = "Active";
    }
}

void unmarkPartitionAsSelected(Table* partitionTable, char name,int index){
    if(index == -1){
        for(int i = 0; i < partitionTable->rowsCount; i++){
            if(partitionTable->rows[i].columns[0][0] == name){
                partitionTable->rows[i].columns[3] = "Inactive";
            }
        }
    }
    else{
        partitionTable->rows[index].columns[3] = "Inactive";
    }
}

void markPartitionAsDefault(Table* partitionTable, char name,int index){
    if(index == -1){
        for(int i = 0; i < partitionTable->rowsCount; i++){
            if(partitionTable->rows[i].columns[0][0] == name){
                partitionTable->rows[i].columns[4] = "Default";
            }
        }
    }
    else{
        partitionTable->rows[index].columns[4] = "Default";
    }
}

void unmarkPartitionAsDefault(Table* partitionTable, char name,int index){
    if(index == -1){
        for(int i = 0; i < partitionTable->rowsCount; i++){
            if(partitionTable->rows[i].columns[0][0] == name){
                partitionTable->rows[i].columns[4] = "Not Default";
            }
        }
    }
    else{
        partitionTable->rows[index].columns[4] = "Not Default";
    }
}

void unmarkPartitionAsFormated(Table* partitionTable, char name,int index){
    if(index == -1){
        for(int i = 0; i < partitionTable->rowsCount; i++){
            if(partitionTable->rows[i].columns[0][0] == name){
                partitionTable->rows[i].columns[2] = "Unformatted";
            }
        }
    }
    else{
        partitionTable->rows[index].columns[2] = "Unformatted";
    }
}

int TERMINAL_WIDTH = 0;
int TERMINAL_HEIGHT = 0;

int selectedRow = 1;
int totalRows = 0;
int seletedPartition = -1;
STATE state = IDLE;
char partitionNames[256] = {0};
int partitionSizes[256] = {0};
bool formated[256] = {false};
int totalFormated = 0;
int maxRows = 10;

MenuBar createMenuBar(POSITION_H hposOffset, POSITION_V vposOffset, int verticalOffet, int fg_color, int bg_color){
    MenuBar menuBar;
    menuBar.items = (MenuItem*)malloc(sizeof(MenuItem));
    menuBar.totalItems = 0;
    menuBar.size = 1;
    menuBar.hposOffset = hposOffset;
    menuBar.vposOffset = vposOffset;
    menuBar.verticalOffet = verticalOffet;
    menuBar.fg_color = fg_color;
    menuBar.bg_color = bg_color;
    return menuBar;
}

MenuItem createMenuItem(char* text, int fg_color, int bg_color, int selected){
    MenuItem item;
    item.text = text;
    item.fg_color = fg_color;
    item.bg_color = bg_color;
    item.selected = selected;
    return item;
}

void addMenuItem(MenuBar* menuBar, MenuItem item){
    if(menuBar->totalItems == menuBar->size){
        menuBar->size *= 2;
        menuBar->items = (MenuItem*)realloc(menuBar->items, menuBar->size * sizeof(MenuItem));
    }else{
        menuBar->items[menuBar->totalItems] = item;
    }
    menuBar->totalItems++;
}

void addPartiotnName(){
    if(totalRows >= maxRows){
        return;
    }
    if(totalRows == 0){
        partitionNames[totalRows] = 'C';
        seletedPartition = 0;
        totalRows++;
    }else{
        char last = partitionNames[totalRows-1];
        partitionNames[totalRows] = last + 1;
        totalRows++;
    }
   return;
}

int getKeyPress(){
    int key = -1;
    #ifdef _WIN32
    key = getch();
    #endif
    #ifdef __linux__
    key = getchar();
    #endif
    return key;
}

void readNumber(char* numberBuffer, bool output) {
    char ch;
    bool reading = true;
    while (reading) {
        ch = getch();
        if (ch >= '0' && ch <= '9') {
            *numberBuffer++ = ch;
            if (output) {
                printf("%c", ch);
            }
        } else if (ch == KEY_BACKSPACE) {
            if (numberBuffer > numberBuffer) {
                *numberBuffer = '\0';
                numberBuffer--;
                if (output) {
                    printf("\b \b");
                }
            }
        } else
        if(ch == KEY_ENTER){
            reading = false;
        }
    }
    *numberBuffer = '\0';
}

int stringToInt(char* str){
    int result = 0;
    for(int i = 0; i < strlen(str); i++){
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

void setBackGroundColor(int color){
    printf("\033[48;5;%dm", color);
}

void moveCursorDiration(DIRECTION dir){
    switch (dir)
    {
    case D_UP:
        printf("\033[A");
        break;
    case D_DOWN:
        printf("\033[B");
        break;
    case D_RIGHT:
        printf("\033[C");
        break;
    case D_LEFT:
        printf("\033[D");
        break;
    default:
        break;
    }
}

void fillScreen(int color){
    int x, y;
    clearScreen();
    moveCursor(0, 0);
    setBackGroundColor(color);
    getConsoleSize(&x, &y);
    for(int i = 0; i < y; i++){
        for(int j = 0; j < x; j++){
            moveCursor(j, i);
            printf(" ");
        }
    }
    moveCursor(0, 0);

}

void resetScreen(){
    printf("\033[0m");
    clearScreen();
}

void resetColor(){
    printf("\033[0m");
}
/**
 * @brief 
 * 
 * @param text - text to print
 * @param color - the color of the text
 * @param hposOffset - control the horizontal position offset of the text 0 for keep it as currnt or  LEFT, RIGHT, CENTER   
 * @param vposOffset - control the vertical position offset of the text 0 for keep it as currnt, -1 for next line or TOP, BOTTOM, MIDDLE
 */
void printText(char* text, int color,POSITION_H hposOffset,POSITION_V vposOffset ){

    int x, y;
    int xoffset = 0;
    int yoffset = 0;
    int h,w;
    getCursorPosition(&x, &y);
    getConsoleSize(&w, &h);
    int len = strlen(text);

    switch (hposOffset)
    {
        case LEFT:
            xoffset = 0;
            break;
        case RIGHT:
            xoffset = w - len;
            break;
        case CENTER:
            xoffset = (w - len) / 2;
            break;
        default:
            xoffset = 0;
            break;
    }
    switch (vposOffset)
    {
        case -1:
            yoffset = y + 1;
            break;
        case TOP:
            yoffset = 0;
            break;
        case BOTTOM:
            yoffset = h - 1;
            break;
        case MIDDLE:
            yoffset = (h - 1) / 2;
            break;
        default:
            yoffset = 0;
            break;
    }
    moveCursor(xoffset, yoffset);
    printf("\033[38;5;%dm%s", color, text);
    
}

void keyPressHandler(int key){
   
    switch (state)
    {
    case CREATE_PARTITION:
            //read number from the user
        if(key >= 48 && key <= 57){
            printf("%c", key);
            //read the rest of the number as string
            char strInt[256] = {0};
            strInt[0] = key;
            readNumber(strInt+1,true);
            int number = stringToInt(strInt);
            partitionSizes[totalRows-1] = number;
            //update the line
            hightlightOn(4);
            char text[256] = {0};
            if(selectedRow - 1 == totalRows-1){
                sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[totalRows - 1],partitionSizes[totalRows-1]);
            }else{
                sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[totalRows - 1],partitionSizes[totalRows-1]);
            }
            printTextInLine(text, 15, 2 + totalRows);
            //clear the last line
            int lastLine;
            hightlightOn(15);
            getConsoleSize(&lastLine, &lastLine);
            clearLine(lastLine-1, 15, 4);
            printBottomBar(15,4);
            state = IDLE;
        }
        break;
        if(key == KEY_ENTER){
            
        }
        break;
    case IDLE:
        switch (key)
        {
        case UP_ARROW:
            if(selectedRow > 1){
                if(selectedRow - 1 >= 0){
                    selectedRow--;
                }
                char text[256] = {0};
                sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[selectedRow-1],partitionSizes[selectedRow-1]);
                printTextInLine(text, 15, 2 + selectedRow);
                sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[selectedRow],partitionSizes[selectedRow]);
                printTextInLine(text, 15, 2 + selectedRow + 1);
            }
            break;
        case DOWN_ARROW:
            if(selectedRow < totalRows){
                if(selectedRow + 1 <= totalRows){
                    selectedRow++;
                }
                char text[256] = {0};
                sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[selectedRow - 2],partitionSizes[selectedRow-2]);
                printTextInLine(text, 15, 2 + selectedRow-1);
                sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[selectedRow - 1],partitionSizes[selectedRow-1]);
                printTextInLine(text, 15, 2 + selectedRow);
            }
            break;
        case KEY_C:
            addPartiotnName();
            for(size_t i = 0; i < totalRows; i++){
                if(i == selectedRow - 1){
                    printPartitionLine(i, true);
                }else{
                    printPartitionLine(i, false);
                }
            }
            int lastLine;
            getConsoleSize(&lastLine, &lastLine);
            clearLine(lastLine-1, 4, 15);
            hightlightOn(15);
            fillLineChar(' ', 15, lastLine-1);
            printText("\t\tEnter Patition size in Bytes: ",15,LEFT,BOTTOM);
            int textLen = strlen("\t\tEnter Patition size in Bytes: ");
            textLen += 14;
            textLen += 1;
            getConsoleSize(&lastLine, &lastLine);
            moveCursor(textLen, lastLine-1);
            state = CREATE_PARTITION;
            break;
        case KEY_S:
            break;  
        case KEY_F:
            // Mark the partition as formated
            // redraw the parition wit  \t\t KFS at the end
            if(!formated[selectedRow-1]){
                formated[selectedRow-1] = true;
                char text[256] = {0};
                sprintf(text, "\t\t[*] %c\t\t%d\t\tKFS", partitionNames[selectedRow-1],partitionSizes[selectedRow-1]);
                printTextInLine(text, 15, 2 + selectedRow);
                totalFormated++;
            }
            break;
        case KEY_ENTER:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void hightlightOn(int color){
    printf("\033[48;5;%dm", color);
}

void hightlightOff(){
    printf("\033[0m");
}

void printTextInLine(char* text, int color, int line){
    int x, y;
    getCursorPosition(&x, &y);
    moveCursor(0, line);
    printf("\033[38;5;%dm%s", color, text);
    moveCursor(x, y);
}

void fillLineChar(char c, int color, int line){
    int x, y;
    getCursorPosition(&x, &y);
    moveCursor(0, line);
    printf("\033[48;5;%dm", color);
    int w, h;
    getConsoleSize(&w, &h);
    for(int i = 0; i < w; i++){
        printf("%c", c);
    }
    moveCursor(x, y);
}

void printBoxMidScreen(char* text, int bg_color,int fg_color, int width, int height){
    /* print box:
    ╔═════════════════╗
    ║      text       ║
    ╚═════════════════╝    
    will make sure text fit in the box with  padding of one \t 
    at the mid of the screen
     */
    int x, y;
    getConsoleSize(&x, &y);
    int textLen = strlen(text);
    int xoffset = (x - width) / 2;
    int yoffset = (y - height) / 2;
    moveCursor(xoffset, yoffset);
    printf("\033[48;5;%dm", bg_color);
    printf("\033[38;5;%dm", fg_color);
    printf("╔");
    for(int i = 0; i < width - 2; i++){
        printf("═");
    }
    printf("╗");
    moveCursor(xoffset, yoffset + 1);
    printf("║");
    for(int i = 0; i < (width - textLen - 2) / 2; i++){
        printf(" ");
    }
    printf("%s", text);
    for(int i = 0; i < (width - textLen - 2) / 2; i++){
        printf(" ");
    }
    printf("║");
    moveCursor(xoffset, yoffset + 2);
    printf("╚");
    for(int i = 0; i < width - 2; i++){
        printf("═");
    }
    printf("╝");
    moveCursor(xoffset, yoffset + 3);
    printf("\033[0m");

}

void printMenu(){
    fillScreen(4);
    hightlightOn(15);
    fillLineChar(' ', 15, 0);
    printText("PiPS", 15, CENTER, TOP);
    printText("Pizza Partition Center", 15, CENTER,-2);
    hightlightOn(4);
    printBottomBar(15,4);
    hideCursor();
    enableRowModeW();
    while(1){
        if(kbhit()){
            int key = getch();
            if(key == KEY_Q){
                
                if(totalFormated == totalRows){
                    break;
                }
            }
            keyPressHandler(key);
        }
    }
    resetColor();
    resetScreen();
    showCursor();
    disableRowModeW();
    dumpTerminalPartitionInfo();
}

void printBottomBar(int highlightColor,int defaultColor){
    hightlightOn(highlightColor);
    fillLineChar(' ', highlightColor,BOTTOM);
    printText(" Create Partition\tFormat Partition\tSelect Partition\tDelete Parition\tQuit",15,LEFT,BOTTOM);
    hightlightOn(defaultColor);
}

void printPartitionLine(int index,bool checked){
    char text[256] = {0};
    if(checked){
        sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[index],partitionSizes[index]);
    }else{
        sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[index],partitionSizes[index]);
    }
    printTextInLine(text, 15, 3 + index);
    return;
}

void dumpTerminalPartitionInfo(){
    for(size_t i = 0; i < totalRows; i++){
        printf("%c\t%d KFS\n", partitionNames[i], partitionSizes[i]);
    }
}