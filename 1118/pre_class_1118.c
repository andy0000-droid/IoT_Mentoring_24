#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// typedef unsigned char u8;
#define MUL2(a) (a<<1)^(a&0x80?0x1b:0)
#define MUL3(a) (MUL2(a))^(a)

uint8_t Sbox[256] = {
0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

uint32_t Te0[256];
uint32_t Te1[256];
uint32_t Te2[256];
uint32_t Te3[256];

uint32_t u4byte_in(uint8_t* x) { //32비트로 리턴
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];  //x[0]||x[1]||x[2]||x[3]
}

void u4byte_out(uint8_t *x, uint32_t y) {
    x[0] = (y >> 24) & 0xff;
    x[0] = (y >> 16) & 0xff;
    x[0] = (y >> 8) & 0xff;
    x[3] = y & 0xff;
}

uint32_t Rcons[10] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000 };

#define RotWord(x) ((x << 8) | (x >> 24))

uint32_t SubWord(uint32_t x)
{
    return (((uint32_t)(Sbox[(uint8_t)(x >> 24)]) << 24) | ((uint32_t)(Sbox[(uint8_t)((x >> 16) & 0xff)]) << 16) | ((uint32_t)(Sbox[(uint8_t)((x >> 8) & 0xff)]) << 8) | ((uint32_t)(Sbox[(uint8_t)(x & 0xff)])));
}

void RoundKeyGeneration128(uint8_t MK[], uint32_t W[]) {
	int32_t i;
	uint32_t T;

	W[0] = u4byte_in(MK);
    W[1] = u4byte_in(MK + 4);
    W[2] = u4byte_in(MK + 8);
    W[3] = u4byte_in(MK + 12);

	for (i = 0; i < 10; i++) {
		//T = G_func(W[4 * i + 3]);
		T = W[4 * i + 3];
		T = RotWord(T);
		T = SubWord(T);
		T ^= Rcons[i];

        W[4 * i + 4] = W[4 * i] ^ T;
        W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];
        W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];
        W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];
	}
}

/*
void AddRoundKey(uint8_t S[16], uint8_t RK[16]) {
	S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3];
	S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7];
	S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11];
	S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];
}

void SubBytes(uint8_t S[16]) {
	S[0] = Sbox[S[0]]; S[1] = Sbox[S[1]]; S[2] = Sbox[S[2]]; S[3] = Sbox[S[3]];
	S[4] = Sbox[S[4]]; S[5] = Sbox[S[5]]; S[6] = Sbox[S[6]]; S[7] = Sbox[S[7]];
	S[8] = Sbox[S[8]]; S[9] = Sbox[S[9]]; S[10] = Sbox[S[10]]; S[11] = Sbox[S[11]];
	S[12] = Sbox[S[12]]; S[13] = Sbox[S[13]]; S[14] = Sbox[S[14]]; S[15] = Sbox[S[15]];
}

void ShiftRows(uint8_t S[16]) {
	uint8_t temp;
	temp = S[1];
	S[1] = S[5]; S[5] = S[9]; S[9] = S[13]; S[13] = temp;
	temp = S[2];
	S[2] = S[10]; S[10] = temp; temp = S[6]; S[6] = S[14]; S[14] = temp;
	temp = S[15];
	S[15] = S[11]; S[11] = S[7]; S[7] = S[3]; S[3] = temp;
}

#define MUL2(a) (a<<1)^(a&0x80?0x1b:0)
#define MUL3(a) (MUL2(a))^(a)

void MixColumns(uint8_t S[16]) {
	uint8_t temp[16];
	int i;
	for (i = 0; i < 16; i += 4) {
		temp[i] = MUL2(S[i]) ^ MUL3(S[i + 1]) ^ S[i + 2] ^ S[i + 3];
		temp[i + 1] = S[i] ^ MUL2(S[i + 1]) ^ MUL3(S[i + 2]) ^ S[i + 3];
		temp[i + 2] = S[i] ^ S[i + 1] ^ MUL2(S[i + 2]) ^ MUL3(S[i + 3]);
		temp[i + 3] = MUL3(S[i]) ^ S[i + 1] ^ S[i + 2] ^ MUL2(S[i + 3]);
	}
	S[0] = temp[0]; S[1] = temp[1]; S[2] = temp[2]; S[3] = temp[3];
	S[4] = temp[4]; S[5] = temp[5]; S[6] = temp[6]; S[7] = temp[7];
	S[8] = temp[8]; S[9] = temp[9]; S[10] = temp[10]; S[11] = temp[11];
	S[12] = temp[12]; S[13] = temp[13]; S[14] = temp[14]; S[15] = temp[15];

}
*/

