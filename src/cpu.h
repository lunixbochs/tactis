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
    int16_t op, a, b;
    char *label;
    int8_t jmp_offset;
} ins_t;

typedef struct {
    int16_t acc;
    int16_t bak;
    int16_t line;
    ins_t *code;
} state_t;
#endif
