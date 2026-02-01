#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
//following the AES-128 spec. 
/*
AES-128 
Nk = 4 words, 128 bits
Nb = 4 words, 128 bits
Nr = 10 rounds.
*/



int EEA(uint16_t a, uint16_t b);
uint16_t polyDiv (uint16_t a, uint16_t b);
uint8_t gfMul(uint8_t b, uint8_t c);
int deg(uint16_t p);
uint32_t polyMul(uint32_t a, uint32_t b);
uint8_t Affine(uint8_t b);
uint16_t gfMultInverse(uint16_t b);
uint32_t RotWord(uint32_t word_in);
int InitState(uint8_t state[4][4], uint8_t* bytes_in);
uint8_t SubBytes(uint8_t byte_in);
uint32_t SubWord(uint32_t word_in);
void printstate();
void InitSbox();
int AES128(uint8_t** buffer_in, uint8_t** buffer_out, int size, uint8_t key[16]);
int Encrypt (uint8_t bytes_in[16], uint8_t bytes_out[16], uint8_t key[16]);
int AddRoundKey(uint8_t state[4][4], int round, uint32_t w[44]);
int KeyExpansion(uint8_t* key_in, uint32_t w[44]);
int ShiftRows();
int MixColumns();
uint8_t xtimes(uint8_t b);



const int Nk = 4;
const int Nb = 4;
const int Nr = 10;
uint8_t sbox[256];
uint8_t state[4][4];
uint32_t w[44];

int AES128(uint8_t** buffer_in, uint8_t** buffer_out, int size, uint8_t key[16]){
    
    InitSbox();
    KeyExpansion(key, w);

    int q = 0;
    q = (size % 16);
    for (int i = 0 ; i < size/16; i++){
        if (i == 0){
            uint8_t ptext[16];
            uint8_t ctext[16];
            memcpy(ptext, *buffer_in+(i*16), 16);
            Encrypt(ptext, ctext, key);
            memcpy(*buffer_out+(i*16), ctext, 16);  
        }
        else {
            uint8_t ptext[16];
            uint8_t ctext[16];
            memcpy(ptext, *buffer_in+(i*16), 16);
            for (int i = 0; i<16 ; i++){
                ptext[i] ^= ctext[i];
            }
            Encrypt(ptext, ctext, key);
            memcpy(*buffer_out+(i*16), ctext, 16); 
        }
        // UNCOMMENT THIS IF YOU WANT ECB MODE. DELETE ABOVE IF,ELSE STATEMENT.
        // uint8_t ptext[16];
        // uint8_t ctext[16];
        // memcpy(ptext, *buffer_in+(i*16), 16);
        // Encrypt(ptext, ctext, key);
        // memcpy(*buffer_out+(i*16), ctext, 16);
    
    }
    if ( q != 0 ){
        uint8_t ptext[16];
        uint8_t ctext[16];
        memset(ptext, 0, 16);
        memset(ctext, 0, 16);
        memcpy(ptext, *buffer_in+((size/16)*16), q);
        Encrypt(ptext, ctext, key);
        memcpy(*buffer_out+((size/16)*16), ctext, q);
    }
    return 0;
}

int Encrypt (uint8_t bytes_in[16], uint8_t bytes_out[16], uint8_t key[16]) {
    
    InitState(state, bytes_in);
    AddRoundKey(state, 0, w);
    for (int i = 1 ; i < (Nr) ; i++){
        for (int j = 0 ; j < 4 ; j++){
            for (int k = 0; k < 4; k++){
                state[k][j] = SubBytes(state[k][j]);
            }
        }
        ShiftRows();
        MixColumns();
        AddRoundKey(state, i, w);
    }
    for (int j = 0 ; j < 4 ; j++){
        for (int k = 0; k < 4; k++){
            state[k][j] = SubBytes(state[k][j]);
        }
    }
    ShiftRows();
    AddRoundKey(state, 10, w);
    memcpy(bytes_out, state, sizeof(state));

}

