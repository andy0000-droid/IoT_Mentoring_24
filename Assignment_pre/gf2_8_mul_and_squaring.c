#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

typedef unsigned char BYTE;

// For GF(2^8)

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

int main() {
    BYTE a = 0xaa;
    BYTE b = 0xbb;
    BYTE res = 0x00;
    res = mul(a, b);
    printf("0x%02x", res);
}