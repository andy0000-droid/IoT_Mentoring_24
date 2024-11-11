#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;
/*
#define xtime(A) ((A << 1) ^ ((A & 0x80) ? 0x1b : 0)) //0x02
#define x2time(A) (xtime(xtime(A))) //0x04
#define x3time(A) (xtime(x2time(A))) //0x08
#define x4time(A) (xtime(x3time(A))) //0x10
#define x5time(A) (xtime(x4time(A))) //0x20
#define x6time(A) (xtime(x5time(A))) //0x40
#define x7time(A) (xtime(x6time(A))) //0x80

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
*/

/*
u8 squaring(u8 a) {
    u8 h = (a & 0xf0) >> 4;
    u8 l = a & 0x0f;
    u8 ah = 0, al = 0;
    u8 res = 0;
    for (int i = 0; i < 4; i++) {
        ah ^= (h & (1<<i)) << i;
        al ^= (l & (1<<i)) << i;
    }
    u8 tah = ah ^ xtime(ah) ^ x3time(ah) ^ x4time(ah);
    res = tah ^ al;
    printf("Squaring Result : %02x\n", res);
    return res;
}
*/

/*
Subfeild Reduction GF((2^4)^2)
*/

#define xtime(A) (((A << 1) ^ ((A & 0x08) ? 0x03 : 0)) & 0x0f) //0x02
#define x2time(A) (xtime(xtime(A))) //0x04
#define x3time(A) (xtime(x2time(A))) //0x08

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
    return res;
}

u8 squaring(u8 a) {
    u8 h = (a & 0x0c) >> 2;
    u8 l = (a & 0x03);
    u8 ah = 0, al = 0;
    u8 res = 0;
    for(int i = 0; i < 2; i++) {
        ah ^= (h & (1<<i)) << i;
        al ^= (l & (1<<i)) << i;
    }
    res = xtime(ah) ^ ah ^ al;
    return res;
}

/*
Direct Method
*/
u8 invmul_1(u8 a) {
    printf("a: 0x%02x\n", a);
    u8 b = (a & 0xf0) >> 4;
    u8 c = a & 0x0f;
    printf("b: 0x%02x, c: 0x%02x\n", b, c);
    const u8 B = 0x01;

    u8 b2 = squaring(b);
    printf("b2: 0x%02x\n", b2);
    u8 b2B = mul(b2, B);
    printf("b2B: 0x%02x\n", b2B);
    u8 c2 = squaring(c);
    printf("c2: 0x%02x\n", c2);
    u8 bmc = mul(b, c);
    printf("bmc: 0x%02x\n", bmc);
    u8 bpc = b ^ c;
    printf("bpc: 0x%02x\n", bpc);
    u8 d = b2B ^ bmc ^ c2;
    printf("d: 0x%02x\n", d);
    u8 invd = 0;
    for(int i = 1; i < 16; i++) {
        if (mul(d, i) == 1) {
            invd = i;
            break;
        }
    }
    printf("invd: 0x%02x\n", invd);
    u8 q = mul(invd, bpc);
    u8 p = mul(invd, b);
    printf("p: 0x%02x, q: 0x%02x\n", p, q);
    return ((p << 4) ^ q);
}

int main() {
    u8 a = 0xab;
    u8 res = invmul_1(a);
    printf("(0x%02x)^-1: 0x%02x\n", a, res);
    printf("0x%02x * 0x%02x: 0x%02x\n", a, res, mul(a, res));
}