#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <getopt.h>

static char *prog_name = "playfair";
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
static int playfair_grid[5][5];

static void print_version();
static void print_help();

static int valid_keyword(char *keyword);
static void fill_in_playfair_grid(char *keyword);
static void print_playfair_grid();
static void encrypt(FILE *fp);

int main(int argc, char **argv) {
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
    } else if ( !valid_keyword(argv[optind]) ) {
        fprintf(stderr, "%s: Keyword must consist only of letters.\n"
                        "Try '%s --help' for more information.\n"
                        , invoc_name, invoc_name);
        exit(EXIT_FAILURE);
    }

    fill_in_playfair_grid(argv[optind]);

    print_playfair_grid();

    if ( optind + 1 == argc ) {
        encrypt(stdin);
    } else {
        for (int i = optind + 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
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

static int valid_keyword(char *keyword) {
    if ( keyword == NULL || strlen(keyword) == 0 )
        return 0;

    for (int i = 0; i < strlen(keyword); i++)
        if ( !isalpha(keyword[i]) )
            return 0;

    return 1;
}

static inline void fill_in_playfair_grid(char *keyword) {
    /* nth bit will be the nth letter of the alphabet */
    int32_t used_letters = 0;
    int n_spaces_filled = 0;

    for (int i = 0; i < strlen(keyword); i++) {
        int letter = toupper(keyword[i]);

        /* There are only 25 spaces in the Playfair
         * Grid but 26 letters of the alphabet so
         * one of the spaces has to be used twice.
         * Thus I doubles up as J.
         */
        if (letter == 'j')
            letter = 'i';

        /* if letter is already in grid skip */
        if (used_letters & (1 << letter - 'A'))
            continue;
        else
            used_letters |= (1 << letter - 'A');

        playfair_grid[n_spaces_filled / 5][n_spaces_filled % 5] = letter;

        n_spaces_filled++;
    }

    /* Fill in any remaining spaces of the grid. */
    for (int i = 0; i < 26; i++) {
        if ( n_spaces_filled == 25 )
            break;
        if ( i == 'J' - 'A' ) /* Skip J because I has its place */
            continue;
        if ( used_letters & (1 << i) )
            continue;

        playfair_grid[n_spaces_filled / 5][n_spaces_filled % 5] = i + 'A';
        n_spaces_filled++;
    }

    return;
}

static void print_playfair_grid() {
    for (int i = 0; i < 5; i++) {
        printf("+---+---+---+---+---+\n");
        for (int j = 0; j < 5; j++) {
            printf("| %c ", playfair_grid[i][j]);
        }
        printf("|\n");
    }
    printf("+---+---+---+---+---+\n");
}

static void encrypt(FILE *fp) {
    int c, i = 0;
    while ( ( c = fgetc(fp) ) != EOF ) {
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
           "Encrypts stdin or FILEs using the Playfair cipher.\n"
           "\n"
           "Options\n"
           "=======\n"
           "\n"
           "    -h, --help      display this help and exit.\n"
           "    -v, --version   display version information and exit.\n"
           "\n"
           "Playfair Cipher\n"
           "===============\n"
           "The Playfair cipher works by arranging the letters of\n"
           "the English alphabet in a 5x5 grid with I representing\n"
           "both itself and J. The plain text is separated into\n"
           "adjacent pairs and these pairs are then encrypted using\n"
           "the grid.\n"
           "\n"
           "Grid Layout\n"
           "-----------\n"
           "The grid is produced using the KEYWORD, putting the first\n"
           "letter in the keyword in the top-left square of the grid\n"
           "and filling the rest of the squares from left to right and\n"
           "top to bottom with the subsequent letters of the keyword.\n"
           "Letters already in the grid are skipped.\n"
           "For example, if the keyword is `PLAYFAIR' then the letters\n"
           "of the keyword will be laid out as follows:\n"
           "\n"
           "                  +---+---+---+---+---+\n"
           "                  | P | L | A | Y | F |\n"
           "                  +---+---+---+---+---+\n"
           "                  | I | R |   |   |   |\n"
           "                  +---+---+---+---+---+\n"
           "                  |   |   |   |   |   |\n"
           "                  +---+---+---+---+---+\n"
           "                  |   |   |   |   |   |\n"
           "                  +---+---+---+---+---+\n"
           "                  |   |   |   |   |   |\n"
           "                  +---+---+---+---+---+\n"
           "\n"
           "Where the second `A' in PLAYFAIR has been skipped. The rest\n"
           "of the grid will be filled up using the remaining letters of\n"
           "the alphabet ignoring any already in the grid, giving:\n"
           "\n"
           "                  +---+---+---+---+---+\n"
           "                  | P | L | A | Y | F |\n"
           "                  +---+---+---+---+---+\n"
           "                  | I | R | B | C | D |\n"
           "                  +---+---+---+---+---+\n"
           "                  | E | G | H | K | M |\n"
           "                  +---+---+---+---+---+\n"
           "                  | N | O | Q | S | T |\n"
           "                  +---+---+---+---+---+\n"
           "                  | U | V | W | X | Z |\n"
           "                  +---+---+---+---+---+\n"
           "\n"
           "Remember that `I' represents both I and J.\n"
           "\n"
           "Encryption Rules\n"
           "----------------\n"
           "The Playfair cipher encrypts the plaintext one\n"
           "pair of letters at a time by the following rules:\n"
           "\n"
           "1. Go through the plaintext one pair of letters at\n"
           "   a time (A, B)\n"
           "2. If it's a double pair eg ('A', 'A'), insert the\n"
           "   letter 'X' between them. The second letter will be\n"
           "   the first letter of the next pair. Eg AA -> AX A with\n"
           "   the second A the first letter of the next pair.\n"
           "3. If the letters of the pair are in the same row of\n"
           "   the above table, shift each letter one to the right\n"
           "   and wrap around to the beginning, if needed.\n"
           "   Eg KE -> EY and WO -> OK in the above table.\n"
           "4. If the letters of the pair are in the same column\n"
           "   of the above table, shift each letter one down and\n"
           "   wrap around, if needed. Eg KR -> RF and MT -> TK in\n"
           "   the above table.\n"
           "5. If the letters in the above table form the corners\n"
           "   of a rectangle, swap each with the other corner in its\n"
           "   row. Eg DI -> BG in the above table.\n"
           ,invoc_name, invoc_name);
}
