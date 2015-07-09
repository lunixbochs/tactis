#include <stdio.h>
#include "display.h"
#include "tactis.h"

void cpu_print_i(node_t *node, int row) {
    cpu_state *cpu = (cpu_state *)node;
    if (cpu->line == row) {
        printf(">");
    } else {
        printf(" ");
    }
    int len = 0;
    if (cpu->labels[row]) {
        len += printf("%s: ", cpu->labels[row]);
    }
    len += ins_print(&cpu->ops[row]);
    int remain = 20 - len;
    // pad out the code lines
    for (int i = 0; i < remain; i++) {
        printf(" ");
    }
    // this draws the cpu registers/status
    switch (row) {
        case 3:
            printf("| ACC ");
            break;
        case 4:
            printf("| %-4d", cpu->acc);
            break;
        case 6:
            printf("| BAK ");
            break;
        case 7:
            printf("| %-4d", cpu->bak);
            break;
        default:
            printf("|     ");
            break;
    }
}

void stack_print_i(node_t *node, int row) {
    // stack_t *stack = (stack_t *)node;
    printf("printing stack: %d\n", row);
}

void error_print_i(node_t *node, int row) {
    io_t *output = (io_t *)node;
    printf("printing error: %d\n", row);
}

void print_io_status(int writing, int reading, const char *text, int16_t output) {
    if (writing) {
        printf("%s %-4d ", text, output);
    } else if (reading) {
        printf("%s ?    ", text);
    } else {
        printf("          ");
    }
}

#define CHECK(node, dir, _status) (node && node->status == _status && (node->io_mask & DIR_##dir || node->io_mask & DIR_ANY))
#define WRITING(node, dir) CHECK(node, dir, IO_WRITE)
#define READING(node, dir) CHECK(node, dir, IO_READ)
void node_print_i(grid_t *grid, int x, int y, int row) {
    node_t *node = grid_get(grid, x, y);
    node_t *up = grid_get(grid, x, y - 1);
    node_t *left = grid_get(grid, x - 1, y);
    node_t *right = grid_get(grid, x + 1, y);
    node_t *down = grid_get(grid, x, y + 1);
    // print the separator between the IO table and grid
    if (x == 0) {
        if (WRITING(node, LEFT)) {
            if (row == 9) {
                printf("<<<<");
            } else if (row == 10) {
                printf("%-4d", node->output);
            } else {
                printf("    ");
            }
        } else {
            printf("    ");
        }
    }
    switch (row) {
        case 0:
            printf("      ");
            print_io_status(WRITING(up, DOWN), READING(node, UP), "vvvv", up->output);
            print_io_status(WRITING(node, UP), READING(up, DOWN), "^^^^", node->output);
            printf("         ");
            return;
        case 1:
            printf("|----------------------|-----|    ");
            return;
        case NODE_HEIGHT + 2:
            printf("|----------------------|-----|    ");
            return;
        case NODE_HEIGHT + 3:
            printf("      ");
            print_io_status(WRITING(node, DOWN), READING(down, UP), "vvvv", node->output);
            print_io_status(WRITING(down, UP), READING(node, DOWN), "^^^^", down->output);
            printf("         ");
            return;
    }
    if (node) {
        int line = row - 2;
        switch (node->type) {
            case NODE_CPU:
                printf("| ");
                cpu_print_i(node, line);
                printf("|");
                break;
            default:
                // can't draw non-cpu nodes yet
                return;
        }
    } else {
        printf("|                            |");
    }
    // this draws IO to the right of a node
    switch (row) {
        case 5:
            if (WRITING(node, RIGHT) || READING(right, LEFT)) {
                printf(">>>>");
            } else {
                printf("    ");
            }
            break;
        case 6:
            if (WRITING(node, RIGHT)) {
                printf("%-4d", node->output);
            } else if (READING(right, LEFT)) {
                printf("?   ");
            } else {
                printf("    ");
            }
            break;
        case 9:
            if (READING(node, RIGHT) || WRITING(right, LEFT)) {
                printf("<<<<");
            } else {
                printf("    ");
            }
            break;
        case 10:
            if (WRITING(right, LEFT)) {
                printf("%-4d", right->output);
            } else if (READING(node, RIGHT)) {
                printf("?   ");
            } else {
                printf("    ");
            }
            break;
        default:
            printf("    ");
            break;
    }
}

void print_io(grid_t *grid, int row) {
    for (int output = 0; output < 2; output++) {
        for (int x = 0; x < grid->width; x++) {
            node_t *node = NULL;
            if (output) {
                node = grid_get_output(grid, x);
            } else {
                node = grid_get_input(grid, x);
            }
            if (!node || (node->type != NODE_INPUT && node->type != NODE_OUTPUT)) {
                continue;
            }
            io_t *io = (io_t *)node;
            int line = row - 2;
            if (row > IO_HEIGHT + 2) {
                printf("       ");
                continue;
            }
            switch (row) {
                case 0:
                    if (output) {
                        printf(" OUT%d |", x);
                    } else {
                        printf("  IN%d |", x);
                    }
                    break;
                case 1:
                    printf("-------");
                    break;
                default:
                    if (line < io->pos && node->type == NODE_INPUT) {
                        printf(" %4d <", io->data[line]);
                    } else if (io->data[line] > -1000) {
                        printf(" %4d |", io->data[line]);
                    } else {
                        printf(" %4d |", 0);
                    }
                    break;
                case IO_HEIGHT + 2:
                    printf("-------");
                    break;
            }
        }
    }
}

void sim_print(grid_t *grid) {
    int global_row = 0;
    for (int y = 0; y < grid->height; y++) {
        // we need one extra row at the end to show the output line
        int height = NODE_HEIGHT + 3 + (y == grid->height - 1);
        for (int row = 0; row < height; row++) {
            print_io(grid, global_row++);
            for (int x = 0; x < grid->width; x++) {
                node_print_i(grid, x, y, row);
            }
            printf("\n");
        }
    }
    while (global_row < 42) {
        print_io(grid, global_row++);
        printf("\n");
    }
}
