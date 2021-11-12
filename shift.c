#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

static char *prog_name = "shift";
static char *prog_version = "1.1";
static char *invoc_name = NULL;
static char *author = "a-chap";

static struct option options[] = {
    { "multiplier", required_argument, NULL, 'm'},
    { "shift", required_argument, NULL, 's'},
    { "keyword", required_argument, NULL, 'k'},
    { "alphabet", no_argument, NULL, 'a'},
    { "backwards-alphabet", no_argument, NULL, 'z'},
    { "progress", no_argument, NULL, 'p'},
    { "atbash", no_argument, NULL, 'b'},
    { "caesar", no_argument, NULL, 'c'},
    { "rot13", no_argument, NULL, 'r'},
    { "decrypt", no_argument, NULL, 'd'},
    { "help", no_argument, NULL, 'h'},
    { "version", no_argument, NULL, 'v'},
    { NULL, 0, NULL, 0 }
};

enum { NONE = 0, DECRYPT = 1, PROGRESS = 2, };

static void print_version();
static void print_help();

static int *set_shift(int shift);
static int valid_keyword(char *keyword);
static void encrypt(FILE *fp, int *keyword, int multiplier, int options);

int main(int argc, char **argv) {
    invoc_name = argv[0];
    int multiplier = 1;
    int *keyword = NULL;
    int cipher_options = NONE;
    int c;
    while ((c = getopt_long(argc, argv, "m:s:k:azpbcrdhv", options, NULL)) != -1) {
        switch(c) {
            case 'm':
                multiplier = atoi(optarg);
                if ( multiplier <= 2  || multiplier >= 26
                  || multiplier == 13 || (multiplier & 1) == 0 ) {
                    fprintf(stderr, "%s: Multiplier needs to be an odd number "
                                    "between 3 and 25 inclusive (except 13).\n",
                                    invoc_name);
                    fprintf(stderr, "Try '%s --help' for more information.\n",
                                    invoc_name);
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                if ( keyword != NULL ) {
                    fprintf(stderr, "%s: Only one keyword or shift needed.\n",
                                    invoc_name);
                    fprintf(stderr, "Try '%s --help' for more information.\n",
                                    invoc_name);
                    exit(EXIT_FAILURE);
                }
                keyword = set_shift(atoi(optarg));
                if ( keyword[0] <= 0 || keyword[0] >= 26 ) {
                    fprintf(stderr, "%s: The shift needs to be a number between "
                                    "1 and 25 inclusive.\n", invoc_name);
                    fprintf(stderr, "Try '%s --help' for more information.\n",
                                    invoc_name);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'a': case 'z':
                if ( keyword != NULL ) {
                    fprintf(stderr, "%s: Only one keyword needed.\n", invoc_name);
                    fprintf(stderr, "Try '%s --help' for more information.\n",
                                    invoc_name);
                    exit(EXIT_FAILURE);
                }
                keyword = calloc(27, sizeof(int));
                if ( keyword == NULL ) {
                    perror(invoc_name);
                    exit(EXIT_FAILURE);
                }
                for (int i = 0; i < 26; i++) {
                    keyword[i] = (c == 'a')? i : 25 - i;
                }
                keyword[26] = -1;
                break;
            case 'k':
                if ( keyword != NULL ) {
                    fprintf(stderr, "%s: Only one keyword needed.\n", invoc_name);
                    fprintf(stderr, "Try '%s --help' for more information.\n",
                                    invoc_name);
                    exit(EXIT_FAILURE);
                }
                if ( ! valid_keyword(optarg) ) {
                    fprintf(stderr, "%s: Keyword must only contain letters.\n",
                                    invoc_name);
                    fprintf(stderr, "Try '%s --help' for more information.\n",
                                    invoc_name);
                    exit(EXIT_FAILURE);
                }

                int length = strlen(optarg);
                keyword = calloc(length + 1, sizeof(int));

                keyword[length] = -1;
                for (int i = 0; i < length; i++)
                    keyword[i] = tolower(optarg[i]) - 'a';

                break;
            case 'p':
                cipher_options |= PROGRESS;
                break;
            case 'c':
                keyword = set_shift(3);
                break;
            case 'r':
                keyword = set_shift(13);
                break;
            case 'b':
                multiplier = 25;
                keyword = set_shift(25);
                break;
            case 'd':
                cipher_options |= DECRYPT;
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
                fprintf(stderr, "Try '%s --help' for more information.\n",
                                invoc_name);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if ( keyword == NULL )
        keyword = set_shift(0);

    if ( optind == argc ) {
        encrypt(stdin, keyword, multiplier, cipher_options);
    } else {
        for (int i = optind; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if ( fp == NULL ) {
                fprintf(stderr, "%s: ", invoc_name);
                perror(argv[i]);
                continue;
            }

            encrypt(fp, keyword, multiplier, cipher_options);

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

static int *set_shift(int shift) {
    int *keyword = calloc(2,sizeof(int));
    if ( keyword == NULL ) {
        perror(invoc_name);
        exit(EXIT_FAILURE);
    }
    keyword[0] = shift;
    keyword[1] = -1;
    return keyword;
}

static void encrypt(FILE *fp, int *keyword, int multiplier, int options) {
    static int i = 0;
    int decrypt = options & DECRYPT;
    int progress_keyword = options & PROGRESS;
    int first_letter;
    int c;

    if ( decrypt && multiplier != 1) {
        /* Can't divide with modular arithmetic,
         * the inverse of multiplication is multiplication
         * a a^{-1} = 1 (mod 26)
         * When solved gives the inverse a^{-1} of multiplication
         * by a.
         */
        int inverse_multipliers[26] = {
             [3]  =  9, [5]  = 21, [7]  = 15, [9]  =  3,
             [11] = 19, [15] =  7, [17] = 23, [19] = 11,
             [21] =  5, [23] = 17, [25] = 25,
        };
        multiplier = inverse_multipliers[multiplier];
    }

    while ( ( c = fgetc(fp) ) != EOF ) {
        if ( isalpha(c) ) {
            /* retain this so can restore case after encryption */
            first_letter = islower(c)? 'a' : 'A';
            c -= first_letter;

            if ( decrypt ) {
                c += 26 - keyword[i];
                c *= multiplier;
            } else {
                c *= multiplier;
                c += keyword[i];
            }

            c %= 26;
            c += first_letter; /* revert to character */

            if ( progress_keyword ) {
                keyword[i]++;
                keyword[i] %= 26;
            }

            i++;
            if ( keyword[i] == -1 )
                i = 0;
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
    printf("Usage: %s [OPTION]... [FILE]...\n"
           "\n"
           "Encrypts given input or FILEs using a shift cipher.\n"
           "By default, won't encrypt input at all.\n"
           "\n"
           "    -s, --shift NUMBER  set how many letters to shift the plain text by.\n"
           "    -m, --multiplier NUMBER  multiply the value of each letter by NUMBER.\n"
           "                             The letters have the values a = 0, b = 1, ... z = 25.\n"
           "    -k, --keyword WORD  set the keyword to use a Vigenere cipher.\n"
           "    -a, --alphabet  use the alphabet as the keyword for the Vigenere cipher.\n"
           "    -z, --backwards-alphabet  use the alphabet but backwards as the keyword for the Vigenere cipher.\n"
           "    -p, --progress  progress the keyword as encryption continues.\n"
           "                    ie keyword becomes lfzxpse for the second set.\n"
           "                    of seven letters and then mgayqtf for the third\n"
           "                    and so on. If using a simple shift, it will be\n"
           "                    incremented after being used for a letter and\n"
           "                    will wrap around to 0 when 26.\n"
           "    -c, --caesar    use a shift of 3 letters.\n"
           "    -r, --rot13     use a shift of 13 letters.\n"
           "    -b, --atbash    use atbash cipher -- swap a with z, b with y etc.\n"
           "    -d, --decrypt   decrypt cipher text.\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n",
           invoc_name);
}
