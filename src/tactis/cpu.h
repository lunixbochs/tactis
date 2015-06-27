#ifndef TACTIS_CPU_H
#define TACTIS_CPU_H

#include <stdint.h>

#define CPU_HEIGHT 15
#define CPU_WIDTH 18

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
    ins_t ops[CPU_HEIGHT];
    char *labels[CPU_HEIGHT];
} cpu_state_t;

void ins_print(ins_t *ins);

#endif
