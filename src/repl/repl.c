#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"

int main(int argc, char **argv) {
    while (1) {
        char line[32];
        fgets(line, 32, stdin);
        if (feof(stdin) || ferror(stdin)) {
            break;
        }
        if (strlen(line) == 1) {
            continue;
        }
        line[strlen(line) - 1] = 0;

        parse_error error = {0};
        ins_t *ins;
        char *label;
        parse_line(line, &label, &ins, &error);
        if (error.msg) {
            printf("%d: %s\n", error.col, error.msg);
            continue;
        } else {
            if (label) printf("%s: ", label);
            if (ins) ins_print(ins);
        }
    }
    return 0;
}
