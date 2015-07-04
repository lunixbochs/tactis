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

void node_print_i(node_t **nodes, int width, int height, int x, int y, int row) {
    // print the separator between the IO table and grid
    if (x == 0) {
        switch (row) {
            case 5:
                printf(">>>>");
                break;
            case 6:
                printf("-999");
                break;
            case 9:
                printf("<<<<");
                break;
            case 10:
                printf("-999");
                break;
            default:
                printf("    ");
                break;
        }
    }
    node_t *node = nodes[x + y * width];
    switch (row) {
        case 0:
            printf("      vvvv -999 ^^^^ -999         ");
            return;
        case 1:
            printf("|----------------------|-----|    ");
            return;
        case NODE_HEIGHT + 2:
            printf("|----------------------|-----|    ");
            return;
        case NODE_HEIGHT + 3:
            printf("      vvvv -999 ^^^^ -999         ");
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
            printf(">>>>");
            break;
        case 6:
            printf("-999");
            break;
        case 9:
            printf("<<<<");
            break;
        case 10:
            printf("-999");
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
