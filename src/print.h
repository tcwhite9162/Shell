#ifndef PRINT_H
#define PRINT_H

#include <unistd.h>

size_t str_len(const char* s);
void print(const char* s);
void print_int(long n);
void print_float(double n, int dp);
void print_char(char c);
void format_print(const char* fmt, ...);


#endif
