#ifndef TACTIS_CPU_H
#define TACTIS_CPU_H

#include <stdint.h>
#include "node.h"

cpu_state *cpu_new(int16_t id, char *code, parse_error *error);
void cpu_free(cpu_state *cpu);
void ins_print(ins_t *ins);

#endif
