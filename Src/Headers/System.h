#ifndef SYSTEM_H 
#define SYSTEM_H

#include <stdio.h>

#define Gb 1073741824
#define Mb 1048576
#define Kb 1024

void printSize(unsigned int size){
    if(size > Gb){
        fprintf(stdout,"%d Gb", size/Gb);
    }else if(size > Mb){
        fprintf(stdout,"%d Mb", size/Mb);
    }else if(size > Kb){
        fprintf(stdout,"%d Kb", size/Kb);
    }else{
        fprintf(stdout,"%d bytes", size);
    }
}

#endif // SYSTEM_H