uint32_t polyMul(uint32_t a, uint32_t b){
    uint32_t tmp = 0;
    while (b) {
        if (b & 1u) {
            tmp ^= a;
        }
        b >>= 1;
        a <<= 1;
    }
    return tmp;
}
uint8_t gfMul(uint8_t b, uint8_t c){ //b(x) * c(x) mod m(x)
    uint8_t h[8];
    uint8_t result = 0;
    for (int i = 0; i < 8 ; i++){
        if (i == 0){
            h[i] = b;
        }
        else {
            if ((h[i-1] >> 7) == 0){
                h[i] = h[i-1]<<1;
            }
            else {
                h[i] = (h[i-1]<<1) ^ 0x11b;
            }
        }
    }
    // 0x33

    for (int i = 0 ; i < 8 ; i++){
        if (c & ((int)pow(2, i))) {
            result ^= h[i];
        }

    }
    return result;
}
int EEA(uint16_t a, uint16_t b) {
    uint16_t old_r = a;
    uint16_t r = b;
    uint16_t old_s = 1;
    uint16_t s = 0;
    uint16_t old_t = 0;
    uint16_t t = 1;

    while (r != 0){
        uint16_t q = polyDiv(old_r, r);
        uint16_t r3 = old_r;
        old_r = r;
        r = r3 ^ (polyMul(q, r));
        uint16_t s3 = old_s;
        old_s = s;
        s = s3 ^ (polyMul(q, s));
        uint16_t t3 = old_t;
        old_t = t;
        t = t3 ^ (polyMul(q, t));
        
    }
    printf("bezout: %02x %02x, gcd: %02x, q by gcd: %02x %02x\n", old_s,old_t,old_r,t,s);
}
uint16_t gfMultInverse(uint16_t b) {
    uint16_t a = 0x11b;
    uint16_t old_r = a;
    uint16_t r = b;
    uint16_t old_s = 1;
    uint16_t s = 0;
    uint16_t old_t = 0;
    uint16_t t = 1;

    while (r != 0){
        uint16_t q = polyDiv(old_r, r);
        uint16_t r3 = old_r;
        old_r = r;
        r = r3 ^ (polyMul(q, r));
        uint16_t s3 = old_s;
        old_s = s;
        s = s3 ^ (polyMul(q, s));
        uint16_t t3 = old_t;
        old_t = t;
        t = t3 ^ (polyMul(q, t));
        
    }
    // printf("bezout: %02x %02x, gcd: %02x, q by gcd: %02x %02x\n", old_s,old_t,old_r,t,s);
    return old_t;
}
uint8_t Affine(uint8_t b) {
    uint8_t c = 0b01100011;
    uint8_t b2 = 0x0;
    for (int i = 0; i < 8; i++){
        b2 = b2 | (((b >> (i)) & 1u)^((b >> ((i+4) % 8)) & 1u)^((b >> ((i+5) % 8)) & 1u)^((b >> ((i+6) % 8)) & 1u)^((b >> ((i+7) % 8)) & 1u)^((c >> (i) & 1u))) << i;
    }
    return b2;
}
uint16_t polyDiv (uint16_t a, uint16_t b) {
    uint16_t q = 0;
    uint16_t r = a;
    while ((deg(r) >= deg(b)) && r != 0){
        int shift = deg(r) - deg(b);
        q ^= (1u << shift);
        r ^= (b << shift);
    }
    return q;
}
int deg(uint16_t p){



    
    int i = 0;
    if (p == 0){
        i = -1;
        return i;
    }
    for (i = 15 ; i > 0 ; i--){
        if ((p >> i) & 0x1) {
            return i;
        }
    }

}

uint8_t SubBytes(uint8_t byte_in) {
    return sbox[byte_in];
}

int InitState(uint8_t state[4][4], uint8_t* bytes_in){
    for (int i = 0; i < 4 ; i++){
        for (int j = 0; j < 4 ; j++){
            state[j][i] = bytes_in[(i*4)+j];
        }
    }
    return 0;
}

int AddRoundKey(uint8_t state[4][4], int round, uint32_t w[44]) {
    for (int i = 0 ; i < 4 ;i++){
        uint32_t tmp = 0;
        for (int j = 0 ; j < 4 ; j++){
            tmp = tmp | (state[j][i] << 24-(j*8));
        }
        uint32_t tmp2 = tmp ^ w[(4*round)+i];
        for (int j = 0 ; j < 4 ; j++){
            state[j][i] = (tmp2 >> 24-(j*8));
        }
    }
}

