#ifndef TACTIS_NODE_H
#define TACTIS_NODE_H

typedef enum {
    DIR_NONE  = 0,
    DIR_UP    = 1,
    DIR_LEFT  = 2,
    DIR_RIGHT = 4,
    DIR_DOWN  = 8,
    DIR_ANY   = 16,
    DIR_LAST  = 32,
} io_dir;

typedef enum {
    IO_WAIT,
    IO_DONE,
} io_status;

typedef struct _node {
    int8_t type;
    int16_t id;
    io_status (*read)(struct _node *node, io_dir dir);
    io_status (*write)(struct _node *node, io_dir dir);
    io_dir read_mask, write_mask;
} node_t;

#endif
