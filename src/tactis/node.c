#include <stdio.h>
#include <stdlib.h>

#include "node.h"

node_t *node_new() {
    return calloc(1, sizeof(node_t));
}

void node_move(node_t *node, int16_t x, int16_t y) {
    node->x = x;
    node->y = y;
}

io_status node_step(node_t *node) {
    if (! node->step)
        return IO_NONE;
    return node->step(node);
}

io_status node_latch(node_t *node) {
    if (! node->latch)
        return IO_NONE;
    return node->latch(node);
}

io_status node_write(node_t *node, io_dir dir, int16_t value) {
    node->status = IO_WRITE;
    node->out_mask = dir;
    node->output = value;
    return node->write(node, dir, value);
}

io_status node_read(node_t *node, io_dir dir, int16_t *value) {
    return node->read(node, dir, value);
}

void node_free(node_t *node) {
    if (! node_free)
        return;
    return node->free(node);
}

void node_print(node_t *node) {
    if (! node->print)
        return;
    if (node->status == IO_WRITE) {
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
