#ifndef TACTIS_NODE_H
#define TACTIS_NODE_H

#include "types.h"

typedef io_status (*node_read_ptr)(node_t *node, io_dir dir, int16_t *data);
typedef io_status (*node_write_ptr)(node_t *node, io_dir dir, int16_t data, int16_t delay);

#endif
