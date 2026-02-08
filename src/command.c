#include <fcntl.h>
#include <sys/stat.h>
#include <syscall.h>
#include <unistd.h>

#include "command.h"
#include "print.h"
#include "tokenize.h"

struct builtin builtins[] = {
    {"ls", builtin_ls},
    {"cp", builtin_cp},
    {"grep", builtin_grep},
    {"exit", builtin_exit},
    {"clear", builtin_clear},
    {"echo", builtin_echo},
    {"pwd", builtin_pwd},
    {"mkdir", builtin_mkdir},
    {NULL, NULL},
};

int builtin_clear(int argc, char* argv[]) {
    (void)argc; // silence unused warnings
    (void)argv;
    print("\033[H\033[2J");
    return 0;
}

int builtin_pwd(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    const char* cwd = getcwd(NULL, 0);
    format_print("%s\n", cwd);

    return 0;
}

int builtin_echo(int argc, char* argv[]) {
    if (argc <= 1)
        return 0;

    for (int idx = 1; idx < argc; idx++) {
        print(argv[idx]);
        print(" ");
    }
    print("\n");

    return 0;
}

int builtin_mkdir(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if (argc <= 1) {
        print("mkdir: missing operand\n");
        print("usage: mkdir <directory-name>\n");
        return 0;
    }

    mode_t init_mode   = 0777;
    mode_t target_mode = 0755;

    int mkdir_ret = mkdir(argv[1], init_mode);
    if (mkdir_ret != 0) {
        format_print("mkdir error: return value (%d)", mkdir_ret);
        return 0;
    }

    int chmod_ret = chmod(argv[1], target_mode);
    if (chmod_ret != 0) {
        format_print("chmod error: return value (%d)", chmod_ret);
        return 0;
    }

    format_print("successfuly created directory %s with permissions rwxr-xr-x (755)\n", argv[1]);
    return 0;
}

int builtin_ls(int argc, char* argv[]) {
    const char* path;
    if (argc > 1)
        path = argv[1];
    else
        path = ".";

    int path_fd = open(path, O_RDONLY | O_DIRECTORY);
    if (path_fd < 0) {
        format_print("ls: could not open path '%s'\n", path);
        return 0;
    }

    char buf[1024];
    int n_read = syscall(SYS_getdents, path_fd, buf, sizeof(buf));

    if (n_read == -1) {
        print("ls: SYS_getdents failed\n");
        return 0;
    }

    int idx = 0;
    while (idx < n_read) {
        struct linux_dirent* d = (struct linux_dirent*)(buf + idx);
        print(d->d_name);
        print("\n");

        idx += d->d_reclen;
    }

    close(path_fd);

    return 0;
}

int builtin_cp(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    print("running: cp\n");
    return 0;
}

int builtin_grep(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    print("running: grep\n");
    return 0;
}

int builtin_exit(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    _exit(0);
}

int run_builtin(int argc, char* argv[]) {
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (str_cmp(argv[0], builtins[i].name))
            return builtins[i].func(argc, argv);
    }

    return -1;
}

void run_external(int argc, char* argv[]) {
    (void)argc; // silence unused warnings
    (void)argv;
}
