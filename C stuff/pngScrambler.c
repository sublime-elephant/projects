//running tips:
//<IN X32 DEVELOPER COMMAND PROMPT> 
//cl.exe main.c /I "C:\Users\Mike\Desktop\web projects\HardStuff\ImageScramble" /link "C:\Users\mike\Desktop\web projects\HardStuff\ImageScramble\zlib.lib" && .\main.exe 

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib-1.3.1/zlib.h>
#include <math.h>
#include <string.h>
#include <openssl\bn.h>
#include "chacha.h"
#include "AES128.h"

typedef struct IHDR_data {
    uint32_t widthLE;
    uint32_t heightLE;
    uint8_t bitdepth;
    uint8_t colortype;
    uint8_t compression;
    uint8_t filtermethod;
    uint8_t interlacemethod;
    uint8_t bpp;
    uLongf uncompressed_size;
    uLongf compressed_size;
} IHDR_data;

typedef struct chunk_fields {
    uint32_t chunk_length;
    uint32_t chunk_type;
    uint8_t* chunk_data;// to be malloc'ed
    uint8_t chunk_CRC[4];
} chunk_fields;

typedef struct RSA_keys{
    BIGNUM *e,*d,*n;
} RSA_keys;

int chunkParser(chunk_fields *s, IHDR_data *ihdr, FILE** fp, uint8_t** IDAT_chunk_data, int *total_IDAT_length);
int defilter(IHDR_data *ihdr, uint8_t *bytes_in);

int XOR_encrypt(char* key, int keylength, uint8_t *bytes_in, uint8_t *bytes_out, IHDR_data* ihdr);

int generate_RSA_keys(RSA_keys *out);
int RSA_encrypt(RSA_keys *rsa_keys, uint8_t *bytes_in, uint8_t *bytes_out, int n);

uint8_t* recompress(uint8_t** encrypted_stream, IHDR_data* idhr);

int rebuild_png(chunk_fields *cf, int chunkcount, uint8_t *compressed_stream, IHDR_data* ihdr);

int rebuild_png2(char* filename, FILE* f2, chunk_fields *cf, int chunkcount, uint8_t *compressed_stream, IHDR_data* ihdr);

uint32_t CRC32(uint8_t* pbs, int len_bytes);

int generate_alot(char* argv, chunk_fields *cf, int chunkcount, uint8_t *bytes_in, IHDR_data* ihdr);







static const uint8_t signature_ideal [8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};



