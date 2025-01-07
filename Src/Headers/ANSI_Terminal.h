#ifndef ANSI_TERMINAL_H

#define ANSI_TERMINAL_H

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#endif // _WIN32

#ifdef __linux__
    #include <termios.h>
    #include <unistd.h>
#endif // __linux__

#include <stdio.h>

typedef enum _FORGROUND_COLOR{
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7
}FORGROUND_COLOR;

typedef enum _BACKGROUND_COLOR{
    BG_BLACK = 0,
    BG_RED = 1,
    BG_GREEN = 2,
    BG_YELLOW = 3,
    BG_BLUE = 4,
    BG_MAGENTA = 5,
    BG_CYAN = 6,
    BG_WHITE = 7
}BACKGROUND_COLOR;


typedef enum _ORIENTATION_H{
    LEFT,
    RIGHT,
    CENTER
}POSITION_H;

typedef enum _ORIENTATION_V{
    TOP,
    BOTTOM,
    MIDDLE
}POSITION_V;

typedef enum _DIRECTION{
    D_UP,
    D_DOWN,
    D_LEFT,
    D_RIGHT
}DIRECTION;


typedef enum _STATE{
    IDLE,
    CREATE_PARTITION,
    DELETE_PARTITION,
    FORMAT_PARTITION,
}STATE;
typedef struct _MENU_ITEM{
    char* text;
    int fg_color;
    int bg_color;
    int selected;
}MenuItem;

typedef struct _MENU_BAR{
    MenuItem* items;
    int totalItems;
    int size;
    POSITION_H hposOffset;
    POSITION_V vposOffset;
    int verticalOffet;
    int fg_color;
    int bg_color;
}MenuBar;

#define UP_ARROW 72
#define DOWN_ARROW 80
#define RIGHT_ARROW 67
#define LEFT_ARROW 68

#define KEY_ENTER 10
#define KEY_ESC 27
#define KEY_C 99



// WINDOWS os
void enableRowModeW();
void disableRowModeW();
void getCursorPosition(int* x, int* y);
void setCursorPosition(int x, int y);
void getConsoleSize(int* x, int* y);


// LINUX os
void enableRowModeL();
void disableRowModeL();

int stringToInt(char* str);

MenuBar createMenuBar(POSITION_H hposOffset, POSITION_V vposOffset, int verticalOffet, int fg_color, int bg_color);
MenuItem createMenuItem(char* text, int fg_color, int bg_color, int selected);
void addMenuItem(MenuBar* menuBar, MenuItem item);
void printMenuBar(MenuBar* menuBar);

void clearScreen();
void clearLine(int line, int bg_color, int fg_color);
void fillScreen(int color);
void printMenu();
void keyPressHandler(int key);
void fillLineChar(char c, int color, int line);
void hightlightOn(int color);
void hightlightOff();
void printTextInLine(char* text, int color, int line);
void printText(char* text, int color,POSITION_H hposOffset,POSITION_V vposOffset );
void deletePrevChar(int n);
void moveCursorDiration(DIRECTION dir);
void moveCursor(int x, int y);

void addPartiotnName();


#endif // ANSI_H