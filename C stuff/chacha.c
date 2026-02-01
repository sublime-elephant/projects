#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "chacha.h"
 // 2 hex chars = 4*2 bits = 8 bits = 1 byte. Word = 4 bytes = 32 bits = 8 hex chars.


/* use a 4x4 matrix representing the state of the ChaCha algorithm.
The matrix receives initialisation based on the supplied key, nonce, and a counter.

INPUT
key = 256 bits, concatenation of 8 32-bit LE integers
nonce = 96 bits, concatenation of 3 32-bit LE integers
counter = 32 bit LE integer

OUTPUT
512 bits which is 64 bytes.
*/

void QR(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
uint32_t ROTL(uint32_t* i, int n);
int ChaCha(uint8_t* bytes_in, uint8_t* bytes_out, int size);
int chacha20_block_function(uint32_t* ctx, uint8_t* keystream);
int chacha20_block_function(uint32_t* ctx, uint8_t* keystream);


int ChaCha(uint8_t* bytes_in, uint8_t* bytes_out, int size) {
    int nBlocks = (size / 64)+1;
    printf("size %d nblocks %d\n", size, nBlocks);
    int remainder = size % 64;
    uint8_t** keystream_blocks = malloc(sizeof(uint8_t*)*nBlocks);
    uint8_t* master_keystream = malloc(size);
    const uint32_t constants[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    uint32_t keys[8] = {0x00010203, 0x04050607,0x08090a0b,0x0c0d0e0f,0x10111213,0x14151617, 0x18191a1b, 0x1c1d1e1f}; //256 bits, 8 32-bit LE integers.
    uint32_t counter = 1;
    uint32_t nonce[3] = {0x00000000, 0x0000004a, 0x00000000};
    uint32_t ctx[16];
    uint32_t a = 0x00;
    uint32_t b = 0x00;
    uint32_t c = 0x00;
    uint32_t d = 0x00;

    for (int i = 0 ; i < nBlocks; i++){
        keystream_blocks[i] = malloc(sizeof(uint8_t)*64);
        if (keystream_blocks[i] == NULL){
            printf("malloc failed\n");
            free(keystream_blocks[i]);
            return 1;
        }

        chacha20_init_ctx(&ctx, keys, nonce, &counter, constants);

        chacha20_block_function(ctx, keystream_blocks[i]);
    }
    for (int i = 0; i < nBlocks-1 ;i++){
        for (int j = 0;j<64;j++){
            master_keystream[(i*64)+j] = keystream_blocks[i][j];
        }
    }
    if (remainder > 0) {
        for (int i = 0; i < remainder-1 ; i++){
            master_keystream[((nBlocks-1)*64)+i] = keystream_blocks[nBlocks-1][i];
        }
    }
    
    // printf("\n");
    for (int i = 0 ; i < 12; i++){
        printf("%02x", bytes_in[i]);
    }
    printf("...");
    // printf("\n");
    for (int i = 0 ; i < size; i++){
        bytes_out[i] = bytes_in[i] ^= master_keystream[i];
    }
    for (int i = 0 ; i < 12; i++){
        printf("%02x", bytes_out[i]);
    }
    printf("...f");
    for (int i = 0 ; i < nBlocks; i++){
        free(keystream_blocks[i]);
    }
    free(master_keystream);
    return 0;
}
    


int chacha20_init_ctx(uint32_t* ctx, uint32_t* keys, uint32_t* nonce, uint32_t* counter, uint32_t* constants){

    for (int i = 0; i<4; i++){
        ctx[i] = constants[i];
    }
    for (int i = 0; i < 8; i++){
        ctx[4+i] = _byteswap_ulong(keys[i]);
    }

    ctx[12] = *counter;
    
    for (int i = 0; i<3; i++){
        ctx[13+i] = _byteswap_ulong(nonce[i]);
    }
    for (int i = 0 ; i<16; i++){
        if (i % 4 == 0){
            // printf("\n");
        }
        // printf("%08x ", ctx[i]);
    }
    // printf("\n");
    *counter +=1;
    return 0;
}



int chacha20_block_function(uint32_t* ctx, uint8_t* keystream){
    uint32_t input[16];
    for (int j = 0; j<16;j++){
        input[j] = ctx[j];
    }
    for (int i = 0; i < 10; i++){
        QR(&(ctx[0]), &(ctx[4]), &(ctx[8]), &(ctx[12]));
        QR(&(ctx[1]), &(ctx[5]), &(ctx[9]), &(ctx[13]));
        QR(&(ctx[2]), &(ctx[6]), &(ctx[10]), &(ctx[14]));
        QR(&(ctx[3]), &(ctx[7]), &(ctx[11]), &(ctx[15]));
        QR(&(ctx[0]), &(ctx[5]), &(ctx[10]), &(ctx[15]));
        QR(&(ctx[1]), &(ctx[6]), &(ctx[11]), &(ctx[12]));
        QR(&(ctx[2]), &(ctx[7]), &(ctx[8]), &(ctx[13]));
        QR(&(ctx[3]), &(ctx[4]), &(ctx[9]), &(ctx[14]));
    }
    for (int j = 0; j<16;j++){
        ctx[j] += input[j];
        if (j % 4 == 0){
            // printf("\n");
        }
        // printf("%08x ", ctx[j]);
    }
    for (int i = 0; i < 16 ;i++){
        for (int k = 0 ; k < 4; k++){
            if (k == 0) {
                (keystream)[(i*4)+k] = (ctx[i] & 0x000000FF);
            }
            if (k == 1) {
                (keystream)[(i*4)+k] = ((ctx[i] & 0x0000FF00)>>8);
            }
            if (k == 2) {
                (keystream)[(i*4)+k] = ((ctx[i] & 0x00FF0000)>>16);
            }
            if (k == 3) {
                (keystream)[(i*4)+k] = ((ctx[i] & 0xFF000000)>>24);
            }
        }
    }
    for (int j = 0 ; j < 64; j++){
            // printf("%02x", keystream[j]);
        }
    return 0;
}

void QR(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    *a += *b; *d ^= *a;*d = ROTL(d, 16);
    *c += *d; *b ^= *c;*b = ROTL(b, 12);
    *a += *b; *d ^= *a;*d = ROTL(d, 8);
    *c += *d; *b ^= *c;*b = ROTL(b, 7);

    /*
    0x1234567
    bit shift << 12
    0x5671234;
    (0x1234567 << 7) | (0x1234567 >> 5-7)
    */
}

uint32_t ROTL(uint32_t* i, int n) {
    uint32_t d = (
            ((*i << n) | (*i >> (32-n)))
    
    );
    return d;
}
