#include <stdio.h>
#include <string.h>

#include "tactis.h"
#include "./display.h"
#include "./error.h"
#include "./grid.h"

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
        node->status = IO_READ;
        return IO_READ;
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
    grid_t *grid = (grid_t *)node->grid;
    node_t *target = grid_get(grid, x, y);
    if (target && target->status == IO_WRITE && target->io_mask & mask) {
        *data = target->output;
        target->status = IO_DONE;
        node->status = IO_NONE;
    } else {
        node->status = IO_READ;
    }
    return node->status;
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
    return IO_WRITE;
}

int main(int argc, char **argv) {
    parse_error error;
    grid_t *grid = grid_new(4, 3, NULL);
    int16_t inputs[IO_HEIGHT] = {1, 2, 3, 4, 5, -1000};
    grid_set_input(grid, 0, input_new(inputs, sim_read, sim_write));
    grid_set_output(grid, 1, output_new(sim_read, sim_write));
    grid_set(grid, 0, 0, cpu_new(code, &error, sim_read, sim_write));
    if (! grid_get(grid, 0, 0)) {
        print_error(code, &error);
    }
    grid_set(grid, 1, 0, cpu_new(sink_code, &error, sim_read, sim_write));
    if (! grid_get(grid, 1, 0)) {
        print_error(code, &error);
    }
    grid_set(grid, 1, 1, cpu_copy(grid_get(grid, 1, 0)));
    grid_set(grid, 1, 2, cpu_copy(grid_get(grid, 1, 0)));
    while (1) {
        sim_print(grid);
        fgetc(stdin);
        grid_step(grid);
        grid_latch(grid);
    }
    return 0;
}
