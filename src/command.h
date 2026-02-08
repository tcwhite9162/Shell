#ifndef COMMAND_H
#define COMMAND_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

void run_external(int argc, char* argv[]);
int run_builtin(int argc, char* argv[]);

int builtin_ls(int argc, char* argv[]);
int builtin_cp(int argc, char* argv[]);
int builtin_grep(int argc, char* argv[]);
int builtin_exit(int argc, char* argv[]);
int builtin_clear(int argc, char* argv[]);
int builtin_echo(int argc, char* argv[]);

typedef int (*builtin_fn)(int argc, char* argv[]);

struct builtin {
    const char* name;
    builtin_fn func;
};

struct linux_dirent {
    ino_t          d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[256];
};

#endif
