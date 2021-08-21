#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static char *prog_name = "block";
static char *prog_version = "1.0";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "block-size", required_argument, NULL, 'b'},
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

static void print_version();
static void print_help();

int main(int argc, char **argv) {
    invoc_name = argv[0];

    int block_size = 5;
    int c;
    while ((c = getopt_long(argc, argv, "b:hv", options, NULL)) != -1) {
        switch(c) {
            case 'b':
                block_size = strtol(optarg, NULL, 10);
                if ( block_size <= 0 )
                    block_size = 5;
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

    for (int i = 0; (c = getchar() ) != EOF; i++) {
        if ( i && i % block_size == 0 )
            printf(" ");
        printf("%c", c);
    }

    return 0;
}

static void print_version() {
    printf("%s %s\n"
           "\n"
           "Written by %s\n",
           prog_name, prog_version, author);
}

static void print_help() {
    printf("Usage: %s [OPTION]...\n"
           "\n"
           "Groups non-whitespace input characters into blocks.\n"
           "The default block size is 5.\n"
           "\n"
           "    -b, --block-size SIZE   set the size of the blocks.\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name);
}
