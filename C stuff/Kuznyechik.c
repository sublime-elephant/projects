//Kuznyechik
#include <stdint.h>
#include <string.h>
#include <math.h>


void S1(uint8_t in[16], uint8_t out[16]);
void S(uint8_t in[16], uint8_t out[16]);
uint8_t SubByte(uint8_t byte);
uint8_t SubByteInv(uint8_t byte);
static uint8_t gfMul(uint8_t b, uint8_t c);
void R(uint8_t in[16], uint8_t out[16]);
void R1(uint8_t in[16], uint8_t out[16]);
uint8_t lt(uint8_t in[16]);
void L(uint8_t in[16], uint8_t out[16]);
void KeySchedule(uint8_t round_keys[10][16], uint8_t key[32], uint8_t constants[32][16]);
void K_constants(uint8_t Ci[32][16]);
void X(uint8_t in[16], uint8_t out[16], uint8_t key[16]);
static void Encrypt(uint8_t round_keys[10][16], uint8_t plaintext[16], uint8_t ciphertext[16]);
void KuznyechikEncrypt(uint8_t* bytes_in, uint8_t* bytes_out, uint8_t key[32], int size);

const uint8_t sbox[256] = {
            252, 238, 221,  17, 207, 110,  49,  22, 251, 196, 250,
            218,  35, 197,   4,  77, 233, 119, 240, 219, 147,  46,
            153, 186,  23,  54, 241, 187,  20, 205,  95, 193, 249,
            24, 101,  90, 226,  92, 239,  33, 129,  28,  60,  66,
            139,   1, 142,  79,   5, 132,   2, 174, 227, 106, 143,
            160,   6,  11, 237, 152, 127, 212, 211,  31, 235,  52,
            44,  81, 234, 200,  72, 171, 242,  42, 104, 162, 253,
            58, 206, 204, 181, 112,  14,  86,   8,  12, 118,  18,
            191, 114,  19,  71, 156, 183,  93, 135,  21, 161, 150,
            41,  16, 123, 154, 199, 243, 145, 120, 111, 157, 158,
            178, 177,  50, 117,  25,  61, 255,  53, 138, 126, 109,
            84, 198, 128, 195, 189,  13,  87, 223, 245,  36, 169,
            62, 168,  67, 201, 215, 121, 214, 246, 124,  34, 185,
            3, 224,  15, 236, 222, 122, 148, 176, 188, 220, 232,
            40,  80,  78,  51,  10,  74, 167, 151,  96, 115,  30,
            0,  98,  68,  26, 184,  56, 130, 100, 159,  38,  65,
            173,  69,  70, 146,  39,  94,  85,  47, 140, 163, 165,
            125, 105, 213, 149,  59,   7,  88, 179,  64, 134, 172,
            29, 247,  48,  55, 107, 228, 136, 217, 231, 137, 225,
            27, 131,  73,  76,  63, 248, 254, 141,  83, 170, 144,
            202, 216, 133,  97,  32, 113, 103, 164,  45,  43,   9,
            91, 203, 155,  37, 208, 190, 229, 108,  82,  89, 166,
            116, 210, 230, 244, 180, 192, 209, 102, 175, 194,  57,
            75,  99, 182
};

const uint8_t inv_sbox[256] = {
    165,  45,  50, 143,  14,  48,  56, 192,  84, 230, 158,
         57,  85, 126,  82, 145, 100,   3,  87,  90,  28,  96,
          7,  24,  33, 114, 168, 209,  41, 198, 164,  63, 224,
         39, 141,  12, 130, 234, 174, 180, 154,  99,  73, 229,
         66, 228,  21, 183, 200,   6, 112, 157,  65, 117,  25,
        201, 170, 252,  77, 191,  42, 115, 132, 213, 195, 175,
         43, 134, 167, 177, 178,  91,  70, 211, 159, 253, 212,
         15, 156,  47, 155,  67, 239, 217, 121, 182,  83, 127,
        193, 240,  35, 231,  37,  94, 181,  30, 162, 223, 166,
        254, 172,  34, 249, 226,  74, 188,  53, 202, 238, 120,
          5, 107,  81, 225,  89, 163, 242, 113,  86,  17, 106,
        137, 148, 101, 140, 187, 119,  60, 123,  40, 171, 210,
         49, 222, 196,  95, 204, 207, 118,  44, 184, 216,  46,
         54, 219, 105, 179,  20, 149, 190,  98, 161,  59,  22,
        102, 233,  92, 108, 109, 173,  55,  97,  75, 185, 227,
        186, 241, 160, 133, 131, 218,  71, 197, 176,  51, 250,
        150, 111, 110, 194, 246,  80, 255,  93, 169, 142,  23,
         27, 151, 125, 236,  88, 247,  31, 251, 124,   9,  13,
        122, 103,  69, 135, 220, 232,  79,  29,  78,   4, 235,
        248, 243,  62,  61, 189, 138, 136, 221, 205,  11,  19,
        152,   2, 147, 128, 144, 208,  36,  52, 203, 237, 244,
        206, 153,  16,  68,  64, 146,  58,   1,  38,  18,  26,
         72, 104, 245, 129, 139, 199, 214,  32,  10,   8,   0,
         76, 215, 116
};

