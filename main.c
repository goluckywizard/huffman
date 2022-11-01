#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
void FreeTree (Node *root)
{
    if (NULL != root)
    {
        FreeTree(root->link[0]);
        FreeTree(root->link[1]);
        free(root);
    }
}

int main(int argc, char *argv[])
{
    int decode = -1;
    FILE *input,
            *output;
    //printf("%s\n", argv[1]);
    if (argc > 3)
    {
        if (strncmp("c", argv[1], 1) == 0)
        {
            decode = 0;
        }
        else if (strncmp("d", argv[1], 1) == 0)
        {
            decode = 1;
        }
        else
        {
            printf("c|d output input\n");
            return 1;
        }

        output = fopen(argv[2], "wb");

        if (NULL == output) {
            printf("Cannot open file '%s'\n", argv[2]);
            return 1;
        }

        input = fopen(argv[3], "rb");

        if (NULL == input) {
            printf("Cannot open file '%s'\n", argv[3]);
            fclose(output);
            return 1;
        }
    }
    else
    {
        input = stdin;
        output = stdout;
        switch (fgetc(stdin)) {
            case 'c':
                decode = 0;
                fgetc(stdin);
                break;

            case 'd':
                decode = 1;
                fgetc(stdin);
                break;

            default:
                printf("Bad input\n");
                return 1;
        }
        fgetc(stdin);
        FILE *stdinput = fopen("input1.txt", "wb");
        size_t size1 = 0;
        char Buffer[1000];
        do
        {
            size1 = fread(Buffer,1,  1000, stdin);
            fwrite(Buffer, 1, size1, stdinput);
        }
        while (size1 > 0);
        fclose(stdinput);
        stdinput = fopen("input1.txt", "rb");
        input = stdinput;
    }

    if (decode == 1)
        dec(output, input);
    if (decode == 0)
        enc(output, input);

    if (input != stdin)
        fclose(input);
    else
        fclose(input);
    if (output != stdout)
        fclose(output);

    return 0;
}
