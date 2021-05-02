#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include "StrChain.h"
long is_file(ValeStr* path) {
    struct stat path_stat;
    stat(path->chars, &path_stat);
    return S_ISREG(path_stat.st_mode);    
}

long is_dir(ValeStr* path) {
    return !is_file(path);
}

long exists(ValeStr* path) {
    if(is_dir(path)) {
        DIR* dir = opendir(path->chars);
        long retval = dir ? 1 : 0;
        closedir(dir);
        return retval;
    }else{
        FILE* file = fopen(path->chars, "r");
        long retval = file ? 1 : 0; 
        fclose(file);
        return retval;
    }
}
