#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"

char *code = (
    "1: MOV 1, ACC\n"
    "ADD 1\n"
    "SUB 1\n"
    "JMP 1\n"
);

int main(int argc, char **argv) {
    parse_error error;
    cpu_state *cpu = cpu_new(0, code, &error);
    if (! cpu) {
        printf("%d:%d: %s\n", error.line, error.col, error.msg);
    }
    return 0;
}
