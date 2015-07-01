#include <stdio.h>

#include "node.h"

void node_move(node_t *node, int16_t x, int16_t y) {
    node->x = x;
    node->y = y;
}

io_status node_step(node_t *node) {
    return node->step(node);
}

io_status node_latch(node_t *node) {
    return node->latch(node);
}

void node_free(node_t *node) {
    return node->free(node);
}

void node_print(node_t *node) {
    if (node->status == IO_WAIT) {
        int16_t mask = node->out_mask;
        printf("moving: (%d, %d) %d %d ", node->x, node->y, node->output, mask);
        if (mask & DIR_UP) printf("UP ");
        if (mask & DIR_LEFT) printf("LEFT ");
        if (mask & DIR_DOWN) printf("DOWN ");
        if (mask & DIR_RIGHT) printf("RIGHT ");
        if (mask & DIR_ANY) printf("ANY ");
    }
    printf("\n");
    node->print(node);
}