void print_te_table() {
    printf("u32 Te0[256]={\n");
    for(int i = 0; i < 256; i++) {
        printf("0x%08x, ", Te0[i]);
        if(i % 8 == 7) printf("\n");
    }
    printf("}\n");

    printf("u32 Te1[256]={\n");
    for(int i = 0; i < 256; i++) {
        printf("0x%08x, ", Te1[i]);
        if(i % 8 == 7) printf("\n");
    }
    printf("}\n");

    printf("u32 Te2[256]={\n");
    for(int i = 0; i < 256; i++) {
        printf("0x%08x, ", Te2[i]);
        if(i % 8 == 7) printf("\n");
    }
    printf("}\n");

    printf("u32 Te3[256]={\n");
    for(int i = 0; i < 256; i++) {
        printf("0x%08x, ", Te3[i]);
        if(i % 8 == 7) printf("\n");
    }
    printf("}\n");
}

void AES_ENC(uint8_t PT[16], uint32_t W[], uint8_t CT[16], int keysize) {
	int Nr = (keysize / 32) + 6; //라운드 수 설정
	int i;
	uint32_t s0, s1, s2, s3, t0, t1, t2, t3;

    print_te_table();
    for (int i = 0; i < 16; i++) printf("0x%02x, ", PT[i]);
    printf("\n");

	s0 = u4byte_in(PT) ^ W[0];
    s1 = u4byte_in(PT + 4) ^ W[1];
    s2 = u4byte_in(PT + 8) ^ W[2];
    s3 = u4byte_in(PT + 12) ^ W[3];

    for (int i = 0; i < Nr - 1; i++) {
        s0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[4 * i + 4];
        s1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[4 * i + 5];
        s2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[4 * i + 6];
        s3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[4 * i + 7];
    }
    s0 = (Te2[s0 >> 24] & 0xff000000) ^ (Te3[(s1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(s2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[s3 & 0xff] ^ 0x000000ff) ^ W[4 * Nr];
    s1 = (Te2[s1 >> 24] & 0xff000000) ^ (Te3[(s2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(s3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[s0 & 0xff] ^ 0x000000ff) ^ W[4 * Nr + 1];
    s2 = (Te2[s2 >> 24] & 0xff000000) ^ (Te3[(s3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(s0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[s1 & 0xff] ^ 0x000000ff) ^ W[4 * Nr + 2];
    s3 = (Te2[s3 >> 24] & 0xff000000) ^ (Te3[(s0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(s1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[s2 & 0xff] ^ 0x000000ff) ^ W[4 * Nr + 3];
    /*
    // 1 round
    t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[4];
    t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[5];
    t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[6];
    t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[7];

    // 2 round
    s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[8];
    s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[9];
    s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[10];
    s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[11];

    // 3 round
    t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[12];
    t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[13];
    t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[14];
    t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[15];

    // 4 round
    s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[16];
    s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[17];
    s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[18];
    s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[19];

    // 5 round
    t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[20];
    t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[21];
    t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[22];
    t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[23];

    // 6 round
    s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[24];
    s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[25];
    s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[26];
    s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[27];

    // 7 round
    t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[28];
    t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[29];
    t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[30];
    t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[31];

    // 8 round
    s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[32];
    s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[33];
    s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[34];
    s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[35];
    
    if(Nr == 10) {
        // 9 round
        t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[36];
        t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[37];
        t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[38];
        t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[39];

        // 10 round
        s0 = (Te2[t0 >> 24] & 0xff000000) ^ (Te3[(t1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t3 & 0xff] ^ 0x000000ff) ^ W[40];
        s1 = (Te2[t1 >> 24] & 0xff000000) ^ (Te3[(t2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t0 & 0xff] ^ 0x000000ff) ^ W[41];
        s2 = (Te2[t2 >> 24] & 0xff000000) ^ (Te3[(t3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t1 & 0xff] ^ 0x000000ff) ^ W[42];
        s3 = (Te2[t3 >> 24] & 0xff000000) ^ (Te3[(t0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t2 & 0xff] ^ 0x000000ff) ^ W[43];
    }
    else if(Nr == 12) {
        // 9 round
        t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[36];
        t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[37];
        t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[38];
        t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[39];

        // 10 round
        s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[40];
        s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[41];
        s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[42];
        s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[43];

        // 11 round
        t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[44];
        t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[45];
        t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[46];
        t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[47];

        // 12 round
        s0 = (Te2[t0 >> 24] & 0xff000000) ^ (Te3[(t1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t3 & 0xff] ^ 0x000000ff) ^ W[48];
        s1 = (Te2[t1 >> 24] & 0xff000000) ^ (Te3[(t2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t0 & 0xff] ^ 0x000000ff) ^ W[49];
        s2 = (Te2[t2 >> 24] & 0xff000000) ^ (Te3[(t3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t1 & 0xff] ^ 0x000000ff) ^ W[50];
        s3 = (Te2[t3 >> 24] & 0xff000000) ^ (Te3[(t0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t2 & 0xff] ^ 0x000000ff) ^ W[51];
    }
    else if(Nr == 14) {
        // 9 round
        t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[36];
        t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[37];
        t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[38];
        t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[39];

        // 10 round
        s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[40];
        s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[41];
        s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[42];
        s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[43];

        // 11 round
        t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[44];
        t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[45];
        t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[46];
        t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[47];

        // 12 round
        s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[48];
        s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[49];
        s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[50];
        s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[51];

        // 13 round
        t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[52];
        t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[53];
        t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[54];
        t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[55];

        // 14 round
        s0 = (Te2[t0 >> 24] & 0xff000000) ^ (Te3[(t1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t3 & 0xff] ^ 0x000000ff) ^ W[56];
        s1 = (Te2[t1 >> 24] & 0xff000000) ^ (Te3[(t2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t0 & 0xff] ^ 0x000000ff) ^ W[57];
        s2 = (Te2[t2 >> 24] & 0xff000000) ^ (Te3[(t3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t1 & 0xff] ^ 0x000000ff) ^ W[58];
        s3 = (Te2[t3 >> 24] & 0xff000000) ^ (Te3[(t0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[t2 & 0xff] ^ 0x000000ff) ^ W[59];
    }
    */
    u4byte_out(CT, s0);
    u4byte_out(CT + 4, s1);
    u4byte_out(CT + 8, s2);
    u4byte_out(CT + 12, s3);
    for (int i = 0; i < 16; i++) printf("0x%02x, ", CT[i]);
    printf("\n");
}

int main() {
    uint8_t arrtemp[4] = {0x00, };
    uint32_t W[60] = {0x00, };
    //Master key 2b7e151628aed2a6abf7158809cf4f3c
    uint8_t MK[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    //Plain Text 6bc1bee22e409f96e93d7e117393172a
    uint8_t PT[16] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
    uint8_t CT[16] = {0x00, };
    //Cipher Text 3ad77bb40d7a3660a89ecaf32466ef97
    uint8_t TCT[16] = {0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97};
    int keysize = 128;

    for(int i = 0; i < 256; i++) {
        uint8_t temp = Sbox[i];
        arrtemp[0] = (uint8_t)MUL2(temp);
        arrtemp[1] = temp;
        arrtemp[2] = temp;
        arrtemp[3] = (uint8_t)MUL3(temp);
        Te0[i] = u4byte_in(arrtemp);
    }

    for(int i = 0; i < 256; i++) {
        uint8_t temp = Sbox[i];
        arrtemp[0] = (uint8_t)MUL3(temp);
        arrtemp[1] = (uint8_t)MUL2(temp);
        arrtemp[2] = temp;
        arrtemp[3] = temp;
        Te1[i] = u4byte_in(arrtemp);
    }

    for(int i = 0; i < 256; i++) {
        uint8_t temp = Sbox[i];
        arrtemp[0] = temp;
        arrtemp[1] = (uint8_t)MUL3(temp);
        arrtemp[2] = (uint8_t)MUL2(temp);
        arrtemp[3] = temp;
        Te2[i] = u4byte_in(arrtemp);
    }

    for(int i = 0; i < 256; i++) {
        uint8_t temp = Sbox[i];
        arrtemp[0] = temp;
        arrtemp[1] = temp;
        arrtemp[2] = (uint8_t)MUL3(temp);
        arrtemp[3] = (uint8_t)MUL2(temp);
        Te3[i] = u4byte_in(arrtemp);
    }

    //print_te_table();

    RoundKeyGeneration128(MK, W);
    AES_ENC(PT, W, CT, keysize);
    printf("Plain Text: \n");
    for (int i = 0; i < 16; i++) printf("0x%02x, ", PT[i]);
    printf("\n");
    printf("Matster Key: \n");
    for (int i = 0; i < 16; i++) printf("0x%02x, ", MK[i]);
    printf("\n");
    printf("Cipher Text: \n");
    for (int i = 0; i < 16; i++) printf("0x%02x, ", CT[i]);
    printf("\n");
    printf("Test Cipher Text: \n");
    for (int i = 0; i < 16; i++) printf("0x%02x, ", TCT[i]);
}