int main(int argc, char* argv[]){
    uint8_t signature[8];
    chunk_fields *cf = NULL;
    uint8_t* uncompressed_stream = NULL;
    uint8_t *encrypted_stream = NULL;
    uint8_t *compressed_stream = NULL;
    uint8_t* IDAT_chunk_data = NULL;
    IHDR_data ihdr;
    int chunkcount = 0;
    int total_IDAT_length = 0;
    RSA_keys rsa_keys;
    
    

    srand(time(NULL));
    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Couldn't open file \n");
        return 1;
    }
    fread(signature, 1, 8, f);
    if (memcmp(signature_ideal, signature, 8) != 0){
        printf("Cannot be a PNG file\n");
        return 1;
    }

    
    while (feof(f) == 0){
        cf = realloc(cf, (chunkcount+1)*sizeof(chunk_fields));
        if (cf == NULL) {
            printf("realloc failed\n");
            return 1;
        }
        if (chunkParser(&cf[chunkcount], &ihdr, &f, &IDAT_chunk_data, &total_IDAT_length) == 1){
            printf("somethings wrong in chunkparser\n");
            return 1;
        }
        chunkcount += 1;
    }
    uncompressed_stream=malloc(ihdr.uncompressed_size);
    switch(uncompress(
        uncompressed_stream, 
        &ihdr.uncompressed_size,
        IDAT_chunk_data, 
        total_IDAT_length))
        {
        case (Z_OK):
        printf("Uncompression okay\n");
        break;
        case (Z_MEM_ERROR):
        printf("Not enough memory\n");
        return 1;
        case (Z_BUF_ERROR):
        printf("Not enough room in the output buffer\n");
        return 1;
        case (Z_DATA_ERROR):
        printf("corrupted/incomplete input stream");
        return 1;
        default:
        printf("Unknown error\n");
        return 1;
    }
    //bytes per pixel calculation = channel amount * no. of bytes per channel
    // Color types:
    /*
        0 = grayscale (1 channel from 0 - 255)
        2 = RGB triple (3 channels)
        3 = Palette index (1 channel?)
        4 = grayscale + alpha (2 channels)
        6 = RGB triple with alpha (4 channels)
    */
   // Bit depths: <8 bit depth is rounded up to 1. 16 bits is 2 bytes.
    
      



    if (defilter(&ihdr, uncompressed_stream) != 0){
        printf("something went wrong in defiltering\n");
        return 1;
    }

    

    if (generate_RSA_keys(&rsa_keys) != 0){
        printf("something went wrong generating RSA keys\n");
        return 1;
    }

    encrypted_stream = malloc(ihdr.uncompressed_size);
    uint8_t* uncompressed_stream_for_batch = malloc(ihdr.uncompressed_size);
    memcpy(uncompressed_stream_for_batch, uncompressed_stream, ihdr.uncompressed_size);

    // RSA_encrypt(&rsa_keys, uncompressed_stream, encrypted_stream, ihdr.uncompressed_size);

    // if (XOR_encrypt(argv[2], strlen(argv[2]), uncompressed_stream, encrypted_stream, &ihdr) != 0) {
    //     printf("something went wrong encrypting with XOR\n");
    //     return 1;
    // }

    // printf("Trying to make salsa...\n");
    // if ((ChaCha(uncompressed_stream, encrypted_stream, ihdr.uncompressed_size)) != 0){
    //     printf("Failed to make yummy Salsa\n");
    // }


    uint8_t key[16] = {0x11,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
    printf("\nTrying AES\n");
    AES128(&uncompressed_stream, &encrypted_stream, ihdr.uncompressed_size, key);
    printf("\n");
    for (int i = 0 ; i < 32; i ++){
        printf("%02x", uncompressed_stream[i]);
    }
    printf("\n---\n");
    for (int i = 0 ; i < 32; i ++){
        printf("%02x", encrypted_stream[i]);
    }
    for (int i = 0 ; i < ihdr.uncompressed_size; i++){
        if (i % (ihdr.bpp*ihdr.widthLE+1) == 0){
            encrypted_stream[i] = 0x00; // re-do filter_byte
        }
    }
    // generate_alot(argv[3], cf, chunkcount, uncompressed_stream_for_batch, &ihdr);

    compressed_stream = recompress(&encrypted_stream, &ihdr);

    if (compressed_stream == NULL){
        printf("recompression failed\n");
        return 1;
    }

    if (rebuild_png(cf, chunkcount, compressed_stream, &ihdr) != 0) {
        printf("something went wrong rebuilding the png\n");
        return 1;
    };



    free(uncompressed_stream);
    free(compressed_stream);
    free(encrypted_stream);
    fclose(f);
    free(cf);
    return 0;
}


