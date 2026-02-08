#include <unistd.h>

#include "print.h"
#include "tokenize.h"
#include "command.h"

int main() {
    char buf[256];
    char* argv[MAX_TOKENS];

    while (1) {
        print(" > ");

        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0)
            break;

        buf[n] = '\0';
        int argc = tokenize(buf, argv);

        if (argc == 0)
            continue;

        if (run_builtin(argc, argv) == -1) {
        }

    }
}
