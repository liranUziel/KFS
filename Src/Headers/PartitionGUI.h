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
#include <stdbool.h>
#include "Table.h"

typedef struct _RGB{
    int r;
    int g;
    int b;
} RGB;


typedef enum BACKGROUND_COLOR{
    BG_BLACK = 0,
    BG_RED = 1,
    BG_GREEN = 2,
    BG_YELLOW = 3,
    BG_BLUE = 4,
    BG_MAGENTA = 5,
    BG_CYAN = 6,
    BG_WHITE = 7
}BACKGROUND_COLOR;

typedef enum FORGROUND_COLOR{
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7
}FORGROUND_COLOR;
// row mode for windows / linux
void enableRowMode();
void disableRowMode();


void printCharRGB(char c, int line,RGB color);
void printCharANSI(char c, int line,BACKGROUND_COLOR bg_color, FORGROUND_COLOR fg_color);
void printSTringRGB(char* str, int line, RGB color);
void printStringANSI(char* str, int line, int bg_color, int fg_color);
void printCharRGBxy(char c,RGB color, int x, int y);
void printCharANSIxy(char c,int bg_color, int fg_color, int x, int y);
void fiilScreenRGB(RGB color);
void fillScreenANSI(BACKGROUND_COLOR bg_color);
void setBackGroundColorRGB(RGB color);
void setForgroundColorRGB(RGB color);
void setBackGroundColorANSI(BACKGROUND_COLOR color);
void setForgroundColorANSI(FORGROUND_COLOR color);




// typedef enum _FORGROUND_COLOR{
//     BLACK = 0,
//     RED = 1,
//     GREEN = 2,
//     YELLOW = 3,
//     BLUE = 4,
//     MAGENTA = 5,
//     CYAN = 6,
//     WHITE = 7
// }FORGROUND_COLOR;

// typedef enum _BACKGROUND_COLOR{
//     BG_BLACK = 0,
//     BG_RED = 1,
//     BG_GREEN = 2,
//     BG_YELLOW = 3,
//     BG_BLUE = 4,
//     BG_MAGENTA = 5,
//     BG_CYAN = 6,
//     BG_WHITE = 7
// }BACKGROUND_COLOR;


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

#define KEY_BACKSPACE 8
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_C 99
#define KEY_S 115
#define KEY_F 102
#define KEY_Q 113





// WINDOWS os
void enableRowModeW();
void disableRowModeW();
void getCursorPosition(int* x, int* y);
void getConsoleSize(int* x, int* y);


// LINUX os
void enableRowModeL();
void disableRowModeL();

Table partitionMenuGUI(int *selectedPartitionID);
void printTableT(Table partitionTable);
void printTableHeader(TableHeader header);
void printTableRow(TableRow row);
void printTableRowI(TableRow row, int index);
void addPartitionToTable(Table* partitionTable, char name, int size, bool formated, bool selected,bool isDefault);
void markPartitionAsFormated(Table* partitionTable, char name,int index);
void markPartitionAsSelected(Table* partitionTable, char name,int index);
void unmarkPartitionAsSelected(Table* partitionTable, char name,int index);
void unmarkPartitionAsFormated(Table* partitionTable, char name,int index); 
void markPartitionAsDefault(Table* partitionTable, char name,int index);
void unmarkPartitionAsDefault(Table* partitionTable, char name,int index);

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
void setBackGroundColor(int color);
void deletePrevChar(int n);
void moveCursorDiration(DIRECTION dir);
void moveCursor(int x, int y);

int getKeyPress();
void readNumber(char* numberBuffer,bool output);

void addPartiotnName();


void printBoxMidScreen(char* text, int bg_color,int fg_color, int width, int height);

void dumpTerminalPartitionInfo();
void printPartitionLine(int index,bool checked);
void printBottomBar(int highlightColor,int defaultColor);
// not implamented  
void saveTerminalBuffer();
void restoreTerminalBuffer();

void resetColor();
void resetScreen();

#endif // ANSI_H