#include <stdio.h>
#include <stdint.h>

#define AES_MOD_POLY 0x11B  // x^8 + x^4 + x^3 + x + 1 다항식 (0x11B는 0b100011011)

// AES GF(2^8)에서의 곱셈과 모듈러 연산을 수행
uint8_t gf_mult(uint8_t a, uint8_t b) {
    uint8_t result = 0;
    while (b > 0) {
        if (b & 1) {
            result ^= a;
        }
        a <<= 1;
        if (a & 0x100) {  // a가 8비트를 넘으면
            a ^= AES_MOD_POLY;
        }
        b >>= 1;
    }
    return result;
}

// AES subfield reduction 함수
uint8_t aes_subfield_reduction(uint8_t x) {
    uint8_t inverse = 0;
    if (x != 0) {
        inverse = gf_mult(x, x);  // Square
        inverse = gf_mult(inverse, x);  // Cube
        inverse = gf_mult(inverse, x);  // x^4
        inverse = gf_mult(inverse, x);  // x^5
        inverse = gf_mult(inverse, x);  // x^6
        inverse = gf_mult(inverse, x);  // x^7
        inverse = gf_mult(inverse, x);  // x^8 (여기서 모듈러 연산이 이루어짐)
    }
    return inverse;
}

int main() {
    uint8_t x = 0x57; // 예시 입력값
    uint8_t result = aes_subfield_reduction(x);
    printf("Subfield reduction result: 0x%02X\n", result);
    return 0;
}