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

node_t *node_get(node_t **nodes, int width, int height, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return NULL;
    return nodes[x + y * width];
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
void node_print_i(node_t **nodes, int width, int height, int x, int y, int row) {
    node_t *node = node_get(nodes, width, height, x, y);
    node_t *up = node_get(nodes, width, height, x, y - 1);
    node_t *left = node_get(nodes, width, height, x - 1, y);
    node_t *right = node_get(nodes, width, height, x + 1, y);
    node_t *down = node_get(nodes, width, height, x, y + 1);
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

void print_io(node_t **nodes, int width, int height, int row) {
    for (int y = 0; y < height; y += height - 1) {
        for (int x = 0; x < width; x++) {
            node_t *node = nodes[x + y * width];
            if (node->type != NODE_INPUT && node->type != NODE_OUTPUT) {
                continue;
            }
            io_t *io = (io_t *)node;
            int line = row - 2;
            switch (row) {
                case 0:
                    if (y == 0) {
                        printf("  IN%d |", x);
                    } else {
                        printf(" OUT%d |", x);
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

void sim_print(node_t **nodes, int width, int height) {
    int global_row = 0;
    for (int y = 1; y < height - 1; y++) {
        for (int row = 0; row < NODE_HEIGHT + 4; row++) {
            print_io(nodes, width, height, global_row++);
            for (int x = 0; x < width; x++) {
                node_print_i(nodes, width, height, x, y, row);
            }
            printf("\n");
        }
    }
    while (global_row < 42) {
        print_io(nodes, width, height, global_row++);
        printf("\n");
    }
}
