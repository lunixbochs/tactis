#ifndef TACTIS_PARSE_H
#define TACTIS_PARSE_H

#include <stdint.h>
#include "types.h"

const char *token_name(int token, token_pair *tpos);
const char *op_name(int token);
const char *reg_name(int token);
int parse_line(char *s, char **label, cpu_ins *ins, parse_error *error);

extern token_pair op_map[];
extern token_pair reg_map[];

#endif