int chunkParser(chunk_fields *s, IHDR_data *ihdr, FILE** fp, uint8_t** IDAT_chunk_data, int *total_IDAT_length) {

    //Signature check already read 8 bits, so we are ready to start reading chunks normally.
    if (fread(&s->chunk_length, 4, 1, *fp) < 1){
        printf("Couldn't read chunk length\n"); 
        return 1;
    }

    s->chunk_length = (
        ((s->chunk_length >> 24) & 0xFF) |
        ((s->chunk_length << 8) & 0xFF0000) |
        ((s->chunk_length >> 8) & 0xFF00) |
        ((s->chunk_length << 24) & 0xFF000000)
    );
    if (fread(&s->chunk_type, 4, 1, *fp) < 1){
        printf("Couldn't read chunk_type\n");
        return 1;
    }
    s->chunk_data = malloc(s->chunk_length);
    if (!s->chunk_data){
        printf("chunk_data malloc failed\n");
        return 1;
    }
    if (fread(s->chunk_data, 1, s->chunk_length, *fp) < s->chunk_length) {
        printf("Incomplete data read\n");
        return 1;
    }
    if (memcmp(&s->chunk_type, "IHDR", 4) == 0){
        ihdr->widthLE = (
        (s->chunk_data[0] << 24) |
        (s->chunk_data[1] << 16) |
        (s->chunk_data[2] << 8)  |
        (s->chunk_data[3]));
        ihdr->heightLE = (
        (s->chunk_data[4] << 24) |
        (s->chunk_data[5] << 16) |
        (s->chunk_data[6] << 8)  |
        (s->chunk_data[7]));
        ihdr->bitdepth = s->chunk_data[8];
        ihdr->colortype = s->chunk_data[9];
        ihdr->compression = s->chunk_data[10];
        ihdr->filtermethod = s->chunk_data[11];
        ihdr->interlacemethod = s->chunk_data[12];
        
        uint8_t channel_amount=0;
        switch (ihdr->colortype) {
        case 0: channel_amount = 1; break;
        case 2: channel_amount = 3; break;
        case 3: channel_amount = 1; break;
        case 4: channel_amount = 2; break;
        case 6: channel_amount = 4; break;
        }

        uint8_t bytes_channel=0;
        switch (ihdr->bitdepth) {
            case 1: bytes_channel = 1; break;
            case 2: bytes_channel = 1; break;
            case 4: bytes_channel = 1; break;
            case 8: bytes_channel = 1; break;
            case 16: bytes_channel = 2; break;
        }
        ihdr->bpp = channel_amount * bytes_channel;
        ihdr->uncompressed_size = ((ihdr->heightLE))*(ihdr->bpp)*((ihdr->widthLE))+((1)*(ihdr->heightLE));

    }
    // IDAT chunk processing
    if (memcmp(&s->chunk_type, "IDAT", 4) == 0){ 
        // printf("IDAT: %d, %d\n", s->chunk_length, *total_IDAT_length);
        *IDAT_chunk_data = realloc(*IDAT_chunk_data, ((s->chunk_length)+(*total_IDAT_length)));
        if (*IDAT_chunk_data == NULL){
            printf("IDAT data chunk realloc failed\n");
            return 1;
        }
        for (int i = 0; i < s->chunk_length ; i++ ){
            (*IDAT_chunk_data)[*total_IDAT_length+i] = s->chunk_data[i];
        }
            
        *total_IDAT_length += s->chunk_length;



    }
    if (memcmp(&s->chunk_type, "IEND", 4) == 0) {
        fread(s->chunk_CRC, 4, 1, *fp);
        fread(s->chunk_CRC, 4,1, *fp); // read more, to trip feof().
    }
    else if (fread(s->chunk_CRC, 4, 1, *fp) < 1){
        printf("failed to read chunk CRC\n");
        return 1;
    }
    return 0;
}

int generate_RSA_keys(RSA_keys *out){
    BIGNUM *p = BN_new();
    BIGNUM *q = BN_new();
    BIGNUM *n = BN_new();

    BIGNUM *phi = BN_new();
    BIGNUM *phiP = BN_new();
    BIGNUM *phiQ = BN_new();
    BIGNUM *e = BN_new();
    BIGNUM *d = BN_new();
    int error;
    BN_CTX *ctx = BN_CTX_new();
    error = BN_generate_prime_ex(p, 1024, 0, NULL, NULL, NULL);
    if (error != 1){
        printf("error0");
        return 1;
    }
    error = BN_generate_prime_ex(q, 1024, 0, NULL, NULL, NULL);
    if (error != 1){
        printf("error1");
        return 1;
    }
    error = BN_mul(n, p, q, ctx);
    if (error != 1){
        printf("error2");
        return 1;
    }

    phiP = BN_copy(phiP, p);
    phiQ = BN_copy(phiQ, q);

    error = BN_sub_word(phiP, (BN_ULONG)1);
    if (error != 1){
        printf("error3");
        return 1;
    }
    error = BN_sub_word(phiQ, (BN_ULONG)1);
    if (error != 1){
        printf("error4");
        return 1;
    }
    error = BN_mul(phi, phiP, phiQ, ctx);
    if (error != 1){
        printf("error5");
        return 1;
    }
    char *hex2 = BN_bn2dec(phi);
    // if (hex2 == NULL){
    //     printf("n");
    //     return 1;
    // }
    // printf("%s",hex2);
    // OPENSSL_free(hex2);

    const char *s = "65537";
    error = BN_dec2bn(&e, s);
    if (error == 0){
        printf("error6");
        return 1;
    }
    // else {
    //     char *hex3 = BN_bn2dec(e);
    //     if (hex3 == NULL){
    //         printf("n");
    //         return 1;
    //     }
    //     printf("\n\n%s",hex3);
    //     OPENSSL_free(hex3);
    // }
    BIGNUM *err;
    err = BN_mod_inverse(d, e, phi, ctx);
    if (err == NULL){
        printf("error7");
        return 1;
    }
    // char *hex = BN_bn2dec(d);
    // if (hex == NULL){
    //     printf("n");
    //     return 1;
    // }
    // printf("\n\n%s",hex);
    // OPENSSL_free(hex);
    out->d = BN_new();
    out->e = BN_new();
    out->n = BN_new();
    if (!BN_copy(out->d, d)){
        printf("copy error");
    }
    if (!BN_copy(out->e, e)){
        printf("copy error");
    }
    if (!BN_copy(out->n, n)){
        printf("copy error");
    }

    return 0;
}

