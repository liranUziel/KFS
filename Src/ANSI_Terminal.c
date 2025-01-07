#include "./Headers/ANSI_Terminal.h"

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
void getCursorPosition(int* x, int* y){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        *x = csbi.dwCursorPosition.X;
        *y = csbi.dwCursorPosition.Y;
    }
}
void getConsoleSize(int* x, int* y){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        *x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

}
void hideCursor(){
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
void showCursor(){
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
#endif // _WIN32

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

#endif // __linux__

int selectedRow = 1;
int totalRows = 0;
STATE state = IDLE;
char partitionNames[256] = {0};
int partitionSizes[256] = {0};
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

void printMenuBar(MenuBar* menuBar){
    
    
}

void addPartiotnName(){
    if(totalRows >= maxRows){
        return;
    }
    char last = partitionNames[totalRows - 1];
    partitionNames[totalRows] = last + 1;
    totalRows++;
}

int stringToInt(char* str){
    int result = 0;
    for(int i = 0; i < strlen(str); i++){
        result = result * 10 + (str[i] - '0');
    }
    return result;
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

void setBackGroundColor(int color){
    printf("\033[48;5;%dm", color);
}
void moveCursor(int x, int y){
    printf("\033[%d;%dH", y, x);
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
    if(state == CREATE_PARTITION){
        int lastLine;
        int total = 0;
        char digit;
        if(key >= 48 && key <= 57){
            digit = key;
            printf("%c", digit);
            total = total * 10 + (digit - '0');
        }
        if(key == KEY_ENTER){
            partitionSizes[selectedRow - 1] = total;
            state = IDLE;
        }
    }
    switch (key)
    {
    case UP_ARROW:
        if(state == IDLE){
            if(selectedRow > 1){
                if(selectedRow - 1 >= 0){
                    selectedRow--;
                }
                char text[256] = {0};
                sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[selectedRow],0);
                printTextInLine(text, 15, 2 + selectedRow + 1);
                sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[selectedRow-1],0);
                printTextInLine(text, 15, 2 + selectedRow);
            }
            break;
        }
    case DOWN_ARROW:
        if(state == IDLE){
            if(selectedRow < totalRows){
                if(selectedRow + 1 <= totalRows){
                    selectedRow++;
                }
                char text[256] = {0};
                sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[selectedRow - 1],0);
                printTextInLine(text, 15, 2 + selectedRow);
                sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[selectedRow - 2],0);
                printTextInLine(text, 15, 2 + selectedRow-1);
            }
            break;
        }   
    case KEY_C:
        if(state == IDLE){
            addPartiotnName();
            for(size_t i = 0; i < totalRows; i++){
                //Generate string "\t\t[ ] %c" where %c is the partition name in partitionNames[i]
                char text[256] = {0};
                sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[i],0);
                printTextInLine(text, 15, 3 + i);
            }
            // apply the selected row *
            char text[256] = {0};
            sprintf(text, "\t\t[*] %c\t\t%d", partitionNames[selectedRow - 1],0);
            printTextInLine(text, 15, 2 + selectedRow);
            int lastLine;
            getConsoleSize(&lastLine, &lastLine);
            clearLine(lastLine-1, 4, 15);
            printText("\t\tEnter Patition size in Bytes: ",15,LEFT,BOTTOM);
            int textLen = strlen("\t\tEnter Patition size in Bytes: ");
            textLen += 14;
            textLen += 1;
            getConsoleSize(&lastLine, &lastLine);
            moveCursor(textLen, lastLine-1);
            state = CREATE_PARTITION;
        }else{

        }
        break;
    case KEY_ENTER:
        // select the current row
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
void printMenu(){
    // clearScreen();
    //build a blue screen
    fillScreen(4);
    hightlightOn(15);
    fillLineChar(' ', 15, 0);
    printText("PiPS", 15, CENTER, TOP);
    printText("Pizza Partition Center 🍕", 15, CENTER,-1);
    hightlightOn(4);
    // read the partition table
    partitionNames[totalRows] = 'C';
    totalRows = 1;
    printTextInLine("\t\t[*] C\t\t0", 15, 3);
    for(size_t i = 1; i < totalRows; i++){
        //Generate string "\t\t[ ] %c" where %c is the partition name in partitionNames[i]
        char text[256] = {0};
        sprintf(text, "\t\t[ ] %c\t\t%d", partitionNames[i],0);
        printTextInLine(text, 15, 3 + i);
    }
    hightlightOn(15);
    printText(" Create Partition",15,LEFT,BOTTOM);
    hightlightOn(4);
    hideCursor();
    enableRowModeW();
    while(1){
        if(kbhit()){
            int key = getch();
            if(key == 113){
                break;
            }
            keyPressHandler(key);
        }
    }
    resetColor();
    resetScreen();
    disableRowModeW();
    showCursor();
}


