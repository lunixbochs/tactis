#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

io_status input_step(node_t *node) {
    io_t *input = (io_t *)node;
    if (node->status == IO_DONE) {
        input->pos++;
        node->status = IO_NONE;
    }
    if (input->pos >= IO_HEIGHT || input->data[input->pos] == -1000) {
        return node->status;
    }
    if (node->status == IO_NONE) {
        node->status = IO_LOAD;
        node->io_mask = DIR_DOWN;
        node->output = input->data[input->pos];
    }
    return node->status;
}

io_status input_latch(node_t *node) {
    if (node->status == IO_LOAD) {
        return node_write(node, node->io_mask, node->output);
    }
    return node->status;
}

void input_free(node_t *node) {
    io_t *input = (io_t *)input;
    free(input);
}

void input_print(node_t *node) {
    io_t *input = (io_t *)node;
    int16_t *pos = input->data + input->pos;
    printf("input queue: ");
    while (*pos != -1000) {
        printf("%d", *pos);
        pos++;
        if (*pos != -1000) {
            printf(", ");
        }
    }
    printf("\n");
}

node_t *input_new(int16_t *inputs, node_read_ptr read, node_write_ptr write) {
    node_t *node = calloc(1, sizeof(io_t));
    node->type = NODE_INPUT;
    node->read = read;
    node->write = write;
    node->step = input_step;
    node->latch = input_latch;
    node->free = input_free;
    node->print = input_print;
    io_t *input = (io_t *)node;
    memcpy(input->data, inputs, IO_HEIGHT * sizeof(int16_t));
    return node;
}


io_status output_step(node_t *node) {
    io_t *output = (io_t *)node;
    int16_t value = 0;
    if (output->pos < IO_HEIGHT && node_read(node, DIR_UP, &value) == IO_NONE) {
        output->data[output->pos++] = value;
    }
    return node->status;
}

io_status output_latch(node_t *node) {
    return node->status;
}

void output_free(node_t *node) {
    free(node);
}

void output_print(node_t *node) {

}

node_t *output_new(node_read_ptr read, node_write_ptr write) {
    node_t *node = calloc(1, sizeof(io_t));
    node->type = NODE_OUTPUT;
    node->read = read;
    node->write = write;
    node->step = output_step;
    node->latch = output_latch;
    node->free = output_free;
    node->print = output_print;
    node->status = IO_READ;
    node->io_mask = DIR_UP;
    return node;
}