int RSA_encrypt(RSA_keys *rsa_keys, uint8_t *bytes_in, uint8_t *bytes_out, int n){
    BIGNUM *k = BN_new();
    BIGNUM *r = BN_new();
    BN_CTX *ctx = BN_CTX_new();
    for (int i = 0; i < n; i++){
        if ((BN_set_word(k, bytes_in[i]))!=1) {
            printf("setword failed\n");
            return 1;
        }
        if ((BN_mod_exp(r, k, rsa_keys->e, rsa_keys->n, ctx)) == 0) {
            printf("bn_mod_exp failed\n");
            return 1;
        }
        if (BN_num_bytes(r) == 0) {
            bytes_out[i]=0;
        }
        else {
            bytes_out[i]=BN_mod_word(r, 256);
        }
    }
    return 0;
}

uint32_t CRC32(uint8_t* pbs, int len_bytes){
    uint32_t polynomial = 0xedb88320u;
    /*
    pbs = pointer_to_byte_stream;
    x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
    1 2 3 4 5 6 7 8 9 (10) 1 2 3 4 5 6 7 8 9 (20) 1 2 3 4 5 6 7 8 9 (30) 1 2
1   1 1 0 1 1 0 1 1 0  1   1 1 0 0 0 1 0 0 0  0   0 1 1 0 0 1 0 0 0   0  0 1
    */
    uint32_t crc = 0xFFFFFFFF;
    for (int i = 0; i < len_bytes; i++){
            uint8_t ith_byte = *(pbs+i);
        for (int j = 0; j < 8; j++) {
            uint8_t bit = (ith_byte >> j) & 1; // is j bit in byte i 1 or 0?
            crc ^= bit;
            if ((crc & 1) == 1) {
                crc = crc >> 1;
                crc ^= polynomial;
            }
            else {
                crc = crc >> 1;
            }
        }
    }
    crc = crc^0xFFFFFFFFu;
    return crc;
}

int XOR_encrypt(char* key, int keylength, uint8_t *bytes_in, uint8_t *bytes_out, IHDR_data* ihdr) {
    for (int i = 0; i < ihdr->uncompressed_size; i++){
        if(1==1) {
             if (i % (ihdr->widthLE*ihdr->bpp+1) == 0){
            bytes_out[i] = 0x00;
            }
            else {
                if (bytes_in[i] > 0x96){
                    bytes_out[i] = bytes_in[i];
                }
                else {
                    bytes_out[i] = bytes_in[i]^(key[i%keylength]);
                }
            }
        }
        else {
            if (i % (ihdr->widthLE*ihdr->bpp+1) == 0 || i == 0){
            bytes_out[i] = 0x00;
            }
            else {
                bytes_out[i] = bytes_in[i];
            }
        }
       
    }
    return 0;
}

int generate_alot(char* argv, chunk_fields *cf, int chunkcount, uint8_t *bytes_in, IHDR_data* ihdr){
    const char charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789";

    int keysize = strtol(argv, NULL, 10);
    printf("keysize is %d\n", keysize);
    uint8_t* encrypted_stream_array[20];
    uint8_t *compressed_stream[20];
    ihdr->compressed_size = 0;
    char* key_array[20];
    char* filenames[20];
    FILE* fparray[20];

    for (int i = 0; i < 20; i++){
        encrypted_stream_array[i] = malloc(ihdr->uncompressed_size);
        key_array[i] = malloc(keysize+1);
        printf("key: ");
        for (int j = 0; j<keysize; j++){
            key_array[i][j] = rand() % 255;
            key_array[i][keysize] = 0x00;
            printf("%c", key_array[i][j]);
        }
        printf("\n");
        filenames[i] = malloc(5+keysize+1);
        snprintf(filenames[i], 5+keysize+1, "%d.png", i);
        fparray[i] = fopen(filenames[i], "wb");
        compressed_stream[i] = malloc(compressBound(ihdr->uncompressed_size));
        XOR_encrypt(key_array[i], keysize, bytes_in, encrypted_stream_array[i], ihdr);
        compressed_stream[i] = recompress(&encrypted_stream_array[i], ihdr);
        rebuild_png2(filenames[i], fparray[i], cf, chunkcount, compressed_stream[i], ihdr);
        free(encrypted_stream_array[i]);
        free(key_array[i]);
        free(filenames[i]);
        free(compressed_stream[i]);
        fclose(fparray[i]);
    }
    return 0;
}

