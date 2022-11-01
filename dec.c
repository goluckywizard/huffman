#include "huffman.h"
#define BUF_SIZE 1000

static unsigned int read_1_bit(FILE *input, unsigned int *bitpos, size_t *inpos, unsigned char *in)
{
    if (*bitpos > 7) {
        //printf("%d ", in[*inpos]);
        *bitpos = 0;
        (*inpos)++;
        if (*inpos >= BUF_SIZE)
        {
            size_t q = fread(in, 1, BUF_SIZE, input);
            if (q != 0)
                *inpos = 0;
        }
        //printf("%d ", *inpos);
    }
    //printf ("%d", (in[*inpos] & (1 << (7 - (*bitpos)))) > 0 ? 1 : 0);
    return (in[*inpos] & (1 << (7 - (*bitpos)++))) > 0 ? 1 : 0;
}

Node *recover_tree (FILE *input, unsigned int *bitpos, size_t *inpos, unsigned char *in)
{
    Node *root = malloc(sizeof(Node));
    if (read_1_bit(input, bitpos, inpos, in) == 1)
    {
        root->link[0] = recover_tree(input, bitpos, inpos, in);
        root->link[1] = recover_tree(input, bitpos, inpos, in);
    }
    else
    {
        unsigned int temp = 0;
        for (int i = 7; i >= 0; --i)
        {
            temp |= read_1_bit(input, bitpos, inpos, in) << i;
        }
        root->symbol = temp;
        root->link[0] = NULL;
        root->link[1] = NULL;

    }
    return root;
}

void decode (FILE *output, FILE *input, Node *root, size_t total_length, unsigned int *bitpos, size_t *inpos, unsigned char *in)
{
    unsigned char out[BUF_SIZE];
    size_t outpos = 0;
    size_t current_length = 0;

    while (current_length < total_length) {
        Node *cur = root;
        while (NULL != cur->link[0] || NULL != cur->link[1]) {
            if (read_1_bit(input, bitpos, inpos, in) == 1)
                cur = cur->link[1];
            else
                cur = cur->link[0];
        }
        if (outpos >= BUF_SIZE)
        {
            fwrite(out, 1, outpos, output);
            //printf("%s", out);
            outpos = 0;
        }
        out[outpos++] = cur->symbol;
        //printf("%d ", out[outpos - 1]);
        current_length++;
    }

    fwrite(out, 1, outpos, output);
}
/*void check(Node *root, unsigned int level)
{
    if(NULL != root)
    {
        if(NULL == root->link[0] || NULL == root->link[1])
        {
            printf(" %d ", root->symbol);
        }
        else
        {
            //printf("0");
            check(root->link[0], level + 1);
            //printf("1");
            check(root->link[1],  level + 1);
        }
    }
}*/
void dec(FILE *output, FILE *input)
{
    size_t total_length = 0;
    Node *root = NULL;
    unsigned char in[BUF_SIZE];
    size_t inpos = BUF_SIZE;
    unsigned int bitpos = 8;
    for (int i = 0; i < 32; ++i) {
        total_length = total_length | ((read_1_bit(input, &bitpos, &inpos, in)) << (31 - i));
    }
    //printf(" %Iu ", total_length);
    if (total_length != 0)
        root = recover_tree(input, &bitpos, &inpos, in);
    decode(output, input, root, total_length, &bitpos, &inpos, in);
    //printf("\n");
    //check(root, 0);

    FreeTree(root);
}
