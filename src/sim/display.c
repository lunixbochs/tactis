#include <stdio.h>
#include "display.h"
#include "tactis.h"

#define l(line, ...) case line: printf(__VA_ARGS__); break;
void cpu_print_i(node_t *node, int row) {
    cpu_state *cpu = (cpu_state *)node;
    switch (row) {
        l(0, "---------------------|");
        l(1, " ACC: %-4d BAK: %-4d |", cpu->acc, cpu->bak);
        l(2, "---------------------|");
        l(18, "---------------------|");
        default:
            if (row >= 3 && row <= CPU_HEIGHT + 4) {
                int line = row - 3;
                if (cpu->line == line) {
                    printf(">");
                } else {
                    printf(" ");
                }
                int len = 0;
                if (cpu->labels[line]) {
                    len += printf("%s: ", cpu->labels[line]);
                }
                len += ins_print(&cpu->ops[line]);
                int remain = 20 - len;
                for (int i = 0; i < remain; i++) {
                    printf(" ");
                }
                printf("|");
            }
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
                        printf(" IN%d|", x);
                    } else {
                        printf("OUT%d|", x);
                    }
                    break;
                case 1:
                    printf("-----");
                    break;
                default:
                    if (line < io->pos && node->type == NODE_INPUT) {
                        printf(" -- |");
                    } else if (io->data[line] > -1000) {
                        printf("%-4d|", io->data[line]);
                    } else {
                        printf("%-4d|", 0);
                    }
                    break;
                case IO_HEIGHT + 2:
                    printf("-----");
                    break;
            }
        }
    }
}

void sim_print(node_t **nodes, int width, int height) {
    int global_row = 0;
    for (int y = 1; y < height - 1; y++) {
        for (int row = 0; row < CPU_HEIGHT + 4; row++) {
            print_io(nodes, width, height, global_row++);
            for (int x = 0; x < width; x++) {
                node_t *node = nodes[x + y * width];
                switch (node->type) {
                    case NODE_CPU:
                        cpu_print_i(node, row);
                        break;
                }
            }
            printf("\n");
        }
    }
    while (global_row < 42) {
        print_io(nodes, width, height, global_row++);
        printf("\n");
    }
}