int defilter(IHDR_data *ihdr, uint8_t *bytes_in) {
    printf("width: %d height: %d", ihdr->widthLE, ihdr->heightLE);
    int lpsl = 0;
    int apal = 0;
    int lpal = 0;
    int filter_type = 0;
    for (int i = 0; i < ihdr->heightLE; i++){
        for (int k = 0; k < (ihdr->widthLE*ihdr->bpp)+1; k++){

            int i_cp = (i*(ihdr->widthLE)*(ihdr->bpp))+k+(1*i);
            int i_lpsl = i_cp-(ihdr->bpp);
            int i_apal = i_cp-(ihdr->bpp*(ihdr->widthLE)+1);
            int i_lpal = i_cp-(ihdr->bpp*(ihdr->widthLE)+1)-ihdr->bpp;


            int current_pixel = bytes_in[i_cp];

            if (k==0){
                filter_type = current_pixel;
                // printf("%02x", current_pixel);
                bytes_in[i_cp] = 0; //needed.
            }

            else {

                if (k>ihdr->bpp){
                    lpsl = bytes_in[i_lpsl];
                }
                else {
                    lpsl = 0;
                }

                if (i > 0) {
                    apal = bytes_in[i_apal];
                }
                else {
                    apal = 0;
                }
                if (i > 0 && k>ihdr->bpp){
                    lpal = bytes_in[i_lpal];
                }
                else {
                    lpal = 0;
                }

                if (filter_type == 0) {
                    // printf("%02x", current_pixel);
                }

                if (filter_type == 1) {
                    bytes_in[i_cp] = (current_pixel+lpsl)%256;
                    current_pixel = bytes_in[i_cp];
                    // printf("%02x", current_pixel);
                }
                else if (filter_type == 2) {
                    bytes_in[i_cp] = (current_pixel+apal)%256;
                    current_pixel = bytes_in[i_cp];
                    // printf("%02x", current_pixel);
                }
                else if (filter_type == 3) {
                    bytes_in[i_cp] = ((current_pixel) + (lpsl+apal)/2)%256;
                    current_pixel = bytes_in[i_cp];
                    // printf("%02x", current_pixel);
                }
                else if (filter_type == 4){ 
                    int p = lpsl+apal-lpal;
                    int pa = abs(p-lpsl);
                    int pb = abs(p-apal);
                    int pc = abs(p-lpal);
                    if (pa <= pb && pa <= pc){
                        p = lpsl;
                    }
                    else if (pb <= pc) {
                        p = apal;
                    }
                    else {p = lpal;}
                    bytes_in[i_cp] = ((current_pixel) + (p))%256;
                    current_pixel = bytes_in[i_cp];
                    // printf("%02x", current_pixel);
                }
            }
        }

    }
    return 0;
}


uint8_t* recompress(uint8_t **encrypted_stream, IHDR_data* ihdr) {
    ihdr->compressed_size = compressBound(ihdr->uncompressed_size);
    uint8_t *compressed_stream = malloc(ihdr->compressed_size);

    switch(compress(
        compressed_stream, 
        &ihdr->compressed_size,
        *encrypted_stream, 
        ihdr->uncompressed_size))
        {
        case (Z_OK):
        printf("Compression okay\n");
        break;
        case (Z_MEM_ERROR):
        printf("Not enough memory\n");
        return NULL;
        case (Z_BUF_ERROR):
        printf("Not enough room in the output buffer\n");
        return NULL;
        case (Z_DATA_ERROR):
        printf("corrupted/incomplete input stream");
        return NULL;
        default:
        printf("Unknown error\n");
        return NULL;
    }
    return compressed_stream;
}

