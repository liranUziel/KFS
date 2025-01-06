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
    UP,
    DOWN,
    LEFT,
    RIGHT
}DIRECTION;

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

void clearScreen();
void fillScreen(int color);
void printMenu();
void keyPressHandler(int key);
void fillLineChar(char c, int color, int line);
void hightlightOn(int color);
void hightlightOff();
void printTextInLine(char* text, int color, int line);
void printText(char* text, int color,POSITION_H hposOffset,POSITION_V vposOffset );
void deletePrevChar(int n);
void moveCurer(DIRECTION dir);



#endif // ANSI_H