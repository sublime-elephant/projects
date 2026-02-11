//Japanese Camellia (128 bit)
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


static void KeySchedule(uint64_t key[2]);
static void GenerateSboxes();
static void lshift(uint64_t out[2], uint64_t in[2], int n);
static uint32_t LROT(uint32_t bytes_in, int n);
static uint64_t FLINV(uint64_t FLINV_IN, uint64_t KE);
static uint64_t F(uint64_t F_IN, uint64_t KE);
static uint64_t FLINV(uint64_t FLINV_IN, uint64_t KE);
static void Encrypt(uint64_t plaintext[2], uint64_t ciphertext[2]);
static uint8_t byteLROT(uint8_t byte, int n );
void CamelliaEncrypt(uint8_t* bytes_in, uint8_t* bytes_out, uint64_t key[2], int size);

const uint8_t sbox1[256] ={
  112, 130,  44, 236, 179,  39, 192, 229, 228, 133,  87,  53, 234,  12, 174,  65,
   35, 239, 107, 147,  69,  25, 165,  33, 237,  14,  79,  78,  29, 101, 146, 189,
  134, 184, 175, 143, 124, 235,  31, 206,  62,  48, 220,  95,  94, 197,  11,  26,
  166, 225,  57, 202, 213,  71,  93,  61, 217,   1,  90, 214,  81,  86, 108,  77,
  139,  13, 154, 102, 251, 204, 176,  45, 116,  18,  43,  32, 240, 177, 132, 153,
  223,  76, 203, 194,  52, 126, 118,   5, 109, 183, 169,  49, 209,  23,   4, 215,
   20,  88,  58,  97, 222,  27,  17,  28,  50,  15, 156,  22,  83,  24, 242,  34,
  254,  68, 207, 178, 195, 181, 122, 145,  36,   8, 232, 168,  96, 252, 105,  80,
  170, 208, 160, 125, 161, 137,  98, 151,  84,  91,  30, 149, 224, 255, 100, 210,
   16, 196,   0,  72, 163, 247, 117, 219, 138,   3, 230, 218,   9,  63, 221, 148,
  135,  92, 131,   2, 205,  74, 144,  51, 115, 103, 246, 243, 157, 127, 191, 226,
   82, 155, 216,  38, 200,  55, 198,  59, 129, 150, 111,  75,  19, 190,  99,  46,
  233, 121, 167, 140, 159, 110, 188, 142,  41, 245, 249, 182,  47, 253, 180,  89,
  120, 152,   6, 106, 231,  70, 113, 186, 212,  37, 171,  66, 136, 162, 141, 250,
  114,   7, 185,  85, 248, 238, 172,  10,  54,  73,  42, 104,  60,  56, 241, 164,
   64,  40, 211, 123, 187, 201,  67, 193,  21, 227, 173, 244, 119, 199, 128, 158,

};
uint8_t sbox2[256];
uint8_t sbox3[256];
uint8_t sbox4[256];

const uint64_t sigma1 = 0xA09E667F3BCC908B;
const uint64_t sigma2 = 0xB67AE8584CAA73B2;
const uint64_t sigma3 = 0xC6EF372FE94F82BE;
const uint64_t sigma4 = 0x54FF53A5F1D36F1C;
const uint64_t sigma5 = 0x10E527FADE682D1D;
const uint64_t sigma6 = 0xB05688C2B3E6C1FD;
const uint64_t MASK8 =  0xff;
const uint64_t MASK32 = 0xffffffff;
const uint64_t MASK64 = 0xffffffffffffffff;
const uint64_t MASK128[2] = {0xffffffffffffffff, 0xffffffffffffffff};
uint64_t kw1,kw2,kw3,kw4;
uint64_t k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11,k12,k13,k14,k15,k16,k17,k18;
uint64_t ke1,ke2,ke3,ke4;

