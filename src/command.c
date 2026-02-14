#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syscall.h>
#include <unistd.h>

#include "include/command.h"
#include "include/print.h"
#include "include/tokenize.h"

struct builtin builtins[] = {
    {"ls", builtin_ls},
    {"cp", builtin_cp},
    {"grep", builtin_grep},
    {"exit", builtin_exit},
    {"clear", builtin_clear},
    {"echo", builtin_echo},
    {"pwd", builtin_pwd},
    {"mkdir", builtin_mkdir},
    {"touch", builtin_touch},
    {"cat", builtin_cat},
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

    char* cwd = getcwd(NULL, 0);
    if (!cwd) {
        print("pwd: error getting current directory\n");
        return 0;
    }
    print("%s\n", cwd);

    free(cwd);
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
    if (argc <= 1) {
        print("mkdir: missing operand\n");
        print("usage: mkdir <directory-name>\n");
        return 0;
    }

    mode_t target_mode = 0755; // drwxr-xr-x

    int mkdir_ret = mkdir(argv[1], target_mode);
    if (mkdir_ret != 0) {
        print("mkdir error: return value (%d)\n", mkdir_ret);
        return 0;
    }

    print("successfuly created directory: %s with permissions rwxr-xr-x (755)\n", argv[1]);
    return 0;
}

int create_file(const char* file_path) {
    mode_t init_mode   = 0777; // -rwxrwxrwx
    mode_t target_mode = 0644; // -rw-r--r--

    int open_ret = open(file_path, O_CREAT | O_WRONLY, init_mode);
    if (open_ret == -1) {
        print("error creating file: %s\n", file_path);
        return -1;
    }
    close(open_ret);

    int chmod_ret = chmod(file_path, target_mode);
    if (chmod_ret != 0) {
        print("chmod error: return value (%d)\n", chmod_ret);
        return -1;
    }

    print("successfuly created file: %s\n", file_path);
    return 0;
}

int builtin_touch(int argc, char* argv[]) {
    if (argc <= 1) {
        print("touch: missing operand\n");
        print("usage: touch <file-name>\n");
        return 0;
    }

    create_file(argv[1]);

    return 0;
}

int file_exists(const char* file_path) {
    struct stat buf;
    return stat(file_path, &buf) == 0;
}

int builtin_cp(int argc, char* argv[]) {
    if (argc == 1) {
        print("cp: missing source file\n");
        print("usage: cp <source-file> <destination-file>\n");
        return 0;
    }
    if (argc == 2) {
        print("cp: missing file destination\n");
        print("usage: cp <source-file> <destination-file>\n");
        return 0;
    }

    const char* src_path = argv[1];
    const char* dst_path = argv[2];

    if (!file_exists(src_path)) {
        print("cp: cannot copy %s, no such file\n", src_path);
        return 0;
    }
    if (file_exists(dst_path)) {
        print("cp: file: %s already exists\n", dst_path);
        return 0;
    }

    if (create_file(dst_path) != 0)
        return 0;

    print("copying file ...\n");

    const int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        print("error open source file: %s\n", src_path);
        return 0;
    }

    const int dst_fd = open(dst_path, O_WRONLY);
    if (dst_fd == -1) {
        print("error open destination file: %s\n", dst_path);
        close(src_fd);
        return 0;
    }

    const size_t buf_size = 256;

    char buf[buf_size];
    ssize_t bytes_read;

    while (1) {
        bytes_read = read(src_fd, buf, buf_size);
        if (bytes_read > 0) {
            write(dst_fd, buf, bytes_read);
        }
        else if (bytes_read == 0) {
            break;
        }
        else {
            print("cp: error while reading %s\n", src_path);
            break;
        }
    }

    close(src_fd);
    close(dst_fd);

    return 0;
}

int builtin_cat(int argc, char* argv[]) {
    if (argc == 1) {
        print("cat: missing file operand\n");
        print("usage: cat <filename>\n");
        return 0;
    }
    if (argc > 2) {
        print("cat: too many arguments\n");
        print("usage: cat <filename>\n");
        return 0;
    }

    const char* file_path = argv[1];
    struct stat stat_buf;
    int stat_ret = stat(file_path, &stat_buf);

    if (stat_ret != 0) {
        print("cat: no such file: %s\n", file_path);
        return 0;
    }

    if (!S_ISREG(stat_buf.st_mode)) {
        print("error: %s is not a file\n", file_path);
        return 0;
    }

    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        print("cat: cannot open file: %s\n", file_path);
        return 0;
    }

    int border_len = 32; // random number that looks good
    print("─────┬");
    for (int i = 0; i < border_len; i++) {
        print("────");
    }
    print("\n");

    print("     │ File: %s\n", file_path);

    print("─────┼");
    for (int i = 0; i < border_len; i++) {
        print("────");
    }
    print("\n");

    const size_t buf_size = 256;
    char buf[buf_size];
    ssize_t bytes_read;

    int line = 1;
    print("%s   %d%s │", START_CYAN, line, END_COLOR);

    while ((bytes_read = read(fd, buf, buf_size)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            char c = buf[i];
            if (c == '\n') {
                line++;
                print("\n");
                if (line < 10)
                    print("%s   %d%s │", START_CYAN, line, END_COLOR);
                else if (line < 100)
                    print("%s  %d%s │", START_CYAN, line, END_COLOR);
                else if (line < 1000)
                    print("%s %d%s │", START_CYAN, line, END_COLOR);
                else
                    print("%s%d%s │", START_CYAN, line, END_COLOR);
            }
            else {
                print("%c", c);
            }
        }
    }
    print("\n");
    print("─────┴");
    for (int i = 0; i < border_len; i++) {
        print("────");
    }
    print("\n");

    if (bytes_read < 0) {
        print("cat: error reading file: %s\n", file_path);
    }

    close(fd);
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
    (void)argc;
    (void)argv;
    // TODO: write this
}
