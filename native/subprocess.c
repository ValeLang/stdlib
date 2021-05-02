#include "StrChain.h"
#include "subprocess.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/subprocess.h
*/

ValeStr* get_env_var(ValeStr* var_name) {
    const char* env_var = getenv((const char*)&var_name->chars);
    unsigned long length = strlen(env_var);
    ValeStr* out = malloc(sizeof(ValeStr) + length + 1);
    out->length = length; 
    strcpy(out->chars, env_var);
    printf("%s\n", out->chars); 
    return out;
}

char** vale_to_char_arr(StrChain* chains) {
    char** args = malloc(chains->length*sizeof(char*));
    for(unsigned long i = 0; i < chains->length; i++) {
        args[i] = &chains->elements[i]->chars[0];
    }
    return args; 
}

typedef struct Command {
    unsigned long handle;
} Command;

Command* launch_command(StrChain* chain) {
    Command* out = (Command*)malloc(sizeof(Command));
    printf("%s\n", chain->elements[1]->chars);
    printf("in run command\n");
    const char** args = (const char**)vale_to_char_arr(chain);
    printf("%s\n", args[1]); 
    struct subprocess_s* subproc = malloc(sizeof(struct subprocess_s));
    if(subprocess_create(args, subprocess_option_inherit_environment, subproc) != 0){
        perror("command creation failed");
    } 
    printf("addr: %x\n", subproc);
    out->handle = (unsigned long)subproc;
    //free(args);
    //ValeReleaseMessage(chain); 
    return out;
}

ValeStr* read_child_stdout(Command* cmd, long bytes) {
    ValeStr* out = malloc(sizeof(ValeStr) + bytes + 1);
    printf("addr: %x\n", cmd->handle); 
    char temp_buf[20]; 
    FILE* stdout = subprocess_stdout((struct subprocess_s*)cmd->handle); 
    printf("fgets return: %d\n", fgets(out->chars, bytes, stdout)); 
    return out;
}

ValeStr* read_child_stderr(Command* cmd, long bytes) {
    ValeStr* out = malloc(sizeof(ValeStr) + bytes + 1);
    FILE* stderr = subprocess_stderr((struct subprocess_s*)cmd->handle); 
    fgets(out->chars, bytes, stderr); 
    return out;
}

void write_child_stdin(Command* cmd, ValeStr* contents) {
    
}
