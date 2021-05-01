#include "StrChain.h"
#include "subprocess.h"
/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/subprocess.h
*/

char** vale_to_char_arr(StrChain* chains) {
    char** args = malloc(chains->length*sizeof(char*));
    for(unsigned long i = 0; i < chains->length; i++) {
        args[i] = &chains->elements[i]->chars[0];
    }
    return args; 
}

typedef struct Command {
    unsigned long handle;
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
    const char** args = (const char**)vale_to_char_arr(chain);
    struct subprocess_s* subproc = malloc(sizeof(struct subprocess_s));
    if(subprocess_create(args, 0, subproc)){
        perror("command creation failed");
    } 
    out->handle = subproc;
    free(args);
    ValeReleaseMessage(chain); 
    return out;
}
