
/*
 * gcc -ansi -pedantic -Wall -Wextra -Werror -o brainfuck brainfuck.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#define DATATYPE int
#define DATATYPE_STR "int"

FILE *prog;

char *code = NULL;

DATATYPE *data = NULL;

unsigned int *stack = NULL;

unsigned int data_size = 1024;

unsigned int stack_size = 128;

char fmt = 'u';

char format[9] = "%u";

int trace = 0;

int compile = 0;

int optimize = 0;

unsigned int cp = 0;

unsigned int dp = 0;

unsigned int max_dp = 0;

unsigned int sp = 0;

char commands[] = "+-<>,.[]cioux";

void
read_code()
{
    int allocated, n, c, comment;

    allocated = 1000;
    n = 0;

    code = (char *)malloc(allocated * sizeof(char));
    comment = 0;
    while (EOF != (c = getc(prog)))
    {
        if (c == ';')   /* end of code */
            break;
        else if (c == '#')
            comment = 1;
        else if (c == '\n' || c == '\r')
            comment = 0;

        if (comment || (NULL == strchr(commands, c)))
            continue;

        if (n >= allocated)
        {
            allocated <<= 1;
            code = (char *)realloc(code, allocated * sizeof(char));
        }
        code[n++] = (char)c;
    }

    if (n >= allocated)
    {
        allocated <<= 1;
        code = (char *)realloc(code, allocated * sizeof(char));
    }
    code[n++] = '\0';
}

void
init_data()
{
    data = (DATATYPE *) calloc(data_size, sizeof(DATATYPE));
}

void
init_stack()
{
    stack = (unsigned int *)malloc(stack_size);
}

int
inc_dp()
{
    if (dp < data_size)
    {
        ++dp;
        if (dp > max_dp)
            max_dp = dp;
        return 1;
    }
    else
    {
        fprintf(stderr, "Data overflow\n");
        return 0;
    }
}

void
dec_dp()
{
    if (dp > 0)
        --dp;
    else
        fprintf(stderr, "Data underflow\n");
}

void
push_cp()
{
    if (sp < stack_size)
        stack[sp++] = cp;
    else
        fprintf(stderr, "Stack overflow\n");
}

void
pop_cp()
{
    if (sp > 0)
        cp = stack[--sp];
    else
        fprintf(stderr, "Stack underflow\n");
}

int
skip()
{
    int ob = 0;

    while (code[cp] != '\0')
    {
        switch (code[cp])
        {
          case '[':
              ++ob;
              break;
          case ']':
              --ob;
              if (0 == ob)
                  return 1;
        }
        ++cp;
    }
    return 0;
}

void
input()
{
    switch (fmt)
    {
      case 'i':
          scanf(format, (signed int *)&(data[dp]));
          break;
      case 'u':
          scanf(format, (unsigned int *)&(data[dp]));
          break;
      case 'c':
          scanf(format, (char *)&(data[dp]));
          break;
      case 'o':
          scanf(format, (unsigned int *)&(data[dp]));
          break;
      case 'x':
          scanf(format, (unsigned int *)&(data[dp]));
          break;
    }
}

void
print_data()
{
    unsigned int i;

    for (i = 0; i < dp; i++)
    {
        printf(" ");
        printf(format, data[i]);
    }

    printf(" [");
    printf(format, data[dp]);
    printf("]");

    for (i = dp + 1; i <= max_dp; i++)
    {
        printf(" ");
        printf(format, data[i]);
    }
    printf("\n");
}

void
switch_format()
{
    switch (fmt)
    {
      case 'i':
          sprintf(format, "%%%c", fmt);
          break;
      case 'u':
          sprintf(format, "%%%c", fmt);
          break;
      case 'c':
          sprintf(format, "%%%c", fmt);
          break;
      case 'o':
          sprintf(format, "0%%%c", fmt);
          break;
      case 'x':
          sprintf(format, "0x%%%c", fmt);
          break;
    }
}

