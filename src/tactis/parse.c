#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "util.h"

token_pair op_map[] = {
    {"NOP", OP_NOP},
    {"MOV", OP_MOV},
    {"SWP", OP_SWP},
    {"SAV", OP_SAV},
    {"ADD", OP_ADD},
    {"SUB", OP_SUB},
    {"NEG", OP_NEG},
    {"JMP", OP_JMP},
    {"JEZ", OP_JEZ},
    {"JNZ", OP_JNZ},
    {"JGZ", OP_JGZ},
    {"JLZ", OP_JLZ},
    {"JRO", OP_JRO},
    {0},
};

token_pair reg_map[] = {
    {"NIL", REG_NIL},
    {"ACC", REG_ACC},
    {"ANY", REG_ANY},
    {"LAST", REG_LAST},
    {"UP", REG_UP},
    {"DOWN", REG_DOWN},
    {"LEFT", REG_LEFT},
    {"RIGHT", REG_RIGHT},
    {0},
};

const char *token_name(int token, token_pair *tpos) {
    token_pair *tok, *pos = op_map;
    while ((tok = tpos++)->name) {
        if (tok->token == token)
            return tok->name;
    }
    return NULL;
}

const char *op_name(int token) {
    return token_name(token, op_map);
}

const char *reg_name(int token) {
    if (token > 4096) {
        static char buf[32] = {0};
        snprintf(buf, 32, "%d", token - 5096);
        return buf;
    }
    return token_name(token, reg_map);
}

static void consume_whitespace(char **s) {
    while (**s == ' ') (*s)++;
}

static void consume_separator(char **s) {
    while (**s == ',' || **s == ' ') (*s)++;
}

static void consume_number(char **s) {
    if (**s == '-') (*s)++;
    while (isdigit(**s)) (*s)++;
}

int parse_number(char **s, int *ret) {
    char *ss = *s;
    int n = atoi(ss);
    if (n) {
        consume_number(s);
        *ret = n;
        return 0;
    }
    if ((ss[0] == '-' && ss[1] == '0') || ss[0] == '0') {
        consume_number(s);
        *ret = 0;
        return 0;
    }
    return -1;
}

int parse_token(char **s, token_pair *tpos) {
    token_pair *tok;
    while ((tok = tpos++)->name) {
        if (strstr(*s, tok->name) == *s) {
            *s += strlen(tok->name);
            consume_whitespace(s);
            return tok->token;
        }
    }
    return -1;
}

int parse_reg(char **s, parse_error *error) {
    int token = parse_token(s, reg_map);
    if (token == -1) {
        if (parse_number(s, &token)) {
            asprintf(&error->msg, "expected register");
            return 0;
        }
        // overflow saturates
        if (token > 999) token = 999;
        if (token < -999) token = -999;
        token += 5096;
    }
    consume_separator(s);
    return token;
}

int parse_line(char *s, char **label, ins_t *ins, parse_error *error) {
    memset(ins, 0, sizeof(ins_t));
    error->msg = NULL;
    strupr(s);
    char *pos = s;
    consume_whitespace(&pos);
    if (strlen(pos) == 0) {
        return 0;
    }
    // parse label
    if (!strchr(s, ':')) {
        *label = NULL;
    } else {
        char *ref = pos;
        pos = strchr(s, ':');
        *label = strndup(ref, pos - ref);
        if (strchr(*label, ' ')) {
            free(*label);
            *label = NULL;
            pos = ref;
            goto invalid_opcode;
        }
        pos += 1;
        consume_whitespace(&pos);
    }
    // parse opcode
    ins->op = parse_token(&pos, op_map);
    if (ins->op == -1) {
        goto invalid_opcode;
    }
    // parse operands
    switch (ins->op) {
        case OP_NOP:
        case OP_SAV:
        case OP_SWP:
        case OP_NEG:
        case OP_JRO:
            break;
        case OP_MOV:
            ins->a = parse_reg(&pos, error);
            if (error->msg) goto err;
            ins->b = parse_reg(&pos, error);
            if (ins->b > 4096) {
                asprintf(&error->msg, "target \"%d\" must be register", ins->b - 5096);
            }
            if (error->msg) goto err;
            break;
        case OP_ADD:
        case OP_SUB:
            ins->a = parse_reg(&pos, error);
            break;
        case OP_JMP:
        case OP_JEZ:
        case OP_JNZ:
        case OP_JGZ:
        case OP_JLZ:
        {
            char *ref = pos;
            if (strchr(pos, ' ')) {
                pos = strchr(pos, ' ');
            } else {
                pos += strlen(pos);
            }
            ins->label = strndup(ref, pos - ref);
            break;
        }
    }
    if (strlen(pos) > 0) {
        asprintf(&error->msg, "too many operands");
        error->len = strlen(pos);
    }
    if (error->msg) goto err;
    return 0;
invalid_opcode:
    {
        // null terminate at first space
        char *c = strchr(pos, ' ');
        if (c) *c = '\0';
        asprintf(&error->msg, "invalid opcode \"%s\"", pos);
        error->len = strlen(pos);
    }
err:
    error->col = pos - s;
    if (! error->len && strchr(pos, ' '))
        error->len = strchr(pos, ' ') - pos;
    return -1;
}
