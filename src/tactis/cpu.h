#ifndef TACTIS_CPU_H
#define TACTIS_CPU_H

#include <stdint.h>
#include "node.h"

node_t *cpu_new(char *code, parse_error *error, node_read_ptr read, node_write_ptr write);
io_status cpu_step(node_t *node);
io_status cpu_latch(node_t *node);
void cpu_free(node_t *node);
void cpu_print(node_t *node);
void ins_print(cpu_ins *ins);

#endif
