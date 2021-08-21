#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

static char *prog_name = "shift";
static char *prog_version = "1.0";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "shift", required_argument, NULL, 's'},
    { "rot13", no_argument, NULL, 'r'},
    { "decrypt", no_argument, NULL, 'd'},
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

static long shift = 3;

static void print_version();
static void print_help();

static void encrypt(FILE *fp, int decrypt);

int main(int argc, char **argv) {
    invoc_name = argv[0];

    int c, decrypt = 0;
    while ((c = getopt_long(argc, argv, "s:rdhv", options, NULL)) != -1) {
        switch(c) {
            case 's':
                shift = strtol(optarg, NULL, 10);
                if ( shift < 0 || shift > 26 )
                    shift = 3;
                break;
            case 'r':
                shift = 13;
                break;
            case 'd':
                decrypt = 1;
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
        encrypt(stdin, decrypt);
    } else {
        for (int n = optind; n < argc; n++) {
            FILE *fp = fopen(argv[n], "r");
            if ( fp == NULL ) {
                perror("Cannot open file");
                continue;
            }

            encrypt(fp, decrypt);

            fclose(fp);
        }
    }

    return 0;
}

static void encrypt(FILE *fp, int decrypt) {
    int c;
    int first_letter;
    while ( ( c = fgetc(fp) ) != EOF ) {
        if ( isalpha(c) ) {
            first_letter = islower(c)?'a':'A';
            c = (c - first_letter + (decrypt?26-shift:shift)) % 26
                + first_letter;
        }
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
    printf("Usage: %s [OPTION]...\n"
           "\n"
           "Encrypts given input or FILEs using a shift cipher.\n"
           "Defaults to a shift of 3.\n"
           "\n"
           "    -s, --shift NUMBER  set how many letters to shift the plain text by.\n"
           "    -r, --rot13     use a shift of 13 letters.\n"
           "    -d, --decrypt   decrypt cipher text.\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name);
}
