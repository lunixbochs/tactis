#ifndef TACTIS_IO_H
#define TACTIS_IO_H

node_t *input_new(int16_t *inputs, node_read_ptr read, node_write_ptr write);
node_t *output_new(node_read_ptr read, node_write_ptr write);

#endif
