#ifndef CPU_H
#define CPU_H
#include <stdint.h>

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
    int32_t op, a, b;
} ins_t;

typedef struct {
    int32_t acc;
    int32_t bak;
    int32_t line;
    ins_t *code;
} state_t;

typedef struct {
    char *msg;
    int line;
    int col;
} error_t;
#endif
