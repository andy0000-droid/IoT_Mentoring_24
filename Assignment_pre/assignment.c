/**
 * @brief 
 * Left to Right Exponentiation Algorithm
 * GF(2^128), Defining Polynomial = x^128 + x^7 + x^2 + x + 1
 * 0b 10000000 00000000 00000000 00000000 00000000 00000000 00000000 10000111
 * 0x 80 00 00 00 00 00 00 87
 * 
 * 128 bits Multiplication & Squaring
 * 128 = 16 * 8 (16 bytes)
 * 
 * Exponent: (2bytes)
 *     0b 00001010 11101101
 *     0x 0a ed
 * 
 * Base: 
 *     Arbitarily (Hard coded)
 * 
 */

/**
 * Left to Right Exponentiation
 * 1. convert exponent(decimal or hex) to binary
 * 2. if exponent is 1, then result * base
 * 3. basically perform squaring of base
 * 4. return result when exponent end
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

typedef unsigned char BYTE;

#define byte 8
void print_res(BYTE *res, int size);

/**
 * @brief Perform xtimes for GF(2^128) Multiplication
 * 
 * @param in Input
 * @param res Output
 * @param size Size of input
 */
void x2(BYTE *in, BYTE *res, int size) {
    bool cf = false;
    int i = 0;

    cf = in[0] & 0x80;
    //printf("Carry Flag: %s\n", cf ? "T" : "F");

    //printf("%d\n", i);
    res[0] = in[0] << 0x01;
    i++;

    for (; i < size; i++) {
        //printf("Index: %02d ", i);
        BYTE temp = 0;
        temp = (in[i] & 0x80) >> 0x07; //Most Significant Bit
        //printf("Carry: %s\n", temp ? "T" : "F");
        res[i] = in[i] << 0x01;
        res[i - 1] ^= temp;
    }
    if (cf) {
        res[size] ^= 0x87;
    }
}

/**
 * @brief Perform Addition for GF(2^128)
 * 
 * @param a Input
 * @param b Input
 * @param res Output
 * @param size Size of input
 */
void add(BYTE *a, BYTE *b, BYTE *res, int size) {
    for (int i = 0; i < size; i++) {
        res[i] = a[i] ^ b[i];
    }
}

/**
 * @brief Perform Multiplication for GF(2^128)
 * 
 * @param a Input
 * @param b Input
 * @param res Output
 * @param size Size of input
 */
void mul(BYTE *a, BYTE *b, BYTE *res, int size) {
    BYTE *temp = a;
    BYTE *res_temp = (BYTE*)calloc(size, sizeof(BYTE));
    for (int i = 1; i <= size; i++) {
        printf("i: %02d, ", i);
        for (int j = 0; j < byte; j++) {
            printf("j: %d ", j);
            if (b[size - i] & (0x01 << j)) {
                for (int k = j; k > 0; k--) {
                    printf("xtime ");
                    x2(temp, res_temp, size);
                    temp = res_temp;
                }
                printf("add ");
                add(a, res_temp, res, size);
                printf("\n");
                print_res(res, size);
            }
        }
        printf("\n");
    }
    free(res_temp);
    res_temp = NULL;
}


void sq(BYTE *in, BYTE *res) {

}

/**
 * @brief Print result in Hex
 * 
 * @param res Input
 * @param size Size of input
 */
void print_res(BYTE *res, int size) {
    for (int i = 0; i < size; i++) {
            printf("0x%02x ", res[i]);
        }
    printf("\n");
}

int main() {
    BYTE base[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}; //16 bytes with big endian
    BYTE test[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07}; //16 bytes with big endian
    BYTE exp[2] = {0x0a, 0xed}; //2 bytes
    BYTE res[16] = {0x00, };
    BYTE res1[16] = {0x00, };
    BYTE res2[16] = {0x00, };
    BYTE res3[16] = {0x00, };
    BYTE res4[16] = {0x00, };
    BYTE temp[16] = {0x00, };
    BYTE res_temp[16] = {0x00, };

    int size = sizeof(base) / sizeof(char);

    //BYTE base1[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}; //16 bytes with big endian
    // add(base, base1, res, size);
    add(base, res1, res, size);
    print_res(res, size);

    x2(base, res2, size);
    add(base, res2, res, size);
    print_res(res, size);
    *temp = *base;
    x2(temp, res_temp, size);
    *temp = *res_temp;
    x2(temp, res_temp, size);
    add(base, res_temp, res, size);
    print_res(res, size);

    printf("Result: ");
    print_res(res, size);

    //
    memset(res, 0x00, 16 * sizeof(char));
    mul(base, test, res, size);
    print_res(res, size);
    print_res(base, size);
}