# Ciphers
C implementation of various ciphers. I wanted to make small ciphers that
could be used as filters for shell scripting so that piping one through
another etc builds up a more interesting cipher.

Currently, I've included various shift ciphers---Caesar, rot13 and Vigenere
amongst them---Xor Cipher and the Playfair cipher. I've also included a way
to group the output letters into blocks to diguise word length. Exept for
the Xor cipher, they all work on the letters of the English alphabet only
and thus don't encrypt any others.

## Compilation
I've included a very basic makefile so all you need to do is run make.
Alternatively, just compile the source files individually. They should
compile with any standard except for playfair.c which needs c99 minimum.
The source does require getopt_long to compile.

## How the ciphers work
### Shift ciphers
The shift ciphers are all included in shift.c. They include the Caesar
cipher, rot13, arbitrary shift and Vigenere ciphers.

The Caesar cipher shifts the plaintext by three places along the alphabet.
Thus 'a' becomes 'd', 'b' becomes 'e' etc. Rot13 works the same but with
a shift of thirteen letters. Encrypting twice with Rot13 is equivalent to
no encryption at all. The text can also be shifted by an arbitrary number
of letters.

Shift defaults to a Caesar cipher, to switch to rot13 use the -r or --rot13
flags. For an arbitrary shift use the -s or --shift flags followed by the
number of letters you would like to shift by.

Vigenere is slightly more complicated. You provide a keyword which is then
used to determine the shift. For each letter of the keyword, the shift is
how many letters you need to move along from a to get to the letter.
The first letter of the input text is encrypted with the first letter of
the keyword, the second letter of the plaintext with the second letter of
the keyword etc. When the letters of the keyword are exhausted, it restarts
with the first letter of the keyword.

The Vigenere can be used by specifying a keyword using the -k or --keyword
flags followed by a string consisting of letters only. There are two
keywords already built in: 'abcdefghijklmnopqrstuvwxyz' and
'zyxwvutsrqponmlkjihgfedcba' which are specified using the -a or --alphabet flags for the former and -z or --backwards_alphabet flags for the latter.

The keyword or shift can be incremented using the -p or --progress flags.
That is to say the first time the shift is used it is the value you have
given but the next time it will be increased by one. This continues until
it reaches 26 when it will wrap around to zero. This means that
./shift -s 0 -p is equivalent to ./shift -a. This also works with the
keywords so a keyword of 'KEYWORD' is used for the first seven letters
but becomes 'LFZXPSE' for the next seven and so on with the letters
wrapping back round to 'A' when they reach the end of the alphabet.

To decrypt instead of encrypt use the -d or --decrypt flags.

### Playfair Cipher
You provide a keyword to encrypt the plaintext. The keyword is used to generate a 5 by 5 table that the cipher needs. As an example, the keyword 'keyword' produces the following table:

    +-------------------+
    | K | E | Y | W | O |
    +---+---+---+---+---+
    | R | D | A | B | C |
    +---+---+---+---+---+
    | F | G | H | I | L |
    +---+---+---+---+---+
    | M | N | P | Q | S |
    +---+---+---+---+---+
    | T | U | V | X | Z |
    +---+---+---+---+---+

Where the letter I represents both I and J.

The Playfair cipher encrypts the plaintext a pair of letters at a time by the following rules:

1. Go through the plaintext one pair of letters at a time (A, B)
2. If it's a double pair eg ('A', 'A'), insert the letter 'X' between them. The second letter will be the first letter of the next pair. Eg AA -> AX A with the second A the first letter of the next pair. If it's somehow a double 'X' then insert a 'Q' between them instead.
3. If the letters of the pair are in the same row of the above table, shift each letter one to the right and wrap around to the beginning, if needed. Eg KE -> EY and WO -> OK in the above table.
4. If the letters of the pair are in the same column of the above table, shift each letter one down and wrap around, if needed. Eg KR -> RF and MT -> TK in the above table.
5. If the letters in the above table form the corners of a rectangle, swap each with the other corner in its row. Eg DI -> BG in the above table.
6. For a single leftover letter add an 'X' unless the leftover letter is
itself an 'X' in which case add a 'Q'.
