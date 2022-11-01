#pragma once
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct st_Node Node;
struct st_Node
{
    Node *link[2];
    size_t freq;
    unsigned char symbol;
};
void FreeTree (Node *root);
void dec(FILE *output, FILE *input);
void enc(FILE *output, FILE *input);

#endif // HUFFMAN_H
