#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>


typedef enum {
    SET_RED,
    SET_BLACK
} SetNode_Color;

typedef struct SetNode {
    SetNode_Color color;
    struct SetNode *parent,
                   *left,
                   *right;
} SetNode;

typedef enum {
    SET_OK,
    SET_ERR
} SetStatus;

typedef enum {
    SetNode root;
} Set;


