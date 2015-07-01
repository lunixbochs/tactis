#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "io.h"
#include "node.h"
#include "parse.h"
#include "./error.h"

typedef struct {
    node_t node;
    int16_t *input;
} input_t;

char *code = (
    "MOV UP, ACC\n"
    "1: SUB 1\n"
    "MOV ACC, RIGHT\n"
    "JNZ 1\n"
);

char *sink_code = "MOV ANY, DOWN\n";

io_status sim_read(node_t *node, io_dir dir, int16_t *data) {
    if (dir == DIR_LAST) {
        dir = node->last;
    }
    if (dir == DIR_ANY) {
        // TODO: IO_DONE?
        if ((sim_read(node, DIR_LEFT, data) == IO_NONE) ||
             (sim_read(node, DIR_RIGHT, data) == IO_NONE) ||
             (sim_read(node, DIR_UP, data) == IO_NONE) ||
             (sim_read(node, DIR_DOWN, data) == IO_NONE)) {
            return IO_NONE;
        }
    }
    int x = -1, y = -1;
    int mask = DIR_ANY;
    if (dir == DIR_LEFT) {
        x = node->x - 1;
        y = node->y;
        mask |= DIR_RIGHT;
    }
    if (dir == DIR_RIGHT) {
        x = node->x + 1;
        y = node->y;
        mask |= DIR_LEFT;
    }
    if (dir == DIR_UP) {
        x = node->x;
        y = node->y - 1;
        mask |= DIR_DOWN;
    }
    if (dir == DIR_DOWN) {
        x = node->x;
        y = node->y + 1;
        mask |= DIR_UP;
    }
    // TODO: don't hardcode bounds
    if (x != -1 && y != -1 && x < 2 && y < 3) {
        node_t *(*nodes)[3] = node->user;
        node_t *node = nodes[x][y];
        if (node->status == IO_WAIT) {
            if (node->out_mask & mask || (node->out_mask & DIR_LAST && node->last & mask)) {
                *data = node->output;
                node->status = IO_DONE;
                return IO_NONE;
            }
        }
    }
    return IO_WAIT;
}

io_status sim_write(node_t *node, io_dir dir, int16_t data) {
    // printf("write from %d, %d\n", node->x, node->y);
    return IO_WAIT;
}

int main(int argc, char **argv) {
    parse_error error;
    node_t *cpu = cpu_new(code, &error, sim_read, sim_write);
    if (! cpu) {
        print_error(code, &error);
    }
    node_t *cpus[2][3] = {0};
    int16_t inputs[] = {1, 2, 3, 4, 5, -1000};
    for (int x = 0; x < 2; x++) {
        node_t *in_bs = input_new(inputs, sim_read, sim_write);
        cpus[x][0] = in_bs;

        node_t *out_print = output_new(sim_read, sim_write);
        cpus[x][2] = out_print;
    }
    cpus[0][1] = cpu;
    cpus[1][1] = cpu_new(sink_code, &error, sim_read, sim_write);
    if (! cpus[1][1]) {
        print_error(code, &error);
    }
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 3; y++) {
            node_move(cpus[x][y], x, y);
            cpus[x][y]->user = (void *)cpus;
        }
    }
    /*
    for (int x = 0; x < 2; x++) {
        for (int y = 1; y < 3; y++) {
            if (cpus[x][y])
                continue;
            cpus[x][y] = cpu_new(sink_code, &error, sim_read, sim_write);
            if (! cpus[x][y]) {
                print_error(code, &error);
            }
        }
    }
    */
    node_print(cpu);
    while (1) {
        fgetc(stdin);
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                node_step(cpus[x][y]);
            }
        }
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                node_latch(cpus[x][y]);
            }
        }
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                if (cpus[x][y]->type == NODE_CPU) {
                }
                node_print(cpus[x][y]);
            }
        }
    }
    return 0;
}
