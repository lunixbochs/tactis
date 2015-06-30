#ifndef TACTIS_CPU_H
#define TACTIS_CPU_H

#include <stdint.h>
#include "node.h"

cpu_state *cpu_new(char *code, parse_error *error, node_read_ptr read, node_write_ptr write);
void cpu_free(cpu_state *cpu);
void cpu_move(cpu_state *cpu, int16_t x, int16_t y);
io_status cpu_step(cpu_state *cpu);
void cpu_print(cpu_state *cpu);
void ins_print(cpu_ins *ins);

#endif
