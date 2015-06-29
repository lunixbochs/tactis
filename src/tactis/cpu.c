#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"
#include "node.h"

io_status cpu_read(node_t *node, io_dir dir, int *data) {
    cpu_state *cpu = (cpu_state *)node;
    printf("cpu_read: %d", dir);
    return IO_WAIT;
}

io_status cpu_write(node_t *node, io_dir dir, int data) {
    cpu_state *cpu = (cpu_state *)node;
    printf("cpu_write: %d, %d", dir, data);
    return IO_WAIT;
}

cpu_state *cpu_new(int16_t id, char *code, parse_error *error) {
    char *lines = strdup(code);
    char *pos = lines;
    cpu_state *cpu = calloc(1, sizeof(cpu_state));
    cpu->node.read = cpu_read;
    cpu->node.write = cpu_write;
    char *line;
    int i;
    while ((line = strsep(&pos, "\n")) != NULL && i < CPU_HEIGHT) {
        if (parse_line(line, &cpu->labels[i], &cpu->ops[i], error)) {
            error->line = i;
            free(lines);
            free(cpu);
            return NULL;
        }
        i++;
    }
    free(lines);
    return cpu;
}

void cpu_free(cpu_state *cpu) {
    for (int i = 0; i < CPU_HEIGHT; i++) {
        free(cpu->labels[i]);
    }
}

void ins_print(ins_t *ins) {
    printf("%s", op_name(ins->op));
    if (ins->a) printf(" %s", reg_name(ins->a));
    if (ins->b) printf(", %s", reg_name(ins->b));
    if (ins->label) printf(" %s", ins->label);
    printf("\n");
}
