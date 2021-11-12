#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static char *prog_name = "xor cipher";
static char *prog_version = "1.0";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

static void print_version();
static void print_help();

static void encrypt(FILE *fp, char *keyword);

int main(int argc, char **argv) {
    char *keyword = NULL;
    invoc_name = argv[0];

    int c;
    while ((c = getopt_long(argc, argv, "hv", options, NULL)) != -1) {
        switch(c) {
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
        fprintf(stderr, "%s: Keyword missing.\n"
                        "Try '%s --help' for more information.\n"
                        , invoc_name, invoc_name);
        exit(EXIT_FAILURE);
    } else if ( strlen(argv[optind]) == 0 ) {
        fprintf(stderr, "%s: Keyword cannot be an empty string.\n"
                        "Try '%s --help' for more information.\n"
                        , invoc_name, invoc_name);
        exit(EXIT_FAILURE);
    }

    keyword = argv[optind];

    if ( optind + 1 == argc ) {
        encrypt(stdin, keyword);
    } else {
        for (int i = optind + 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if ( fp == NULL ) {
                fprintf(stderr, "%s: ", argv[i]);
                perror(argv[i]);
                continue;
            }

            encrypt(fp, keyword);

            fclose(fp);
        }
    }

    return 0;
}

static void encrypt(FILE *fp, char *keyword) {
    int c, i = 0;
    while ( ( c = fgetc(fp) ) != EOF ) {
        c ^= keyword[i];

        if ( i < strlen(keyword) )
            i++;
        else
            i = 0;

        printf("%c", c);
    }
}

static void print_version() {
    printf("%s %s\n"
           "\n"
           "Written by %s\n",
           prog_name, prog_version, author);
}

static void print_help() {
    printf("Usage: %s KEYWORD [FILE]...\n"
           "   or: %s [OPTION]\n"
           "\n"
           "Encrypts stdin or FILEs using an xor cipher using\n"
           "the given KEYWORD. To decrypt, re-encrypt using\n"
           "the same keyword.\n"
           "\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name, invoc_name);
}
