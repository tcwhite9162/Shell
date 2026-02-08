#include "tokenize.h"

int str_cmp(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL)
        return 0;

    while (*s1 && *s2) {
        if (*s1 != *s2)
            return 0;
        s1++;
        s2++;
    }

    return *s1 == *s2;
}

int tokenize(char* input, char* argv[]) {
    int argc = 0;

    while (*input != '\0') {

        while (*input == ' ' || *input == '\t' || *input == '\n')
            input++;

        if (*input == '\0')
            break;

        argv[argc++] = input;

        if (argc >= MAX_TOKENS - 1)
            break;

        while (*input != '\0' && *input != ' ' && *input != '\t' && *input != '\n')
            input++;

        if (*input != '\0') {
            *input = '\0';
            input++;
        }
    }

    argv[argc] = NULL;
    return argc;
}
