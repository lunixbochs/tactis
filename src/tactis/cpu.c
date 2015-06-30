#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"
#include "node.h"
#include "util.h"

cpu_state *cpu_new(char *code, parse_error *error, node_read_ptr read, node_write_ptr write) {
    char *lines = strdup(code);
    char *pos = lines;
    cpu_state *cpu = calloc(1, sizeof(cpu_state));
    cpu->node.read = read;
    cpu->node.write = write;
    char *line;
    int i;
    while ((line = strsep(&pos, "\n")) != NULL && i < CPU_HEIGHT) {
        if (parse_line(line, &cpu->labels[i], &cpu->ops[i], error)) {
            error->line = i;
            free(lines);
            free(cpu);
            return NULL;
        }
        i++;
    }
    for (int i = 0; i < CPU_HEIGHT; i++) {
        for (int j = 0; j < CPU_HEIGHT; j++) {
            if (cpu->labels[i] && cpu->ops[j].label &&
                    strcmp(cpu->labels[i], cpu->ops[j].label) == 0) {
                cpu->ops[j].jmp_offset = i - j;
            }
        }
    }
    free(lines);
    return cpu;
}

void cpu_free(cpu_state *cpu) {
    for (int i = 0; i < CPU_HEIGHT; i++) {
        free(cpu->labels[i]);
    }
}

void cpu_move(cpu_state *cpu, int16_t x, int16_t y) {
    cpu->node.x = x;
    cpu->node.y = y;
}

static io_status reg_get(cpu_state *cpu, int16_t imm, int16_t *value) {
    *value cpu->output;
    if (*value == 0) {
        if (imm > 4096) {
            *value = imm - 5096;
        } else switch (imm) {
            case REG_NIL:
                value = 0;
                break;
            case REG_ACC:
                *value = cpu->acc;
                break;
            // LAST is handled in port_read()
            case REG_LAST:
            case REG_ANY:
            case REG_UP:
            case REG_LEFT:
            case REG_RIGHT:
            case REG_DOWN:
                return cpu->node.read(cpu->node, imm, value);
        }
    }
    return IO_NONE;
}

io_status cpu_step(cpu_state *cpu) {
    int16_t value;
    cpu_ins *ins;
    ins = &cpu->ops[cpu->line];
    switch (ins->op) {
        case OP_NOP:
            break;
        case OP_MOV:
            // source
            if (reg_get(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            // dst
            switch (ins->b) {
                case REG_NIL:
                    break;
                case REG_ACC:
                    cpu->acc = value;
                    break;
                case REG_LAST:
                case REG_ANY:
                case REG_UP:
                case REG_LEFT:
                case REG_RIGHT:
                case REG_DOWN:
                    printf("port write to %d\n", ins->b);
                    return IO_WAIT;
            }
            break;
        case OP_SWP:
        {
            int16_t tmp = cpu->acc;
            cpu->acc = cpu->bak;
            cpu->bak = tmp;
            break;
        }
        case OP_SAV:
            cpu->bak = cpu->acc;
            break;
        case OP_ADD:
            if (reg_get(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            cpu->acc += value;
            break;
        case OP_SUB:
            if (reg_get(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            cpu->acc -= value;
            break;
        case OP_NEG:
            cpu->acc = -cpu->acc;
            break;
        case OP_JMP:
            cpu->line += ins->jmp_offset;
            return IO_NONE;
        case OP_JEZ:
            if (cpu->acc == 0) {
                cpu->line += ins->jmp_offset;
            }
            return IO_NONE;
        case OP_JNZ:
            if (cpu->acc != 0) {
                cpu->line += ins->jmp_offset;
            }
            return IO_NONE;
        case OP_JGZ:
            if (cpu->acc > 0) {
                cpu->line += ins->jmp_offset;
            }
            return IO_NONE;
        case OP_JLZ:
            if (cpu->acc < 0) {
                cpu->line += ins->jmp_offset;
            }
            return IO_NONE;
        case OP_JRO:
            if (reg_get(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            cpu->line += value;
            return IO_NONE;
    }
    cpu->acc = clamp_999(cpu->acc);
    cpu->line = (cpu->line + 1) % CPU_HEIGHT;
    // skip empty lines
    for (int i = 0; i < CPU_HEIGHT; i++) {
        if (cpu->ops[cpu->line].op == OP_NONE) {
            cpu->line = (cpu->line + 1) % CPU_HEIGHT;
        } else {
            break;
        }
    }
    return IO_NONE;
}

void cpu_print(cpu_state *cpu) {
    printf("------------------\n");
    printf("| ACC: %d, BAK: %d\n", cpu->acc, cpu->bak);
    printf("------------------\n");
    int height;
    for (int i = 0; i < CPU_HEIGHT; i++) {
        if (cpu->ops[i].op != OP_NONE) {
            height = i + 1;
        }
    }
    for (int i = 0; i < height; i++) {
        if (cpu->line == i) {
            printf("> ");
        } else {
            printf("  ");
        }
        if (cpu->labels[i]) {
            printf("%s: ", cpu->labels[i]);
        }
        ins_print(&cpu->ops[i]);
    }
    printf("------------------\n");
}

void ins_print(cpu_ins *ins) {
    if (ins->op) {
        printf("%s", op_name(ins->op));
        if (ins->a) printf(" %s", reg_name(ins->a));
        if (ins->b) printf(", %s", reg_name(ins->b));
        if (ins->label) printf(" %s", ins->label);
    }
    printf("\n");
}