void
run_code()
{
    char cmd;

    unsigned int d;

    while (0 != (cmd = code[cp]))
    {
        switch (cmd)
        {
          case '[':
              if (data[dp])
              {
                  push_cp();
                  ++cp;
              }
              else
              {
                  skip();
                  ++cp;
              }
              break;
          case ']':
              pop_cp();
              break;
          case 'Z':
              data[dp] = 0;
              ++cp;
              break;
          case 'I':    /* I23 -> add 22 to the current cell */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              (data[dp]) += (DATATYPE) d;
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case 'D':    /* D23 -> subtract 22 from the current cell */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              (data[dp]) -= (DATATYPE) d;
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case '+':
              ++(data[dp]);
              ++cp;
              break;
          case '-':
              --(data[dp]);
              ++cp;
              break;
          case 'R':    /* R3 -> move right by 3 cells */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              dp += d;
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case 'L':    /* L2 -> move left by 2 cells */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              dp -= d;
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case '>':
              inc_dp();
              ++cp;
              break;
          case '<':
              dec_dp();
              ++cp;
              break;
          case ',':
              input();
              ++cp;
              break;
          case '.':
              if (!trace)
              {
                  printf(format, data[dp]);
                  if (fmt != 'c')
                      printf(" ");
              }
              ++cp;
              break;
          case 'i':
          case 'u':
          case 'c':
          case 'o':
          case 'x':
              fmt = cmd;
              switch_format();
              ++cp;
              break;
          default:
              ++cp;
        }
        if (trace && cmd != ' ')
        {
            fprintf(stderr, "%c:", cmd);
            print_data();
        }
    }
}

void
optimize_code()
{
    char *new_code, *substr;

    unsigned int i, j, k;

    DATATYPE d;

    char tmp[64], cmd;

    new_code = calloc(strlen(code) + 1, sizeof(char));

    i = j = 0;
    while (0 != (cmd = code[i]))
    {
        switch (cmd)
        {
          case '+':
          case '-':    /* compress ++--+---+... */
              d = 0;
              while (0 != (cmd = code[i]))
              {
                  if (cmd == '+')
                      ++d;
                  else if (cmd == '-')
                      --d;
                  else
                      break;
                  ++i;
              }
              if (d == 1)
                  new_code[j++] = '+';
              else if (d == -1)
                  new_code[j++] = '-';
              else if (d != 0)
              {
                  if (d < 0)
                      sprintf(tmp, "D%u", -d);
                  else
                      sprintf(tmp, "I%u", d);
                  k = 0;
                  while (tmp[k])
                      new_code[j++] = tmp[k++];
              }
              break;
          case '>':
          case '<':    /* compress >><<>>>>> */
              d = 0;
              while (0 != (cmd = code[i]))
              {
                  if (cmd == '>')
                      ++d;
                  else if (cmd == '<')
                      --d;
                  else
                      break;
                  ++i;
              }
              if (d == 1)
                  new_code[j++] = '>';
              else if (d == -1)
                  new_code[j++] = '<';
              else if (d != 0)
              {
                  if (d < 0)
                      sprintf(tmp, "L%u", -d);  /* move left by d */
                  else
                      sprintf(tmp, "R%u", d);   /* move right by d */
                  k = 0;
                  while (tmp[k])
                      new_code[j++] = tmp[k++];
              }
              break;
          default:
              new_code[j++] = code[i++];
        }
    }

    /* FIXME valid while we use finite integers */
    substr = new_code;
    while (NULL != (substr = strstr(substr, "[-]")))
    {
        strncpy(substr, "Z  ", 3); /* [-] set current cell to 0 */
    }
    substr = new_code;
    while (NULL != (substr = strstr(substr, "[+]")))
    {
        strncpy(substr, "Z  ", 3); /* [-] set current cell to 0 */
    }

    free(code);
    code = new_code;
}

void
bf2c()
{
    char cmd;

    unsigned int d;

    printf("#include <stdlib.h>\n#include <stdio.h>\n\n");
    printf("%s data[%u];\n", DATATYPE_STR, data_size);
    printf("unsigned int dp = 0;\n\n");
    printf("int main()\n{\n");

    while ('\0' != (cmd = code[cp]))
    {
        switch (cmd)
        {
          case 'Z':
              printf("data[dp] = 0;\n");
              ++cp;
              break;
          case '[':
              printf("while (data[dp])\n{\n");
              ++cp;
              break;
          case ']':
              printf("}\n");
              ++cp;
              break;
          case 'I':    /* I23 -> add 22 to the current cell */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              printf("data[dp] += %u;\n", d);
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case 'D':    /* D23 -> subtract 22 from the current cell */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              printf("data[dp] -= %u;\n", d);
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case '+':
              printf("++(data[dp]);\n");
              ++cp;
              break;
          case '-':
              printf("--(data[dp]);\n");
              ++cp;
              break;
          case 'R':    /* R3 -> move right by 3 cells */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              printf("dp += %u;\n", d);
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case 'L':    /* L2 -> move left by 2 cells */
              ++cp;
              sscanf(&(code[cp]), "%u", &d);
              printf("dp -= %u;\n", d);
              while (code[cp] >= '0' && code[cp] <= '9')
                  ++cp; /* skip digits */
              break;
          case '>':
              printf("++dp;\n");
              ++cp;
              break;
          case '<':
              printf("--dp;\n");
              ++cp;
              break;
          case ',':
              switch (fmt)
              {
                case 'i':
                    printf("scanf(\"%s\", (signed int *)&(data[dp]));\n",
                           format);
                    break;
                case 'u':
                    printf("scanf(\"%s\", (unsigned int *)&(data[dp]));\n",
                           format);
                    break;
                case 'c':
                    printf("scanf(\"%s\", (char *)&(data[dp]));\n", format);
                    break;
                case 'o':
                    printf("scanf(\"%s\", (unsigned int *)&(data[dp]));\n",
                           format);
                    break;
                case 'x':
                    printf("scanf(\"%s\", (unsigned int *)&(data[dp]));\n",
                           format);
                    break;
              }
              ++cp;
              break;
          case '.':
              if (fmt == 'c')
                  printf("printf(\"%s\", data[dp]);\n", format);
              else
                  printf("printf(\"%s \", data[dp]);\n", format);
              ++cp;
              break;
          case 'i':
          case 'u':
          case 'c':
          case 'o':
          case 'x':
              fmt = cmd;
              switch_format();
              ++cp;
              break;
          default:
              ++cp;
        }
    }
    printf("exit(EXIT_SUCCESS);\n}\n");
}

