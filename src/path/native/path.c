#include "list.h"
#include<stdlib.h>
#include<stdio.h>
#include <stdint.h>
#include <assert.h>
#include<string.h>
#include<sys/stat.h>
#include<errno.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#else
#include<unistd.h>
#include<dirent.h>
#endif

#include "stdlib/StrArray.h"
#include "stdlib/is_dir.h"
#include "stdlib/is_file.h"
#include "stdlib/exists.h"
#include "stdlib/iterdir.h"
#include "stdlib/makeDirectory.h"
#include "stdlib/readFileAsString.h"
#include "stdlib/writeStringToFile.h"

static int8_t is_file_internal(char* path) {
#ifdef _WIN32
  return !!(GetFileAttributes(path) & FILE_ATTRIBUTE_DIRECTORY);
#else
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
#endif
}

static int8_t exists_internal(char* path) {
#ifdef _WIN32
  WIN32_FIND_DATA FindFileData;
  HANDLE handle = FindFirstFile(path, &FindFileData) ;
  int found = handle != INVALID_HANDLE_VALUE;
  if (found) {
    //FindClose(&handle); this will crash
    FindClose(handle);
  }
  return found;
#else
  if (!is_file_internal(path)) {
    DIR* dir = opendir(path);
    int8_t retval = dir ? 1 : 0;
    if (retval) { closedir(dir); }
    return retval;
  } else {
    FILE* file = fopen(path, "r");
    int8_t retval = file ? 1 : 0; 
    if (retval) { fclose(file); }
    return retval;
  }
#endif
}

static long makeDirectory_internal(char* path) {
  if (!exists_internal(path)) {
     return mkdir(path, 0700);
    }
    return 1;
}

static ValeStr* readFileAsString_internal(char* filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror(filename);
    exit(1);
  }

  fseek(fp, 0L, SEEK_END);
  long lSize = ftell(fp);
  rewind(fp);

  /* allocate memory for entire content */
  char *buffer = malloc(lSize);
  if (!buffer) {
    fclose(fp);
    fputs("memory alloc fails", stderr);
    exit(1);
  }

  /* copy the file into the buffer */
  if (1 != fread(buffer, lSize, sizeof(char), fp)) {
    fclose(fp);
    free(buffer);
    fputs("Failed to read file: ", stderr);
    fputs(filename, stderr);
    fputs("\n", stderr);
    exit(1);
  }

  ValeStr* result = ValeStrNew(lSize);
  strncpy(result->chars, buffer, lSize);

  fclose(fp);
  free(buffer);

  return result;
}

static void writeStringToFile_internal(char* filename, char* contents, int contentsLen) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror(filename);
    exit(1);
  }

  if (contentsLen > 0) {
    if (1 != fwrite(contents, contentsLen, 1, fp)) {
      fclose(fp);
      fputs("Failed to write file", stderr);
      exit(1);
    }
  }

  fclose(fp);
}

static stdlib_StrArray* iterdir_internal(char* dirPath) {
  vale_queue* entries = vale_queue_empty();

#ifdef _WIN32
  WIN32_FIND_DATA fdFile; 
  HANDLE hFind = NULL; 

  wchar_t sPath[2048]; 

  //Specify a file mask. *.* = We want everything! 
  sprintf(sPath, L"%s\\*.*", dirPath); 

  if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Path not found: [%s]\n", dirPath);
    stdlib_StrArray* retval = malloc(sizeof(long));
    retval->length = 0;
    return retval;
  } 

  do {
    //Find first file will always return "."
    //    and ".." as the first two directories. 
    if (wcscmp(fdFile.cFileName, L".") != 0 &&
        wcscmp(fdFile.cFileName, L"..") != 0) { 
      vale_queue_push(entries, fdFile.cFileName); 
    }
  } while(FindNextFile(hFind, &fdFile)); //Find the next file.

  FindClose(hFind); //Always, Always, clean things up!

#else
  if (is_file_internal(dirPath)) {
      perror("is a file not a path");
      exit(0);
  }
  DIR* d;
  struct dirent *dir;
  d = opendir(dirPath);
  if (d) {
      while((dir = readdir(d)) != NULL){
          int64_t length = strlen(dir->d_name);
          ValeStr* path_name = ValeStrNew(length);
          strcpy(path_name->chars, dir->d_name);
          vale_queue_push(entries, path_name); 
      }
      closedir(d); 
  } else {
      printf("cannot open directory: %s\n", dirPath);
      stdlib_StrArray* retval = malloc(sizeof(long));
      retval->length = 0;
      return retval;
  }
#endif

  stdlib_StrArray* retval = (stdlib_StrArray*)vale_queue_to_array(entries); 
  vale_queue_destroy(entries);
  return retval;
}



extern int8_t stdlib_exists(ValeStr* path) {
  long result = exists_internal(path->chars);
  free(path);
  return result;
}

// Aborts on failure, beware!
extern ValeStr* stdlib_readFileAsString(ValeStr* filenameVStr) {
  ValeStr* result = readFileAsString_internal(filenameVStr->chars);
  free(filenameVStr);
  return result;
}

extern void stdlib_writeStringToFile(ValeStr* filenameVStr, ValeStr* contentsVStr) {
  writeStringToFile_internal(filenameVStr->chars, contentsVStr->chars, contentsVStr->length);
  free(filenameVStr);
  free(contentsVStr);
}

extern stdlib_StrArray* stdlib_iterdir(ValeStr* path) {
  stdlib_StrArray* result = iterdir_internal(path->chars);
  free(path);
  return result;
}

extern int8_t stdlib_is_file(ValeStr* path) {
  long result = exists_internal(path->chars) && is_file_internal(path->chars);
  free(path);
  return result;
}

extern int8_t stdlib_is_dir(ValeStr* path) {
  long result = !is_file_internal(path->chars);
  free(path);
  return result;
}

extern ValeInt stdlib_makeDirectory(ValeStr* path) {
  long result = makeDirectory_internal(path->chars);
  free(path);
  return result;
}

extern ValeStr* stdlib_GetPathSeparator() {
#ifdef _WIN32
  return ValeStrFrom("\\");
#else
  return ValeStrFrom("/");
#endif
}

extern ValeStr* stdlib_resolve(ValeStr* relative_path) {
#ifdef _WIN32
  char path[MAX_PATH];
  int length = GetFullPathNameA(relative_path->chars, MAX_PATH, path, NULL);
  if (length == 0) {
    fprintf(stderr, "resolve: GetFullPathNameA failed for input \"%s\", error %ld\n", relative_path->chars, GetLastError());
    exit(1);
  }
  ValeStr* result = ValeStrFrom(path);
  return result;
#else

  char* realpath_input = relative_path->chars;

  char relative_path_with_home_replaced[PATH_MAX];

  if (relative_path->chars[0] == '~') {
    char* home = getenv("HOME");
    if (home == NULL) {
      fprintf(stderr, "resolve: Couldn't get home directory for ~ replacement.\n");
      exit(1);
    }
    strcpy(relative_path_with_home_replaced, home);
    strcat(relative_path_with_home_replaced, relative_path->chars + 1);
    realpath_input = relative_path_with_home_replaced;
  }

  char* absolute_path = realpath(realpath_input, NULL);
  if (absolute_path == NULL) {
    fprintf(stderr, "resolve: Realpath failed for input \"%s\": ", realpath_input);
    perror("");
    exit(1);
  }
  ValeStr* result = ValeStrFrom(absolute_path);
  free(absolute_path);
  return result;
#endif
}
