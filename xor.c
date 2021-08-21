#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static char *prog_name = "xor cipher";
static char *prog_version = "1.0";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "keyword", required_argument, NULL, 'k'},
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

static char *keyword = NULL;

static void print_version();
static void print_help();

static int valid_keyword(char *keyword);
static void encrypt(FILE *fp);

int main(int argc, char **argv) {
    invoc_name = argv[0];

    int c;
    while ((c = getopt_long(argc, argv, "k:hv", options, NULL)) != -1) {
        switch(c) {
            case 'k':
                if ( keyword != NULL ) {
                    fprintf(stderr, "Only one keyword needed.\n");
                    exit(EXIT_FAILURE);
                } else if ( strlen(optarg) == 0 ) {
                    fprintf(stderr, "Keyword cannot be empty string.\n");
                    exit(EXIT_FAILURE);
                }

                keyword = optarg;
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

    if ( keyword == NULL ) {
        fprintf(stderr, "Keyword is needed.\n");
        exit(EXIT_FAILURE);
    }

    if ( optind == argc ) {
        encrypt(stdin);
    } else {
        for (int n = optind; n < argc; n++) {
            FILE *fp = fopen(argv[n], "r");
            if ( fp == NULL ) {
                perror("Cannot open file");
                continue;
            }

            encrypt(fp);

            fclose(fp);
        }
    }

    return 0;
}

static void encrypt(FILE *fp) {
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
    printf("Usage: %s [OPTION]... FILE...\n"
           "\n"
           "Encrypts stdin or FILEs using an xor cipher.\n"
           "To decrypt, re-encrypt using the same keyword.\n"
           "\n"
           "    -k, --keyword WORD  set the keyword to use a Vigenere cipher.\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name);
}
