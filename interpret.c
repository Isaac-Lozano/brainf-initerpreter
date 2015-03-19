#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 64

typedef struct Mem
{
    struct mem *prev;
    unsigned int data[MEM_SIZE];
    struct mem *next;
} Memory;

typedef struct
{
    size_t dp;
    Memory *mem;
    Memory *first;
    char *file;
    size_t fp;
    size_t file_len;
    int debug;
} Interpreter;

void brainf_init(Interpreter *i, FILE *_fd)
{
    memset(i, 0, sizeof(Interpreter));
    i->mem = malloc(sizeof(Memory));
    memset(i->mem, 0, sizeof(Memory));

    char readbuf[64];

    while(fgets(readbuf, 64, _fd))
    {
        i->file = realloc(i->file, i->file_len + strlen(readbuf));
        strcpy(i->file + i->file_len, readbuf);
        i->file_len += strlen(readbuf);
    }
}

int brainf_run(Interpreter *i)
{
    while(1)
    {
        if(i->fp == i->file_len)
        {
            return 0;
        }
        int ch = i->file[i->fp];
        if(i->debug)
        {
            printf("ch: %c\n\tfp: %d\n", ch, i->fp);
        }
        switch(ch)
        {
            case '+':
                /* Increment the current memory location */
                /* TODO: do over/underflow checks here. */
                i->mem->data[i->dp]++;
                if(i->mem->data[i->dp] == 0)
                {
                    fprintf(stderr, "Error: incrementing overflow\n");
                    return 1;
                }
                break;
            case '-':
                /* Decrement the current memory location */
                /* TODO: do over/underflow checks here. */
                if(i->mem->data[i->dp] != 0)
                {
                    i->mem->data[i->dp]--;
                }
                else
                {
                    fprintf(stderr, "Error: decrementing underflow\n");
                    return 1;
                }
                break;
            case '<':
                /* Decrement data pointer */
                if(i->dp == 0)
                {
                    if(i->mem->prev == NULL)
                    {
                        Memory *new_mem = malloc(sizeof(Memory));
                        memset(new_mem, 0, sizeof(Memory));
                        i->mem->prev = new_mem;
                        new_mem->next = i->mem;
                        i->mem = new_mem;
                        i->dp = MEM_SIZE - 1;
                    }
                    else
                    {
                        i->mem = i->mem->prev;
                    }
                }
                else
                {
                    i->dp--;
                }
                break;
            case '>':
                /* Decrement data pointer */
                if(i->dp == MEM_SIZE + 1)
                {
                    if(i->mem->next == NULL)
                    {
                        Memory *new_mem = malloc(sizeof(Memory));
                        memset(new_mem, 0, sizeof(Memory));
                        i->mem->next = new_mem;
                        new_mem->prev = i->mem;
                        i->mem = new_mem;
                        i->dp = 0;
                    }
                    else
                    {
                        i->mem = i->mem->next;
                    }
                }
                else
                {
                    i->dp++;
                }
                break;
            case ',':
                /* Read char from input */
                i->mem->data[i->dp] = fgetc(stdin);
                break;
            case '.':
                /* Print content of char into stdout */
                printf("%c", i->mem->data[i->dp]);
                fflush(stdout);
                break;
            case '[':
                if(i->mem->data[i->dp] == 0)
                {
                    int begins = 1;
                    while(i->file[i->fp] != ']' || begins > 0)
                    {
                        i->fp++;
                        if(i->file[i->fp] == '[')
                        {
                            begins++;
                        }
                        else if(i->file[i->fp] == ']')
                        {
                            begins--;
                        }
                        if(i->fp == i->file_len)
                        {
                            fprintf(stderr, "Error: Missing end bracket\n");
                            return 1;
                        }
                    }
                }
                break;
            case ']':
                if(i->mem->data[i->dp] != 0)
                {
                    int begins = 1;
                    while(i->file[i->fp] != '[' || begins > 0)
                    {
                        i->fp--;
                        if(i->fp == -1)
                        {
                            fprintf(stderr, "Error: Missing start bracket\n");
                            return 1;
                        }
                        if(i->file[i->fp] == ']')
                        {
                            begins++;
                        }
                        else if(i->file[i->fp] == '[')
                        {
                            begins--;
                        }
                    }
                }
                break;
        }
        i->fp++;
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("No.\n");
    }
    FILE *file;

    if(strcmp(argv[1], "-") == 0)
    {
        file = stdin;
    }
    else
    {
        file = fopen(argv[1], "r");
    }

    Interpreter *i = malloc(sizeof(Interpreter));
    brainf_init(i, file);

    if(argc == 3 && strcmp(argv[2], "-d") == 0)
    {
        i->debug = 1;
    }

    brainf_run(i);
    printf("\nInterpretation finished.\n");

    return 0;
}
