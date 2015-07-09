#ifndef TACTIS_GRID_H
#define TACTIS_GRID_H

#include <stdint.h>
#include <stdio.h>
#include "types.h"

grid_t *grid_new(int16_t width, int16_t height, node_t **nodes);
static inline int grid_pos_invalid(grid_t *grid, int x, int y);
void grid_step(grid_t *grid);
void grid_latch(grid_t *grid);
int grid_set(grid_t *grid, int x, int y, node_t *node);
node_t *grid_get(grid_t *grid, int x, int y);
int grid_set_input(grid_t *grid, int x, node_t *node);
node_t *grid_get_input(grid_t *grid, int x);
int grid_set_output(grid_t *grid, int x, node_t *node);
node_t *grid_get_output(grid_t *grid, int x);
int grid_load(grid_t *grid, FILE *save);

#endif
