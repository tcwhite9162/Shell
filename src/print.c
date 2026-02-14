#include <limits.h>
#include <stdarg.h>
#include <unistd.h>

#include "include/print.h"

size_t str_len(const char* s) {
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

void print_int(long n) {
    if (n == 0) {
        write(STDOUT_FILENO, "0", 1);
        return;
    }

    if (n == LONG_MIN) { // because -LONG_MIN is undefined
        print("-9223372036854775808");
        return;
    }
    if (n < 0) {
        write(STDOUT_FILENO, "-", 1);
        n = -n;
    }

    char buf[32];
    int idx = 0;
    while (n > 0) {
        buf[idx++] = '0' + (n % 10);
        n /= 10;
    }

    while (idx > 0) {
        idx--;
        write(STDOUT_FILENO, &buf[idx], 1);
    }
}

void print_float(double n, int dp) {
    if (dp < 0)
        dp = 6;

    if (n < 0) {
        write(STDOUT_FILENO, "-", 1);
        n = -n;
    }

    long whole     = (long)n;
    double decimal = n - (double)whole;

    print_int(whole);
    if (dp > 0)
        write(STDOUT_FILENO, ".", 1);

    // 6 dp
    for (int i = 0; i < dp; i++) {
        decimal *= 10;
        int digit = (int)decimal;
        char c    = '0' + digit;
        write(STDOUT_FILENO, &c, 1);
        decimal -= digit;
    }
}

void print_char(char c) {
    write(STDOUT_FILENO, &c, 1);
}

void print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {

            i++;
            char spec = fmt[i];

            if (spec == '\0') {
                print_char('%');
                return;
            }

            if (spec == 'd') {
                long n = va_arg(args, long);
                print_int(n);
            }
            else if (spec == 'f') {
                double f = va_arg(args, double);
                print_float(f, -1);
            }
            else if (spec == 'c') {
                char c = (char)va_arg(args, int);
                print_char(c);
            }
            else if (spec == 's') {
                char* s = va_arg(args, char*);
                write(STDOUT_FILENO, s, str_len(s));
            }
            else if (spec == '%') {
                print_char('%');
            }
            else {
                print_char(spec);
            }
        }
        else {
            write(STDOUT_FILENO, &fmt[i], 1);
        }
    }
}
