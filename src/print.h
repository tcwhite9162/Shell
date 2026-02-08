#ifndef PRINT_H
#define PRINT_H

#include <unistd.h>

#define START_BLACK   "\033[30m"
#define START_RED     "\033[31m"
#define START_GREEN   "\033[32m"
#define START_YELLOW  "\033[33m"
#define START_BLUE    "\033[34m"
#define START_MAGENTA "\033[35m"
#define START_CYAN    "\033[36m"
#define START_WHITE   "\033[37m"

#define END_COLOR "\033[0m"

size_t str_len(const char* s);
void print_int(long n);
void print_float(double n, int dp);
void print_char(char c);
void print(const char* fmt, ...);

#endif
