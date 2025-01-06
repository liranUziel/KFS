#include "./Headers/Terminal.h"



const char *CMD_STR[] = {
    "exit",
    "help",
    "touch",
};

void printKFSLogo(){
    printf("\033[0;31m");
    printf("        ,--.                       \n");
    printf("    ,--/  /|     ,---,.  .--.--.    \n");
    printf(" ,---,': / '   ,'  .' | /  /    '.  \n");
    printf(" :   : '/ /  ,---.'   ||  :  /`. /  \n");
    printf(" |   '   ,   |   |   .';  |  |--`   \n");
    printf(" '   |  /    :   :  :  |  :  ;_     \n");
    printf(" |   ;  ;    :   |  |-, \\  \\    `.  \n");
    printf(" :   '   \\   |   :  ;/|  `----.   \\ \n");
    printf(" |   |    '  |   |   .'  __ \\  \\  | \n");
    printf(" '   : |.  \\ |   :  '   /  /`--'  / \n");
    printf(" |   | '_\\.' |   |  |  '--'.     /  \n");
    printf(" '   : |     |   :  \\    `--'---'   \n");
    printf(" ;   |,'     |   | ,'                \n");
    printf(" '---'       `----'                  \n");
    printf("\033[0;37m");
    printf("Welcome to KFS, ");
    printf("\033[1m");
    printf("Krazy File System\n");
    printf("\033[0m");

}
void printPizzaLogo(){
    //  print the LOGO PIPS in Orange to Yellow and the rest in white
    //  first letters of Pizza Partition System in bold
    /*  
        8888888b.  d8b 8888888b.   .d8888b.  
        888   Y88b Y8P 888   Y88b d88P  Y88b 
        888    888     888    888 Y88b.      
        888   d88P 888 888   d88P  "Y888b.   
        8888888P"  888 8888888P"      "Y88b. 
        888        888 888              "888 
        888        888 888        Y88b  d88P 
        888        888 888         "Y8888P"  
                Pizza Partition System                            
    */
    printf("\033[38;2;255;140;0m8888888b.  d8b 8888888b.   .d8888b.  \n");
    printf("\033[38;2;255;165;0m888   Y88b Y8P 888   Y88b d88P  Y88b \n");
    printf("\033[38;2;255;190;0m888    888     888    888 Y88b.      \n");
    printf("\033[38;2;255;215;0m888   d88P 888 888   d88P  \"Y888b.   \n");
    printf("\033[38;2;255;240;0m8888888P\"  888 8888888P\"      \"Y88b. \n");
    printf("\033[38;2;255;255;0m888        888 888              \"888 \n");
    printf("\033[38;2;255;255;0m888        888 888        Y88b  d88P \n");
    printf("\033[38;2;255;255;0m888        888 888         \"Y8888P\"  \n");
    printf("\033[0m\033[1mP\033[22mizza \033[1mP\033[22martition \033[1mS\033[22mystem\033[0m\n");
}

void printHelp(){
    printf("List of commands:\n");
    printf("exit: Exit the terminal\n");
    printf("help: Print this help message\n");
}

void dispatch(char *cmd){
    if(strcmp(cmd, CMD_STR[CMD_HELP]) == 0){
        printHelp();
    }
    else if(strcmp(cmd, CMD_STR[CMD_EXIT]) == 0){
        printf("Exiting...\n");
    }
    else if(strcmp(cmd, "touch") == 0){
        touch("test");
    } else if(strcmp(cmd,"edit") == 0){
        assert(false && "This command not implemented");
    }
    else{
        printf("Unknown command\n");
    }
}

