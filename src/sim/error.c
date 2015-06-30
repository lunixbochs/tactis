#include <stdio.h>
#include "parse.h"
#include "./error.h"

void print_error(char *code, parse_error *error) {
    printf("%d:%d: %s\n", error->line, error->col, error->msg);
    print_line(code, error->line);
    for (int i = 0; i < error->col; i++) {
        fputc(' ', stdout);
    }
    for (int i = 0; i < error->len; i++) {
        fputc('^', stdout);
    }
    printf("\n");
}

void print_line(char *code, int line) {
    int i = 0;
    while (*code != '\0' && i <= line) {
        if (i == line)
            fputc(*code, stdout);
        if (*code == '\n')
            i++;
        code++;
    }
}
