#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>

static char *prog_name = "shift";
static char *prog_version = "1.0";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "shift", required_argument, NULL, 's'},
    { "keyword", required_argument, NULL, 'k'},
    { "progress", no_argument, NULL, 'p'},
    { "rot13", no_argument, NULL, 'r'},
    { "decrypt", no_argument, NULL, 'd'},
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

static int shift = 3;
static int *keyword = NULL;
static int keyword_length = 0;
static int progress_keyword = 0;

static void print_version();
static void print_help();

static int valid_keyword(char *keyword);
static void encrypt(FILE *fp, int decrypt);

static inline int is_simple_shift_cipher() {
    return keyword == NULL;
}
static inline int is_vigenere_cipher() {
    return keyword != NULL;
}

int main(int argc, char **argv) {
    invoc_name = argv[0];

    int c, decrypt = 0, tmp_shift;
    while ((c = getopt_long(argc, argv, "s:k:prdhv", options, NULL)) != -1) {
        switch(c) {
            case 's':
                tmp_shift = atoi(optarg);
                if ( tmp_shift >= 0 && tmp_shift <= 26 )
                    shift = tmp_shift;
                break;
            case 'k':
                if ( keyword != NULL ) {
                    fprintf(stderr, "Only one keyword needed.\n");
                    exit(EXIT_FAILURE);
                } else if ( ! valid_keyword(optarg) ) {
                    fprintf(stderr, "Keyword must only be letters.\n");
                    exit(EXIT_FAILURE);
                }

                keyword_length = strlen(optarg);
                keyword = calloc(keyword_length, sizeof(int));

                for (int i = 0; i < keyword_length; i++)
                    keyword[i] = optarg[i] - 'a';

                break;
            case 'p':
                progress_keyword = 1;
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

static int valid_keyword(char *keyword) {
    if ( keyword == NULL || strlen(keyword) == 0 )
        return 0;

    for (int i = 0; i < strlen(keyword); i++)
        if ( !isalpha(keyword[i]) )
            return 0;

    return 1;
}

static void encrypt(FILE *fp, int decrypt) {
    int c, i = 0;
    int first_letter;
    while ( ( c = fgetc(fp) ) != EOF ) {
        if ( isalpha(c) ) {
            if ( is_vigenere_cipher() ) {
                shift = keyword[i];

                if ( progress_keyword ) {
                    keyword[i]++;
                    keyword[i] %= 26;
                }

                i = (i + 1) % keyword_length;
            }

            /*
             * Retain first letter so I know whether
             * the letter should come out as upper
             * or lower case.
             */
            first_letter = islower(c)?'a':'A';
            c = (c - first_letter + (decrypt?26-shift:shift)) % 26
                + first_letter;

            if ( progress_keyword && is_simple_shift_cipher() )
                shift = (shift + 1) % 26;
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
    printf("Usage: %s [OPTION]... FILE...\n"
           "\n"
           "Encrypts given input or FILEs using a shift cipher.\n"
           "Defaults to a shift of 3.\n"
           "\n"
           "    -s, --shift NUMBER  set how many letters to shift the plain text by.\n"
           "    -k, --keyword WORD  set the keyword to use a Vigenere cipher.\n"
           "    -p, --progress  progress the keyword as encryption continues.\n"
           "                    ie keyword becomes lfzxpse for the second set.\n"
           "                    of seven letters and then mgayqtf for the third\n"
           "                    and so on. If using a simple shift, it will be\n"
           "                    incremented after being used for a letter and\n"
           "                    will wrap around to 0 when 26.\n"
           "    -r, --rot13     use a shift of 13 letters.\n"
           "    -d, --decrypt   decrypt cipher text.\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name);
}