uint8_t mulTable[8][255];

void LoadTables() {
    for (int i = 0; i < 8 ; i++){
        for (int j = 0 ; j < 255 ; j++){
            switch(i){
                case 0:
                mulTable[0][j] = gfMul(148, j);
                break;
                case 1:
                mulTable[1][j] = gfMul(32, j);
                break;
                case 2:
                mulTable[2][j] = gfMul(133, j);
                break;
                case 3:
                mulTable[3][j] = gfMul(16, j);
                break;
                case 4:
                mulTable[4][j] = gfMul(194, j);
                break;
                case 5:
                mulTable[5][j] = gfMul(192, j);
                break;
                case 6:
                mulTable[6][j] = gfMul(1, j);
                break;
                case 7:
                mulTable[7][j] = gfMul(251, j);
                break;

            }
        }
    }
}

void KuznyechikEncrypt(uint8_t* bytes_in, uint8_t* bytes_out, uint8_t key[32], int size) {
    int q = size/16;
    uint8_t plaintext[16];
    uint8_t constants[32][16];
    uint8_t round_keys[10][16];
    uint8_t ciphertext[16];
    K_constants(constants);
    KeySchedule(round_keys, key, constants);
    LoadTables();
    for (int i = 0 ; i < q ; i++){
        memcpy(plaintext, bytes_in+(i*16), 16);
        Encrypt(round_keys, plaintext, ciphertext);
        memcpy(bytes_out+(i*16), ciphertext, 16);
    }
    if (q % 16 != 0) {
        memcpy(plaintext, bytes_in+(q*16), (16 - (q%16)));
        Encrypt(round_keys, plaintext, ciphertext);
        memcpy(bytes_out+(q*16), ciphertext, (16 - (q%16)));
    }
    
}

uint8_t lt(uint8_t in[16]){
    return (mulTable[0][in[0]] ^ mulTable[1][in[1]] ^
   mulTable[2][in[2]] ^ mulTable[3][in[3]] ^ mulTable[4][in[4]] ^
   mulTable[5][in[5]] ^ mulTable[1][in[6]] ^ mulTable[7][in[7]] ^ mulTable[1][in[8]] ^
   mulTable[5][in[9]] ^ mulTable[4][in[10]] ^ mulTable[3][in[11]] ^ mulTable[2][in[12]] ^
   mulTable[1][in[13]] ^ mulTable[0][in[14]] ^ mulTable[1][in[15]]);
}

void X(uint8_t in[16], uint8_t out[16], uint8_t key[16]){ //Xor input with 16 byte key
    // printf("X is: ");
    for (int i = 0 ; i < 16; i++){
        out[i] = in[i] ^ key[i];
        // printf("%02x", out[i]);
    }
    // printf("\n");
    // printf("key was: ");
    // for (int i = 0 ; i < 16; i++){
    //     printf("%02x", key[i]);
    // }
    // printf("\n");
    
    
}

void S(uint8_t in[16], uint8_t out[16]){ //S-box

    // printf("S is: ");
    for (int i = 0 ; i < 16; i++){
        out[i] = SubByte(in[i]);
        // printf("%02x", out[i]);
    }
    // printf("\n");
    
}

void S1(uint8_t in[16], uint8_t out[16]){
    for (int i = 0 ; i < 16; i++){
        out[i] = SubByteInv(in[i]);
    }
}

void R(uint8_t in[16], uint8_t out[16]) { // rshift
    uint8_t temp[16];
    temp[0] = lt(in);
    for (int i = 1; i < 16 ; i++){
        temp[i] = in[i-1];
    }
    memcpy(out, temp, 16);
}

void R1(uint8_t in[16], uint8_t out[16]){ //lshift
    uint8_t temp[16];
    temp[0] = lt(in);
    for (int i = 1; i < 16 ; i++){
        temp[i] = in[i+1];
    }
    memcpy(out, temp, 16);
}

