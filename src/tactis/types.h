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
    IO_NONE,
    // read or write is blocking
    IO_WAIT,
    // returned during write delay
    IO_LOAD,
} io_status;

typedef struct _node {
    int8_t type;
    int16_t x, y;
    io_status (*read)(struct _node *node, io_dir dir, int16_t *data);
    io_status (*write)(struct _node *node, io_dir dir, int16_t data, int16_t delay);
    // this is the number of cycles a write delays
    int16_t delay;
    // this is a bitfield of the directions allowed for output
    io_dir out_mask;
    // this is the value held for output
    int16_t output;
    // IO_NONE with no data, IO_LOAD during delay, and IO_WAIT while waiting for receiver
    io_status status;
    // useful place to store cpu table for callbacks
    void *user;
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
} cpu_op;

typedef enum {
    REG_NONE,
    REG_NIL,
    REG_ACC,
    REG_ANY,
    REG_LAST,
    REG_UP,
    REG_LEFT,
    REG_RIGHT,
    REG_DOWN,
} cpu_reg;

typedef struct {
    int16_t op, a, b;
    char *label;
    int8_t jmp_offset;
} cpu_ins;

typedef struct _cpu_state {
    node_t node;
    int16_t acc, bak;
    int16_t last;
    int16_t line;
    cpu_ins ops[CPU_HEIGHT];
    char *labels[CPU_HEIGHT];
} cpu_state;

#endif
