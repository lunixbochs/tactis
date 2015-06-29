#ifndef TACTIS_NODE_H
#define TACTIS_NODE_H

#include "types.h"

io_status (*node_read_ptr)(node_t *node, io_dir dir, int *data);
io_status (*node_write_ptr)(node_t *node, io_dir dir, int data);

#endif
