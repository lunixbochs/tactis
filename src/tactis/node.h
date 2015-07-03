#ifndef TACTIS_NODE_H
#define TACTIS_NODE_H

#include <stdint.h>

#include "types.h"

typedef io_status (*node_read_ptr)(node_t *node, io_dir dir, int16_t *data);
typedef io_status (*node_write_ptr)(node_t *node, io_dir dir, int16_t data);
typedef io_status (*node_step_ptr)(node_t *node);
typedef io_status (*node_latch_ptr)(node_t *node);
typedef io_status (*node_free_ptr)(node_t *node);

node_t *node_new();
io_status node_latch(node_t *node);
io_status node_step(node_t *node);
void node_free(node_t *node);
void node_move(node_t *node, int16_t x, int16_t y);
void node_print(node_t *node);

#endif