void CamelliaEncrypt(uint8_t* bytes_in, uint8_t* bytes_out, uint64_t key[2], int size){
    GenerateSboxes();
    KeySchedule(key);
    uint64_t plaintext[2];
    uint64_t ciphertext[2];
    int q = size/16;
    int r = size % 16;
    for (int i = 0 ; i < q ; i++){
        for (int j = 0 ; j < 2 ; j++){
            plaintext[j] = 0;
            for (int k = 0 ; k < 8 ; k++){
                plaintext[j] = plaintext[j] | (*(bytes_in+(i*16)+(j*8)+k) << (56-(8*k)));
            }
        }
        Encrypt(plaintext, ciphertext);
        memcpy(bytes_out+(i*16), ciphertext, 16);
    }
    if (r != 0){
        for (int j = 0 ; j < 2 ; j++){
            plaintext[j] = 0;
            for (int k = 0 ; k < 8 ; k++){
                if ((2*j)+k < r){
                    plaintext[j] = plaintext[j] | (*(bytes_in+(q*16)+(j*8)+k) << (56-(8*k)));
                }
                else {
                    plaintext[j] = plaintext[j] | (0 << 24-(8*k));
                }
            }
        }
        Encrypt(plaintext, ciphertext);
        memcpy(bytes_out+q, ciphertext, r);
    }
}



void KeySchedule(uint64_t key[2]){
    uint64_t D1,D2;
    uint64_t KL[2];
    uint64_t KR[2] = {0};
    uint64_t KA[2];
    uint64_t pickme[2];
    KL[0] = key[0];
    KL[1] = key[1];
    D1 = KL[0] ^ KR[0];
    D2 = KL[1] ^ KR[1];
    D2 = D2 ^ F(D1, sigma1);
    D1 = D1 ^ F(D2, sigma2);
    D1 = D1 ^ (KL[0]);
    D2 = D2 ^ (KL[1]);
    D2 = D2 ^ F(D1, sigma3);
    D1 = D1 ^ F(D2, sigma4);
    KA[0] = D1;
    KA[1] = D2;

    
    kw1 = KL[0];
    kw2 = KL[1];
    k1 = KA[0];
    k2 = KA[1];
    lshift(pickme, KL, 15);
    k3 = pickme[0];
    k4 = pickme[1];
    lshift(pickme, KA, 15);
    k5 = pickme[0];
    k6 = pickme[1];
    lshift(pickme, KA, 30);
    ke1 = pickme[0];
    ke2 = pickme[1];
    lshift(pickme, KL, 45);
    k7 = pickme[0];
    k8 = pickme[1];
    lshift(pickme, KA, 45);
    k9 = pickme[0];
    lshift(pickme, KL, 60);
    k10 = pickme[1];
    lshift(pickme, KA, 60);
    k11 = pickme[0];
    k12 = pickme[1];
    lshift(pickme, KL, 77);
    ke3 = pickme[0];
    ke4 = pickme[1];
    lshift(pickme, KL, 94);
    k13 = pickme[0];
    k14 = pickme[1];
    lshift(pickme, KA, 94);
    k15 = pickme[0];
    k16 = pickme[1];
    lshift(pickme, KL, 111);
    k17 = pickme[0];
    k18 = pickme[1];
    lshift(pickme, KA, 111);
    kw3 = pickme[0];
    kw4 = pickme[1];
    


}

uint64_t F(uint64_t F_IN, uint64_t KE) {
    uint64_t x;
    uint64_t t1,t2,t3,t4,t5,t6,t7,t8;
    uint64_t y1,y2,y3,y4,y5,y6,y7,y8;
    x = F_IN ^ KE;
    t1 =  x >> 56;
    t2 = (x >> 48) & 0xFF;
    t3 = (x >> 40) & 0xFF;
    t4 = (x >> 32) & 0xFF;
    t5 = (x >> 24) & 0xFF;
    t6 = (x >> 16) & 0xFF;
    t7 = (x >>  8) & 0xFF;
    t8 =  x        & 0xFF;
    t1 = sbox1[t1];
    t2 = sbox2[t2];
    t3 = sbox3[t3];
    t4 = sbox4[t4];
    t5 = sbox2[t5];
    t6 = sbox3[t6];
    t7 = sbox4[t7];
    t8 = sbox1[t8];
    y1 = t1 ^ t3 ^ t4 ^ t6 ^ t7 ^ t8;
    y2 = t1 ^ t2 ^ t4 ^ t5 ^ t7 ^ t8;
    y3 = t1 ^ t2 ^ t3 ^ t5 ^ t6 ^ t8;
    y4 = t2 ^ t3 ^ t4 ^ t5 ^ t6 ^ t7;
    y5 = t1 ^ t2 ^ t6 ^ t7 ^ t8;
    y6 = t2 ^ t3 ^ t5 ^ t7 ^ t8;
    y7 = t3 ^ t4 ^ t5 ^ t6 ^ t8;
    y8 = t1 ^ t4 ^ t5 ^ t6 ^ t7;
    return ((y1 << 56) | (y2 << 48) | (y3 << 40) | (y4 << 32)
    | (y5 << 24) | (y6 << 16) | (y7 <<  8) | y8);
}

