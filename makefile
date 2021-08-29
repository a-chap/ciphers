all: shift xor block playfair
shift: shift.c
	gcc --std=c99 -o shift shift.c -Wall
xor: xor.c
	gcc --std=c99 -o xor xor.c -Wall
block: block.c
	gcc --std=c99 -o block block.c -Wall
playfair: playfair.c
	gcc --std=c99 -o playfair playfair.c -Wall