void L(uint8_t in[16], uint8_t out[16]){
    uint8_t tmp[16];
    for (int i = 0 ; i < 16 ; i++){
        R(in, tmp);
        in = tmp;
    }
    memcpy(out, tmp, 16);

    // printf("L is: ");
    for (int i = 0 ; i < 16; i++){
        // printf("%02x", out[i]);
    }
    // printf("\n");
    
}

void L1(uint8_t in[16], uint8_t out[16]){
    uint8_t tmp[16];
    for (int i = 0 ; i < 16 ; i++){
        R1(in, tmp);
        in = tmp;
    }
    memcpy(out, tmp, 16);
}

void F(uint8_t a1[16], uint8_t a0[16], uint8_t xorkey[16], uint8_t a1_out[16], uint8_t a0_out[16]){

    uint8_t out_x[16];
    uint8_t out_s[16];
    uint8_t out_l[16];
    X(a1, out_x, xorkey);
    S(out_x, out_s);
    L(out_s, out_l);
    memcpy(a0_out, a1, 16);
    for (int i = 0 ; i < 16 ; i++){
        a1_out[i] = out_l[i] ^ a0[i];
    }
    // printf("K1 is: ");
    // for (int i = 0 ; i < 16; i++){
    //     printf("%02x", a1_out[i]);
    // }
    // printf("\n");
    // printf("K2 is: ");
    // for (int i = 0 ; i < 16; i++){
    //     printf("%02x", a0_out[i]);
    // }
    // printf("\n");
    

}

void K_constants(uint8_t Ci[32][16]) {
    for (int i = 0 ; i < 32 ; i ++){
        uint8_t* current = Ci[i];
        for (int j = 0 ; j < 16 ; j++){
            memset(current+j, 0, 1);
        }
        Ci[i][15] = i+1;
        L(Ci[i], Ci[i]);
    }
    // for (int i = 1 ; i<32;i++){
    //         for (int j=0;j<16;j++){
    //             printf("%02x", Ci[i][j]);
    //         }
    // }
    // printf("\n");
}

void KeySchedule(uint8_t round_keys[10][16], uint8_t key[32], uint8_t constants[32][16]) {
    for (int i = 0; i<32;i++){
        if (i < 16) {
            round_keys[0][i%16] = key[i];
        }
        else {
            round_keys[1][i%16] = key[i];
        }
    }
    uint8_t tmp1[16];
    uint8_t tmp2[16];
    uint8_t tmp3[16];
    uint8_t tmp4[16];
    

    for (int j = 0; j < 4; j++){
        memcpy(tmp1, round_keys[2*j], 16);
        memcpy(tmp2, round_keys[2*j+1],16);
        for (int i = 0 ; i<8;i++){

                F(tmp1, tmp2, constants[(8*j)+i], tmp3, tmp4);
                memcpy(tmp1,tmp3,16);
                memcpy(tmp2,tmp4,16);
        }
        memcpy(round_keys[2*j+2], tmp3, 16);
        memcpy(round_keys[2*j+3], tmp4, 16);
    }



    // for (int i = 0; i < 10 ; i ++){
    //     for (int j = 0 ; j < 16 ; j++){
    //         printf("%02x", round_keys[i][j]);
    //     }
    //     printf("\n");
    // }
}


static void Encrypt(uint8_t round_keys[10][16], uint8_t plaintext[16], uint8_t ciphertext[16]){
    uint8_t outx[16];
    uint8_t outs[16];
    uint8_t outl[16];
    X(plaintext, outx, round_keys[0]);
    S(outx, outs);
    L(outs, outl);
    for (int i=1;i<9;i++){
        X(outl, outx, round_keys[i]);
        S(outx, outs);
        L(outs, outl);
    }
    X(outl, outx, round_keys[9]);
    memcpy(ciphertext,outx,16);
}

/*
  
   Round keys K_i, i=1, 2, ..., 10 are derived from key
   K=k_255||...||k_0 belonging to V_256, k_i belongs to V_1, i=0, 1,
   ..., 255, as follows:

   K_1=k_255||...||k_128;
   K_2=k_127||...||k_0;
   (K_(2i+1),K_(2i+2))=F[C_(8(i-1)+8)]...
    F[C_(8(i-1)+1)](K_(2i-1),K_(2i)), i=1,2,3,4. */

uint8_t SubByte(uint8_t byte){
    return sbox[byte];
}
uint8_t SubByteInv(uint8_t byte){
    return inv_sbox[byte];
}


static uint8_t gfMul(uint8_t b, uint8_t c){ //b(x) * c(x) mod m(x)
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
                h[i] = (h[i-1]<<1) ^ 0x1c3;
            }
        }
    }
    // 0x33

    for (int i = 0 ; i < 8 ; i++){
        if (c & (1u << i)) {
            result ^= h[i];
        }

    }
    return result;
}