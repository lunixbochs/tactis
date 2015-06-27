#ifndef PARSE_H
#define PARSE_H
#include <stdint.h>
#include "cpu.h"

typedef struct { 
    const char *name; 
    int32_t token; 
} token_pair; 

const char *token_name(int token, token_pair *tpos);
const char *op_name(int token);
const char *reg_name(int token);
ins_t *parse(char *s, error_t *error);

extern token_pair op_map[];
extern token_pair reg_map[];
#endif
