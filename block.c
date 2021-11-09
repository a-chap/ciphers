#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

static char *prog_name = "block";
static char *prog_version = "1.0";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "block-size", required_argument, NULL, 'b'},
    { "newline", required_argument, NULL, 'n' },
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

static void print_version();
static void print_help();

static void block_file(FILE *fp, int block_size, int nblocks);

int main(int argc, char **argv) {
    int block_size = 5;
    int nblock_line = 0;

    invoc_name = argv[0];

    int c;
    while ((c = getopt_long(argc, argv, "b:n:hv", options, NULL)) != -1) {
        switch(c) {
            case 'b':
                block_size = atoi(optarg);
                if ( block_size <= 0 )
                    block_size = 5;
                break;
            case 'n':
                nblock_line = atoi(optarg);
                if ( nblock_line < 0 )
                    nblock_line = 0;
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                print_version();
                exit(EXIT_SUCCESS);
                break;
            default:
                fprintf(stderr, "Try '%s --help' for more information.\n", invoc_name);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if ( optind == argc ) {
        block_file(stdin, block_size, nblock_line);
    } else {
        for (int n = optind; n < argc; n++) {
            FILE *fp = fopen(argv[n], "r");
            if ( fp == NULL ) {
                perror(invoc_name);
                continue;
            }

            block_file(fp, block_size, nblock_line);

            fclose(fp);
        }
    }

    return 0;
}

static void block_file(FILE *fp, int block_size, int nblocks) {
    static int char_pos = 0;
    static int block_num = 0;
    int c;
    while ( ( c = fgetc(fp) ) != EOF ) {
        if ( !isprint(c) || isspace(c) )
            continue;
        if ( char_pos && char_pos % block_size == 0 ) {
            if ( nblocks )
                block_num++;

            if ( nblocks && block_num == nblocks ) {
                printf("\n");
                block_num = 0;
            } else
                printf(" ");

            char_pos = 0;
        }
        printf("%c", c);
        char_pos++;
    }
}

static void print_version() {
    printf("%s %s\n"
           "\n"
           "Written by %s\n",
           prog_name, prog_version, author);
}

static void print_help() {
    printf("Usage: %s [OPTION]... [FILE]...\n"
           "\n"
           "Groups non-whitespace input characters into blocks.\n"
           "The default block size is 5.\n"
           "Takes input from FILEs, if given, or from stdin.\n"
           "\n"
           "    -b, --block-size SIZE   set the size of the blocks.\n"
           "    -n, --newline N         start a new line after every Nth block.\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name);
}
