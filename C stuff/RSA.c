//compile: cl.exe RSA.c /I "C:\Program Files\OpenSSL-Win64\include" /link /LIBPATH:"C:\Program Files\OpenSSL-Win64\lib\VC\x64\MD" libcrypto.lib && .\RSA.exe

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <openssl\bn.h>

typedef struct RSA_keys{
    BIGNUM *e,*d,*n;
} RSA_keys;

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