int rebuild_png(chunk_fields *cf, int chunkcount, uint8_t *compressed_stream, IHDR_data* ihdr) {
    FILE* f2 = fopen("out.png", "wb");
    if (f2 == NULL) {
        printf("Couldn't open file \n");
        return 1;
    }
    fwrite(signature_ideal, 1, 8, f2);

    int a = 0;
    for (int i = 0; i<chunkcount;i++){
        if ((memcmp(&cf[i].chunk_type, "IDAT", 4) == 0) && a == 1){
            // do nothing
            continue;
        }
        if ((memcmp(&cf[i].chunk_type, "IDAT", 4) == 0) && a == 0){
            uint32_t datalen = ihdr->compressed_size;
            datalen = _byteswap_ulong(datalen);
            fwrite(&datalen, 1, 4, f2);
            datalen = _byteswap_ulong(datalen);
            fwrite(&cf[i].chunk_type, 1, 4, f2);
            fwrite(compressed_stream, 1, datalen, f2);
            uint8_t *chunk_type_and_data = malloc(datalen+4);
            memcpy(chunk_type_and_data, &cf[i].chunk_type, 4);
            memcpy(chunk_type_and_data+4, compressed_stream, datalen);
            uint32_t crc = CRC32(chunk_type_and_data, datalen+4);
            crc = _byteswap_ulong(crc);
            fwrite(&crc, 1, 4, f2);
            a = 1;
            free(chunk_type_and_data);
        }

        else {
            uint32_t datalen = (*(cf+i)).chunk_length;
            datalen = _byteswap_ulong(datalen);
            fwrite(&datalen, 1, 4, f2);
            datalen = _byteswap_ulong(datalen);
            fwrite(&cf[i].chunk_type, 1, 4, f2);
            fwrite(cf[i].chunk_data, 1, datalen, f2);
            uint8_t *chunk_type_and_data = malloc(datalen+4);
            memcpy(chunk_type_and_data, &cf[i].chunk_type, 4);
            memcpy(chunk_type_and_data+4, cf[i].chunk_data, datalen);
            uint32_t crc = CRC32(chunk_type_and_data, datalen+4);
            crc = _byteswap_ulong(crc);
            fwrite(&crc, 1, 4, f2);
            free(chunk_type_and_data);
        }
    }
    fclose(f2);
    return 0;
}

int rebuild_png2(char* filename, FILE* f2, chunk_fields *cf, int chunkcount, uint8_t *compressed_stream, IHDR_data* ihdr) {
    f2 = fopen(filename, "wb");
    if (f2 == NULL) {
        printf("Couldn't open file \n");
        return 1;
    }
    printf("printing to %s\n", filename);
    unsigned char sig[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    fwrite(sig, 1, 8, f2);

    int a = 0;
    for (int i = 0; i<chunkcount;i++){
        if ((memcmp(&cf[i].chunk_type, "IDAT", 4) == 0) && a == 1){
            // do nothing
            continue;
        }
        if ((memcmp(&cf[i].chunk_type, "IDAT", 4) == 0) && a == 0){
            uint32_t datalen = ihdr->compressed_size;
            datalen = _byteswap_ulong(datalen);
            fwrite(&datalen, 1, 4, f2);
            datalen = _byteswap_ulong(datalen);
            fwrite(&cf[i].chunk_type, 1, 4, f2);
            fwrite(compressed_stream, 1, datalen, f2);
            uint8_t *chunk_type_and_data = malloc(datalen+4);
            memcpy(chunk_type_and_data, &cf[i].chunk_type, 4);
            memcpy(chunk_type_and_data+4, compressed_stream, datalen);
            uint32_t crc = CRC32(chunk_type_and_data, datalen+4);
            crc = _byteswap_ulong(crc);
            fwrite(&crc, 1, 4, f2);
            a = 1;
            free(chunk_type_and_data);
        }

        else {
            uint32_t datalen = (*(cf+i)).chunk_length;
            datalen = _byteswap_ulong(datalen);
            fwrite(&datalen, 1, 4, f2);
            datalen = _byteswap_ulong(datalen);
            fwrite(&cf[i].chunk_type, 1, 4, f2);
            fwrite(cf[i].chunk_data, 1, datalen, f2);
            uint8_t *chunk_type_and_data = malloc(datalen+4);
            memcpy(chunk_type_and_data, &cf[i].chunk_type, 4);
            memcpy(chunk_type_and_data+4, cf[i].chunk_data, datalen);
            uint32_t crc = CRC32(chunk_type_and_data, datalen+4);
            crc = _byteswap_ulong(crc);
            fwrite(&crc, 1, 4, f2);
            free(chunk_type_and_data);
        }
    }
    fclose(f2);
    return 0;
}
