#include <stdio.h>

typedef unsigned char u8;
typedef unsigned int u32;


/*
Irreducible Polynomial: x^8 + x^4 + x^3 + x + 1 (0x011b in hex)

Multiplication in GF(2^8)
Using xtimes

Example)
0x57 * 0x13
= 0x57 * (0x1 + 0x2 + 0x10)

Implementation   xtime
xtime
    xtime(A) -> (A<<1 ^ ((A & 0x80)? 0x1b : 0))

0x57 * 0x1 = 0x57
0x57 * 0x2 = xtimes(0x57) = 0xae

*/

#define xtime(A) ((A << 1) ^ ((A & 0x80) ? 0x1b : 0)) //0x02
#define x2time(A) (xtime(xtime(A))) //0x04
#define x3time(A) (xtime(x2time(A))) //0x08
#define x4time(A) (xtime(x3time(A))) //0x10
#define x5time(A) (xtime(x4time(A))) //0x20
#define x6time(A) (xtime(x5time(A))) //0x40
#define x7time(A) (xtime(x6time(A))) //0x80


void bin(u8 n) {
    long i;
    printf("0b");
    for (i = 1 << 7; i > 0; i >>= 1) {
        if ((n & i) != 0) {
            printf("1");
        }
        else {
            printf("0");
        }
    }
}

/*
u8 mul(u8 a, u8 b) {
    
    // Multiplication inputs in GF(2^8)
    
    u8 arr[8] = {0, };
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            arr[i] = a;
        }
        else if(i % 2 == 1) {
            u8 tmp = 0;
            u8 k = 1;
            for (int j = i; j >= 1; j >>= k) {
                if (j == i) {
                    tmp = xtime(a);
                }
                else {
                    tmp = xtime(tmp);
                }
                k++;
            }
            arr[i] = tmp;
        }
        else {
            arr[i] = arr[i - 1] ^ a;
        }
    }
    for (int i = 0; i < 8; i++) {
        printf("arr[%d]: 0x%02x, ", i, arr[i]);
    }
    printf("\n");
    u8 res = 0;
    for (int i = 0; i < 8; i++) {
        if (((b >> i) & 0x01) == 1) {
            res ^= arr[i];
        }
    }
    return res;
}
*/

u8 mul(u8 a, u8 b) {
    u8 res = 0;
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

u8 squaring(u8 a) {
    /*
    Squaring input in GF(2^8)
    */
    u8 res = mul(a, a);
    return res;
}

int main(void) {
    u8 a = 0x57;
    u8 b = 0x3;
    u8 c = mul(a, b);
    u8 d = squaring(b);
    printf("Polynomial a:");
    bin(a);
    printf("\n");

    printf("Polynomial b:");
    bin(b);
    printf("\n");

    printf("xtime(0x%02x): 0x%02x\n", a, xtime(a));
    printf("Multiplication(0x%02x, 0x%02x) = 0x%02x\n", a, b, c);
    printf("Result: 0x%02x, ", c);
    bin(c);
    printf("\n");

    printf("xtime(0x%02x): 0x%02x\n", b, xtime(b));
    printf("Squaring(0x%02x) = 0x%02x\n", b, d);
    printf("Result: 0x%02x, ", d);
    bin(d);
    printf("\n");

    return 0;
}