#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <unistd.h>

#define MAX_TOKENS 64

int str_cmp(const char* s1, const char* s2);

int tokenize(char* input, char* argv[]);

#endif
