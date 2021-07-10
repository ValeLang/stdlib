#include "subprocess.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ValeBuiltins.h"
#include "stdlib/StrArray.h"

long read_into_buffer(char* buffer, ValeInt bytes, FILE* stream){
    long i = 0;
    for(i = 0; i < bytes; i++){
        // feof comes after fgetc but before its result is used, taken from example at
        // https://www.tutorialspoint.com/c_standard_library/c_function_fgetc.htm
        int c = fgetc(stream);
        if(feof(stream)) {
            break;
        }
        buffer[i] = c;
    }
    return i;
}

/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/subprocess.h
*/

ValeStr* stdlib_get_env_var(ValeStr* var_name) {
    const char* env_var = getenv((const char*)&var_name->chars);
    unsigned long length = strlen(env_var);
    ValeStr* out = malloc(sizeof(ValeStr) + length + 1);
    out->length = length; 
    strcpy(out->chars, env_var);
    ValeReleaseMessage(var_name);
    return out;
}

char** stdlib_vale_to_char_arr(stdlib_StrArray* chains) {
    char** args = malloc(chains->length*sizeof(char*)+sizeof(char*));
    for(unsigned long i = 0; i < chains->length; i++) {
        args[i] = &chains->elements[i]->chars[0];
    }
    args[chains->length] = 0;
    return args; 
}

int64_t stdlib_launch_command(stdlib_StrArray* chain) {
    int64_t out = 0;
    const char** args = (const char**)stdlib_vale_to_char_arr(chain);
    struct subprocess_s* subproc = malloc(sizeof(struct subprocess_s));
    if(subprocess_create(args, subprocess_option_inherit_environment, subproc) != 0){
        perror("command creation failed");
    }
    out = (unsigned long)subproc;
    free(args);
    for(long i = 0; i < chain->length; i++){
        //ValeReleaseMessage(&chain->elements[i]);
    }
    ValeReleaseMessage(chain);
    return out;
}

ValeStr* stdlib_read_child_stdout(int64_t cmd, long bytes) {
    ValeStr* out = ValeStrNew(bytes+1); 
    FILE* stdout_handle = subprocess_stdout((struct subprocess_s*)cmd); 
    long read_len = read_into_buffer(out->chars, bytes, stdout_handle);
    out->chars[bytes] = '\0'; 
    out->length = read_len;
    fclose(stdout_handle);
    return out;
}

ValeStr* stdlib_read_child_stderr(int64_t cmd, long bytes) {
    ValeStr* out = ValeStrNew(bytes+1);
    FILE* stderr_handle = subprocess_stderr((struct subprocess_s*)cmd); 
    long read_len = read_into_buffer(out->chars, bytes, stderr_handle);
    out->chars[bytes] = '\0'; 
    out->length = read_len;
    fclose(stderr_handle);
    return out;
}

void stdlib_write_child_stdin(int64_t cmd, ValeStr* contents) {
    FILE* stdin_handle = subprocess_stdin((struct subprocess_s*)cmd); 
    for (int i = 0; i < contents->length; i++) {
        fputc(contents->chars[i], stdin_handle);
    }
    fclose(stdin_handle);
    ValeReleaseMessage(contents);
}

long stdlib_join_subprocess(long handle){
    int result;
    subprocess_join((struct subprocess_s*)handle, &result);
    return result;  
}