void
free_all()
{
    if (code != NULL)
        free(code);
    if (data != NULL)
        free(data);
    if (stack != NULL)
        free(stack);
}

void
usage(char *self)
{
    printf("%s: Brainfuck programming language interpreter\n", self);
    printf("See <http://en.wikipedia.org/wiki/Brainfuck> for more details\n\n");

    printf("Usage: %s [options] [file]\n\n", self);

    printf("Size of each data cell is %u byte(s)\n", sizeof(DATATYPE));
    printf("All data cells are zeros initially\n\n");
    printf("Options (defaults are in brackets):\n");
    printf("   -s num         stack size (%u)\n", stack_size);
    printf("   -d num         data size (%u)\n", data_size);
    printf("   -t             trace execution for debugging\n");
    printf("   -O             optimize code\n");
    printf("   -C             translate into C (to stdout)\n");
    printf("\n");
    printf("Formats for operators '.' and ',' (output and input):\n");
    printf
        ("   -c, -i, -u, -o, -x  char, signed int, unsigned int, octal, hexadecimal\n");
    printf
        ("                       octal number must be prepended by '0' (zero),\n");
    printf("                       and hexadecimal - by '0x'\n");
    printf("Default i/o format -%c\n", fmt);
    printf("\n");
    printf("   -h             this help message\n");
    printf("\n");
    printf("  file            file to execute,\n");
    printf("                  if omitted read stdin\n");
    printf("\n");
    printf("Standard operators: <>+-[].,\n");
    printf("Extensions:\n");
    printf
        ("            ciuox - change i/o format (same as -c & others above)\n");
    printf("            ;     - end of code (useful when reading stdin)\n");
    printf
        ("            #     - comment to the end of line (useful when reading files)\n");
    printf("\n");
    printf("Examples:\n");
    printf(" echo '+++[.-]' | %s # count down from 3 to 1\n", self);
    printf(" echo ',+++.;5' | %s # shows 8\n", self);
    printf(" echo ',>,<[->+<]>.;4 5' | %s # shows 4+5=9\n", self);
    printf(" echo 'c,u.;h' | %s # shows 104 (ASCII code for 'h')\n", self);
    printf(" echo ', [-[->+<]>];4' | %s -t # move data pointer by 4 \n", self);
    printf
        (" echo ',>,< [> [->+<] < -[->+<]> ]; 3 7' | %s -t # move '7' by 3 \n\n",
         self);

    exit(EXIT_SUCCESS);
}

int
main(int argc, char **argv)
{
    char *filename;

    char *self = argv[0];

    int opt;

    while (-1 != (opt = getopt(argc, argv, "OCts:d:h?iucox")))
    {
        switch (opt)
        {
          case 's':
              sscanf(optarg, "%u", &stack_size);
              break;
          case 'd':
              sscanf(optarg, "%u", &data_size);
              break;
          case 't':
              trace = 1;
              break;
          case 'C':
              compile = 1;
              break;
          case 'O':
              optimize = 1;
              break;
          case 'i':
          case 'u':
          case 'c':
          case 'o':
          case 'x':
              fmt = opt;
              switch_format();
              break;
          default:
              usage(self);
        }
    }

    if (optind < argc)
    {
        filename = argv[optind];
        if (NULL == (prog = fopen(filename, "r")))
        {
            fprintf(stderr, "%s: open '%s': %s\n", self, filename,
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else
        prog = stdin;

    read_code();

    if (prog != stdin)
        fclose(prog);

    if (optimize)
        optimize_code();

    if (compile)
    {
        bf2c();
    }
    else
    {
        init_data();
        init_stack();
        run_code();
    }

    free_all();
    return EXIT_SUCCESS;
}
