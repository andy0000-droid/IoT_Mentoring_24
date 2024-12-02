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



#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned char BYTE;

#define byte 8

void x2(BYTE *in, BYTE *res, int size) {
    /*
     * Perform xtimes for multiplication
     */
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

void mul(BYTE *a, BYTE *b, BYTE *res) {
    
}

void sq(BYTE *in, BYTE *res) {

}


int main() {
    BYTE base[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}; //16 bytes with big endian
    BYTE exp[2] = {0x0a, 0xed}; //2 bytes
    BYTE res[16] = {0x00, };
    int size = sizeof(base) / sizeof(char);
    //x2(base, res, size);
    /*for (int i = 0; i < size; i++) {
        printf("0x%02x ", res[i]);
    }*/
}