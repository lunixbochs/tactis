#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"
#include "node.h"
#include "util.h"

node_t *cpu_new(char *code, parse_error *error, node_read_ptr read, node_write_ptr write) {
    char *lines = strdup(code);
    char *pos = lines;
    cpu_state *cpu = calloc(1, sizeof(cpu_state));
    node_t *node = (node_t *)cpu;
    node->type = NODE_CPU;
    node->read = read;
    node->write = write;
    node->step = cpu_step;
    node->latch = cpu_latch;
    node->free = cpu_free;
    node->print = cpu_print;
    char *line;
    int i = 0;
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
    return node;
}

void cpu_free(node_t *node) {
    cpu_state *cpu = (cpu_state *)node;
    for (int i = 0; i < CPU_HEIGHT; i++) {
        free(cpu->labels[i]);
    }
}

static io_status cpu_read(cpu_state *cpu, int16_t imm, int16_t *value) {
    if (imm > 4096) {
        *value = imm - 5096;
        return IO_NONE;
    } else {
        switch (imm) {
            case REG_NIL:
                *value = 0;
                return IO_NONE;
            case REG_ACC:
                *value = cpu->acc;
                return IO_NONE;
            case REG_LAST:
                imm = cpu->node.last || REG_NIL;
            case REG_ANY:
            case REG_UP:
            case REG_LEFT:
            case REG_RIGHT:
            case REG_DOWN:
                return cpu->node.read(&cpu->node, imm, value);
            default:
                fprintf(stderr, "error: invalid register\n");
                return IO_WAIT;
        }
    }
    return IO_NONE;
}

static io_status cpu_write(cpu_state *cpu, int16_t mask, int16_t value) {
    cpu->node.status = IO_LOAD;
    cpu->node.out_mask = mask;
    cpu->node.output = value;
    return IO_LOAD;
}

void cpu_advance(node_t *node) {
    cpu_state *cpu = (cpu_state *)node;
    cpu->line = (cpu->line + 1) % CPU_HEIGHT;
    // skip empty lines
    for (int i = 0; i < CPU_HEIGHT; i++) {
        if (cpu->ops[cpu->line].op == OP_NONE) {
            cpu->line = (cpu->line + 1) % CPU_HEIGHT;
        } else {
            break;
        }
    }
}

io_status cpu_step(node_t *node) {
    cpu_state *cpu = (cpu_state *)node;
    if (cpu->node.status != IO_NONE) {
        return cpu->node.status;
    }
    int16_t value, reg;
    cpu_ins *ins;
    ins = &cpu->ops[cpu->line];
    switch (ins->op) {
        case OP_NOP:
            break;
        case OP_MOV:
            // source
            if (cpu_read(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            // dst
            reg = ins->b;
            switch (reg) {
                case REG_NIL:
                    break;
                case REG_ACC:
                    cpu->acc = value;
                    break;
                case REG_LAST:
                    reg = cpu->node.last || REG_NIL;
                case REG_ANY:
                case REG_UP:
                case REG_LEFT:
                case REG_RIGHT:
                case REG_DOWN:
                    return cpu_write(cpu, reg, value);
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
            if (cpu_read(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            cpu->acc += value;
            break;
        case OP_SUB:
            if (cpu_read(cpu, ins->a, &value) == IO_WAIT) {
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
                return IO_NONE;
            }
            break;
        case OP_JNZ:
            if (cpu->acc != 0) {
                cpu->line += ins->jmp_offset;
                return IO_NONE;
            }
            break;
        case OP_JGZ:
            if (cpu->acc > 0) {
                cpu->line += ins->jmp_offset;
                return IO_NONE;
            }
            break;
        case OP_JLZ:
            if (cpu->acc < 0) {
                cpu->line += ins->jmp_offset;
                return IO_NONE;
            }
            break;
        case OP_JRO:
            if (cpu_read(cpu, ins->a, &value) == IO_WAIT) {
                return IO_WAIT;
            }
            cpu->line += value;
            return IO_NONE;
    }
    cpu->acc = clamp_999(cpu->acc);
    cpu_advance(node);
    return IO_NONE;
}

io_status cpu_latch(node_t *node) {
    cpu_state *cpu = (cpu_state *)node;
    if (node->status == IO_LOAD) {
        node->status = node->write(node, node->out_mask, node->output);
    } else if (node->status == IO_DONE) {
        node->status = IO_NONE;
        cpu_advance(node);
    }
    return node->status;
}

void cpu_print(node_t *node) {
    cpu_state *cpu = (cpu_state *)node;
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
