#include <windows.h>
#include <stdio.h>

int x,y;
int prev_x, prev_y;


int main()
{
    POINT cursor;
    if(GetCursorPos(&cursor))
    {
        x = cursor.x;
        y = cursor.y;
        prev_x = x;
        prev_y = y;
        printf("X: %d Y: %d\r", x, y);
        while(TRUE){
            if(GetCursorPos(&cursor))
            {
                prev_x = x;
                prev_y = y;
                x = cursor.x;
                y = cursor.y;
                if(prev_x != x || prev_y != y)
                {
                    printf("X: %d Y: %d\r", x, y);
                    if(x == y){
                        MessageBox(NULL, "X and Y are equal to-->", "Equal", MB_OK);
                    }
                }
            }
            else
            {
                fprintf(stderr, "Error getting cursor position\nERROR CODE: %d\n", GetLastError());
            }
        }
    }
    else
    {

       fprintf(stderr, "Error getting cursor position\nERROR CODE: %d\n", GetLastError());
    }
  
    return 0;
}