#include "./Headers/Ctring.h"

STRING toString( char *_str){
    STRING str;
    str.length = strlen(_str);
    str.str = (char *)malloc(str.length);
    strcpy(str.str, _str);
    return str;
}

void printString(STRING str){
    printf("%s\n", str.str);
}

unsigned int Length(STRING str){
    return str.length;
}

int findChar(STRING str, char c){
    for(int i = 0; i < str.length; i++){
        if(str.str[i] == c){
            return i;
        }
    }
    return -1;
}

int findString(STRING str, char *s){
    int len = str.length;
    int slen = strlen(s);
    for(size_t i = 0; i < len; i++){
        if(str.str[i] == s[0]){
            int index = i;
            for(size_t j = 1;j < slen; j++){
                if(str.str[j] != s[j]){
                    break;
                }
                if(j == slen - 1){
                    return index;
                }
            }
        }
    }
}

void replaceChar(STRING str, char c, char r){
    for(size_t i = 0; i < str.length; i++){
        if(str.str[i] == c){
            str.str[i] = r;
        }
    }
}

void splitByChar(char *str, STRING *strings, char c){
    unsigned int len = strlen(str);
    unsigned int count = 1;
    for(size_t i = 0; i < len; i++){
        if(str[i] == c){
            count++;
        }
    }
    strings = (STRING *)malloc(count);
    unsigned int start = 0;
    unsigned int end = 0;
    unsigned int index = 0;
    for(size_t i = 0; i < len; i++){
        if(str[i] == c){
            end = i;
            strings[index].str = (char *)malloc(end - start + 1);
             for (size_t j = start; j < end; j++) {
                strings[index].str[j - start] = str[j];
            }
            strings[index].str[end - start] = '\0'; // Correctly place the null terminator
            strings[index].length = end - start;
            index++;
            start = end + 1;
        }
    }
    
} 

bool cointainString(const char* src,const char *key){
    bool found;
    int len = strlen(key);
    if(strlen(src) < len){
        return false;
    }
    for(size_t i = 0; i < strlen(src) - len; i++){
        if(src[i] == key[0]){
            found = true;
            for(size_t j = 1; j < len && found; j++){
                if(src[i + j] != key[j]){
                    found = false;
                }
            }
            if(found){
                return true;
            }
        }
    }
    return false;
}
bool startWith(const char* src, const char *key){
    int len = strlen(key);
    if(strlen(src) < len){
        return false;
    }
    for(size_t i = 0; i < len; i++){
        if(src[i] != key[i]){
            return false;
        }
    }
    return true;
}


void cleanString(STRING str){
    free(str.str);
}
void cleanStrings(STRING *strings, unsigned int stringsCount){
    for(size_t i = 0; i < stringsCount; i++){
        free(strings[i].str);
    }
    free(strings);
}