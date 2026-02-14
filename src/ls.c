#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syscall.h>
#include <unistd.h>

#include "include/command.h"
#include "include/print.h"

int builtin_ls(int argc, char* argv[]) {
    const char* path;
    if (argc > 1)
        path = argv[1];
    else
        path = ".";

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
        print(d->d_name);
        print("\n");

        idx += d->d_reclen;
    }

    close(path_fd);

    return 0;
}

