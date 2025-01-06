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
int totalRows = 2;

void clearScreen(){
    printf("\033[2J");
    printf("\033[1;1H");
}
void setBackGroundColor(int color){
    printf("\033[48;5;%dm", color);
}
void moveCursor(int x, int y){
    printf("\033[%d;%dH", y, x);
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
    switch (key)
    {
    case UP_ARROW:
        // change the selected row
        if(selectedRow > 1){
            selectedRow--;
            printTextInLine("\t\t[*]", 15, 2);
            printTextInLine("\t\t[ ]", 15, 3);
        }
        break;
    case DOWN_ARROW:
        // change the selected row
        if(selectedRow < totalRows){
            selectedRow++;
            printTextInLine("\t\t[ ]", 15, 2);
            printTextInLine("\t\t[*]", 15, 3);
        }
        break;
    case KEY_C:
        // create a new partition
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
    printTextInLine("\t\t[*] C ", 15, 2);
    printTextInLine("\t\t[ ] D ", 15, 3);
    hightlightOn(15);
    printText(" Create Partition",15,LEFT,BOTTOM);
    hightlightOn(4);
    hideCursor();
    enableRowModeW();
    while(1){
        if(kbhit()){
            int key = getch();
            // printf("key: %d\n", key);
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


