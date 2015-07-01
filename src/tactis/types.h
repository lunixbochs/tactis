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

// this constants must be synced with REG_*
typedef enum {
    DIR_NONE  = 0,
    DIR_UP    = 4,
    DIR_LEFT  = 8,
    DIR_RIGHT = 16,
    DIR_DOWN  = 32,
    DIR_ANY   = 64,
    DIR_LAST  = 128,
} io_dir;

typedef enum {
    IO_NONE,
    // returned during write delay
    IO_LOAD,
    // read or write is blocking
    IO_WAIT,
    // read/write has finished
    IO_DONE,
} io_status;

typedef enum {
    NODE_NONE,
    NODE_CPU,
    NODE_STACK,
    NODE_ERROR,
    NODE_INPUT,
    NODE_OUTPUT,
} node_type;

typedef struct _node {
    int8_t type;
    int16_t x, y;
    int16_t last;
    // this is a bitfield of the directions allowed for output
    io_dir out_mask;
    // this is the value held for output
    int16_t output;
    // IO_NONE with no data, IO_LOAD waiting for latch, and IO_WAIT while waiting for receiver
    io_status status;
    // useful place to store cpu table for callbacks
    void *user;
    // function pointers
    io_status (*read)(struct _node *node, io_dir dir, int16_t *data);
    io_status (*write)(struct _node *node, io_dir dir, int16_t data);
    io_status (*step)(struct _node *node);
    io_status (*latch)(struct _node *node);
    void (*free)(struct _node *node);
    void (*print)(struct _node *node);
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
    REG_NONE = 0,
    REG_NIL  = 1,
    REG_ACC  = 2,

    REG_UP    = 4,
    REG_LEFT  = 8,
    REG_RIGHT = 16,
    REG_DOWN  = 32,
    REG_ANY   = 64,
    REG_LAST  = 128,
} cpu_reg;

typedef struct {
    int16_t op, a, b;
    char *label;
    int8_t jmp_offset;
} cpu_ins;

typedef struct _cpu_state {
    node_t node;
    int16_t acc, bak;
    int16_t line;
    cpu_ins ops[CPU_HEIGHT];
    char *labels[CPU_HEIGHT];
} cpu_state;

#endif
