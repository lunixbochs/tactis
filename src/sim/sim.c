#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "node.h"
#include "parse.h"
#include "./error.h"

char *code = (
    "MOV 1, ACC\n"
    "1: ADD 2\n"
    "SUB 1\n"
    "SAV\n"
    "JMP 1\n"
);

char *sink_code = "MOV ANY, ACC\n";

io_status sim_read(node_t *node, io_dir dir, int16_t *data) {
    printf("read from %d, %d\n", node->x, node->y);
    return IO_WAIT;
}

io_status sim_write(node_t *node, io_dir dir, int16_t data, int16_t delay) {
    printf("write from %d, %d\n", node->x, node->y);
    return IO_WAIT;
}

int main(int argc, char **argv) {
    parse_error error;
    // TODO: what if I put the error struct on the cpu
    cpu_state *cpu = cpu_new(code, &error, sim_read, sim_write);
    if (! cpu) {
        print_error(code, &error);
    }
    cpu_move(cpu, 0, 0);
    cpu_state *cpus[] = { cpu, 0, 0, 0, 0, 0 };
    for (int i = 1; i < 5; i++) {
        cpus[i] = cpu_new(sink_code, &error, sim_read, sim_write);
        if (! cpus[i]) {
            print_error(code, &error);
        }
    }
    cpu->node.user = (void *)cpus;
    while (1) {
        cpu_print(cpu);
        fgetc(stdin);
        cpu_step(cpu);
    }
    return 0;
}
