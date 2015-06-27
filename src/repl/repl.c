#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"

int main(int argc, char **argv) {
    while (1) {
        char line[CPU_WIDTH + 1];
        fgets(line, CPU_WIDTH + 1, stdin);
        if (feof(stdin) || ferror(stdin)) {
            break;
        }
        if (strlen(line) == 1) {
            continue;
        }
        line[strlen(line) - 1] = 0;

        parse_error error = {0};
        ins_t ins = {0};
        char *label;
        if (parse_line(line, &label, &ins, &error)) {
            printf("%d: %s\n", error.col, error.msg);
            continue;
        } else {
            if (label) printf("%s: ", label);
            if (ins.op) ins_print(&ins);
        }
    }
    return 0;
}
