#ifndef TACTIS_PARSE_H
#define TACTIS_PARSE_H

#include <stdint.h>
#include "cpu.h"

typedef struct { 
    const char *name; 
    int32_t token; 
} token_pair; 

typedef struct {
    char *msg;
    int line, col, len;
} parse_error;

const char *token_name(int token, token_pair *tpos);
const char *op_name(int token);
const char *reg_name(int token);
int parse_line(char *s, char **label, ins_t **ins, parse_error *error);

extern token_pair op_map[];
extern token_pair reg_map[];

#endif