int KeyExpansion(uint8_t* key_in, uint32_t w[44]) {
    //will generate 4*(Nr+1) words = 44 words.
    //key must be 4*Nk bytes = 16 bytes.
    uint32_t Rcon[10] = {(0x01 << 24),(0x02 << 24),(0x04 << 24),(0x08 << 24),(0x10 << 24),(0x20 << 24),(0x40 << 24),(0x80 << 24),(0x1b << 24),(0x36 << 24)};
    uint32_t temp = 0;

    uint8_t i = 0;
    for (i ; i < Nk; i++){
        w[i] = (
            ((key_in[4*i]) << 24) | ((key_in[(4*i)+1]) << 16) | ((key_in[(4*i)+2]) << 8) | (key_in[(4*i)+3])
        );
        // printf(" %08x\n", w[i]);
    }
    i = Nk;
    while (i <= ((4*Nr)+3)) {
        temp = w[i-1];
        if (i % Nk == 0){
            // printf("\n\ti %d temp %08x subword %08x rotword %08x rcon %08x xor %08x\n", i, temp, SubWord(RotWord(temp)), RotWord(temp), Rcon[(i/Nk)-1], SubWord(RotWord(temp)) ^ Rcon[(i/Nk)-1]);
            temp = SubWord(RotWord(temp)) ^ Rcon[(i/Nk)-1];
            
        }
        w[i] = w[i-Nk] ^ temp;
        // printf("%08x\n", w[i]);
        i+=1;
    }


}

uint32_t RotWord(uint32_t word_in){
    return word_in = (
        (((word_in << 8) & 0xFF000000) | ((word_in <<8) & 0xFF0000) | ((word_in << 8) & 0xFF00) | ((word_in >> 24) & 0xFF))
    );
}

uint32_t SubWord(uint32_t word_in) {
    return word_in = ((SubBytes((word_in >> 24) & 0xFF) << 24) | (SubBytes((word_in >> 16) & 0xFF) << 16) | (SubBytes((word_in >> 8) & 0xFF) << 8) | (SubBytes((word_in) & 0xFF)));
}

int ShiftRows() { // no param because I've chosen state to be global.
    uint8_t tmp[4];
    for (int i = 1 ; i < 4; i++){
        for (int j = 0; j < 4 ; j++){
            tmp[j] = state[i][j];
            state[i][j] = tmp[(j+i) % 4];
        }
        for (int j = 0; j < 4 ; j++){
            state[i][j] = tmp[(j+i) % 4];
        }
    }
    return 0;
}

int MixColumns(){
    uint8_t tmpstate[4][4];
    memmove(tmpstate, state, sizeof(state));
    for (int i = 0 ; i<4 ;i++){
        uint8_t s0 = xtimes(state[0][i]);
        uint8_t s1 = xtimes(state[1][i]);
        uint8_t s2 = xtimes(state[2][i]);
        uint8_t s3 = xtimes(state[3][i]);
        tmpstate[0][i] = (s0) ^ (s1^state[1][i]) ^ (state[2][i]) ^ (state[3][i]);
        tmpstate[1][i] = (state[0][i]) ^ (s1) ^ (s2^state[2][i]) ^ (state[3][i]);
        tmpstate[2][i] = (state[0][i]) ^ (state[1][i]) ^ (s2) ^ (s3^state[3][i]);
        tmpstate[3][i] = (s0^state[0][i]) ^ (state[1][i]) ^ (state[2][i]) ^ (s3);
    }
    memmove(state, tmpstate, sizeof(tmpstate));
    return 0;
}

void printstate() {
    for (int i = 0; i < 4 ; i++){
        printf("\n");
        for (int j = 0; j < 4; j++){
            printf("%02x ",state[i][j]);
        }
    }
    printf("\n");
}

void InitSbox(){
    for (int i = 0 ; i < 256; i++){
        sbox[i] = Affine(gfMultInverse(i));
    }
}

uint8_t xtimes(uint8_t b){
    if (((b >> 7) & 1u) == 0) {
        b <<= 1;
    }
    else {
        b <<= 1;
        b ^= 0x1b;
    }
    return b;
}