uint64_t FL(uint64_t FL_IN, uint64_t KE){
    uint32_t x1,x2;
    uint32_t m1,m2;
    uint64_t out = 0;

    x1 = FL_IN >> 32;
    x2 = FL_IN & 0xffffffff;
    m1 = KE >> 32;
    m2 = KE & 0xffffffff;
    x2 = x2 ^ LROT((x1 & m1), 1);
    x1 = x1 ^ (x2 | m2);
    out = x1;
    out = out << 32;
    out = out | x2;
    return out;
}

uint64_t FLINV(uint64_t FLINV_IN, uint64_t KE){
    uint32_t y1,y2;
    uint32_t m1,m2;
    uint64_t out;
    y1 = FLINV_IN >> 32;
    y2 = FLINV_IN & 0xffffffff;
    m1 = KE >> 32;
    m2 = KE & 0xffffffff;
    y1 = y1 ^ (y2 | m2);
    y2 = y2 ^ (LROT((y1 & m1),1));
    out = y1;
    out = out << 32;
    out = out | y2;
    return out; 
}

void Encrypt(uint64_t plaintext[2], uint64_t ciphertext[2]){
    uint64_t D1,D2;
    D1 = plaintext[0];
    D2 = plaintext[1];
    D1 = D1 ^ kw1;
    D2 = D2 ^ kw2;
    D2 = D2 ^ F(D1, k1);
    D1 = D1 ^ F(D2, k2);     
    D2 = D2 ^ F(D1, k3);     
    D1 = D1 ^ F(D2, k4);     
    D2 = D2 ^ F(D1, k5);    
    D1 = D1 ^ F(D2, k6);
    D1 = FL   (D1, ke1);     
    D2 = FLINV(D2, ke2);     
    D2 = D2 ^ F(D1, k7);     
    D1 = D1 ^ F(D2, k8);     
    D2 = D2 ^ F(D1, k9);     
    D1 = D1 ^ F(D2, k10);    
    D2 = D2 ^ F(D1, k11);    
    D1 = D1 ^ F(D2, k12);    
    D1 = FL   (D1, ke3);     
    D2 = FLINV(D2, ke4);     
    D2 = D2 ^ F(D1, k13);    
    D1 = D1 ^ F(D2, k14);    
    D2 = D2 ^ F(D1, k15);    
    D1 = D1 ^ F(D2, k16);    
    D2 = D2 ^ F(D1, k17);    
    D1 = D1 ^ F(D2, k18);    
    D2 = D2 ^ kw3;           
    D1 = D1 ^ kw4;
    ciphertext[0] = D2;
    ciphertext[1] = D1;
}

void GenerateSboxes(){
    for (int i = 0; i < 256; i++){
        sbox2[i] = byteLROT(sbox1[i], 1); //...
        sbox3[i] = byteLROT(sbox1[i], 7);
        sbox4[i] = sbox1[(byteLROT(i, 1))];
    }
}

void limbXOR(uint64_t out[2], uint64_t in1[2], uint64_t in2[2]){
    out[0] = in1[0] ^ in2[0];
    out[1] = in1[1] ^ in2[1];
}
void lshift(uint64_t out[2], uint64_t in[2], int n){
    if (n < 64){
        out[0] = (
            (in[0] << n) | (in[1] >> (64-n))
        );
        out[1] = (
            (in[1] << n ) | (in[0] >> (64-n))
        );        
    }
    else if (n > 64){
        uint64_t tmp1;
        uint64_t tmp2;
        tmp1 = in[1];
        tmp2 = in[0]; //SWAP AND DO AS N<64.
        n -= 64;
        out[0] = (
            (tmp1 << n) | (tmp2 >> (64-n))
        );
        out[1] = (
            (tmp2 << n ) | (tmp1 >> (64-n))
        );      
    }
    else if (n == 64){
        out[0] = in[1];
        out[1] = in[0];
    }
}

uint32_t LROT(uint32_t bytes_in, int n){
    return  (
            ((bytes_in << n) | (bytes_in >> (32-n)))
    );

}

uint8_t byteLROT(uint8_t byte, int n ){
    return (
        ((byte << n) | (byte >> (8-n)))
    );
}