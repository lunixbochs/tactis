#include <stdio.h>
#include <stdlib.h>

#include "node.h"

typedef struct {
    node_t node;
    int16_t *input;
    int16_t pos;
} input_t;

io_status input_step(node_t *node) {
    input_t *input = (input_t *)node;
    if (node->status == IO_DONE) {
        input->pos++;
        node->status = IO_NONE;
    }
    if (node->status == IO_NONE && input->input[input->pos] > -1000) {
        node->status = IO_WAIT;
        node->out_mask = DIR_DOWN;
        node->output = input->input[input->pos];
        node->write(node, DIR_DOWN, input->input[input->pos]);
    }
    return node->status;
}

io_status input_latch(node_t *node) {
    input_t *input = (input_t *)node;
    return node->status;
}

void input_free(node_t *node) {
    input_t *input = (input_t *)input;
    free(input);
}

void input_print(node_t *node) {
    input_t *input = (input_t *)node;
    int16_t *pos = input->input + input->pos;
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
    node_t *node = calloc(1, sizeof(input_t));
    node->type = NODE_INPUT;
    node->read = read;
    node->write = write;
    node->step = input_step;
    node->latch = input_latch;
    node->free = input_free;
    node->print = input_print;
    input_t *input = (input_t *)node;
    input->input = inputs;
    return node;
}


io_status output_step(node_t *node) {
    int16_t value = 0;
    if (node->read(node, DIR_UP, &value) != IO_WAIT) {
        printf("output: %d\n", value);
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
    node_t *node = malloc(sizeof(node_t));
    node->type = NODE_OUTPUT;
    node->read = read;
    node->write = write;
    node->step = output_step;
    node->latch = output_latch;
    node->free = output_free;
    node->print = output_print;
    return node;
}
