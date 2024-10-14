#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define x(A) ((A << 1) ^ ((A & 0x80) ? 0x1b : 0))
#define x2(A) x(x(A))
#define x3(A) x(x2(A))
#define x4(A) x(x3(A))

int main() {
    u8 a = 0x3b;
    u8 t1 = 0, t2 = 0;
    t1 = (a & 0xf0) >> 4;
    t2 = (a & 0x0f);
    printf("t1: 0x%02x, t2: 0x%02x\n", t1, t2);
    u8 a_h = 0, a_l = 0;
    int j = 0;
    for (int i = 1; i < (1<<4); i <<= 1) {
        a_h ^= ((t1 & i) << (j));
        a_l ^= ((t2 & i) << (j));
        j++;
    }
    printf("a_H: 0x%02x, a_L: 0x%02x\n", a_h, a_l);
    printf("x4: 0x%02x, x3: 0x%02x, x: 0x%02x, a_H: 0x%02x, a_L: 0x%02x\n", x4(a_h), x3(a_h), x(a_h), a_h, a_l);
    printf("x2: 0x%02x\n", x2(a_h));
    u8 res = x4(a_h) ^ x3(a_h) ^ x(a_h) ^ a_h ^ a_l;
    printf("Result: 0x%02x", res);
    return 0;
}