#include <stdio.h>
#include <string.h>

#include "tactis.h"
#include "./display.h"
#include "./error.h"

char *code = (
    "MOV UP, ACC\n"
    "1: SUB 1\n"
    "MOV ACC, RIGHT\n"
    "JNZ 1\n"
);

char *sink_code = "MOV ANY, DOWN\n";

static io_status sim_read_any(node_t *node, io_dir dir, int16_t *data);

io_status sim_read(node_t *node, io_dir dir, int16_t *data) {
    if (dir == DIR_ANY) {
        // TODO: IO_DONE?
        if ((sim_read_any(node, DIR_LEFT, data) == IO_NONE) ||
             (sim_read_any(node, DIR_RIGHT, data) == IO_NONE) ||
             (sim_read_any(node, DIR_UP, data) == IO_NONE) ||
             (sim_read_any(node, DIR_DOWN, data) == IO_NONE)) {
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
        node_t **nodes = node->user;
        node_t *node = nodes[x + y * 2];
        if (node->status == IO_WAIT) {
            if (node->out_mask & mask) {
                *data = node->output;
                node->status = IO_DONE;
                return IO_NONE;
            }
        }
    }
    return IO_WAIT;
}

static io_status sim_read_any(node_t *node, io_dir dir, int16_t *data) {
    io_status status = sim_read(node, dir, data);
    if (status == IO_NONE) {
        node->last = dir;
    }
    return status;
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
    node_t *nodes[3][2] = {0};
    int16_t inputs[IO_HEIGHT] = {1, 2, 3, 4, 5, -1000};
    node_t *in_bs = input_new(inputs, sim_read, sim_write);
    nodes[0][0] = in_bs;
    for (int x = 0; x < 2; x++) {
        node_t *out_print = output_new(sim_read, sim_write);
        nodes[2][x] = out_print;
    }
    nodes[1][0] = cpu;
    nodes[1][1] = cpu_new(sink_code, &error, sim_read, sim_write);
    if (! nodes[1][1]) {
        print_error(code, &error);
    }
    node_t *null_node = node_new();
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 3; y++) {
            if (nodes[y][x]) {
                node_move(nodes[y][x], x, y);
                nodes[y][x]->user = (void *)nodes;
            } else {
                nodes[y][x] = null_node;
            }
        }
    }
    /*
    for (int x = 0; x < 2; x++) {
        for (int y = 1; y < 3; y++) {
            if (nodes[x][y])
                continue;
            nodes[x][y] = cpu_new(sink_code, &error, sim_read, sim_write);
            if (! nodes[x][y]) {
                print_error(code, &error);
            }
        }
    }
    */
    while (1) {
        sim_print((node_t **)nodes, 2, 3);
        fgetc(stdin);
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                node_step(nodes[y][x]);
            }
        }
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 3; y++) {
                node_latch(nodes[y][x]);
            }
        }
    }
    return 0;
}
