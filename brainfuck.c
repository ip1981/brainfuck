/*
 * gcc -ansi -pedantic -Wall -Wextra -Werror -o brainfuck brainfuck.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#define DATATYPE int

FILE * prog;

char *   code = NULL;
DATATYPE *   data = NULL;
unsigned int  *  stack = NULL;

unsigned int data_size  = 30000;
unsigned int stack_size = 128;

char format = 'u';

unsigned int cp = 0;
unsigned int dp = 0;
unsigned int sp = 0;

void read_code()
{
    int allocated, n, c;

    allocated = 1000;
    n = 0;

    code = (char*) malloc(allocated * sizeof(char));
    while (EOF != (c = getc(prog)))
    {
        if (n >= allocated)
        {
            allocated <<= 1;
            code = (char*) realloc(code, allocated * sizeof(char));
        }
        code[n++] = (char) c;
    }
    
    if (n >= allocated)
    {
        allocated <<= 1;
        code = (char*) realloc(code, allocated * sizeof(char));
    }
    code[n++] = '\0';
}

void init_data()
{
    data = (DATATYPE*) calloc(data_size, sizeof(DATATYPE));
}

void init_stack()
{
    stack = (unsigned int*) malloc(stack_size);
}


int inc_dp()
{
    if (dp < data_size)
    {
        ++dp;
        return 1;
    }
    else
    {
        fprintf(stderr, "Data overflow\n");
        return 0;
    }
}

void dec_dp()
{
    if (dp > 0)
        --dp;
    else
        fprintf(stderr, "Data underflow\n");
}


void push_cp()
{
    if (sp < stack_size)
        stack[sp++] = cp;
    else
        fprintf(stderr, "Stack overflow\n");
}

void pop_cp()
{
    if (sp > 0)
        cp = stack[--sp];
    else
        fprintf(stderr, "Stack underflow\n");
}

int skip()
{
    while (code[cp] != '\0')
    {
        if (code[++cp] == ']')
            return 1;
    }
    return 0;
}


void print()
{
    switch (format)
    {
        case 'i': printf("%i\n",   data[dp]); break;
        case 'u': printf("%u\n",   data[dp]); break;
        case 'c': printf("%c",     data[dp]); break;
        case 'o': printf("0%o\n",  data[dp]); break;
        case 'x': printf("0x%X\n", data[dp]); break;
    }
}

void run_code()
{
    char cmd;
    while ('\0' != (cmd = code[cp]))
    {
        /*fprintf(stderr, "%c", cmd);*/
        switch (cmd)
        {
            case '[': if (data[dp])
                      {
                          push_cp(); ++cp;
                      }
                      else
                      {
                          skip(); ++cp;
                      }
                      break;
            case ']': pop_cp(); break;
            case '+': ++(data[dp]); ++cp; break;
            case '-': --(data[dp]); ++cp; break;
            case '>': inc_dp();     ++cp; break;
            case '<': dec_dp();     ++cp; break;
            case '.': print();      ++cp; break;

            case 'i':
            case 'u':
            case 'c':
            case 'o':
            case 'x':
                      format = cmd; ++cp; break;
            default: ++cp;
        }
    }
}

void free_all()
{
    if (code  != NULL) free(code);
    if (data  != NULL) free(data);
    if (stack != NULL) free(stack);
}

void usage(char * self)
{
    printf("%s: Brainfuck programming language interpreter\n", self);
    printf("See http://en.wikipedia.org/wiki/Brainfuck for more details\n\n");

    printf("Usage: %s [options] [file]\n\n", self);

    printf("Size of each data cell is %u byte(s)\n\n", sizeof(DATATYPE));
    printf("Options (defaults are in brackets):\n");
    printf("   -s num         stack size (%u)\n", stack_size);
    printf("   -d num         data size (%u)\n", data_size);
    printf("\n");
    printf("Output formats for operator '.':\n");
    printf("   -c, -i, -u, -o, -x  char, signed int, unsigned int, octal, hexadecimal\n");
    printf("\n");
    printf("   -h             this help message\n");
    printf("\n");
    printf("  file            file to execute,\n");
    printf("                  if omitted read stdin\n");
    printf("\n");
    printf("Standard operators: <>+-[].,\n");
    printf("Extensions: ciuox - change format output (same as -c & others, see above)\n");
    printf("\n");
    printf("Example:\n");
    printf(" echo '+++[.-]' | %s\n", self);

    exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv)
{
    char * filename;
    char * self = argv[0];
    int opt;

    while (-1 != (opt = getopt(argc, argv, "s:d:h?iucox")))
    {
       switch (opt)
       {
           case 's':
               sscanf(optarg, "%u", &stack_size);
               break;
           case 'd':
               sscanf(optarg, "%u", &data_size);
               break;
           case 'i':
           case 'u':
           case 'c':
           case 'o':
           case 'x':
               format = opt;
               break;
           default: usage(self);
       }
    }

    if (optind < argc)
    {
        filename = argv[optind];
        if (NULL == (prog = fopen(filename, "r")))
        {
            fprintf(stderr, "%s: open '%s': %s\n", self, filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else
        prog = stdin;

    read_code();

    if (prog != stdin)
        fclose(prog);

    init_data();
    init_stack();

    run_code();

    free_all();
    return 0;
}
