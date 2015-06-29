#ifndef TACTIS_TYPES_H
#define TACTIS_TYPES_H

#include "const.h"

// error types

typedef struct {
    const char *name;
    int32_t token;
} token_pair;

typedef struct {
    char *msg;
    int line, col, len;
} parse_error;

// node type

typedef enum {
    DIR_NONE  = 0,
    DIR_UP    = 1,
    DIR_LEFT  = 2,
    DIR_RIGHT = 4,
    DIR_DOWN  = 8,
    DIR_ANY   = 16,
    DIR_LAST  = 32,
} io_dir;

typedef enum {
    IO_WAIT,
    IO_DONE,
} io_status;

typedef struct _node {
    int8_t type;
    int16_t id;
    io_status (*read)(struct _node *node, io_dir dir, int *data);
    io_status (*write)(struct _node *node, io_dir dir, int data);
    io_dir read_mask, write_mask;
} node_t;

// cpu type

typedef enum {
    OP_NONE,
    OP_NOP,
    OP_MOV,
    OP_SWP,
    OP_SAV,
    OP_ADD,
    OP_SUB,
    OP_NEG,
    OP_JMP,
    OP_JEZ,
    OP_JNZ,
    OP_JGZ,
    OP_JLZ,
    OP_JRO,
} operator_t;

typedef enum {
    REG_NONE,
    REG_NIL,
    REG_ACC,
    REG_ANY,
    REG_LAST,
    REG_UP,
    REG_DOWN,
    REG_LEFT,
    REG_RIGHT,
} reg_t;

typedef struct {
    int16_t op, a, b;
    char *label;
    int8_t jmp_offset;
} ins_t;

typedef struct _cpu_state {
    node_t node;
    int16_t acc;
    int16_t bak;
    int16_t line;
    ins_t ops[CPU_HEIGHT];
    char *labels[CPU_HEIGHT];
} cpu_state;

#endif
