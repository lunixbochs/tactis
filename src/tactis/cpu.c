#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "parse.h"

void ins_print(ins_t *ins) {
    printf("%s", op_name(ins->op));
    if (ins->a) printf(" %s", reg_name(ins->a));
    if (ins->b) printf(", %s", reg_name(ins->b));
    if (ins->label) printf(" %s", ins->label);
    printf("\n");
}
