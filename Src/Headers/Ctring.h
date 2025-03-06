/**
 * @file Ctring.h
 * @author Liran Uziel (liran.uzi.l@gmail.com)
 * @brief This is a header file for Ctring.c
 * @version 0.1
 * @date 2024-12-30
 * 
 * @copyright Copyright (c) 2024
 * 
 * ###############################################
 * #                                             #
 * #                 Ctring.h                    #
 * # This is a C lib for wroking with strings    # 
 * #                                             #
 * #                                             #
 * #                                             #
 * #                                             #   
 * ###############################################
 */


#ifndef CTRING_H
#define CTRING_H

#ifdef __DEBUG__

#endif /* __DEBUG__ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
/**
 * 
 * @brief String struct a new data type for strings
 * @param str - pointer to the string
 * @param length - the length of the string
 */
typedef struct _STRING {
    char *str;
    int length;
} STRING;




/**
 * @brief Create a new STRING object and copy the content of the str to it
 * 
 * @param str
 * @return STRING 
 */
STRING toString( char *str);
/**
 * @brief print a STRING object
 * 
 * @param str 
 */
void printString(STRING str);
/**
 * @brief return the length of the STRING object
 * 
 * @param str 
 * @return unsigned int 
 */
unsigned int Length(STRING str);
/**
 * @brief return the index of the first appearance of the char c in the STRING object or -1 if not found
 * 
 * @param str 
 * @param c 
 * @return int 
 */
int findChar(STRING str, char c);
/**
 * @brief retunr the index of the first appearance of the string s in the STRING object or -1 if not found
 * 
 * @param str 
 * @param s 
 * @return unsigned int 
 */
int findString(STRING str, char *s);

void replaceChar(STRING str, char c, char r);

/**
 * @brief Split string by char c and save the result in the strings array
 * 
 * @param str 
 * @param c 
 */
void splitByChar(char *str, STRING *strings, char c);
/**
 * @brief Join the strings array to a single string
 * 
 * @param strings 
 * @param str 
 * @param stringsCount 
 */
void joinStrings(STRING *strings, char *str,unsigned int stringsCount);

char charAt(STRING str, unsigned int index);

bool cointainString(const char* src,const char *key);
bool startWith(const char* src,const char *key);
/**
 * @brief free the memory of the string
 * 
 * @param str 
 */
void cleanString(STRING str);
void cleanStrings(STRING *strings, unsigned int stringsCount);



#endif // CTRING_H

