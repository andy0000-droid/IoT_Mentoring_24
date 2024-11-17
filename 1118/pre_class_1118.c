#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef unsigned char u8;

uint8_t buffer_rx[256];
uint8_t buffer_index = 0;
uint8_t RK[176] = {0,};

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

uint32_t u4byte_in(uint8_t* x) { //32비트로 리턴
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];  //x[0]||x[1]||x[2]||x[3]
}

uint8_t Rcons[10] = { 0x01,0x02,0x04,0x08, 0x10,0x20,0x40,0x80, 0x1b,0x36 };

void RotWord(uint8_t x[4])
{
    uint8_t tmp;
    
    tmp=x[0];
    x[0] = x[1];
    x[1] = x[2];
    x[2] = x[3];
    x[3] = tmp;
}
	
void SubWord(uint8_t x[4])
{
    x[0] = Sbox[x[0]];
    x[1] = Sbox[x[1]];
    x[2] = Sbox[x[2]];
    x[3] = Sbox[x[3]];
}

/*
void RoundKeyGeneration128(uint8_t MK[16], uint8_t RK[176]) {
    uint32_t W[44];
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

    RK[16*i+16] = RK[16*i]^T[0];
    RK[16*i+17] = RK[16*i+1]^T[1];
    RK[16*i+18] = RK[16*i+2]^T[2];
    RK[16*i+19] = RK[16*i+3]^T[3];
    RK[16*i+20] = RK[16*i+4]^RK[16*i+16];
    RK[16*i+21] = RK[16*i+5]^RK[16*i+17];
    RK[16*i+22] = RK[16*i+6]^RK[16*i+18];
    RK[16*i+23] = RK[16*i+7]^RK[16*i+19];
    RK[16*i+24] = RK[16*i+8]^RK[16*i+20] ;
    RK[16*i+25] = RK[16*i+9]^RK[16*i+21];
    RK[16*i+26] = RK[16*i+10]^RK[16*i+22];
    RK[16*i+27] = RK[16*i+11]^RK[16*i+23];
    RK[16*i+28] = RK[16*i+12]^RK[16*i+24];
    RK[16*i+29] = RK[16*i+13]^RK[16*i+25];
    RK[16*i+30] = RK[16*i+14]^RK[16*i+26];
    RK[16*i+31] = RK[16*i+15]^RK[16*i+27];
	}
}
*/
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

void AES_ENC(uint8_t PT[16], uint8_t RK[], uint8_t CT[16]) {
	int Nr = 10; //라운드 수 설정
	int i;
	uint8_t temp[16];

	for (i = 0; i < 16; i++) temp[i] = PT[i];

	AddRoundKey(temp, RK); //temp의 16byte와 RK의 첫 16byte를 XOR하여 temp에 결과를 담는 함수
	for (i = 0; i < Nr - 1; i++) {
		SubBytes(temp);
		ShiftRows(temp);
		MixColumns(temp);
		AddRoundKey(temp, RK + 16 * (i + 1));
    
	}
	SubBytes(temp);
	ShiftRows(temp);
	AddRoundKey(temp, RK + 16 * (i + 1));

	for (i = 0; i < 16; i++) CT[i] = temp[i];
}

uint8_t strToHex(char ch)
{
    uint8_t result;
    switch(ch)
    {
    case '0': result=0; break;
    case '1': result=1; break;
    case '2': result=2; break;
    case '3': result=3; break;
    case '4': result=4; break;
    case '5': result=5; break;
    case '6': result=6; break;
    case '7': result=7; break;
    case '8': result=8; break;
    case '9': result=9; break;
    case 'a': result=10; break;
    case 'b': result=11; break;
    case 'c': result=12; break;
    case 'd': result=13; break;
    case 'e': result=14; break;
    case 'f': result=15; break;
    case 'A': result=10; break;
    case 'B': result=11; break;
    case 'C': result=12; break;
    case 'D': result=13; break;
    case 'E': result=14; break;
    case 'F': result=15; break;
    default: result=0; break;
    }
    return result;
    }
/*
int main() {
    uint8_t input[16] = {0, };
    uint8_t output[16] = {0, };
    uint8_t MK[16] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef, 0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0 };
    uint8_t PT[16] = { 0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77, 0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff };
    char ioc[8];
    int i = 0;
    // put your main code here, to run repeatedly:
    
    for(uint8_t p=0; p<16; p++)
    {
    input[p] = (strToHex(buffer_rx[2*p])<<4)^(strToHex(buffer_rx[2*p+1]));
    }
    RoundKeyGeneration128(MK, RK);
    AES_ENC(input, RK, output);
    for(i=0; i<16; i++){
        sprintf(ioc, "0x%02X ", output[i]);
        printf(ioc);
    }
}
*/

int main() {
    uint8_t arrtemp[4] = {0, };
    uint32_t Te0[256] = {0, };
    uint32_t Te1[256] = {0, };
    uint32_t Te2[256] = {0, };
    uint32_t Te3[256] = {0, };

    for(int i = 0; i < 256; i++) {
        u8 temp = Sbox[i];
        arrtemp[0] = (u8)MUL2(temp);
        arrtemp[1] = temp;
        arrtemp[2] = temp;
        arrtemp[3] = (u8)MUL3(temp);
        Te0[i] = u4byte_in(arrtemp);
    }

    for(int i = 0; i < 256; i++) {
        u8 temp = Sbox[i];
        arrtemp[0] = (u8)MUL3(temp);
        arrtemp[1] = (u8)MUL2(temp);
        arrtemp[2] = temp;
        arrtemp[3] = temp;
        Te1[i] = u4byte_in(arrtemp);
    }

    for(int i = 0; i < 256; i++) {
        u8 temp = Sbox[i];
        arrtemp[0] = temp;
        arrtemp[1] = (u8)MUL3(temp);
        arrtemp[2] = (u8)MUL2(temp);
        arrtemp[3] = temp;
        Te2[i] = u4byte_in(arrtemp);
    }

    for(int i = 0; i < 256; i++) {
        u8 temp = Sbox[i];
        arrtemp[0] = temp;
        arrtemp[1] = temp;
        arrtemp[2] = (u8)MUL3(temp);
        arrtemp[3] = (u8)MUL2(temp);
        Te3[i] = u4byte_in(arrtemp);
    }

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