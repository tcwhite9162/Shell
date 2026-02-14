#include "include/tokenize.h"

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

        char* start = input;

        if (*input == '"' || *input == '\'') {
            char quote = *input++;
            start      = input;

            while (*input != '\0' && *input != quote)
                input++;

            if (*input == quote) {
                *input = '\0';
                input++;
            }

            argv[argc++] = start;
        }
        else {
            start = input;

            while (*input != '\0' && *input != ' ' && *input != '\t' && *input != '\n')
                input++;

            if (*input != '\0') {
                *input = '\0';
                input++;
            }

            argv[argc++] = start;
        }
    }

    argv[argc] = NULL;
    return argc;
}
