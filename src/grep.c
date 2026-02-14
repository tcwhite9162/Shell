#include "include/command.h"
#include "include/print.h"
#include "include/tokenize.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUF_SIZE 256
#define MAX_LINE_LEN 512

struct grep_flags {
    int ignore_case;
    int print_lines;
    int help;
    int recurse;
    int pattern_idx;
};

int get_dirents(const int path_fd, const char buf[], const int buf_size) {
    int n_read = syscall(SYS_getdents, path_fd, buf, buf_size);
    return n_read;
}

char to_lower(char c) {
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');
    return c;
}

int substrings_match_caseless(const char* txt, const char* pattern) {
    for (int i = 0; txt[i] != '\0'; i++) {
        int j = 0;

        while (pattern[j] != '\0' && txt[i + j] != '\0' && to_lower(txt[i + j]) == to_lower(pattern[j])) {
            j++;
        }

        if (pattern[j] == '\0') {
            return 1;
        }
    }

    return 0;
}

int substrings_match(const char* txt, const char* pattern) {
    for (int i = 0; txt[i] != '\0'; i++) {
        int j = 0;

        while (pattern[j] != '\0' && txt[i + j] != '\0' && txt[i + j] == pattern[j]) {
            j++;
        }

        if (pattern[j] == '\0') {
            return 1;
        }
    }

    return 0;
}

int match_line(const char* line, const char* pattern, int ignore_case) {
    return ignore_case ? substrings_match_caseless(line, pattern) : substrings_match(line, pattern);
}

struct grep_flags parse_grep_flags(int argc, char* argv[]) {
    struct grep_flags flags = {0, 0, 0, 0, 1};

    while (flags.pattern_idx < argc && argv[flags.pattern_idx][0] == '-') {
        const char* flag = argv[flags.pattern_idx] + 1;

        for (int i = 0; flag[i] != '\0'; i++) {
            if (flag[i] == 'i') {
                flags.ignore_case = 1;
            }
            else if (flag[i] == 'n') {
                flags.print_lines = 1;
            }
            else if (flag[i] == 'h') {
                flags.help = 1;
            }
            else if (flag[i] == 'r') {
                flags.recurse = 1;
            }
            else {
                print("grep: unknown flag -%s\n", flag);
                print("enter 'grep -h' for information\n");
                return flags;
            }
        }
        flags.pattern_idx++;
    }

    return flags;
}

void print_help() {
    print("usage: grep ... <options> ... <pattern> <file>\n");
    print("if <pattern> contains whitespace or newlines, it must be surrounded by quotes\n");
    print("Options:\n");
    print("-i: ignore case\n");
    print("-n: show line numbers on matched lines\n");
    print("-r: search files recursively\n");
}

void strip_quotes(char* pattern) {
    size_t len = str_len(pattern);

    if (len >= 2) {
        char first = pattern[0];
        char last  = pattern[len - 1];

        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            memmove(pattern, pattern + 1, len - 2);
            pattern[len - 2] = '\0';
        }
    }
}

void print_match(const char* path, const char* line, const int line_number, const struct grep_flags* flags) {
    if (flags->recurse) {
        print("%s%s%s: ", START_RED, path, END_COLOR);
    }
    if (flags->print_lines) {
        print("%s%d%s: ", START_CYAN, line_number, END_COLOR);
    }
    print("%s", line);
    print("\n");
}

void process_file(const int fd, const char* path, const char* pattern, struct grep_flags* flags) {
    char buf[BUF_SIZE];
    char line[MAX_LINE_LEN];
    int line_len    = 0;
    int line_number = 1;

    ssize_t n;
    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            char c = buf[i];
            if (c == '\n') {
                line[line_len] = '\0';

                int is_match = match_line(line, pattern, flags->ignore_case);
                if (is_match)
                    print_match(path, line, line_number, flags);

                line_number++;
                line_len = 0;
            }
            else {
                if (line_len < (int)sizeof(line) - 1) {
                    line[line_len] = c;
                    line_len++;
                }
            }
        }
    }

    if (line_len > 0) {
        line[line_len] = '\0';

        int is_match   = match_line(line, pattern, flags->ignore_case);
        if (is_match)
            print_match(path, line, line_number, flags);
    }
}

char* concat_path(const char* base, const char* file) {
    const size_t base_len = str_len(base);
    const size_t file_len = str_len(file);

    int slash_needed = (base[base_len - 1] != '/');

    const size_t total_len = base_len + slash_needed + file_len + 1;

    char* out = (char*)malloc(total_len);

    char* p = out;
    memcpy(p, base, base_len);
    p += base_len;

    if (slash_needed) {
        *p = '/';
        p++;
    }

    memcpy(p, file, file_len);
    p += file_len;

    *p = '\0';

    return out;
}

int grep_recursive(const char* path, const char* pattern, struct grep_flags* flags) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        print("grep: path %s does not exist or error occured", path);
        return -1;
    }
    else if (S_ISDIR(statbuf.st_mode)) {

        const int path_fd = open(path, O_RDONLY | O_DIRECTORY);
        if (path_fd < 0) {
            print("ls: could not open path '%s'\n", path);
            return 0;
        }

        const size_t buf_size = 4096;
        char buf[buf_size];
        const int n_read = get_dirents(path_fd, buf, buf_size);

        if (n_read == -1) {
            print("ls: SYS_getdents failed\n");
            return 0;
        }

        int idx = 0;
        while (idx < n_read) {
            struct linux_dirent* d = (struct linux_dirent*)(buf + idx);

            if (d->d_reclen == 0)
                break;

            if (str_cmp(d->d_name, ".") != 0 || str_cmp(d->d_name, "..") != 0) {
                idx += d->d_reclen;
                continue;
            }

            const char* new_path = concat_path(path, d->d_name);
            grep_recursive(new_path, pattern, flags);
            free((void*)new_path);

            idx += d->d_reclen;
        }
        close(path_fd);

        return 0;
    }
    else if (S_ISREG(statbuf.st_mode)) {
        int fd = open(path, O_RDONLY);
        if (fd < 0) {
            print("grep: error opening file %s\n", path);
            return 0;
        }
        process_file(fd, path, pattern, flags);

        close(fd);
        return 0;
    }
    else {
        print("grep: path %s is not a file or directory\n", path);
        return 0;
    }
}

int builtin_grep(int argc, char* argv[]) {
    if (argc == 1) {
        print("usage: grep <pattern> <file>\n");
        print("enter 'grep -h' for information\n");
        return 0;
    }
    if (argc == 2) {
        print("usage: grep <pattern> <file>\n");
        print("enter 'grep -h' for information\n");
        return 0;
    }

    struct grep_flags flags = parse_grep_flags(argc, argv);

    if (flags.help) {
        print_help();
        return 0;
    }

    if (flags.pattern_idx + 2 > argc) {
        print("usage: grep <pattern> <file>\n");
        print("enter 'grep -h' for information\n");
        return 0;
    }

    char* pattern = argv[flags.pattern_idx];
    strip_quotes(pattern);
    const char* file = argv[flags.pattern_idx + 1];

    if (flags.recurse) {
        return grep_recursive(file, pattern, &flags);
    }

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        print("grep: error opening file %s\n", file);
        return 0;
    }

    process_file(fd, file, pattern, &flags);

    close(fd);
    return 0;
}
