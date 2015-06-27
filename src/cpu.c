#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"

void ins_print(ins_t *ins) {
    printf("ins: %s (%s)", op_name(ins->op), reg_name(ins->a));
    printf(", (%s)\n", reg_name(ins->b));
}

int main(int argc, char **argv) {
    while (1) {
        char line[32];
        fgets(line, 32, stdin);
        if (strlen(line) == 0) {
            break;
        }
        if (strlen(line) == 1) {
            continue;
        }
        line[strlen(line) - 1] = 0;
        error_t error = {0};
        ins_t *ins = parse(line, &error);
        if (ins) {
            ins_print(ins);
        }
        if (error.msg) {
            printf("error: %s\n", error.msg);
            continue;
        }
    }
    return 0;
}
