#include "StrChain.h"
#include "subprocess.h"
/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/subprocess.h
*/

char** vale_to_char_arr(StrChain* chains) {
    
}

typedef struct Command {
    unsigned long stdout;
    unsigned long stdin;
    unsigned long stderr;
} Command;

Command* launch_command(StrChain* chain) {
    Command* out = (Command*)malloc(sizeof(Command));
    out->stdout = 0;
    out->stderr = 0;
    out->stdin = 0;
    printf("%s\n", chain->elements[0]->chars);
    printf("in run command\n");
     
    return out;
}
