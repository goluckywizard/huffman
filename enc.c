#include "huffman.h"
#include <string.h>
#include <stdlib.h>

size_t count_freq(FILE *input, size_t *freq)
{
    size_t input_length = 0;
    unsigned char newpart[500];
    size_t size = 0;
    do
    {
        size = fread(newpart,1,  500, input);
        size_t i = 0;
        while (i < size)
        {
            freq[newpart[i]]++;
            i++;
        }
        input_length += size;
    }
    while (size > 0);
    rewind(input);
    return input_length;
}

Node *make_tree(size_t *freq)
{
    Node *slice[256];
    unsigned int i,
            n = 0;
    slice[0] = NULL;

    for(i = 0; i < 256; i++)
    {
        if(freq[i] != 0)
        {
            Node *leaf = (Node *)malloc(sizeof(Node));
            leaf->link[0] = NULL;
            leaf->link[1] = NULL;
            leaf->freq = freq[i];
            leaf->symbol = i;
            slice[n++] = leaf;
        }
    }
    while (n > 1)
    {
        //int N = (256 - n);
        for (i = 0; i < n; ++i)
        {
            for (unsigned int j = n - 1; j > i; --j) {
                if (slice[j - 1]->freq > slice[j]->freq) {
                    Node *temp = slice[j - 1];
                    slice[j - 1] = slice[j];
                    slice[j] = temp;
                }
            }
        }
        Node *parent = (Node *)malloc(sizeof(Node));
        parent->freq = slice[0]->freq + slice[1] ->freq;
        parent->link[0] = slice[0];
        parent->link[1] = slice[1];
        slice[0] = parent;
        slice[1] = slice[n - 1];
        n--;
    }
    return slice[0];
}

void make_code(Node *root, char *code[256],  char path[256], unsigned int level)
{
    if(NULL != root)
    {
        if(NULL == root->link[0])
        {
            path[level] = 0;
            char *ch_code = (char*)malloc(level + 1);
            //printf("%s ", path);
            memcpy(ch_code, path, level + 1);
            //printf("%s ", ch_code);
            code[root->symbol] = ch_code;
            //printf("%s ", code[root->symbol]);
            //free(ch_code);
        }
        else
        {
            path[level] = '0';
            make_code(root->link[0], code, path, level + 1);
            path[level] = '1';
            make_code(root->link[1], code, path, level + 1);
        }
    }
}

void make_bit (FILE *output, int bit, int *bitpos, unsigned char *byte)
{
    *byte = (*byte << 1) | bit;
    //printf("%d", bit);
    (*bitpos)++;
    if (*bitpos >= 8)
    {
        *bitpos = 0;
        fwrite(byte, 1, 1, output);
        //printf("%c", *byte);
        *byte = 0;
    }
}

void write_Huffman(Node *root, FILE *output, int *bitpos, unsigned char *byte)
{

    if (NULL != root->link[0])
    {
        make_bit(output,1, bitpos, byte);
        //printf("1");
        write_Huffman(root->link[0], output, bitpos, byte);

        write_Huffman(root->link[1], output, bitpos, byte);

    }
    else
    {
        make_bit(output,0, bitpos, byte);
        //printf("0 ");
        unsigned char mask = 128;
        for (int i = 0; i < 8; ++i)
        {
            make_bit(output, (mask & root->symbol) > 0 ? 1 : 0, bitpos, byte);
            //printf("%d ", mask & root->symbol);
            //printf("%d", (mask & root->symbol) > 0 ? 1:0);
            mask /= 2;
        }
        //printf("\n");
    }

}

void encode (char *code[256], FILE *input, FILE *output, int *bitpos, unsigned char *byte)
{
    rewind(input);
    unsigned char *newpart = (unsigned char*)malloc(500 * sizeof(char));
    size_t size = 0;
    do{
        size = fread(newpart, 1, 500, input);
        size_t i = 0;
        while (i < size)
        {
            size_t j = 0;
            //printf("newpart %d ", newpart[i]);
            while (code[newpart[i]][j] != 0)
            {
                if (code[newpart[i]][j] == '0')
                    make_bit(output, 0 , bitpos, byte);
                else
                    make_bit(output, 1 , bitpos, byte);
                j++;
            }
            //printf("e");
            i++;
        }
    }
    while (size > 0);
    free(newpart);
    while (*bitpos != 0)
    {
        make_bit(output, 0, bitpos, byte);
    }
}

void enc(FILE *output, FILE *input)
{
    rewind(input);
    char path[256];
    char *code[256] = {0};
    size_t freq[256] = {0};
    size_t input_length;
    //unsigned int level = 0;
    int bitpos = 0;
    unsigned char byte = 0;
    input_length = count_freq(input, freq);
    //printf("%zu ", input_length);
    Node *root = make_tree(freq);
    memset(code, 0, sizeof(char *) * 256);
    make_code(root, code, path, 0);
    //printf(" %s ", code[255]);
    /*for (int i = 0; i <= 255; i++)
    {
        if (code[i] != NULL)
            printf("%ld %d %c %s\n", freq[i], i, i, code[i]);
    }*/
    //printf("%s", code[97]);
    for (int i = 0; i < 32; ++i) {
        make_bit(output, (input_length & (1u << (31 - i))) > 0 ? 1 : 0, &bitpos, &byte);
    }
    //printf("\nslovar ");
    if (NULL != root)
        write_Huffman(root, output, &bitpos, &byte);
    //printf("\nkod ");
    encode(code, input, output, &bitpos, &byte);
    FreeTree(root);
    for (int i = 0; i <= 255; i++)
    {
        if (code[i] != NULL)
            free(code[i]);
    }
    //while (1)
    //{printf("z");}
}
