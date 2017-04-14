#include <stdlib.h>
#include <string.h>

#include "grid.h"
#include "node.h"

grid_t *grid_new(int16_t width, int16_t height, node_t **nodes) {
    grid_t *grid = malloc(sizeof(grid_t));
    grid->width = width;
    grid->height = height;
    size_t size = sizeof(node_t *) * width * height;
    grid->nodes = calloc(1, size);
    grid->input = calloc(1, sizeof(node_t *) * width);
    grid->output = calloc(1, sizeof(node_t *) * width);
    if (nodes != NULL) {
        memcpy(grid->nodes, nodes, size);
    }
    return grid;
}

static inline int grid_pos_invalid(grid_t *grid, int x, int y) {
    if (x < 0 || x >= grid->width || y < 0 || y >= grid->height) {
        return 1;
    }
    return 0;
}

void grid_step(grid_t *grid) {
    for (int x = 0; x < grid->width; x++) {
        for (int y = -1; y <= grid->height; y++) {
            node_step(grid_get(grid, x, y));
        }
    }
}

void grid_latch(grid_t *grid) {
    for (int x = 0; x < grid->width; x++) {
        for (int y = -1; y <= grid->height; y++) {
            node_latch(grid_get(grid, x, y));
        }
    }
}

int grid_set(grid_t *grid, int x, int y, node_t *node) {
    if (grid_pos_invalid(grid, x, y))
        return -1;
    node_move(node, x, y);
    node->grid = (void *)grid;
    grid->nodes[x + y * grid->width] = node;
    return 0;
}

node_t *grid_get(grid_t *grid, int x, int y) {
    if (y == -1) {
        return grid_get_input(grid, x);
    } else if (y == grid->height) {
        return grid_get_output(grid, x);
    } else if (grid_pos_invalid(grid, x, y)) {
        return NULL;
    }
    return grid->nodes[x + y * grid->width];
}

int grid_set_input(grid_t *grid, int x, node_t *node) {
    if (grid_pos_invalid(grid, 0, x))
        return -1;
    node_move(node, x, -1);
    node->grid = (void *)grid;
    grid->input[x] = node;
    return 0;
}

node_t *grid_get_input(grid_t *grid, int x) {
    if (grid_pos_invalid(grid, 0, x))
        return NULL;
    return grid->input[x];
}

int grid_set_output(grid_t *grid, int x, node_t *node) {
    if (grid_pos_invalid(grid, 0, x))
        return -1;
    node_move(node, x, grid->height);
    node->grid = (void *)grid;
    grid->output[x] = node;
    return 0;
}

node_t *grid_get_output(grid_t *grid, int x) {
    if (grid_pos_invalid(grid, 0, x))
        return NULL;
    return grid->output[x];
}

int grid_load(grid_t *grid, FILE *save) {
    int loaded = 0;
    int avail = (CPU_WIDTH + 1) * NODE_HEIGHT;
    char code[avail];
    code[0] = '\0';
    char line[CPU_WIDTH + 1];
    int remain = avail;
    while (1) {
        fgets(line, CPU_WIDTH + 1, save);
        if (feof(save) || ferror(save)) {
            // parse one last code block?
            return loaded;
        }
        if (strlen(line) == 1)
            continue;
        if (line[0] != '@') {
            remain = avail - strlen(line);
            strncat(code, line, remain - 1);
        } else {
            if (strlen(code) > 0) {
                int num = atoi(line + 1);
                printf("got node: %d\n", num);
                printf("code: %s\n", code);
                code[0] = '\0';
            }
        }
    }
}
