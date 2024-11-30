/*
Left to Right Exponentiation Algorithm
GF(2^128), Defining Polynomial = x^128 + x^7 + x^2 + x + 1
0b 10000000 00000000 00000000 00000000 00000000 00000000 00000000 10000111
0x 80 00 00 00 00 00 00 87

128 bits Multiplication & Squaring
128 = 16 * 8 (16 bytes)

Exponent: (2bytes)
    0b 00001010 11101101
    0x 0a ed

Base: 
    Arbitarily (Hard coded)
*/

/*
For GF(2^8)
#define xtime(A) ((A << 1) ^ ((A & 0x80) ? 0x1b : 0)) //0x02
#define x2time(A) (xtime(xtime(A))) //0x04
#define x3time(A) (xtime(x2time(A))) //0x08
#define x4time(A) (xtime(x3time(A))) //0x10
#define x5time(A) (xtime(x4time(A))) //0x20
#define x6time(A) (xtime(x5time(A))) //0x40
#define x7time(A) (xtime(x6time(A))) //0x80

BYTE mul(BYTE a, BYTE b) {
    BYTE res = 0;
    if (b & 0x01) {
        res ^= a;
    }
    if (b & 0x02) {
        res ^= xtime(a);
    }
    if (b & 0x04) {
        res ^= x2time(a);
    }
    if (b & 0x08) {
        res ^= x3time(a);
    }
    if (b & 0x10) {
        res ^= x4time(a);
    }
    if (b & 0x20) {
        res ^= x5time(a);
    }
    if (b & 0x40) {
        res ^= x6time(a);
    }
    if (b & 0x80) {
        res ^= x7time(a);
    }
    return res;
}

BYTE squaring(BYTE a) {
    BYTE h = (a & 0xf0) >> 4;
    BYTE l = a & 0x0f;
    BYTE ah = 0, al = 0;
    BYTE res = 0;
    for (int i = 0; i < 4; i++) {
        ah ^= (h & (1<<i)) << i;
        al ^= (l & (1<<i)) << i;
    }
    BYTE tah = ah ^ xtime(ah) ^ x3time(ah) ^ x4time(ah);
    res = tah ^ al;
    printf("Squaring Result : %02x\n", res);
    return res;
}
*/
#include <stdio.h>
#include <stdint.h>

typedef unsigned char BYTE;

#define byte 8;

BYTE x2(BYTE *a) {
    
}


int main() {
    BYTE base[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}; //16 bytes
    BYTE exp[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xed}; //16 bytes



}